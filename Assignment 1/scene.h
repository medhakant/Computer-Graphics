#include "shapes.h"
#include "constants.h"
#include <vector>
#include <map>
#include <GLFW/glfw3.h>
#define MAXDEPTH 4
class scene{
    private:
        std::vector<light*> light_sources;
        std::vector<shape*> visible_objects;
        std::vector<shape*> all_objects;
        int height;
        int width;

    public:
        scene(){
            height = HEIGHT;
            width = WIDTH;
        }

        void render(GLubyte* data,double xoffset,double yoffset){
            double sintheta = sin(xoffset);
            double costheta = cos(xoffset);
            for(int i=0;i<height*width*3;i++) data[i] = 0;
            for(int i=0;i<height;i++){
                for(int j=0;j<width;j++){
                    double xcoor = (j-(WIDTH/2.0))*tan(PI*FOVX/360.0)/(WIDTH/2.0);
                    double ycoor = -1*(i-(HEIGHT/2.0))*tan(PI*FOVY/360.0)/(HEIGHT/2.0);
                    double zcoor = -1;
                    vec3 dir = vec3(xcoor*costheta+zcoor*sintheta,ycoor,-1*xcoor*sintheta+zcoor*costheta);
                    vec3 camera = vec3(7*sintheta,0,7*costheta);
                    ray r = ray(camera,dir);
                    color c = getIntersectionColor(r,0);
                    data[((i*height)+j)*3] = c.getRed();
                    data[((i*height)+j)*3+1] = c.getGreen();
                    data[((i*height)+j)*3+2] = c.getBlue();
                }
            }
        }

        color getIntersectionColor(ray r,int depth){
            std::map<double,shape*> rayintersection;
            if(depth==0){
                for(int i=0;i<visible_objects.size();i++){
                    if(visible_objects[i]->willIntersect(r)){
                        double distance = visible_objects[i]->getIntersectionDistance(r);
                        rayintersection.insert(std::make_pair(distance,visible_objects[i]));
                    }
                }
                if(rayintersection.size()>0){
                    return rayintersection.begin()->second->getColor();
                }else{
                    return color(0,0,0);
                }
            }else if(depth<MAXDEPTH){
                return color(0,0,0);
            }else{
                return color(0,0,0);
            }
        }

        void addLight(light* l){
            light_sources.push_back(l);
            visible_objects.push_back(l);
        }

        void addSphere(shape* s){
            all_objects.push_back(s);
            visible_objects.push_back(s);
        }

        void addPlane(plane* p){
            all_objects.push_back(p);
        }

};