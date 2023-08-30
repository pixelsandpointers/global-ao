#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "AOGenerator.hxx"
#include "BVH.hxx"

struct RenderNodeCompute{
    float aabb_min_x, aabb_min_y, aabb_min_z;
    float aabb_max_x, aabb_max_y, aabb_max_z;
    int left = -1;
    int right = -1;
    int startTriOffset = 0;
    int numTri = 0;
};

class AOCompute
{
private:
    GLuint ID;
    GLuint ssbo_vertices, ssbo_vertex_normals, ssbo_triangles, ssbo_aoOutput, ssbo_renderNodes, ssbo_perNodeTriIndices;
public:
    AOCompute(const char* compPath);
    ~AOCompute();
    void run(BVH &bvh, bool useBvh);
};

AOCompute::AOCompute(const char* compPath = "../../global-ao/shader/test.comp")
{
    std::string shaderString;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    int success;
    char infoLog[1024];
    try
    {
        file.open(compPath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        shaderString = buffer.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER - failed to read compute shader:\n" << e.what() << std::endl;
    }

    const char* cShaderCode = shaderString.c_str();
    GLuint compute;
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &cShaderCode, NULL);
    glCompileShader(compute);

    glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(compute, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER - failed to compile compute shader:\n" << infoLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);


    // create buffer
    glGenBuffers(1, &ssbo_vertices);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_vertices);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_vertices);

    glGenBuffers(1, &ssbo_vertex_normals);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_vertex_normals);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_vertex_normals);

    glGenBuffers(1, &ssbo_triangles);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_triangles);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo_triangles);

    glGenBuffers(1, &ssbo_aoOutput);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_aoOutput);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo_aoOutput);

    glGenBuffers(1, &ssbo_renderNodes);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_renderNodes);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ssbo_renderNodes);

    glGenBuffers(1, &ssbo_perNodeTriIndices);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_perNodeTriIndices);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, ssbo_perNodeTriIndices);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

AOCompute::~AOCompute()
{
}

