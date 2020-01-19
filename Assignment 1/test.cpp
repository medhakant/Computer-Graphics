#include"shapes.h"
#include<iostream>

using namespace std;

int main(){
    ray r = ray(vec3(0,0,2),vec3(-.5,-.5,0)-vec3(0,0,2));
    light l1 = light(vec3(0,-0.5,0),vec3(0,1,0),1,1,color(255,0,0),0.7,0.3,0.9,0,9);
    for(int i=-20;i<21;i++){
        for(int j=-20;j<21;j++){
            vec3 dir = vec3(j/20.0,i/20.0,-1);
         if(l1.willIntersect(ray(vec3(0,0,4),dir))){
            std::cout << "  ";
         }else{
             std::cout << "0 ";
         }
        }
        std::cout << "\n";
    }
}