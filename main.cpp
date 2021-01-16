//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"

#include <iostream>

int glWindowWidth = 1200;
int glWindowHeight =800;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

glm::mat4 model; //
glm::mat4 model2; //
GLuint modelLoc;//
glm::mat4 view;//
GLuint viewLoc;//
glm::mat4 projection;//
GLuint projectionLoc;//
glm::mat3 normalMatrix;//
GLuint normalMatrixLoc;//
glm::mat4 lightRotation;

glm::vec3 lightDir;//
GLuint lightDirLoc;//
glm::vec3 lightColor;//
GLuint lightColorLoc;//

gps::Camera myCamera(
				glm::vec3(-19.0f, 4.0f, 55.0f), 
				glm::vec3(0.0f,0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.2f;

bool pressedKeys[1024];
float angleY = 0.0f;
float angleDoor = 0.0f;
float angleChair = 0.0f;
float angleRoof = 0.0f;
bool move_chair = false;
bool aprinde_lampa = false;
bool schimba_directia=false;
bool move_roof = false;
GLfloat lightAngle;

//gps::Model3D nanosuit;
//gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D teren;
gps::Model3D chair;
gps::Model3D lamp;
gps::Model3D bec;
gps::Model3D door;
gps::Model3D window;
gps::Model3D cabana;
gps::Model3D acoperis;
gps::Model3D perete;
gps::Model3D foc;
gps::Model3D baza;
gps::Model3D cloud;



gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;
gps::Shader transparencyShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

const int maxcloud = 10;
int cloudsGenerated = 0;
int j = 0;
std::pair <float, float> cloudPositions[30];
gps::Model3D clouds[30];
int i = 0;
int cntnori = 0;
int alpha = 0;
int beta = 0;

bool showDepthMap;
float lastX = 800.0f / 2.0;
float lastY = 800.0f / 2.0;
float pitch = 0.0f;
float yaw = -90.0f;
int firstMouse = 1;
float delta = 0;
float mouseSpeed = 0.1f;
int animatie = 0;
int animatie2 = 0;
GLuint fogDensityLoc;
GLuint fogLoc;
bool fog = false;
bool nori = false;

GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow * window, double xpos, double ypos) {

		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = 0;
		}
		else {
			double deltax = xpos - lastX;
			double deltay = ypos - lastY;

			myCamera.rotate(-1 * deltay * mouseSpeed, deltax * mouseSpeed);

			lastX = xpos;
			lastY = ypos;

		}

	}

void processMovement()
{
	if (pressedKeys[GLFW_KEY_ENTER]) {
		//cloudsGenerated = 0;
		nori = false;
	}

	if (pressedKeys[GLFW_KEY_SPACE]) {
		nori = true;
		alpha += 1;
		beta += 1;
	}
	if (pressedKeys[GLFW_KEY_Q]) {
		angleY -= 0.05f;		
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angleY += 0.05f;		
	}
	
	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;		
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);	
		//update view matrix
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);	
		//update view matrix
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
		//update view matrix
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
		//update view matrix
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}
	if (pressedKeys[GLFW_KEY_U]) {
		
			angleDoor = -1.5f;	
	}
	if (pressedKeys[GLFW_KEY_X]) {

		angleDoor = 0.0f;
	}
	if (pressedKeys[GLFW_KEY_C]) {
		move_chair = true;
		
	}
	if (pressedKeys[GLFW_KEY_O]) {
		aprinde_lampa = true;
		
	}
	if (pressedKeys[GLFW_KEY_P]) {
		aprinde_lampa = false;
		
	}
	if (pressedKeys[GLFW_KEY_V]) {
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (pressedKeys[GLFW_KEY_N]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_B]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	if (pressedKeys[GLFW_KEY_Z]) {
		move_chair = false;
	}
	if (pressedKeys[GLFW_KEY_T]) {
		move_roof = true;
	}
	if (pressedKeys[GLFW_KEY_Y]) {
		move_roof = false;
	}
	if (pressedKeys[GLFW_KEY_F]) {
		fog = true;
	}
	if (pressedKeys[GLFW_KEY_G]) {
		fog = false;
		
	}
	if (pressedKeys[GLFW_KEY_K]) {
		animatie = 1;
		animatie2 = 1;
	}
	
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //decomenteaz-o candva

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	
	teren.LoadModel("objects/cersipamant/mediu6.obj");
	lightCube.LoadModel("objects/cube/soare.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");
	chair.LoadModel("objects/chair/chair4.obj");
	lamp.LoadModel("objects/lamp/lamp2.obj");
	door.LoadModel("objects/door/door2.obj");
	window.LoadModel("objects/window/window3.obj");
	acoperis.LoadModel("objects/acoperis/acoperis2.obj");
	perete.LoadModel("objects/pereteFata/perete.obj");
	cabana.LoadModel("objects/cabana/cabana2.obj");
	baza.LoadModel("objects/cabana/baza.obj");
	bec.LoadModel("objects/lamp/bulb2.obj");
	foc.LoadModel("objects/foc/foc2.obj");
	cloud.LoadModel("objects/cloud/clouds2.obj");

}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthMapShader.loadShader("shaders/shadow.vert", "shaders/shadow.frag");
	depthMapShader.useShaderProgram();
	transparencyShader.loadShader("shaders/transparenta.vert", "shaders/transparenta.frag");
	transparencyShader.useShaderProgram();

}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(1.0f, 0.0f, 1.0f);//modific
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	myCustomShader.useShaderProgram();
	fogLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fog");
	//glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3fv(fogLoc, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
	//GLuint shadowMapFBO;
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);
	//GLuint depthMapTexture;
	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
	//TODO - Return the light-space transformation matrix
	glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = 0.1f, far_plane = 40.0f;
	glm::mat4 lightProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
	return lightSpaceTrMatrix;
}

