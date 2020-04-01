#include "scene.h"
#include <chrono>

using namespace std;
using namespace std::chrono;

scene::scene(){
    height = HEIGHT;
    width = WIDTH;
}

void scene::addLight(light* l){
    tubelights.insert(l);
    visible_objects.insert(l);
}

void scene::addSphere(shape* s){
    all_objects.insert(s);
    visible_objects.insert(s);
}

void scene::addPlane(plane* p){
    all_objects.insert(p);
    visible_objects.insert(p);
}

void scene::addVisible(shape* s){
    visible_objects.insert(s);
}

void scene::render(GLubyte* data,double xoffset,double yoffset) const{
    time_point<system_clock> start, end;
    duration<double> elapsed_seconds;
    start = system_clock::now(); 
    double sintheta = sin(xoffset);
    double costheta = cos(xoffset);
    vec3 camera = vec3(CAMERAD*sintheta,0,CAMERAD*costheta);
    double zcoor = -1;
    for(int i=0;i<height*width*3;i++) data[i] = 0;
    double tanfovx = tan(PI*FOVX/360.0);
    double tanfovy = tan(PI*FOVY/360.0);
    #pragma omp parallel for collapse(2) num_threads(8)
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            int aarays = sqrt(AARAYS);
            double length = 1/aarays;
            color c = color(0,0,0);
            for(int k=0;k<aarays;k++){
                for(int l=0;l<aarays;l++){
                    double xcoor = (j-(WIDTH/2.0) + (-0.5+length/2 + length*k))*tanfovx/(WIDTH/2.0);
                    double ycoor = -1*(i-(HEIGHT/2.0) + (-0.5+length/2 + length*l))*tanfovy/(HEIGHT/2.0);
                    vec3 dir = vec3(xcoor*costheta+zcoor*sintheta,ycoor,-1*xcoor*sintheta+zcoor*costheta);
                    ray r = ray(camera,dir);
                    c = c + getIntersectionColor(r,0);
                }
            }
            c = c/pow(aarays,2);
            data[((i*height)+j)*3] = std::min(c.getRed(),255);
            data[((i*height)+j)*3+1] = std::min(c.getGreen(),255);
            data[((i*height)+j)*3+2] = std::min(c.getBlue(),255);
        }
        // std::cout << i << "\n";
    }
    end = system_clock::now(); 
    elapsed_seconds = end - start; 
    cout<<"Render Time: "<<elapsed_seconds.count()<<endl;
}

bool scene::isLightBlocked(ray r) const{
    std::set<shape*>::iterator it;
    for(it=visible_objects.begin();it!=visible_objects.end();it++){
        if(!(*it)->shapeType("light") && (*it)->willIntersect(r) && (*it)->getCenter()!=vec3(0,2,0)){
            return true;
        }
    }
    return false;
}

color scene::getIntersectionColor(ray r,int depth) const{
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

color scene::getLightIntersection(raytrace rt,shape* s) const{
    color c = color(0,0,0);
    std::set<light*>::iterator it;
    for(it=tubelights.begin();it!=tubelights.end();it++){
        vec3 ray_dir_to_light = ((*it)->getMidPoint()-rt.getIntersection()).getUnitVector();
        double angle = abs(ray_dir_to_light.dot((*it)->getNormal()));
        ray light_ray = ray(rt.getIntersection(),ray_dir_to_light);
        if(!isLightBlocked(light_ray)){
            double distance = ((*it)->getMidPoint()-rt.getIntersection()).magnitude();
            double distance_factor = DA*pow(distance,2) + (DB*distance) + DC;
            vec3 n = rt.getNormal().getUnitVector();
            vec3 h = (ray_dir_to_light - rt.getIncident().getDirection().getUnitVector()).getUnitVector();
            vec3 v = rt.getIncident().getDirection().getUnitVector()*-1;
            double diffuse_component = (s->getDiffuse()*(rt.getNormal().getUnitVector().dot(ray_dir_to_light)));
            double cossq_alpha = pow(h.dot(rt.getNormal().getUnitVector()),2);
            double f = F0 + (1-F0)*pow((1-v.dot(h)),5);
            double d = exp(-((1/cossq_alpha) -1)/MSQ)/(PI*MSQ*pow(cossq_alpha,2));
            double g = std::min(1.0,2*(n.dot(h))*(n.dot(v))/(v.dot(h)));
            g = std::min(g,2*(n.dot(h))*(n.dot(ray_dir_to_light))/(v.dot(h)));
            double rsk = f*d*g/(4*n.dot(ray_dir_to_light)*(n.dot(v)));
            double specular_component = (s->getSpecular()*(rt.getNormal().getUnitVector().dot(ray_dir_to_light)))*rsk;
            c = c + s->getColor()*(((diffuse_component + specular_component)*LIGHT_POWER)/(distance_factor))*pow(angle,0.5);
        }
    }
    return c;
}