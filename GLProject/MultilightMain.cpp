//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <vector>
//#include "tools/Polygon.h"
//#include "tools/Cube.h"
//#include <learnopengl/shader_m.h>
//#include <learnopengl/camera.h>
//#include "stb_image.h"
//#include <random> 
//#include <chrono>
//
//using namespace std;
//using namespace glm;
//
//int SCR_WIDTH = 1280;
//int SCR_HEIGHT = 720;
//
//Camera camera(vec3(0.0f, 0.0f, 4.0f));
//float lastX = SCR_WIDTH / 2.0f;
//float lastY = SCR_HEIGHT / 2.0f;
//bool firstMouse = true;
//
//float deltaTime = 0.0f;
//float lastFrame = 0.0f;
//
//vec3 firstLight(0.0f, -1.5f, 0.0f);
//vec3 lightPos(0.0f, -1.5f, 0.0f);
//
//bool blinn = false;
//
//
//void processInput(GLFWwindow* window);
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//
//vector<GLuint> loadTextures(vector<string> paths, GLuint wrapOption = GL_REPEAT, GLuint filterOption = GL_LINEAR) {
//	vector<GLuint> textures = {};
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapOption);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapOption);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterOption);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterOption);
//
//	for (string path : paths)
//	{
//		unsigned int texture;
//		glGenTextures(1, &texture);
//		glBindTexture(GL_TEXTURE_2D, texture);
//		int width, height, nrChannels;
//		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
//
//
//		if (data)
//		{
//			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//			glGenerateMipmap(GL_TEXTURE_2D);
//		}
//		else
//			std::cout << "Failed to load texture" << std::endl;
//		stbi_image_free(data);
//
//		textures.push_back(texture);
//	}
//
//	return textures;
//}
//
//int main()
//{
//	glfwInit();
//	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lecture 5", NULL, NULL);
//	glfwMakeContextCurrent(window);
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//	glfwSetCursorPosCallback(window, mouse_callback);
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
//
//	Shader allShader("./shaders/vs/L5.vs", "./shaders/fs/L5-Multi.fs");
//	Shader lightSourceShader("./shaders/vs/LightSource.vs", "./shaders/fs/LightSource.fs");
//
//	glEnable(GL_DEPTH_TEST);
//
//	stbi_set_flip_vertically_on_load(true);
//
//	vec3 pointLightPositions[] = {
//		glm::vec3(0.7f,  0.2f,  2.0f),
//		glm::vec3(2.3f, -3.3f, -4.0f),
//		glm::vec3(-4.0f,  2.0f, -12.0f),
//		glm::vec3(0.0f,  0.0f, -3.0f)
//	};
//	glm::vec3 pointLightColors[] = {
//	glm::vec3(0.1f, 0.1f, 0.1f),
//	glm::vec3(0.1f, 0.1f, 0.1f),
//	glm::vec3(0.1f, 0.1f, 0.1f),
//	glm::vec3(0.3f, 0.1f, 0.1f)
//	};
//
//	vector<string> texturePaths = {};
//	texturePaths.push_back("./textures/white.jpg");
//	vector<GLuint> textures = loadTextures(texturePaths);
//
//	std::random_device rd;
//	std::mt19937 generator(rd() ^ std::chrono::high_resolution_clock::now().time_since_epoch().count());
//	int min_val = -3;
//	int max_val = 3;
//	std::uniform_real_distribution<> distribution(min_val, max_val);
//
//	vector<Cube> cubes = {};
//	for (int i = 0; i < 8; i++) {
//		float random_numberX = (float)distribution(generator);
//		float random_numberY = (float)distribution(generator);
//		float random_numberZ = (float)distribution(generator);
//		Cube temp(vec3(random_numberX, random_numberY, random_numberZ), 1.0f, vec3(abs(random_numberX / 3.0f), abs(random_numberY / 3.0f), abs(random_numberZ / 3.0f)));
//		cubes.push_back(temp);
//	}
//	 
//	while (!glfwWindowShouldClose(window))
//	{
//		allShader.use();
//		allShader.setVec3("viewPos", camera.Position);
//
//		#pragma region Light Settings
//		// Directional light
//		allShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
//		allShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
//		allShader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.05f);
//		allShader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);
//		// Point light 1
//		allShader.setVec3("pointLights[0].position", pointLightPositions[0]);
//		allShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1f, pointLightColors[0].y * 0.1f, pointLightColors[0].z * 0.1f);
//		allShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
//		allShader.setVec3("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
//		allShader.setFloat("pointLights[0].constant", 1.0f);
//		allShader.setFloat("pointLights[0].linear", 0.14f);
//		allShader.setFloat("pointLights[0].quadratic", 0.07f);
//		// Point light 2
//		allShader.setVec3("pointLights[1].position", pointLightPositions[1]);
//		allShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1f, pointLightColors[1].y * 0.1f, pointLightColors[1].z * 0.1f);
//		allShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
//		allShader.setVec3("pointLights[1].specular", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
//		allShader.setFloat("pointLights[1].constant", 1.0f);
//		allShader.setFloat("pointLights[1].linear", 0.14f);
//		allShader.setFloat("pointLights[1].quadratic", 0.07f);
//		// Point light 3
//		allShader.setVec3("pointLights[2].position", pointLightPositions[2]);
//		allShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1f, pointLightColors[2].y * 0.1f, pointLightColors[2].z * 0.1f);
//		allShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
//		allShader.setVec3("pointLights[2].specular", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
//		allShader.setFloat("pointLights[2].constant", 1.0f);
//		allShader.setFloat("pointLights[2].linear", 0.22f);
//		allShader.setFloat("pointLights[2].quadratic", 0.20f);
//		// Point light 4
//		allShader.setVec3("pointLights[3].position", pointLightPositions[3]);
//		allShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1f, pointLightColors[3].y * 0.1f, pointLightColors[3].z * 0.1f);
//		allShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
//		allShader.setVec3("pointLights[3].specular", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
//		allShader.setFloat("pointLights[3].constant", 1.0f);
//		allShader.setFloat("pointLights[3].linear", 0.14f);
//		allShader.setFloat("pointLights[3].quadratic", 0.07f);
//		// SpotLight
//		allShader.setVec3("spotLight.position", camera.Position);
//		allShader.setVec3("spotLight.direction", camera.Front);
//		allShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
//		allShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
//		allShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
//		allShader.setFloat("spotLight.constant", 1.0f);
//		allShader.setFloat("spotLight.linear", 0.09f);
//		allShader.setFloat("spotLight.quadratic", 0.032f);
//		allShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
//		allShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
//#pragma endregion
//
//		float currentFrame = static_cast<float>(glfwGetTime());
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//		float time = (float)glfwGetTime();
//
//		processInput(window);
//
//		glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
//		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		mat4 projection = mat4(1.0f);
//		projection = perspective(radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//		allShader.setMat4("projection", projection);
//
//		mat4 view = mat4(1.0f);
//		view = view = camera.GetViewMatrix();
//		allShader.setMat4("view", view);
//		allShader.setVec3("lightPos", lightPos);
//
//
//		for (int i = 0; i < 8; i++) {
//			cubes[i].draw(allShader);
//		}
//
//		lightSourceShader.use();
//		lightSourceShader.setMat4("projection", projection);
//		lightSourceShader.setMat4("view", view);
//
//		// we now draw as many light bulbs as we have point lights.
//		mat4 model = glm::mat4(1.0f);
//		for (unsigned int i = 0; i < 4; i++)
//		{
//			model = glm::mat4(1.0f);
//			model = glm::translate(model, pointLightPositions[i]);
//			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
//			lightSourceShader.setMat4("model", model);
//			Cube LightSource(firstLight, 0.2f, pointLightColors[i]);
//			LightSource.transformation(model);
//			LightSource.draw(lightSourceShader);
//		}
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	glfwTerminate();
//	return 0;
//}
//
//void processInput(GLFWwindow* window)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		camera.ProcessKeyboard(FORWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		camera.ProcessKeyboard(BACKWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		camera.ProcessKeyboard(LEFT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		camera.ProcessKeyboard(RIGHT, deltaTime);
//}
//
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//{
//	float xpos = static_cast<float>(xposIn);
//	float ypos = static_cast<float>(yposIn);
//
//	if (firstMouse)
//	{
//		lastX = xpos;
//		lastY = ypos;
//		firstMouse = false;
//	}
//
//	float xoffset = xpos - lastX;
//	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//	lastX = xpos;
//	lastY = ypos;
//
//	camera.ProcessMouseMovement(xoffset, yoffset);
//}
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//	glViewport(0, 0, width, height);
//}
