#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camera.h"



const unsigned int width = 800;
const unsigned int height = 800;



// Vertices coordinates
GLfloat vertices[] =
{ //     COORDINATES     /        COLORS      /   TexCoord  //
	    -0.5f,-0.5f,-0.5f,  1.0f,0.0f,0.0f,  0.0f,0.0f, // 0
		 0.5f,-0.5f,-0.5f,  0.0f,1.0f,0.0f,  1.0f,0.0f, // 1
		 0.5f, 0.5f,-0.5f,  0.0f,0.0f,1.0f,  1.0f,1.0f, // 2
		-0.5f, 0.5f,-0.5f,  1.0f,1.0f,0.0f,  0.0f,1.0f, // 3
		-0.5f,-0.5f, 0.5f,  1.0f,0.0f,1.0f,  0.0f,0.0f, // 4
		 0.5f,-0.5f, 0.5f,  0.0f,1.0f,1.0f,  1.0f,0.0f, // 5
		 0.5f, 0.5f, 0.5f,  1.0f,1.0f,1.0f,  1.0f,1.0f, // 6
		-0.5f, 0.5f, 0.5f,  0.0f,0.0f,0.0f,  0.0f,1.0f  // 7
};

// Indices for vertices order
GLuint indices[] =
{
	// back face
	0,1,2, 2,3,0,
	// front face
	4,5,6, 6,7,4,
	// left face
	4,0,3, 3,7,4,
	// right face
	1,5,6, 6,2,1,
	// bottom face
	4,5,1, 1,0,4,
	// top face
	3,2,6, 6,7,3
};


int main()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "YoutubeOpenGL", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	glViewport(0, 0, width, height);


	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");



	// Generates Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();

	
	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(vertices, sizeof(vertices));
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indices, sizeof(indices));

	// Links VBO attributes such as coordinates and colors to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();



	

	// Texture
	Texture brickTex("container.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	brickTex.texUnit(shaderProgram, "tex0", 0);



	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		// Handles camera inputs (still allow manual input if desired)
		camera.Inputs(window);

		// Move camera along a cubic Bezier curve over time and upload camMatrix
		// Control points (ajustables)
		glm::vec3 p0 = glm::vec3(0.0f, 0.0f, 3.0f); //Initial position
		glm::vec3 p1 = glm::vec3(2.0f, 1.5f, 3.0f);//First control point
		glm::vec3 p2 = glm::vec3(-2.0f, 1.5f, 3.0f);//Second control point
		glm::vec3 p3 = glm::vec3(0.0f, 0.0f, -3.0f);//Final position

		float duration = 6.0f; // Seconds for a full loop
		float t = fmod(glfwGetTime() / duration, 1.0f);// Gives back time since the program started in seconds, divided by duration to get a value that loops every 'duration' seconds, and fmod to wrap it between 0 and 1
		float u = 1.0f - t;// u is the complement of t, used in the Bezier formula
		glm::vec3 camPos = u * u * u * p0 + 3.0f * u * u * t * p1 + 3.0f * u * t * t * p2 + t * t * t * p3;	// Cubic Bezier formula to calculate camera position based on control points and time

		glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);// The point the camera is looking at (the center of the scene)
		glm::mat4 view = glm::lookAt(camPos, target, glm::vec3(0.0f, 1.0f, 0.0f));	// View matrix calculated with the camera position, target, and up vector
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);// Projection matrix with a 45 degree field of view, aspect ratio based on window size, and near/far planes
		glm::mat4 camMatrix = proj * view;// Final camera matrix is the product of projection and view matrices

		shaderProgram.Activate();
		GLint camLoc = glGetUniformLocation(shaderProgram.ID, "camMatrix");// Get the location of the "camMatrix" uniform in the shader program
		glUniformMatrix4fv(camLoc, 1, GL_FALSE, &camMatrix[0][0]);// Upload the camera matrix to the shader

		// Binds texture so that is appears in rendering
		brickTex.Bind();
		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	brickTex.Delete();
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}