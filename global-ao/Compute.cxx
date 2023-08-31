#include "Compute.hxx"

AOCompute::AOCompute(bool useBVH_, const char* compPath)
{
    useBVH = useBVH_;
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

    // Impovised linking
    if (useBVH) shaderString.replace(shaderString.find("uint useBvh=")+12, 1, "1");

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

    glGenBuffers(1, &ssbo_perSample);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_perSample);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ssbo_perSample);

    if (useBVH){
        glGenBuffers(1, &ssbo_renderNodes);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_renderNodes);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, ssbo_renderNodes);

        glGenBuffers(1, &ssbo_perNodeTriIndices);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_perNodeTriIndices);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, ssbo_perNodeTriIndices);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

AOCompute::~AOCompute()
{
}

void AOCompute::run(BVH &bvh)
{
    size_t numIters = 1;
    GLint numSamples[3];

    glUseProgram(ID);

    glUniform1ui(glGetUniformLocation(ID, "num_verts"), bvh.verts_pos.size());

    glGetProgramiv(ID, GL_COMPUTE_WORK_GROUP_SIZE, numSamples);
    
    std::vector<uint32_t> aoOutput(bvh.verts_pos.size(), 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_aoOutput);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t)*aoOutput.size(), aoOutput.data(),  GL_DYNAMIC_COPY);

    std::vector<uint32_t> perSample((numSamples[1]/32)*bvh.verts_pos.size(), 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_perSample);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t)*perSample.size(), perSample.data(),  GL_DYNAMIC_COPY);
    
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

    if (useBVH){
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
    }
    

    // launch compute shaders!
    for (size_t i = 0; i < numIters; ++i){
        glUniform1ui(glGetUniformLocation(ID, "num_tris"), bvh.tris.size());
        //glDispatchCompute(bvh.verts_pos.size(), 1, 1);
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