bool rayTriangleTest(glm::vec3 origin, glm::vec3 direction, glm::uint index, bool backfaceCulling, glm::uint* triangles, float* vertex_pos){
    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    const float EPSILON = 0.0000001;
    glm::uvec3 tri = glm::uvec3(triangles[3*index+0], triangles[3*index+1], triangles[3*index+2]);
    glm::vec3 v0 = glm::vec3(vertex_pos[3*tri.x+0], vertex_pos[3*tri.x+1], vertex_pos[3*tri.x+2]);
    glm::vec3 v1 = glm::vec3(vertex_pos[3*tri.y+0], vertex_pos[3*tri.y+1], vertex_pos[3*tri.y+2]);
    glm::vec3 v2 = glm::vec3(vertex_pos[3*tri.z+0], vertex_pos[3*tri.z+1], vertex_pos[3*tri.z+2]);
    glm::vec3 edge1, edge2, h, s, q;
    float a, f, u, v;
    edge1 = v1-v0;
    edge2 = v2-v0;

    if (backfaceCulling){
        glm::vec3 normal = cross(edge1, edge2);
        if (dot(direction, normal) < 0.0) return false;
    }

    h = cross(direction, edge2);
    a = dot(edge1, h);

    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.

    f = 1.0 / a;
    s = origin - v0;
    u = f * dot(s, h);

    if (u < 0.0 || u > 1.0)
        return false;

    q = cross(s, edge1);
    v = f * dot(direction, q);

    if (v < 0.0 || u + v > 1.0)
        return false;

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * dot(edge2, q);

    if (t > EPSILON) // ray intersection
    {
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

bool rayAABBTest(float aabb_min_x, float aabb_min_y, float aabb_min_z, float aabb_max_x, float aabb_max_y, float aabb_max_z, glm::vec3 origin, glm::vec3 dir)
{
    // reference https://web.archive.org/web/20090803054252/http://tog.acm.org/resources/GraphicsGems/gems/RayBox.c
    bool inside = true;
	glm::uint quadrant[3];
	int whichPlane;
	float maxT[3];
	float candidatePlane[3];

    const int NUMDIM = 3;
    const int RIGHT = 0;
    const int LEFT = 1;
    const int MIDDLE = 2;

    const bool FALSE = false;
    const bool TRUE = true;

    glm::vec3 minB = glm::vec3(aabb_min_x, aabb_min_y, aabb_min_z);
    glm::vec3 maxB = glm::vec3(aabb_max_x, aabb_max_y, aabb_max_z);

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
			coord[i] = origin[i] + maxT[whichPlane] * dir[i];
			if (coord[i] < minB[i] || coord[i] > maxB[i])
				return (FALSE);
		} else {
			coord[i] = candidatePlane[i];
		}
	return (TRUE);// ray hits box
}


void AOCompute::run(BVH &bvh, bool useBvh)
{
    size_t numIters = 1;
    GLint numSamples[3];
    
    std::vector<uint32_t> aoOutput(bvh.verts_pos.size(), 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_aoOutput);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t)*aoOutput.size(), aoOutput.data(),  GL_DYNAMIC_COPY);
    
    std::vector<float> positons(3*bvh.verts_pos.size(), 0.0f);
    for (size_t i = 0; i < bvh.verts_pos.size(); ++i){
        positons[3*i+0] = bvh.verts_pos[i].x;
        positons[3*i+1] = bvh.verts_pos[i].y;
        positons[3*i+2] = bvh.verts_pos[i].z;
    } 
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_vertices);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float)*positons.size(), positons.data(),  GL_DYNAMIC_COPY);

    std::vector<float> normals(3*bvh.verts_pos.size(), 0.0f);
    for (size_t i = 0; i < bvh.verts_pos.size(); ++i){
        normals[3*i+0] = bvh.verts[i].normal.x;
        normals[3*i+1] = bvh.verts[i].normal.y;
        normals[3*i+2] = bvh.verts[i].normal.z;
    } 
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_vertex_normals);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float)*normals.size(), normals.data(),  GL_DYNAMIC_COPY);

    std::vector<glm::uint> triangles(3*bvh.tris.size(), 0);
    for (size_t i = 0; i < bvh.tris.size(); ++i){
        triangles[3*i+0] = bvh.tris[i].x;
        triangles[3*i+1] = bvh.tris[i].y;
        triangles[3*i+2] = bvh.tris[i].z;
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_triangles);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 3*sizeof(glm::uint)*bvh.tris.size(), triangles.data(), GL_DYNAMIC_COPY);

    // flatten Render Nodes
    std::vector<RenderNodeCompute> renderNodesCompute(bvh.render_nodes.size());
    for (size_t i = 0; i < bvh.render_nodes.size(); ++i){
        renderNodesCompute[i].aabb_min_x = bvh.render_nodes[i].aabb.min.x;
        renderNodesCompute[i].aabb_min_y = bvh.render_nodes[i].aabb.min.y;
        renderNodesCompute[i].aabb_min_z = bvh.render_nodes[i].aabb.min.z;
        renderNodesCompute[i].aabb_max_x = bvh.render_nodes[i].aabb.max.x;
        renderNodesCompute[i].aabb_max_y = bvh.render_nodes[i].aabb.max.y;
        renderNodesCompute[i].aabb_max_z = bvh.render_nodes[i].aabb.max.z;
        renderNodesCompute[i].left = bvh.render_nodes[i].left;
        renderNodesCompute[i].right = bvh.render_nodes[i].right;
        renderNodesCompute[i].startTriOffset = bvh.render_nodes[i].startTriOffset;
        renderNodesCompute[i].numTri = bvh.render_nodes[i].numTri;
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_renderNodes);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(RenderNodeCompute)*bvh.render_nodes.size(), bvh.render_nodes.data(),  GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_perNodeTriIndices);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t)*bvh.perNodeTriIndices.size(), bvh.perNodeTriIndices.data(),  GL_DYNAMIC_COPY);

       
    { // launch compute shaders!
    glUseProgram(ID);

    glUniform1ui(glGetUniformLocation(ID, "num_verts"), bvh.verts_pos.size());
    glUniform1ui(glGetUniformLocation(ID, "num_tris"), bvh.tris.size());
    glUniform1ui(glGetUniformLocation(ID, "useBvh"), glm::uint(useBvh));

    glGetProgramiv(ID, GL_COMPUTE_WORK_GROUP_SIZE, numSamples);

    for (size_t i = 0; i < numIters; ++i) glDispatchCompute(bvh.verts_pos.size(), 1, 1);
    }
   
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_aoOutput);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float)*aoOutput.size(), aoOutput.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    for (size_t i = 0; i < bvh.verts_pos.size(); ++i){
        float val = 1.0f-float(aoOutput[i])/float(numIters*numSamples[1]);
        bvh.verts[i].color = glm::vec4(val, val, val, 1.0f);
    } 
}