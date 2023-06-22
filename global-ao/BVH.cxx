#include "BVH.hxx"

BVH::BVH(std::vector<Vertex>* vertices, std::vector<Triangle>* triangles)
{
    verts = (*vertices);
    tris = (*triangles);
    triLimit = 10;
    Node root;
    root.aabb.min = glm::vec3(INFINITY, INFINITY, INFINITY);
    root.aabb.max = glm::vec3(-INFINITY, -INFINITY, -INFINITY);
    for (size_t i = 0; i < tris.size(); ++i)
    {
        Triangle& tri = tris[i];

        root.aabb.min = glm::min(verts[tri.x].position, root.aabb.min);
        root.aabb.min = glm::min(verts[tri.y].position, root.aabb.min);
        root.aabb.min = glm::min(verts[tri.z].position, root.aabb.min);

        root.aabb.max = glm::max(verts[tri.x].position, root.aabb.max);
        root.aabb.max = glm::max(verts[tri.y].position, root.aabb.max);
        root.aabb.max = glm::max(verts[tri.z].position, root.aabb.max);

        root.triangles.push_back(i);
    }
    nodes.push_back(root);
}

BVH::~BVH()
{
}


void BVH::build(){
    int currentIDX = 0;
    while (currentIDX < nodes.size())
    {
        Node& node = nodes[currentIDX];
        if (node.triangles.size() > triLimit)
        {
            glm::vec3 span = node.aabb.max - node.aabb.min;
            int maxI;
            float maxSpan = -INFINITY;
            for (int i = 0; i < 3; ++i){
                if(span[i] > maxSpan){
                    maxI = i;
                    maxSpan = span[i];
                }
            }
            // compute new aabbs
            Node left, right;
            left.aabb = node.aabb;
            right.aabb = node.aabb;
            left.aabb.max[maxI] = node.aabb.max[maxI] - span[maxI]/2.0;
            right.aabb.min[maxI] = node.aabb.min[maxI] + span[maxI]/2.0;
            

            // sort triangles into new aabbs
            for (auto& idx : node.triangles){
                glm::vec3 v0, v1, v2;
                v0 = verts[tris[idx].x].position;
                v1 = verts[tris[idx].y].position;
                v2 = verts[tris[idx].z].position;

                auto SAT = [&v0, &v1, &v2](AABB& aabb) {
                    // reference https://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/code/tribox3.txt
                    auto planeBox = [](glm::vec3 normal, glm::vec3 vert, glm::vec3 maxbox) {
                        int q;
                        glm::vec3 vmin, vmax;
                        float v;
                        for(q=0;q<=2;q++)
                        {
                            v=vert[q];
                            if(normal[q]>0.0f) {
                            vmin[q]=-maxbox[q] - v;
                            vmax[q]= maxbox[q] - v;
                            } else {
                            vmin[q]= maxbox[q] - v;
                            vmax[q]=-maxbox[q] - v;
                            }
                        }
                        if(glm::dot(normal,vmin)>0.0f) return false;
                        if(glm::dot(normal,vmax)>=0.0f) return true;
                        return false;
                    };

                    auto findMinMax = [](float x0, float x1, float x2, float& min, float& max) {
                        min = max = x0;
                        if(x1<min) min=x1;
                        if(x1>max) max=x1;
                        if(x2<min) min=x2;
                        if(x2>max) max=x2;
                    };

                    glm::vec3 boxhalfsize = (aabb.max - aabb.min)/2.0f;
                    glm::vec3 boxcenter = aabb.max - boxhalfsize;

                    auto axisTest = [boxhalfsize](float a, float b, float fa, float fb, glm::vec3 va, glm::vec3 vb, int axis1, int axis2){
                        float r = a*va[axis1] - b*va[axis1];
                        float s = a*va[axis1] - b*va[axis1];
                        float min, max, rad;
                        if (r < s){min = r;max = s;} else {min = s; max = r;}
                        rad = fa * boxhalfsize[axis1] * fb * boxhalfsize[axis2];
                        if (min>rad || max<-rad) return false;
                        else return true;
                    };

                    // center on box
                    glm::vec3 vc0, vc1, vc2;
                    vc0 = v0-boxcenter;
                    vc1 = v1-boxcenter;
                    vc2 = v2-boxcenter;
                    // edges
                    glm::vec3 e0, e1, e2;
                    e0 = vc1-vc0;
                    e1 = vc2-vc1;
                    e2 = vc0-vc2;

                    // 9 cross tests
                    float fex, fey, fez;
                    fex = fabsf(e0[0]);
                    fey = fabsf(e0[1]);
                    fez = fabsf(e0[2]);
                    if(axisTest(e0[2], e0[1], fez, fey, vc0, vc2, 1, 2)) return false;
                    if(axisTest(e0[2], e0[0], fez, fex, vc0, vc2, 0, 2)) return false;
                    if(axisTest(e0[1], e0[0], fey, fex, vc1, vc2, 0, 1)) return false;

                    fex = fabsf(e1[0]);
                    fey = fabsf(e1[1]);
                    fez = fabsf(e1[2]);
                    if(axisTest(e1[2], e1[1], fez, fey, vc0, vc2, 1, 2)) return false;
                    if(axisTest(e1[2], e1[0], fez, fex, vc0, vc2, 0, 2)) return false;
                    if(axisTest(e1[1], e1[0], fey, fex, vc0, vc1, 0, 1)) return false;

                    fex = fabsf(e2[0]);
                    fey = fabsf(e2[1]);
                    fez = fabsf(e2[2]);
                    if(axisTest(e2[2], e2[1], fez, fey, vc0, vc1, 1, 2)) return false;
                    if(axisTest(e2[2], e2[0], fez, fex, vc0, vc1, 0, 2)) return false;
                    if(axisTest(e2[1], e2[0], fey, fex, vc1, vc2, 0, 1)) return false;

                    // normal directions
                    float min, max;
                    findMinMax(vc0[0], vc1[0], vc2[0], min,max);
                    if(min>boxhalfsize[0] || max<-boxhalfsize[0]) return false;

                    findMinMax(vc0[1], vc1[1], vc2[1], min,max);
                    if(min>boxhalfsize[1] || max<-boxhalfsize[1]) return false;

                    findMinMax(vc0[2], vc1[2], vc2[2], min,max);
                    if(min>boxhalfsize[2] || max<-boxhalfsize[2]) return false;

                    // plane test
                    auto normal = glm::cross(e0, e1);
                    if(!planeBox(normal, v0, boxhalfsize)) return false;
                    return true; 
                };

                if (SAT(left.aabb)) left.triangles.push_back(idx);
                else if (SAT(right.aabb)) right.triangles.push_back(idx);
            }
            left.parent = currentIDX;
            right.parent = currentIDX;

            node.left = nodes.size() + 0;
            node.right = nodes.size() + 1;
            node.triangles.resize(0);

            nodes.push_back(left);
            nodes.push_back(right);
        }
        currentIDX += 1;
    }
}