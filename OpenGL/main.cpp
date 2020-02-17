// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.h"
#include "texture.h"
#include "objloader.h"

#define WIDTH 1000
#define HEIGHT 1000

float theta = M_PI_2f32,phi=0;
float radius = 7;
std::vector<vec3> translation(10);

static void error_callback(int error, const char* description){
    fprintf(stderr, "Error: %s\n", description);
}

void computeInputs(){
	float mouseSpeed = 0.005f;

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);

	// Compute new orientation
	float verticalAngle = phi;
	theta += mouseSpeed * float(WIDTH/2 - xpos );
	verticalAngle   += mouseSpeed * float( HEIGHT/2 - ypos );
	if(verticalAngle>-1*M_PI_2f32 && verticalAngle<M_PI_2f32){
		phi = verticalAngle;
	}

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

void initPosition(){
	translation[0]= vec3(0.0f, -2.0f, 0.0f);
	translation[1]= vec3(0.0f, 1.0f, 0.0f);
	translation[2]= vec3(0.0f, 0.4f,  0.0f);
	translation[3]= vec3(0.0f, 0.2825f, 0.0f);
	translation[4]= vec3(1*cos(M_PI*0/3),-1.75f,1*sin(M_PI*0/3));
	translation[5]= vec3(1*cos(M_PI*1/3),-1.75f,1*sin(M_PI*1/3));
	translation[6]= vec3(1*cos(M_PI*2/3),-1.75f,1*sin(M_PI*2/3));
	translation[7]= vec3(1*cos(M_PI*3/3),-1.75f,1*sin(M_PI*3/3));
	translation[8]= vec3(1*cos(M_PI*4/3),-1.75f,1*sin(M_PI*4/3));
	translation[9]= vec3(1*cos(M_PI*5/3),-1.75f,1*sin(M_PI*5/3));
}

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwSetErrorCallback(error_callback);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( WIDTH, HEIGHT, "Tutorial 08 - Basic Shading", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "VertexShader.glsl", "FragmentShader.glsl" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Load the texture
	GLuint Texture = loadDDS("smiley.dds");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Store vertices,uvs,normal for all object types
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	// Read our plane.obj file
	std::vector<glm::vec3> planevertices;
	std::vector<glm::vec2> planeuvs;
	std::vector<glm::vec3> planenormals;
	bool res = loadOBJ("plane.obj", planevertices, planeuvs, planenormals);
	int planestart = 0;
	int planesize = planevertices.size();
	for(int i=0;i<planesize;i++){
		vertices.push_back(planevertices[i]);
		uvs.push_back(planeuvs[i]);
		normals.push_back(planenormals[i]);
	}
	// Read our sphere.obj file
	std::vector<glm::vec3> spherevertices;
	std::vector<glm::vec2> sphereuvs;
	std::vector<glm::vec3> spherenormals;
	res = loadOBJ("sphere.obj", spherevertices, sphereuvs, spherenormals);
	int spherestart = planesize;
	int spheresize = spherevertices.size();
	for(int i=0;i<spheresize;i++){
		vertices.push_back(spherevertices[i]);
		uvs.push_back(sphereuvs[i]);
		normals.push_back(spherenormals[i]);
	}
	// Read our sphere60.obj file
	std::vector<glm::vec3> sphere60vertices;
	std::vector<glm::vec2> sphere60uvs;
	std::vector<glm::vec3> sphere60normals;
	res = loadOBJ("sphere60.obj", sphere60vertices, sphere60uvs, sphere60normals);
	int sphere60start = planesize+spheresize;
	int sphere60size = sphere60vertices.size();
	for(int i=0;i<sphere60size;i++){
		vertices.push_back(sphere60vertices[i]);
		uvs.push_back(sphere60uvs[i]);
		normals.push_back(sphere60normals[i]);
	}

	// Load it into a VBO
	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()* sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	GLuint ColorID = glGetUniformLocation(programID, "Color");
	GLuint ObjectID = glGetUniformLocation(programID,"ID");
	GLuint SpecularID = glGetUniformLocation(programID,"Specular_component");
	GLuint DiffuseID = glGetUniformLocation(programID,"Diffuse_component");


	initPosition();


	do{

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		glm::vec3 camera_worldposition = glm::vec3(radius*cos(theta)*cos(phi),radius*sin(phi),radius*sin(theta)*cos(phi));

		glm::vec3 lightPos = camera_worldposition;
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// When MAGnifying the image (no bigger mipmap available), use LINEAR filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		// Generate mipmaps, by the way.
		glGenerateMipmap(GL_TEXTURE_2D);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);

		// Compute the MVP matrix
		glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
		glm::mat4 ViewMatrix = glm::lookAt(
								camera_worldposition, // Camera 
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						);

		// compute Inputs
		computeInputs();

		// draw the plane

		{
			// glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 RotationMatrix = rotate(mat4(1.0f),M_PI_2f32,vec3(0,1,0));
			glm::mat4 TranslationMatrix = translate(mat4(1.0f),translation[0]); // A bit to the right
			glm::mat4 ScalingMatrix = scale(mat4(1.0f), vec3(2.0f, 2.0f, 2.0f));
			glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			// 1.5f, 0.0f, 0.0f

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

			// Send Color and Object ID
			glm::vec3 color = glm::vec3(1,1,1);
			glUniform3f(ColorID,color.r, color.g, color.b);
			glUniform1i(ObjectID,0);

			//Send specular and diffuse values
			glUniform1f(SpecularID,0.0f);
			glUniform1f(DiffuseID,1.0f);

			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, planestart, planesize );
		}


		// the snowman
		{
			// glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 RotationMatrix = rotate(mat4(1.0f),M_PI_2f32,vec3(0,1,0));
			vec3 position = translation[0]+translation[1];
			glm::mat4 TranslationMatrix = translate(mat4(1.0f),position); // A bit to the right
			glm::mat4 ScalingMatrix = scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
			glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			// 1.5f, 0.0f, 0.0f

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

			// Send Color and Object ID
			glm::vec3 color = glm::vec3(1,1,1);
			glUniform3f(ColorID,color.r, color.g, color.b);
			glUniform1i(ObjectID,1);

			//Send specular and diffuse values
			glUniform1f(SpecularID,0.35f);
			glUniform1f(DiffuseID,0.65f);

			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, spherestart, spheresize );
		}
		{
			// glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 RotationMatrix = rotate(mat4(1.0f),M_PI_2f32,vec3(0,1,0));
			vec3 position = translation[0]+translation[1]+translation[1]+translation[2];
			glm::mat4 TranslationMatrix = translate(mat4(1.0f),position); // A bit to the right
			glm::mat4 ScalingMatrix = scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
			glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			// 1.5f, 0.0f, 0.0f

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

			// Send Color and Object ID
			glm::vec3 color = glm::vec3(1,1,1);
			glUniform3f(ColorID,color.r, color.g, color.b);
			glUniform1i(ObjectID,2);

			//Send specular and diffuse values
			glUniform1f(SpecularID,0.35f);
			glUniform1f(DiffuseID,0.65f);

			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, sphere60start, sphere60size );
		}
		{
			// glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 RotationMatrix = rotate(mat4(1.0f),M_PI_2f32,vec3(0,1,0));
			vec3 position = translation[0]+translation[1]+translation[1]+translation[2]+translation[2]+translation[3];
			glm::mat4 TranslationMatrix = translate(mat4(1.0f),position); // A bit to the right
			glm::mat4 ScalingMatrix = scale(mat4(1.0f), vec3(0.25f, 0.25f, 0.25f));
			glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			// 1.5f, 0.0f, 0.0f

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

			// Send Color and Object ID
			glm::vec3 color = glm::vec3(1,1,1);
			glUniform3f(ColorID,color.r, color.g, color.b);
			glUniform1i(ObjectID,3);

			//Send specular and diffuse values
			glUniform1f(SpecularID,0.35f);
			glUniform1f(DiffuseID,0.65f);

			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, spherestart, spheresize );
		}

		//draw orbs
		{
			// glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 RotationMatrix = rotate(mat4(1.0f),M_PI_2f32*2 - ((M_PI_2f32*2*0)/3),vec3(0,1,0));
			vec3 position = translation[4];
			glm::mat4 TranslationMatrix = translate(mat4(1.0f),position); // A bit to the right
			glm::mat4 ScalingMatrix = scale(mat4(1.0f), vec3(0.25f, 0.25f, 0.25f));
			glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			// 1.5f, 0.0f, 0.0f

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

			// Send Color and Object ID
			glm::vec3 color = glm::vec3(0,1,1);
			glUniform3f(ColorID,color.r, color.g, color.b);
			glUniform1i(ObjectID,4);

			//Send specular and diffuse values
			glUniform1f(SpecularID,0.75f);
			glUniform1f(DiffuseID,0.25f);

			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, spherestart, spheresize );
		}
		{
			// glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 RotationMatrix = rotate(mat4(1.0f),M_PI_2f32*2 - ((M_PI_2f32*2*1)/3),vec3(0,1,0));
			vec3 position = translation[5];
			glm::mat4 TranslationMatrix = translate(mat4(1.0f),position); // A bit to the right
			glm::mat4 ScalingMatrix = scale(mat4(1.0f), vec3(0.25f, 0.25f, 0.25f));
			glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			// 1.5f, 0.0f, 0.0f

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

			// Send Color and Object ID
			glm::vec3 color = glm::vec3(1,0.5,0);
			glUniform3f(ColorID,color.r, color.g, color.b);
			glUniform1i(ObjectID,5);

			//Send specular and diffuse values
			glUniform1f(SpecularID,0.75f);
			glUniform1f(DiffuseID,0.25f);

			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, spherestart, spheresize );
		}
		{
			// glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 RotationMatrix = rotate(mat4(1.0f),M_PI_2f32*2 - ((M_PI_2f32*2*2)/3),vec3(0,1,0));
			vec3 position = translation[6];
			glm::mat4 TranslationMatrix = translate(mat4(1.0f),position); // A bit to the right
			glm::mat4 ScalingMatrix = scale(mat4(1.0f), vec3(0.25f, 0.25f, 0.25f));
			glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			// 1.5f, 0.0f, 0.0f

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

			// Send Color and Object ID
			glm::vec3 color = glm::vec3(1,0,1);
			glUniform3f(ColorID,color.r, color.g, color.b);
			glUniform1i(ObjectID,6);

			//Send specular and diffuse values
			glUniform1f(SpecularID,0.75f);
			glUniform1f(DiffuseID,0.25f);

			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, spherestart, spheresize );
		}
		{
			// glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 RotationMatrix = rotate(mat4(1.0f),M_PI_2f32*2 - ((M_PI_2f32*2*3)/3),vec3(0,1,0));
			vec3 position = translation[7];
			glm::mat4 TranslationMatrix = translate(mat4(1.0f),position); // A bit to the right
			glm::mat4 ScalingMatrix = scale(mat4(1.0f), vec3(0.25f, 0.25f, 0.25f));
			glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			// 1.5f, 0.0f, 0.0f

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

			// Send Color and Object ID
			glm::vec3 color = glm::vec3(0,1,1);
			glUniform3f(ColorID,color.r, color.g, color.b);
			glUniform1i(ObjectID,7);

			//Send specular and diffuse values
			glUniform1f(SpecularID,0.75f);
			glUniform1f(DiffuseID,0.25f);

			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, spherestart, spheresize );
		}
		{
			// glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 RotationMatrix = rotate(mat4(1.0f),M_PI_2f32*2 - ((M_PI_2f32*2*4)/3),vec3(0,1,0));
			vec3 position = translation[8];
			glm::mat4 TranslationMatrix = translate(mat4(1.0f),position); // A bit to the right
			glm::mat4 ScalingMatrix = scale(mat4(1.0f), vec3(0.25f, 0.25f, 0.25f));
			glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			// 1.5f, 0.0f, 0.0f

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

			// Send Color and Object ID
			glm::vec3 color = glm::vec3(1,0.5,0);
			glUniform3f(ColorID,color.r, color.g, color.b);
			glUniform1i(ObjectID,8);

			//Send specular and diffuse values
			glUniform1f(SpecularID,0.75f);
			glUniform1f(DiffuseID,0.25f);

			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, spherestart, spheresize );
		}
		{
			// glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 RotationMatrix = rotate(mat4(1.0f),M_PI_2f32*2 - ((M_PI_2f32*2*5)/3),vec3(0,1,0));
			vec3 position = translation[9];
			glm::mat4 TranslationMatrix = translate(mat4(1.0f),position); // A bit to the right
			glm::mat4 ScalingMatrix = scale(mat4(1.0f), vec3(0.25f, 0.25f, 0.25f));
			glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			// 1.5f, 0.0f, 0.0f

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

			// Send Color and Object ID
			glm::vec3 color = glm::vec3(1,0,1);
			glUniform3f(ColorID,color.r, color.g, color.b);
			glUniform1i(ObjectID,9);

			//Send specular and diffuse values
			glUniform1f(SpecularID,0.75f);
			glUniform1f(DiffuseID,0.25f);

			// Draw the triangles !
			glDrawArrays(GL_TRIANGLES, spherestart, spheresize );
		}
		

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

