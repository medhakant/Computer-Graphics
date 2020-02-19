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
#include <sstream> 
using namespace glm;

#include "shader.h"
#include "texture.h"
#include "objloader.h"

#define WIDTH 1000
#define HEIGHT 1000

float theta = M_PI_2f32,phi=0;
float radius = 7;
std::vector<vec3> translation(10);
bool lmouse_button = false;
bool shift_button = false;
int selection = -1;
glm::mat4 ProjectionMatrix;
glm::mat4 ViewMatrix;
glm::vec3 camera_worldposition;

static void error_callback(int error, const char* description){
    fprintf(stderr, "Error: %s\n", description);
}

//Handle the inputs and perform the tranformation at each frame
void computeInputs(){
	static bool pressed_down = false;
	static double lastTime;
	static double lastX;
	static double lastY;
	if(!lmouse_button){
		pressed_down = false;
	}else{
		if(!pressed_down){
			pressed_down = true;
			lastTime = glfwGetTime();
			glfwGetCursorPos(window, &lastX, &lastY);
		}
	}
	
	if(lmouse_button){
		float mouseSpeed = 0.005f;
		// Compute time difference between current and last frame
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);

		// Get mouse position
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		// Reset mouse position for next frame
		// glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);

		if(selection==-1){
			if(!shift_button){
				// Compute new orientation
				float verticalAngle = phi;
				theta -= mouseSpeed * float(lastX - xpos );
				verticalAngle   -= mouseSpeed * float(lastY - ypos );
				if(verticalAngle>-1*M_PI_2f32 && verticalAngle<M_PI_2f32){
					phi = verticalAngle;
				}
			}else{
				radius -= float(lastY - ypos)*0.05;
			}
		}else if(selection==1){
			vec4 transl = vec4((lastX-xpos),(lastY-ypos),0,1);
			mat4 rotation = glm::lookAt(
								glm::vec3(0,0,0),
								camera_worldposition, // Camera
								glm::vec3(0,1,0)
						);
			vec4 transf = (transl*rotation);
			vec3 newpos = translation[1] + vec3(transf.x*0.007,transf.y*0.007,transf.z*0.007);
			float magnitude = sqrt(pow(newpos.x,2)+pow(newpos.y,2)+pow(newpos.z,2));
			if(magnitude>0){
				translation[1] = newpos/magnitude;
			}
			
		}else if(selection==2){
			vec4 transl = vec4((lastX-xpos),(lastY-ypos),0,1);
			mat4 rotation = glm::lookAt(
								glm::vec3(0,0,0),
								camera_worldposition, // Camera
								glm::vec3(0,1,0)
						);
			vec4 transf = (transl*rotation);
			vec3 newpos = translation[2] + vec3(transf.x*0.05,transf.y*0.05,transf.z*0.05);
			float magnitude = sqrt(pow(newpos.x,2)+pow(newpos.y,2)+pow(newpos.z,2));
			if(magnitude>0){
				translation[2] = (newpos/magnitude)*1.4f;
			}
			
		}else if(selection==3){
			vec4 transl = vec4((lastX-xpos),(lastY-ypos),0,1);
			mat4 rotation = glm::lookAt(
								glm::vec3(0,0,0),
								camera_worldposition, // Camera
								glm::vec3(0,1,0)
						);
			vec4 transf = (transl*rotation);
			vec3 newpos = translation[3] + vec3(transf.x*0.05,transf.y*0.05,transf.z*0.05);
			float magnitude = sqrt(pow(newpos.x,2)+pow(newpos.y,2)+pow(newpos.z,2));
			if(magnitude>0){
				translation[3] = (newpos/magnitude)*0.6825f;
			}			
		}else if (selection!=0){
			vec4 transl = vec4(lastX-xpos,lastY-ypos,0,1);
			mat4 rotation = glm::lookAt(
								glm::vec3(0,0,0),
								camera_worldposition, // Camera
								glm::vec3(0,1,0)
						);
			vec4 transf = (transl*rotation);
			vec3 newpos = vec3(transf.x*0.005,transf.y*0.005,transf.z*0.005);
			translation[selection] += newpos;
			
		}

		// For the next frame, the "last time" will be "now"
		lastTime = currentTime;
		lastX = xpos;
		lastY = ypos;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		lmouse_button = true;
	}else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
		lmouse_button = false;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS){
	   	shift_button = true;
   	}else if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE){
		shift_button = false;
	}
}

