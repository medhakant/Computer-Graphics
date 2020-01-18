#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "constants.h"
#include "shapes.h"
#include <vector>
#include <algorithm>
#define QUIT(m,v)      { fprintf(stderr, "%s:%s\n", m, v); exit(1); }

float xoffset = 0;
float yoffset = 0;
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

void render(){
   std::cout << "Frame Rendered!\n";
   int height = HEIGHT;
   int width = WIDTH;
   sphere s1 = sphere(vec3(0,-1,0),vec3(0,1,0),1,90,color(255,0,0),0.7,0.3,0.9,0,9);
   sphere s2 = sphere(vec3(0,0.4,0),vec3(0,1,0),0.5,60,color(255,0,0),0.7,0.3,0.9,0,9);
   sphere s3 = sphere(vec3(0,1.0,0),vec3(0,1,0),0.30,90,color(255,0,0),0.7,0.3,0.9,0,9);
   plane p1 = plane(vec3(0,-2,0),vec3(0,1,0),4,color(255,0,0),0.7,0.3,0.9,0,9);
   plane p2 = plane(vec3(0,0,-2),vec3(0,0,1),4,color(255,0,0),0.7,0.3,0.9,0,9);
   light l1 = light(vec3(2,-2,-2),vec3(0,1,0),1/25.0,1,color(255,0,0),0.7,0.3,0.9,0,9);
   light l2 = light(vec3(-2,-2,-2),vec3(0,1,0),1/25.0,1,color(255,0,0),0.7,0.3,0.9,0,9);
   light l3 = light(vec3(-2,-2, 2),vec3(0,1,0),1/25.0,1,color(255,0,0),0.7,0.3,0.9,0,9);
   light l4 = light(vec3(2,-2, 2),vec3(0,1,0),1/25.0,1,color(255,0,0),0.7,0.3,0.9,0,9);

   double sintheta = sin(xoffset);
   double costheta = cos(xoffset);
   for(int i=0;i<height*width*3;i++) data[i] = 0;
   for(int i=0;i<height;i++){
      for(int j=0;j<width;j++){
         double xcoor = (j-(WIDTH/2.0))*tan(PI*FOVX/360.0)/(WIDTH/2.0);
         double ycoor = -1*(i-(HEIGHT/2.0))*tan(PI*FOVY/360.0)/(HEIGHT/2.0);
         double zcoor = -1;
         vec3 dir = vec3(xcoor*costheta+zcoor*sintheta,ycoor,-1*xcoor*sintheta+zcoor*costheta);
         vec3 camera = vec3(5*sintheta,0,5*costheta);
         ray r = ray(camera,dir);
         std::vector<double> intersection;
         if(s1.willIntersect(r)){
            double distance = (camera- s1.intersection(r)).magnitude();
            intersection.push_back(distance);
         }
         if(s2.willIntersect(r)){
            double distance = (camera- s2.intersection(r)).magnitude();
            intersection.push_back(distance);
         }
         if(s3.willIntersect(r)){
            double distance = (camera- s3.intersection(r)).magnitude();
            intersection.push_back(distance);
         }
         if(p1.willIntersect(r)){
            double distance = (camera- p1.intersection(r)).magnitude();
            intersection.push_back(distance);
         }
         if(p2.willIntersect(r)){
            double distance = (camera- p2.intersection(r)).magnitude();
            intersection.push_back(distance);
         }
         if(intersection.size()>0){
            std::sort(intersection.begin(),intersection.end());
            double distance = intersection.at(0);
            double division_factor = (1*pow(distance,2) + 0.1*distance + 0.1);
            data[((i*height)+j)*3] = (255/division_factor);
            data[((i*height)+j)*3+1] = (255/division_factor);
            data[((i*height)+j)*3+2] = (255/division_factor);
         }
         if(l1.willIntersect(r)){
            data[((i*height)+j)*3] = 255;
            data[((i*height)+j)*3+1] = 255;
            data[((i*height)+j)*3+2] = 255;
         }
         if(l2.willIntersect(r)){
            data[((i*height)+j)*3] = 255;
            data[((i*height)+j)*3+1] = 255;
            data[((i*height)+j)*3+2] = 255;
         }
         if(l3.willIntersect(r)){
            data[((i*height)+j)*3] = 255;
            data[((i*height)+j)*3+1] = 255;
            data[((i*height)+j)*3+2] = 255;
         }
         if(l4.willIntersect(r)){
            data[((i*height)+j)*3] = 255;
            data[((i*height)+j)*3+1] = 255;
            data[((i*height)+j)*3+2] = 255;
         }
         
      }
   }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
      xoffset += -(15*PI)/180.0;
      render();
   }else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
      xoffset += (15*PI)/180.0;
      render();
   }else if (key == GLFW_KEY_UP && action == GLFW_PRESS){
      yoffset += -15;
      render();
   }else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
      yoffset += 15;
      render();
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

   glfwSwapInterval(5);
   glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
   glfwSetKeyCallback(window,key_callback);

   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   printf("Width = %d, Height = %d\n", width, height);

   OpenGLdraw opengl;
   opengl.init(width, height);

   while (glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 )
   {
      render();
      glClearColor(0.5, 1, 1, 1.0);
      glClear(GL_COLOR_BUFFER_BIT);
      opengl.draw(data, WIDTH, HEIGHT,0,0);
      glfwSwapBuffers(window);
      glfwPollEvents();
   }
            
   glfwTerminate();
   return 0;
}
