#include "BVH.hxx"

#include <queue>


BVH::BVH(std::vector<Vertex>* vertices, std::vector<Triangle>* triangles)
{
    verts = (*vertices);
    verts_pos.reserve(verts.size());
    for (auto& vert : verts) verts_pos.emplace_back(vert.position);
    tris = (*triangles);
    Node root;
    root.aabb.min = glm::vec3(INFINITY, INFINITY, INFINITY);
    root.aabb.max = glm::vec3(-INFINITY, -INFINITY, -INFINITY);
    root.triangles.reserve(tris.size());
    for (size_t i = 0; i < tris.size(); ++i)
    {
        Triangle& tri = tris[i];

        root.aabb.min = glm::min(verts_pos[tri.x], root.aabb.min);
        root.aabb.min = glm::min(verts_pos[tri.y], root.aabb.min);
        root.aabb.min = glm::min(verts_pos[tri.z], root.aabb.min);

        root.aabb.max = glm::max(verts_pos[tri.x], root.aabb.max);
        root.aabb.max = glm::max(verts_pos[tri.y], root.aabb.max);
        root.aabb.max = glm::max(verts_pos[tri.z], root.aabb.max);

        root.triangles.emplace_back(i);
    }
    build_nodes.emplace_back(root);

    RenderNode renderRoot;
    renderRoot.aabb = root.aabb;
    renderRoot.startTriOffset = 0;
    renderRoot.numTri = root.triangles.size();
    perNodeTriIndices.resize(tris.size());
    std::iota(begin(perNodeTriIndices), end(perNodeTriIndices), 0);
    render_nodes.emplace_back(renderRoot);
}

BVH::~BVH()
{
}

void BVH::build(){
    perNodeTriIndices.reserve(tris.size()*2);
    perNodeTriIndices.clear();
    render_nodes[0].numTri = 0;

    int currentIDX = 0;
    while (currentIDX < build_nodes.size())
    {
        Node& node = build_nodes[currentIDX];
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
                v0 = verts_pos[tris[idx].x];
                v1 = verts_pos[tris[idx].y];
                v2 = verts_pos[tris[idx].z];

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
                        float r = a*va[axis1] - b*va[axis2];
                        float s = a*vb[axis1] - b*vb[axis2];
                        float min, max, rad;
                        if (s < r) {min = s; max = r;} else {min = r; max = s;}
                        rad = fa * boxhalfsize[axis1] + fb * boxhalfsize[axis2];
                        if (min>rad || max<-rad) return true;
                        else return false;
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
                    if(axisTest(e0[0], e0[2], fex, fez, vc0, vc2, 2, 0)) return false;//flip
                    if(axisTest(e0[1], e0[0], fey, fex, vc2, vc1, 0, 1)) return false;//flip vc

                    fex = fabsf(e1[0]);
                    fey = fabsf(e1[1]);
                    fez = fabsf(e1[2]);
                    if(axisTest(e1[2], e1[1], fez, fey, vc0, vc2, 1, 2)) return false;
                    if(axisTest(e1[0], e1[2], fex, fez, vc0, vc2, 2, 0)) return false;//flip
                    if(axisTest(e1[1], e1[0], fey, fex, vc0, vc1, 0, 1)) return false;

                    fex = fabsf(e2[0]);
                    fey = fabsf(e2[1]);
                    fez = fabsf(e2[2]);
                    if(axisTest(e2[2], e2[1], fez, fey, vc0, vc1, 1, 2)) return false;
                    if(axisTest(e2[0], e2[2], fex, fez, vc0, vc1, 2, 0)) return false;//flip
                    if(axisTest(e2[1], e2[0], fey, fex, vc2, vc1, 0, 1)) return false;//flip vc

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
                    if(!planeBox(normal, vc0, boxhalfsize)) return false;
                    return true; 
                };

                left.triangles.reserve(node.triangles.size());
                right.triangles.reserve(node.triangles.size());
                if (SAT(left.aabb)) left.triangles.emplace_back(idx);
                if (SAT(right.aabb)) right.triangles.emplace_back(idx);
                left.triangles.shrink_to_fit();
                right.triangles.shrink_to_fit();
            }
            left.parent = currentIDX;
            right.parent = currentIDX;

            node.left = build_nodes.size() + 0;
            node.right = build_nodes.size() + 1;
            node.triangles.resize(0);

            auto& currentRenderNode = render_nodes[currentIDX];
            RenderNode leftRN, rightRN;
            leftRN.aabb = left.aabb;
            rightRN.aabb = right.aabb;
            currentRenderNode.left = node.left;
            currentRenderNode.right = node.right;
            render_nodes.emplace_back(leftRN);
            render_nodes.emplace_back(rightRN);
        
            build_nodes.push_back(left);
            build_nodes.push_back(right);
        } else {
            // current node is child node, so copy it tri indices to list
            auto& currentRenderNode = render_nodes[currentIDX];
            currentRenderNode.startTriOffset = perNodeTriIndices.size();
            currentRenderNode.numTri = node.triangles.size();
            //std::copy(node.triangles.begin(), node.triangles.end(), perNodeTris.begin()+perNodeTris.size());
            for (int i = 0; i < node.triangles.size(); ++i) perNodeTriIndices.emplace_back(node.triangles[i]);
        }
        currentIDX += 1;
    }
    perNodeTriIndices.shrink_to_fit();
}

