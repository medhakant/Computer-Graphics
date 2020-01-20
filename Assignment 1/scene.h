#include "shapes.h"
#include "constants.h"
#include <map>
#include <set>
#include <GLFW/glfw3.h>
#include <iterator>
#include "omp.h"
class scene{
    private:
        std::set<light*> tubelights;
        std::set<shape*> visible_objects;
        std::set<shape*> all_objects;
        int height;
        int width;

    public:
        scene(){
            height = HEIGHT;
            width = WIDTH;
        }

        void addLight(light* l){
            tubelights.insert(l);
            visible_objects.insert(l);
        }

        void addSphere(shape* s){
            all_objects.insert(s);
            visible_objects.insert(s);
        }

        void addPlane(plane* p){
            all_objects.insert(p);
        }

        void addVisible(shape* s){
            visible_objects.insert(s);
        }

        void render(GLubyte* data,double xoffset,double yoffset) const{
            double sintheta = sin(xoffset);
            double costheta = cos(xoffset);
            vec3 camera = vec3(CAMERAD*sintheta,0,CAMERAD*costheta);
            double zcoor = -1;
            for(int i=0;i<height*width*3;i++) data[i] = 0;
            double tanfovx = tan(PI*FOVX/360.0);
            double tanfovy = tan(PI*FOVY/360.0);
            #pragma omp parallel num_thread(4)
            for(int i=0;i<height;i++){
                #pragma omp parallel num_thread(4)
                for(int j=0;j<width;j++){
                    double xcoor = (j-(WIDTH/2.0))*tanfovx/(WIDTH/2.0);
                    double ycoor = -1*(i-(HEIGHT/2.0))*tanfovy/(HEIGHT/2.0);
                    double xpos = (j+0.25-(WIDTH/2.0))*tanfovx/(WIDTH/2.0);
                    double ypos = -1*(i+0.25-(HEIGHT/2.0))*tanfovy/(HEIGHT/2.0);
                    double xneg = (j-0.25-(WIDTH/2.0))*tanfovx/(WIDTH/2.0);
                    double yneg = -1*(i-0.25-(HEIGHT/2.0))*tanfovy/(HEIGHT/2.0);
                    vec3 dir = vec3(xcoor*costheta+zcoor*sintheta,ycoor,-1*xcoor*sintheta+zcoor*costheta);
                    vec3 aa1 = vec3(xpos*costheta+zcoor*sintheta,ypos,-1*xpos*sintheta+zcoor*costheta);
                    vec3 aa2 = vec3(xneg*costheta+zcoor*sintheta,ypos,-1*xneg*sintheta+zcoor*costheta);
                    vec3 aa3 = vec3(xneg*costheta+zcoor*sintheta,yneg,-1*xneg*sintheta+zcoor*costheta);
                    vec3 aa4 = vec3(xpos*costheta+zcoor*sintheta,yneg,-1*xpos*sintheta+zcoor*costheta);
                    ray r = ray(camera,dir);
                    ray a1 = ray(camera,aa1);
                    ray a2 = ray(camera,aa2);
                    ray a3 = ray(camera,aa3);
                    ray a4 = ray(camera,aa4);
                    color c = (getIntersectionColor(r,0)+getIntersectionColor(a1,0)+getIntersectionColor(a2,0)+getIntersectionColor(a3,0)+getIntersectionColor(a4,0))/5.0;
                    data[((i*height)+j)*3] = std::min(c.getRed(),255);
                    data[((i*height)+j)*3+1] = std::min(c.getGreen(),255);
                    data[((i*height)+j)*3+2] = std::min(c.getBlue(),255);
                }
            }
        }

        bool isLightBlocked(ray r) const{
            std::set<shape*>::iterator it;
            #pragma omp parallel num_thread(4)
            for(it=visible_objects.begin();it!=visible_objects.end();it++){
                if(!(*it)->shapeType("light") && (*it)->willIntersect(r)){
                    return true;
                }
            }
            return false;
        }

        color getIntersectionColor(ray r,int depth) const{
            std::map<double,shape*> rayintersection;
            if(depth==0){
                std::set<shape*>::iterator it;
                for(it=visible_objects.begin();it!=visible_objects.end();it++){
                    if((*it)->willIntersect(r)){
                        double distance = (*it)->getIntersectionDistance(r);
                        rayintersection.insert(std::make_pair(distance,(*it)));
                    }
                }
                if(rayintersection.size()>0){
                    shape* s = rayintersection.begin()->second;
                    if(s->shapeType("light")){
                        return s->getColor();
                    }else{
                        raytrace rt = s->getIntersection(r);
                        color result = rt.getColor()*AMBIENT + getLightIntersection(rt,s) + getIntersectionColor(rt.getReflected(),depth+1)*s->getReflection()*REFLEC;
                        if(s->getRefractiveIndex()>0 && rt.getRefracted().getDirection().magnitude()>0){
                            result = result + getIntersectionColor(rt.getRefracted(),depth+1)*s->getRefraction()*REFRAC;
                        }
                        return result;
                    }
                }else{
                    return color(0,0,0);
                }
            }else if(depth<MAXDEPTH){
                std::set<shape*>::iterator it;
                #pragma omp parallel num_thread(4) shared data
                for(it=all_objects.begin();it!=all_objects.end();it++){
                    if((*it)->willIntersect(r)){
                        double distance = (*it)->getIntersectionDistance(r);
                        rayintersection.insert(std::make_pair(distance,(*it)));
                    }
                }
                if(rayintersection.size()>0){
                    shape* s = rayintersection.begin()->second;
                    if(s->shapeType("light")){
                        return s->getColor();
                    }else{
                        raytrace rt = s->getIntersection(r);
                        color result =  getLightIntersection(rt,s) + getIntersectionColor(rt.getReflected(),depth+1)*s->getReflection()*REFLEC;
                        if(s->getRefractiveIndex()>0 && rt.getRefracted().getDirection().magnitude()>0){
                            result = result + getIntersectionColor(rt.getRefracted(),depth+1)*s->getRefraction()*REFRAC;
                        }
                        return result;
                    }
                }else{
                    return color(0,0,0);
                }
                return color(0,0,0);
            }else{
                return color(0,0,0);
            }
        }

        color getLightIntersection(raytrace rt,shape* s) const{
            color c;
            std::set<light*>::iterator it;
            #pragma omp parallel num_thread(4) shared data
            for(it=tubelights.begin();it!=tubelights.end();it++){
                vec3 ray_dir_to_light = ((*it)->getMidPoint()-rt.getIntersection()).getUnitVector();
                ray light_ray = ray(rt.getIntersection(),ray_dir_to_light);
                if(!isLightBlocked(light_ray)){
                    double distance = ((*it)->getMidPoint()-rt.getIntersection()).magnitude();
                    double distance_factor = DA*pow(distance,2) + (DB*distance) + DC;
                    vec3 h = (ray_dir_to_light - rt.getIncident().getDirection().getUnitVector()).getUnitVector();
                    double diffuse_component = (s->getDiffuse()*(rt.getNormal().getUnitVector().dot(ray_dir_to_light)));
                    double specular_component = s->getSpecular()*pow(rt.getNormal().getUnitVector().dot(h),SPECULAR);
                    c = c + s->getColor()*((diffuse_component + specular_component)/distance_factor);
                }
            }
            return c;
        }

};