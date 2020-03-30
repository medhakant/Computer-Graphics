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
double xrotb = 0;
double xrott = 0;
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

void snowman_renderer(){
   scene snowman;
   //snow sphere
   sphere s1(vec3(0,-1,0),vec3(0,1,0),1,90,color(255,255,255),0.65,0.35,0.9,0,-2);
   snowman.addSphere(&s1);
   sphere s2(vec3(0,0.4,0),vec3(0,1,0),0.5,60,color(255,255,255),0.65,0.35,0.9,0,-2);
   snowman.addSphere(&s2);
   sphere s3(vec3(0,1.08,0),vec3(0,1,0),0.25,90,color(255,255,255),0.65,0.35,0.9,0,-2);
   snowman.addSphere(&s3);
   //plane
   plane p1 = plane(vec3(0,-2,0),vec3(0,1,0),4,color(255,255,255),1,0,1,0,-1);
   snowman.addPlane(&p1);
   // snowman.addVisible(&p1);
   plane p2 = plane(vec3(0,0,-2),vec3(0,0,1),4,color(255,0,0),0,1,1,0,-1);
   snowman.addPlane(&p2);
   plane p3 = plane(vec3(0,0,2),vec3(0,0,-1),4,color(255,0,0),1,0,1,0,-1);
   snowman.addPlane(&p3);
   plane p4 = plane(vec3(2,0,0),vec3(-1,0,0),4,color(255,0,0),1,0,1,0,-1);
   snowman.addPlane(&p4);
   plane p5 = plane(vec3(-2,0,0),vec3(1,0,0),4,color(255,0,0),1,0,1,0,-1);
   snowman.addPlane(&p5);
   plane p6 = plane(vec3(0,2,0),vec3(0,-1,0),4,color(0,255,0),1,0,1,0,-1);
   snowman.addPlane(&p6);
   //lights
   light l1 = light(vec3(1.95*sin(PI*135/180 + xrott)*sqrt(2),-2,1.95*cos(PI*135/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l1);
   light l2 = light(vec3(1.95*sin(PI*135/180 + xrott)*sqrt(2),-1.75,1.95*cos(PI*135/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l2);
   light l3 = light(vec3(1.95*sin(PI*135/180 + xrott)*sqrt(2),-1.5,1.95*cos(PI*135/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l3);
   light l4 = light(vec3(1.95*sin(PI*135/180 + xrott)*sqrt(2),-1.25,1.95*cos(PI*135/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l4);

   light l5 = light(vec3(1.95*sin(PI*225/180 + xrott)*sqrt(2),-2,1.95*cos(PI*225/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l5);
   light l6 = light(vec3(1.95*sin(PI*225/180 + xrott)*sqrt(2),-1.75,1.95*cos(PI*225/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l6);
   light l7 = light(vec3(1.95*sin(PI*225/180 + xrott)*sqrt(2),-1.50, 1.95*cos(PI*225/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l7);
   light l8 = light(vec3(1.95*sin(PI*225/180 + xrott)*sqrt(2),-1.25, 1.95*cos(PI*225/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l8);

   light l9 = light(vec3(1.95*sin(PI*315/180 + xrott)*sqrt(2),-2, 1.95*cos(PI*315/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l9);
   light l10 = light(vec3(1.95*sin(PI*315/180 + xrott)*sqrt(2),-1.75,1.95*cos(PI*315/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l10);
   light l11 = light(vec3(1.95*sin(PI*315/180 + xrott)*sqrt(2),-1.50, 1.95*cos(PI*315/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l11);
   light l12 = light(vec3(1.95*sin(PI*315/180 + xrott)*sqrt(2),-1.25, 1.95*cos(PI*315/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l12);

   light l13 = light(vec3(1.95*sin(PI*45/180 + xrott)*sqrt(2),-2, 1.95*cos(PI*45/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l13);
   light l14 = light(vec3(1.95*sin(PI*45/180 + xrott)*sqrt(2),-1.75,1.95*cos(PI*45/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l14);
   light l15 = light(vec3(1.95*sin(PI*45/180 + xrott)*sqrt(2),-1.50, 1.95*cos(PI*45/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l15);
   light l16 = light(vec3(1.95*sin(PI*45/180 + xrott)*sqrt(2),-1.25, 1.95*cos(PI*45/180 + xrott)*sqrt(2)),vec3(0,1,0),1/25.0,0.25,color(255,255,255),0.7,0.3,0.9,0,-1);
   snowman.addLight(&l16);
   //balls
   sphere b1(vec3(1.5*cos(PI*0/180 + xrotb),-1.75,1.5*sin(PI*0/180 + xrotb)),vec3(0,1,0),.25,90,color(0,255,255),0.25,0.75,0.9,0,-1);
   snowman.addSphere(&b1);
   sphere b2(vec3(1.5*cos(PI*30/180  + xrotb),-1.75,1.5*sin(PI*30/180 + xrotb)),vec3(0,1,0),.25,90,color(0,0,0),0.25,0.75,0.3,0.6,1.41);
   snowman.addSphere(&b2);
   sphere b3(vec3(1.5*cos(PI*60/180  + xrotb),-1.75,1.5*sin(PI*60/180 + xrotb)),vec3(0,1,0),.25,90,color(255,128,0),0.25,0.75,0.9,0,-1);
   snowman.addSphere(&b3);
   sphere b4(vec3(1.5*cos(PI*90/180 + xrotb),-1.75,1.5*sin(PI*90/180 + xrotb)),vec3(0,1,0),.25,90,color(0,0,0),0.25,0.75,0.3,0.6,1.41);
   snowman.addSphere(&b4);
   sphere b5(vec3(1.5*cos(PI*120/180 + xrotb),-1.75,1.5*sin(PI*120/180 + xrotb)),vec3(0,1,0),.25,90,color(255,0,255),0.25,0.75,0.9,0,-1);
   snowman.addSphere(&b5);
   sphere b6(vec3(1.5*cos(PI*150/180 + xrotb),-1.75,1.5*sin(PI*150/180 + xrotb)),vec3(0,1,0),.25,90,color(0,0,0),0.25,0.75,0.3,0.6,1.41);
   snowman.addSphere(&b6);
   sphere b7(vec3(1.5*cos(PI*180/180 + xrotb),-1.75,1.5*sin(PI*180/180 + xrotb)),vec3(0,1,0),.25,90,color(0,255,255),0.25,0.75,0.9,0,-1);
   snowman.addSphere(&b7);
   sphere b8(vec3(1.5*cos(PI*210/180 + xrotb),-1.75,1.5*sin(PI*210/180 + xrotb)),vec3(0,1,0),.25,90,color(0,0,0),0.25,0.75,0.3,0.6,1.41);
   snowman.addSphere(&b8);
   sphere b9(vec3(1.5*cos(PI*240/180 + xrotb),-1.75,1.5*sin(PI*240/180) + xrotb),vec3(0,1,0),.25,90,color(255,128,0),0.25,0.75,0.9,0,-1);
   snowman.addSphere(&b9);
   sphere b10(vec3(1.5*cos(PI*270/180 + xrotb),-1.75,1.5*sin(PI*270/180 + xrotb)),vec3(0,1,0),.25,90,color(0,0,0),0.25,0.75,0.3,0.6,1.41);
   snowman.addSphere(&b10);
   sphere b11(vec3(1.5*cos(PI*300/180 + xrotb),-1.75,1.5*sin(PI*300/180 + xrotb)),vec3(0,1,0),.25,90,color(255,0,255),0.25,0.75,0.9,0,-1);
   snowman.addSphere(&b11);
   sphere b12(vec3(1.5*cos(PI*330/180 + xrotb),-1.75,1.5*sin(PI*330/180 + xrotb)),vec3(0,1,0),.25,90,color(0,0,0),0.25,0.75,0.3,0.6,1.41);
   snowman.addSphere(&b12);
   snowman.render(data,xoffset,yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (key == GLFW_KEY_T && action == GLFW_PRESS){
      t = (t+1)%3;
   }
   if(t==0){
      if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
         xoffset += -(15*PI)/180.0;
         snowman_renderer();
      }else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
         xoffset += (15*PI)/180.0;
         snowman_renderer();
      }
   }else if(t==1){
      if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
         xrotb += -(15*PI)/180.0;
         snowman_renderer();
      }else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
         xrotb += (15*PI)/180.0;
         snowman_renderer();
      }
   }else{
      if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
         xrott += (15*PI)/180.0;
         snowman_renderer();
      }else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
         xrott += -(15*PI)/180.0;
         snowman_renderer();
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

   GLFWwindow *window = glfwCreateWindow(WIDTH,HEIGHT, "Snow Man", NULL, NULL);
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
      snowman_renderer();
      glClearColor(0.5, 1, 1, 1.0);
      glClear(GL_COLOR_BUFFER_BIT);
      opengl.draw(data, WIDTH, HEIGHT,0,0);
      glfwSwapBuffers(window);
      glfwPollEvents();
   }
            
   glfwTerminate();
   return 0;
}