bool BVH::rayAABBTest(AABB& aabb, glm::vec3 origin, glm::vec3 dir)
{
    // reference https://web.archive.org/web/20090803054252/http://tog.acm.org/resources/GraphicsGems/gems/RayBox.c
    bool inside = true;
	char quadrant[3];
	int whichPlane;
	double maxT[3];
	double candidatePlane[3];

    const int NUMDIM = 3;
    const int RIGHT = 0;
    const int LEFT = 1;
    const int MIDDLE = 2;

    const bool FALSE = false;
    const bool TRUE = true;

    glm::vec3& minB = aabb.min;
    glm::vec3& maxB = aabb.max;

    glm::vec3 coord;

	// Find candidate planes; this loop can be avoided if rays cast all from the eye(assume perpsective view)
	for (int i=0; i<3; i++){
		if(origin[i] < minB[i]) {
			quadrant[i] = LEFT;
			candidatePlane[i] = minB[i];
			inside = FALSE;
		}else if (origin[i] > maxB[i]) {
			quadrant[i] = RIGHT;
			candidatePlane[i] = maxB[i];
			inside = FALSE;
		}else	{
			quadrant[i] = MIDDLE;
        }
    }
		

	// Ray origin inside bounding box
	if(inside)	{
		coord = origin;
		return (TRUE);
	}


	// Calculate T distances to candidate planes
	for (int i = 0; i < NUMDIM; i++)
		if (quadrant[i] != MIDDLE && dir[i] !=0.)
			maxT[i] = (candidatePlane[i]-origin[i]) / dir[i];
		else
			maxT[i] = -1.;

	// Get largest of the maxT's for final choice of intersection
	whichPlane = 0;
	for (int i = 1; i < NUMDIM; i++)
		if (maxT[whichPlane] < maxT[i])
			whichPlane = i;

	// Check final candidate actually inside box
	if (maxT[whichPlane] < 0.) return (FALSE);
	for (int i = 0; i < NUMDIM; i++)
		if (whichPlane != i) {
			coord[i] = origin[i] + maxT[whichPlane] *dir[i];
			if (coord[i] < minB[i] || coord[i] > maxB[i])
				return (FALSE);
		} else {
			coord[i] = candidatePlane[i];
		}
	return (TRUE);// ray hits box
}

bool BVH::rayTriangleTest(glm::vec3 origin, glm::vec3 direction, glm::uint index, bool backfaceCulling){
    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    const float EPSILON = 0.0000001;
    auto tri = tris[index];
    glm::vec3 v0 = verts_pos[tri.x];
    glm::vec3 v1 = verts_pos[tri.y];
    glm::vec3 v2 = verts_pos[tri.z];
    glm::vec3 edge1, edge2, h, s, q;
    float a, f, u, v;
    edge1 = v1-v0;
    edge2 = v2-v0;

    if (backfaceCulling){
        auto normal = glm::cross(edge1, edge2);
        if (glm::dot(direction, normal) < 0.0) return false;
    }

    h = glm::cross(direction, edge2);
    a = glm::dot(edge1, h);

    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.

    f = 1.0 / a;
    s = origin - v0;
    u = f * glm::dot(s, h);

    if (u < 0.0 || u > 1.0)
        return false;

    q = glm::cross(s, edge1);
    v = f * dot(direction, q);

    if (v < 0.0 || u + v > 1.0)
        return false;

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * glm::dot(edge2, q);

    if (t > EPSILON) // ray intersection
    {
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

bool BVH::collissionCheck(glm::vec3 origin, glm::vec3 dir) {
    std::queue<int> nodeIndices;
    nodeIndices.push(0);
    while (nodeIndices.size() != 0){
        auto& currentNode = render_nodes[nodeIndices.front()];

        if(rayAABBTest(currentNode.aabb, origin, dir)){
            for (int i = 0; i < currentNode.numTri; ++i){
                if(rayTriangleTest(origin, dir, perNodeTriIndices[i+currentNode.startTriOffset])) return true;
            }

            if (currentNode.left != -1) nodeIndices.emplace(currentNode.left);
            if (currentNode.right != -1) nodeIndices.emplace(currentNode.right);
        }
        nodeIndices.pop();
    }
    return false;
}