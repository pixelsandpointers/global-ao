#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "ShaderProgram.hxx"
#include "Camera.hxx"
#include "TriangleMesh.hxx"
#include "AOGenerator.hxx"

#include <chrono>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, Camera* camera, TriangleMesh* mesh)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// move camera
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->move(glm::vec3(-1.0, 0.0, 0.0));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->move(glm::vec3(1.0, 0.0, 0.0));
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->move(glm::vec3(0.0, 1.0, 0.0));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->move(glm::vec3(0.0, -1.0, 0.0));
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera->move(glm::vec3(0.0, 0.0, 1.0));
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera->move(glm::vec3(0.0, 0.0, -1.0));

	// rotate object
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		mesh->rotate(glm::vec3(0.0, 1.0, 0.0));
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		mesh->rotate(glm::vec3(0.0, -1.0, 0.0));
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		mesh->rotate(glm::vec3(1.0, 0.0, 0.0));
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		mesh->rotate(glm::vec3(-1.0, 0.0, 0.0));
}

int main() {
	// setup GLFW window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(800, 800, "Global Ambient Occlusion", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "ERROR::GLFW - failed to create window\n" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// setup GLAD
	if (!gladLoadGL(glfwGetProcAddress))
	{
		std::cout << "ERROR::GLAD - failed to initialize GLAD\n" << std::endl;
		return -1;
	}

	// setup viewport
	glViewport(0, 0, 800, 800);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// setup
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	ShaderProgram program("../../../global-ao/shader/test.vert", "../../../global-ao/shader/test.frag");
	Camera camera(glm::vec3(-0.025, 0.1, 0.25), glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0));
	
	// Mesh
	TriangleMesh bunny("../../../global-ao/resource/bunny.txt");

	auto start = std::chrono::steady_clock::now();
	BVH bvh = BVH(bunny.getVertices(), bunny.getIndices());
	bvh.buildManager(true);
	auto endBVH = std::chrono::steady_clock::now();
	//bvhAOFlat(bvh, 1);
	auto AOGen = AOGenerator(&bvh);
	AOGen.bake(15);
	bunny.setVertices(bvh.verts);
	bunny.setIndices(bvh.tris);
	auto stop = std::chrono::steady_clock::now();
	std::cout << "BVH AO completed in: " << std::chrono::duration<float, std::milli>(stop - start).count() << "ms "
	 << "BVH: " << std::chrono::duration<float, std::milli>(endBVH - start).count() << "ms "
	 << "Render: " << std::chrono::duration<float, std::milli>(stop - endBVH).count() << "ms\n";
	// BVH raytracing 15smp ~1324.9ms [RelWithDebug]
	bunny.update();

	program.use();
	program.setMat4("modelMatrix", bunny.getModelMatrix());
	program.setMat4("viewMatrix", camera.getViewMatrix());
	program.setMat4("projectionMatrix", glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f));

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window, &camera, &bunny);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		program.use();
		program.setMat4("modelMatrix", bunny.getModelMatrix());
		program.setMat4("viewMatrix", camera.getViewMatrix());
		bunny.draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
