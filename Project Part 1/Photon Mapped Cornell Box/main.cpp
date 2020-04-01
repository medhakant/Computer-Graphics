#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include "scene.h"
#define QUIT(m,v)      { fprintf(stderr, "%s:%s\n", m, v); exit(1); }
double xoffset = 0;
double yoffset = 0;
double lightx = 0;
double lightz = 0;
static int t = 0;
GLubyte data[WIDTH*HEIGHT*3];
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

class OpenGLdraw {
public:
   void init(int W, int H) {
      unsigned int texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
  
      width = W; height = H;
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
      // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
      glGenFramebuffers(1, &fbo);
      glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
      glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

      glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
   }

   void draw(GLubyte* image, GLuint W=0, GLuint H=0, int atx=0, int aty=0) {
      if(H == 0) H = height;
      if(W == 0) W = width;
      invertImage(image);
      glTexSubImage2D(GL_TEXTURE_2D, 0, atx, aty, W, H, GL_RGB, GL_UNSIGNED_BYTE, image);

      glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
      glBlitFramebuffer(0, 0, W, H, atx, aty, W, H, GL_COLOR_BUFFER_BIT, GL_NEAREST);
      glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

   }
private:
	int width, height;
	GLuint fbo;

   void invertImage(GLubyte* image){
      int size = width*height*3;
      for(int i=0;i<=height/2;i++){
         for(int j=0;j<=width*3;j++){
            GLubyte temp = image[i*width*3 + j];
            image[i*width*3 + j] = image[(height-1-i)*width*3 + j];
            image[(height-1-i)*width*3 + j] = temp;
         }
      }
   }
};

void cornell_box_renderer(){
   scene cornell_box;
   //sphere
   sphere s1(vec3(-0.5,-1,-0.5),vec3(0,1,0),1,90,color(240, 54, 62),0.5,0.5,0.9,0,-2);
   cornell_box.addSphere(&s1);
   sphere s2(vec3(-0.3,-1.5,1),vec3(0,1,0),0.5,90,color(141, 245, 66),0.5,0.5,0.9,0,-2);
   cornell_box.addSphere(&s2);
   sphere s3(vec3(0.6,-1.5,0.5),vec3(0,1,0),0.5,90,color(62, 127, 240),0.5,0.5,0.9,0,-2);
   cornell_box.addSphere(&s3);

   //plane
   //floor
   plane p1 = plane(vec3(0,-2,0),vec3(0,1,0),4,color(255,255,193),0.5,0.5,0.9,0,-1);
   cornell_box.addPlane(&p1);
   //back wall
   plane p2 = plane(vec3(0,0,-2),vec3(0,0,1),4,color(255,255,193),0.5,0.5,0.9,0,-1);
   cornell_box.addPlane(&p2);
   //right wall
   plane p3 = plane(vec3(2,0,0),vec3(-1,0,0),4,color(64,235,52),0.5,0.5,0.9,0,-1);
   cornell_box.addPlane(&p3);
   //left wall
   plane p4 = plane(vec3(-2,0,0),vec3(1,0,0),4,color(235,64,52),0.5,0.5,0.9,0,-1);
   cornell_box.addPlane(&p4);
   //roof
   plane p5 = plane(vec3(0,2,0),vec3(0,-1,0),4,color(255,255,193),0.5,0.5,0.9,0,-1);
   cornell_box.addPlane(&p5);

   //lights
   light l = light(vec3(lightx,2,lightz),vec3(0,-1,0),1,color(255,255,255),1,0,0.5,0,-1);
   cornell_box.addLight(&l);
   int height = HEIGHT;
   int width = WIDTH;
   for(int i=0;i<=height/2;i++){
      for(int j=0;j<=width*3;j++){
      GLubyte temp = data[i*width*3 + j];
      data[i*width*3 + j] = data[(height-1-i)*width*3 + j];
      data[(height-1-i)*width*3 + j] = temp;
      }
   }
   cornell_box.render(data,xoffset,yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (key == GLFW_KEY_T && action == GLFW_PRESS){
      t = (t+1)%2;
   }
   if(t==0){
      if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
         xoffset += -(15*PI)/180.0;
         cornell_box_renderer();
      }else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
         xoffset += (15*PI)/180.0;
         cornell_box_renderer();
      }
   }else if(t==1){
      if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
         lightx += -0.1;
         cornell_box_renderer();
      }else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
         lightx += 0.1;
         cornell_box_renderer();
      }else if (key == GLFW_KEY_UP && action == GLFW_PRESS){
         lightz += 0.1;
         cornell_box_renderer();
      }else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
         lightz += -0.1;
         cornell_box_renderer();
      }
   }
}

int main( int argc, char* args[] )
{
   int M, m;

   if (!glfwInit())
      QUIT("gWindow_GLFW", "Could not Initialize GLFW");

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

   glfwSetErrorCallback(error_callback);

   bool coreProfile = true;
   if(coreProfile){
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   #ifdef __APPLE__ 
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   #endif
   }

   GLFWwindow *window = glfwCreateWindow(WIDTH,HEIGHT, "Cornell Box Photon Mapping", NULL, NULL);
   if (!window) {
      glfwTerminate();
      QUIT("gWindow_GLFW", "Could not create Window");
   }

   glfwMakeContextCurrent(window);

   if ( GLEW_OK != glewInit() ) {
      glfwTerminate();
      QUIT("gWindow_GLFW","glewInit failed");
   }

   glfwSwapInterval(1);
   glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
   glfwSetKeyCallback(window,key_callback);

   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   printf("Width = %d, Height = %d\n", width, height);

   OpenGLdraw opengl;
   opengl.init(width, height);
   for(int i=0;i<height*width*3;i++) data[i]=0;

   while (glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 )
   {
      cornell_box_renderer();
      glClearColor(0.5, 1, 1, 1.0);
      glClear(GL_COLOR_BUFFER_BIT);
      opengl.draw(data, WIDTH, HEIGHT,0,0);
      glfwSwapBuffers(window);
      glfwPollEvents();
   }
            
   glfwTerminate();
   return 0;
}