void drawObjects(gps::Shader shader, bool depthPass) {
		
	shader.useShaderProgram();
	model = glm::mat4(1.0f);
	model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));

	
	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	//scaun
	model = glm::translate(model, glm::vec3(0.728f, 5.021f, 3.217f));
	model = glm::rotate(model, angleChair, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-0.728f, -5.021f, -3.217f));
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	chair.Draw(shader);


	//usa
	model = glm::mat4(1.0f);
	model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-13.77f, 2.6775f, 0.047f));
	model = glm::rotate(model, angleDoor, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(13.77f, -2.6775f, -0.047f));
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	door.Draw(shader);
	
	
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	if (!move_roof) {
		//acoperis
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-6.1997f, 13.688f, -1.284f));
		model = glm::rotate(model, angleRoof, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(6.1997f, -13.688f, 1.284f));
		acoperis.Draw(shader);
	}
		
	
	//baza
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	baza.Draw(shader);
	

	
	//cabana
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	cabana.Draw(shader);

	model = glm::mat4(1.0f);
	
	if (!aprinde_lampa) {

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::translate(model, 1.0f * lightDir);
		//model = glm::scale(model, glm::vec3(0.5f));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		bec.Draw(shader);
		lamp.Draw(shader);
	}

	//peretefata
	model = glm::mat4(1.0f);

	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	


	//perete.Draw(shader);

	//teren apa si ceru
	model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	teren.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	
	//transparencyShader.useShaderProgram();
	window.Draw(transparencyShader);



	model = glm::mat4(1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	
	foc.Draw(myCustomShader);


	if (nori) {
		if (cntnori < 50000) {
			
			
			myCustomShader.useShaderProgram();
			model = translate(glm::mat4(1.0f), glm::vec3(-(rand()%160), 1.0f, -(rand() % 160)));
			
			model = glm::translate(model, glm::vec3(2.0f, -1.0f, 3.0f));
			
			glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
			//std::cout << cloudsGenerated << "SA:";
			cloud.Draw(myCustomShader);
			cntnori++;
		}
		
	}
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	
	
}

void renderScene() {

	// depth maps creation pass
	//TODO - Send the light-space transformation matrix to the depth map creation shader and
	//		 render the scene in the depth map
	if (animatie == 1)
	{
		if (animatie2 == 1)
		{	
			
			myCamera.changeCameraPosition(glm::vec3(-12.0f, 10.0f, 64.0f));
			//std::cout << "x : " << myCamera.getCameraTarget().x << "y: " << myCamera.getCameraTarget().y << "z :" << myCamera.getCameraTarget().z;
			view = myCamera.getViewMatrix();
			myCustomShader.useShaderProgram();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			myCamera.move(gps::MOVE_LEFT, cameraSpeed * 0.8f);
			//update view matrix
			view = myCamera.getViewMatrix();
			myCustomShader.useShaderProgram();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			// compute normal matrix for teapot
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

			if (myCamera.getCameraTarget().x < -15)
			{
				animatie2 = 2;
				
			}
			
		}

		if (animatie2 == 2)
		{

			myCamera.move(gps::MOVE_RIGHT, cameraSpeed*0.8f);
			//update view matrix
			view = myCamera.getViewMatrix();
			myCustomShader.useShaderProgram();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			// compute normal matrix for teapot
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			if (myCamera.getCameraPosition().x > 5)
			{
				animatie2 = 3;
			}
		}
		if (animatie2 == 3)
		{
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed * 0.8f);
			//update view matrix
			view = myCamera.getViewMatrix();
			myCustomShader.useShaderProgram();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			// compute normal matrix for teapot
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			if (myCamera.getCameraPosition().y < 15)
			{
				animatie2 = 4;
			}
		}
		if (animatie2 == 4) {
			
			animatie2 = 0;
			animatie = 0;
		}
	}



	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	drawObjects(depthMapShader, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// render depth map on screen - toggled with the M key

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		// final scene rendering pass (with shadows)

		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCustomShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				
		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 1.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

		//draw a white cube around the light

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		
		model = lightRotation;
		model = glm::translate(model, glm::vec3(-10.0f, 1.0f, 7.0f));
		model = glm::translate(model, 1.0f * lightDir);
		//model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));


		lightCube.Draw(lightShader);

		
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, 1.0f * lightDir);
		//model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		//glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		if (aprinde_lampa) {
			model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
			//model = glm::translate(model, 1.0f * lightDir);
			//model = glm::scale(model, glm::vec3(0.5f));
			glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
			bec.Draw(lightShader);
			lamp.Draw(lightShader);
		}
		
	}
	if (move_chair) {
		if (!schimba_directia) {
			if (angleChair > -1.0f) {
				angleChair -= 0.001f;

			}
			schimba_directia = true;
		}
		else
			angleChair += 0.001f;
		
	}
	if (move_roof) {
		if (angleRoof > -2.0f) {
			angleRoof -= 0.1f;
		}
	}
	else
		angleRoof = 0.0f;

	
	if (fog) {
		myCustomShader.useShaderProgram();
		glUniform3fv(fogLoc, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
	}
	else {
		myCustomShader.useShaderProgram();
		glUniform3fv(fogLoc, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
	}
	
}

void cleanup() {
	glDeleteTextures(1,& depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();

	glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();		

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
