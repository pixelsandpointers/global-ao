#include "AOGenerator.hxx"
#include <queue>


bool rayTriangleTest(glm::vec3 origin, glm::vec3 direction, glm::uint index, std::vector<Vertex>* vertices, std::vector<Triangle>* triangles){
    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    const float EPSILON = 0.0000001;
    auto tri = (*triangles)[index];
    glm::vec3 v0 = (*vertices)[tri.x].position;
    glm::vec3 v1 = (*vertices)[tri.y].position;
    glm::vec3 v2 = (*vertices)[tri.z].position;
    glm::vec3 edge1, edge2, h, s, q;
    float a, f, u, v;
    edge1 = v1-v0;
    edge2 = v2-v0;
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

bool rayAABBTest(AABB& aabb, glm::vec3 origin, glm::vec3 dir)
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

bool rayBVHTest(BVH bvh, glm::vec3 origin, glm::vec3 dir){
    std::queue<Node> nodes;
    nodes.push(bvh.nodes[0]);
    while (nodes.size() != 0){
        Node& currentNode = nodes.front();
        if(rayAABBTest(currentNode.aabb, origin, dir)){
            for (auto& idx : currentNode.triangles){
                if(rayTriangleTest(origin, dir, idx, &(bvh.verts), &(bvh.tris))) return true;
            }
            if (currentNode.left != -1) nodes.push(bvh.nodes[currentNode.left]);
            if (currentNode.right != -1) nodes.push(bvh.nodes[currentNode.right]);
        }
        nodes.pop();
    }
    return false;
}

glm::vec3 spherePoint(){
    for (int i = 0; i < 10; ++i){
        float x = 2.0*float(rand())/float(RAND_MAX)-1.0;
        float y = 2.0*float(rand())/float(RAND_MAX)-1.0;
        float z = 2.0*float(rand())/float(RAND_MAX)-1.0;
        glm::vec3 dir = glm::vec3(x, y, z);
        if (glm::length(dir) < 1.0)
        {
            glm::normalize(dir);
            return dir;
        }
    }
    return glm::vec3(0, 0, 0);
}

float rayTracing(std::vector<Vertex>* vertices, std::vector<Triangle>* triangles, Vertex& vtx, int samples){
    int sum = 0;
    for (int d = 0; d < samples; ++d){
        bool hitFound = false;
        auto hemidir = spherePoint();
        hemidir = glm::dot(hemidir, vtx.normal)<0?-hemidir:hemidir;
        for (int t = 0; t < triangles->size(); ++t){
            //auto hit = rayTriangleTest(vtx.position, hemidir, t, vertices, triangles);
            auto hit = rayTriangleTest(vtx.position, hemidir, t, vertices, triangles);
            hitFound |= hit;
            if (hit) break;
        }
        if (hitFound) ++sum;
    }
    return float(sum)/float(samples);
}

float rayTracingBVH(BVH bvh, std::vector<Triangle>* triangles, Vertex& vtx, int samples){
    int sum = 0;
    for (int d = 0; d < samples; ++d){
        auto hemidir = spherePoint();
        hemidir = glm::dot(hemidir, vtx.normal)<0?-hemidir:hemidir;
        if (rayBVHTest(bvh, vtx.position, hemidir)) ++sum;
    }
    return float(sum)/float(samples);
}


bool bvhAO(BVH& bvh, int samples){
    for(int i = 0; i < bvh.verts.size(); ++i){
        auto& vtx = bvh.verts[i];
        float value = rayTracingBVH(bvh, &(bvh.tris), vtx, samples);
        vtx.color = glm::vec4(1.0f-value, 1.0f-value, 1.0f-value, 1.0);
    }
    return true;
}


bool layerOutput(std::vector<Vertex>* vertices, std::vector<Triangle>* triangles) {
    for(int i = 0; i < vertices->size(); ++i){
        auto& vtx = (*vertices)[i];
        float value = rayTracing(vertices, triangles, vtx, 5);
        vtx.color = glm::vec4(1.0f-value, 1.0f-value, 1.0f-value, 1.0);
    }
    return true;
}