void initPosition(){
	translation[0]= vec3(0.0f, -2.0f, 0.0f);
	translation[1]= vec3(0.0f, 1.0f, 0.0f);
	translation[2]= vec3(0.0f, 1.4f,  0.0f);
	translation[3]= vec3(0.0f, 0.6825f, 0.0f);
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
	window = glfwCreateWindow( WIDTH, HEIGHT, "OpenGL Assignment 2 (COL781)", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window.\n" );
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
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	glfwSetKeyCallback(window,key_callback);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();

    // glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);

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
	GLuint programID = LoadShaders( "shaders/VertexShader.glsl", "shaders/FragmentShader.glsl" );
	GLuint pickingProgramID = LoadShaders( "shaders/VertexShaderSelection.glsl", "shaders/FragmentShaderSelection.glsl" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	GLuint PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");

	// Load the texture
	GLuint Texture1 = loadDDS("textures/smiley.dds");
	GLuint Texture2 = loadDDS("textures/snow.dds");
	
	
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
	bool res = loadOBJ("objects/plane.obj", planevertices, planeuvs, planenormals);
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
	res = loadOBJ("objects/sphere.obj", spherevertices, sphereuvs, spherenormals);
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
	res = loadOBJ("objects/sphere60.obj", sphere60vertices, sphere60uvs, sphere60normals);
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

	// Get a handle for our "pickingColorID" uniform
	GLuint pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");

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


		camera_worldposition = glm::vec3(radius*cos(theta)*cos(phi),radius*sin(phi),radius*sin(theta)*cos(phi));
		// Compute the MVP matrix
		ProjectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
		ViewMatrix = glm::lookAt(
								camera_worldposition, // Camera 
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						);
		

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) && !lmouse_button){

			// Clear the screen in white
			glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(pickingProgramID);

			// Only the positions are needed (not the UVs and normals)
			glEnableVertexAttribArray(0);

			// draw the plane

			{
				// glm::mat4 ModelMatrix = glm::mat4(1.0);
				glm::mat4 RotationMatrix = rotate(mat4(1.0f),M_PI_2f32,vec3(0,1,0));
				glm::mat4 TranslationMatrix = translate(mat4(1.0f),translation[0]); // A bit to the right
				glm::mat4 ScalingMatrix = scale(mat4(1.0f), vec3(2.0f, 2.0f, 2.0f));
				glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
				glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

				glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

				// Convert "i", the integer mesh ID, into an RGB color
				int r = (0 & 0x000000FF) >>  0;
				int g = (0 & 0x0000FF00) >>  8;
				int b = (0 & 0x00FF0000) >> 16;

				// OpenGL expects colors to be in [0,1], so divide by 255.
				glUniform4f(pickingColorID, r/255.0f, g/255.0f, b/255.0f, 1.0f);

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

				glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

				// Convert "i", the integer mesh ID, into an RGB color
				int r = (1 & 0x000000FF) >>  0;
				int g = (1 & 0x0000FF00) >>  8;
				int b = (1 & 0x00FF0000) >> 16;

				// OpenGL expects colors to be in [0,1], so divide by 255.
				glUniform4f(pickingColorID, r/255.0f, g/255.0f, b/255.0f, 1.0f);

				// Draw the triangles !
				glDrawArrays(GL_TRIANGLES, spherestart, spheresize );
			}
			{
				// glm::mat4 ModelMatrix = glm::mat4(1.0);
				glm::mat4 RotationMatrix = rotate(mat4(1.0f),M_PI_2f32,vec3(0,1,0));
				vec3 position = translation[0]+translation[1]+translation[2];
				glm::mat4 TranslationMatrix = translate(mat4(1.0f),position); // A bit to the right
				glm::mat4 ScalingMatrix = scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
				glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
				glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

				glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

				// Convert "i", the integer mesh ID, into an RGB color
				int r = (2 & 0x000000FF) >>  0;
				int g = (2 & 0x0000FF00) >>  8;
				int b = (2 & 0x00FF0000) >> 16;

				// OpenGL expects colors to be in [0,1], so divide by 255.
				glUniform4f(pickingColorID, r/255.0f, g/255.0f, b/255.0f, 1.0f);

				// Draw the triangles !
				glDrawArrays(GL_TRIANGLES, sphere60start, sphere60size );
			}

			{
				// glm::mat4 ModelMatrix = glm::mat4(1.0);
				glm::mat4 RotationMatrix = rotate(mat4(1.0f),M_PI_2f32,vec3(0,1,0));
				vec3 position = translation[0]+translation[1]+translation[2]+translation[3];
				glm::mat4 TranslationMatrix = translate(mat4(1.0f),position); // A bit to the right
				glm::mat4 ScalingMatrix = scale(mat4(1.0f), vec3(0.25f, 0.25f, 0.25f));
				glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
				glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

				glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

				// Convert "i", the integer mesh ID, into an RGB color
				int r = (3 & 0x000000FF) >>  0;
				int g = (3 & 0x0000FF00) >>  8;
				int b = (3 & 0x00FF0000) >> 16;

				// OpenGL expects colors to be in [0,1], so divide by 255.
				glUniform4f(pickingColorID, r/255.0f, g/255.0f, b/255.0f, 1.0f);

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

				glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

				// Convert "i", the integer mesh ID, into an RGB color
				int r = (4 & 0x000000FF) >>  0;
				int g = (4 & 0x0000FF00) >>  8;
				int b = (4 & 0x00FF0000) >> 16;

				// OpenGL expects colors to be in [0,1], so divide by 255.
				glUniform4f(pickingColorID, r/255.0f, g/255.0f, b/255.0f, 1.0f);
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
				
				glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

				// Convert "i", the integer mesh ID, into an RGB color
				int r = (5 & 0x000000FF) >>  0;
				int g = (5 & 0x0000FF00) >>  8;
				int b = (5 & 0x00FF0000) >> 16;

				// OpenGL expects colors to be in [0,1], so divide by 255.
				glUniform4f(pickingColorID, r/255.0f, g/255.0f, b/255.0f, 1.0f);

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

				glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

				// Convert "i", the integer mesh ID, into an RGB color
				int r = (6 & 0x000000FF) >>  0;
				int g = (6 & 0x0000FF00) >>  8;
				int b = (6 & 0x00FF0000) >> 16;

				// OpenGL expects colors to be in [0,1], so divide by 255.
				glUniform4f(pickingColorID, r/255.0f, g/255.0f, b/255.0f, 1.0f);

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

				glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

				// Convert "i", the integer mesh ID, into an RGB color
				int r = (7 & 0x000000FF) >>  0;
				int g = (7 & 0x0000FF00) >>  8;
				int b = (7 & 0x00FF0000) >> 16;

				// OpenGL expects colors to be in [0,1], so divide by 255.
				glUniform4f(pickingColorID, r/255.0f, g/255.0f, b/255.0f, 1.0f);

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

				glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

				// Convert "i", the integer mesh ID, into an RGB color
				int r = (8 & 0x000000FF) >>  0;
				int g = (8 & 0x0000FF00) >>  8;
				int b = (8 & 0x00FF0000) >> 16;

				// OpenGL expects colors to be in [0,1], so divide by 255.
				glUniform4f(pickingColorID, r/255.0f, g/255.0f, b/255.0f, 1.0f);

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

				glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

				// Convert "i", the integer mesh ID, into an RGB color
				int r = (9 & 0x000000FF) >>  0;
				int g = (9 & 0x0000FF00) >>  8;
				int b = (9 & 0x00FF0000) >> 16;

				// OpenGL expects colors to be in [0,1], so divide by 255.
				glUniform4f(pickingColorID, r/255.0f, g/255.0f, b/255.0f, 1.0f);

				// Draw the triangles !
				glDrawArrays(GL_TRIANGLES, spherestart, spheresize );
			}

			glDisableVertexAttribArray(0);
			// Wait until all the pending drawing commands are really done.
			// Ultra-mega-over slow ! 
			// There are usually a long time between glDrawElements() and
			// all the fragments completely rasterized.
			glFlush();
			glFinish(); 


			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			// Read the pixel at the center of the screen.
			// You can also use glfwGetMousePos().
			// Ultra-mega-over slow too, even for 1 pixel, 
			// because the framebuffer is on the GPU.
			unsigned char data[4];
			// Get mouse position
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			glReadPixels(xpos, HEIGHT - ypos,1,1, GL_RGBA, GL_UNSIGNED_BYTE, data);

			// Convert the color back to an integer ID
			int pickedID = 
				data[0] + 
				data[1] * 256 +
				data[2] * 256*256;

			if (pickedID == 0x00ffffff){ // Full white, must be the background !
				selection = -1;
			}else{
				selection = pickedID;
			}

			// glfwSwapBuffers(window);
			// continue;


		}

		// Dark blue background
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//Re-clear the screen for real rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::vec3 lightPos = camera_worldposition;
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		// Bind our texture in Texture Unit 0
		glBindTexture(GL_TEXTURE_2D,Texture2);
		glActiveTexture(GL_TEXTURE0);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);

		// Use our shader
		glUseProgram(programID);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

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
			vec3 position = translation[0]+translation[1]+translation[2];
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

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture1);
		glUniform1i(TextureID, 0);

		{
			// glm::mat4 ModelMatrix = glm::mat4(1.0);
			glm::mat4 RotationMatrix = rotate(mat4(1.0f),M_PI_2f32,vec3(0,1,0));
			vec3 position = translation[0]+translation[1]+translation[2]+translation[3];
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
	glDeleteTextures(1, &Texture1);
	glDeleteTextures(1, &Texture2);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

