#include "scene.h"
#include <chrono>
#include<climits>
#include <random>

using namespace std;
using namespace std::chrono;
mt19937 mt_rand(time(0));
scene::scene(){
    height = HEIGHT;
    width = WIDTH;
}

void scene::render(GLubyte* data,double xoffset,double yoffset){
    time_point<system_clock> start, end;
    duration<double> elapsed_seconds;
    start = system_clock::now(); 
    castPhoton();
    double sintheta = sin(xoffset);
    double costheta = cos(xoffset);
    vec3 camera = vec3(CAMERAD*sintheta,0,CAMERAD*costheta);
    double zcoor = -1;
    double tanfovx = tan(PI*FOVX/360.0);
    double tanfovy = tan(PI*FOVY/360.0);
    for(int i=0;i<height*width*3;i++) data[i]=0;
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
                    c = c + getIntersectionColor(r);
                }
            }
            c = c/pow(aarays,2);
            data[((i*height)+j)*3] = std::min(c.getRed(),255);
            data[((i*height)+j)*3+1] = std::min(c.getGreen(),255);
            data[((i*height)+j)*3+2] = std::min(c.getBlue(),255);
        }
    }
    end = system_clock::now(); 
    elapsed_seconds = end - start; 
    cout<<"Render Time: "<<elapsed_seconds.count()<<endl;
}


color scene::getIntersectionColor(ray r) const{
    std::map<double,shape*> rayintersection;
    std::set<shape*>::iterator it;
    for(it=objects.begin();it!=objects.end();it++){
        if((*it)->willIntersect(r)){
            double distance = (*it)->getIntersectionDistance(r);
            rayintersection.insert(std::make_pair(distance,(*it)));
        }
    }
    if(rayintersection.size()>0){
        shape* s = rayintersection.begin()->second;
        double distance = rayintersection.begin()->first;
        double distance_factor = DA*pow(distance,2) + (DB*distance) + DC;
        raytrace rt = s->getIntersection(r);
        return (searchPhoton(0,global_photonmap.size()-1,0,rt.getIntersection(),false)*LIGHT_POW/distance_factor) + s->getColor()*AMBIENT + (searchPhoton(0,caustic_photonmap.size()-1,0,rt.getIntersection(),true)*CAUSTIC_POW/distance_factor);
    }else{
        return color(0,0,0);
    }
}

void scene::addLight(light* l){
    arealight = l;
}

void scene::addSphere(shape* s){
    objects.insert(s);
}

void scene::addPlane(plane* p){
    objects.insert(p);
}

photon* scene::makePhoton() const{
    double x = mt_rand()*0.5/INT_MAX - 0.5;
    double y = mt_rand()*0.5/INT_MAX - 0.5;
    double z = mt_rand()*0.5/INT_MAX - 0.5;
    int llength = arealight->getLength();
    if(arealight->getNormal().getX()==-1){
        x = abs(x);
    }else if(arealight->getNormal().getY()==-1){
        y = abs(y)*-1;
    }else if(arealight->getNormal().getX()==-1){
        z = abs(z);
    }
    vec3 direction = vec3(x,y,z);
    vec3 point_source;
    if(arealight->getNormal().getX()==-1){
        double y_s = (mt_rand()*0.5/INT_MAX - 0.5)*llength;
        double z_s = (mt_rand()*0.5/INT_MAX - 0.5)*llength;
        point_source = arealight->getCenter() + vec3(0,y_s,z_s);
    }else if(arealight->getNormal().getY()==-1){
        double x_s = (mt_rand()*0.5/INT_MAX - 0.5)*llength;
        double z_s = (mt_rand()*0.5/INT_MAX - 0.5)*llength;
        point_source = arealight->getCenter() + vec3(x_s,0,z_s);
    }else if(arealight->getNormal().getX()==-1){
        double x_s = (mt_rand()*0.5/INT_MAX - 0.5)*llength;
        double y_s = (mt_rand()*0.5/INT_MAX - 0.5)*llength;
        point_source = arealight->getCenter() + vec3(x_s,y_s,0);
    }
    photon*  p = new photon(ray(point_source,direction),arealight->getColor());
    return p;
}

void scene::castPhoton(){
    for(int i=0;i<MAX_PHOTONS;i++){
        photon* p = makePhoton();
        if(mt_rand()*0.5/INT_MAX < 0.2){
            global_photonmap.push_back(p);
        }
        photon* b = getBounce(p);
        if(b->getFlag()){
            if(b->getCaustic()){
                caustic_photonmap.push_back(b);
            }else{
                global_photonmap.push_back(b);
            }            
        }
    }
    cout << "GLOBAL MAP: " << global_photonmap.size() << "\n";
    cout << "CAUSTIC MAP: " << caustic_photonmap.size() << "\n";
    balancePhoton(0,caustic_photonmap.size()-1,0,true);
    cout << "BUILD K-D TREE(GLOBAL)\n";
    balancePhoton(0,global_photonmap.size()-1,0,false);
    cout << "BUILD K-D TREE(CAUSTICS)\n";
}

photon* scene::getBounce(const photon* init_photon) const{
    std::map<double,shape*> photonintersection;
    std::set<shape*>::iterator it;
    for(it=objects.begin();it!=objects.end();it++){
        if((*it)->willIntersect(init_photon->getPhotonRay()) && !(init_photon->getBounce()==0 && (*it)->getCenter()==arealight->getCenter())){
            double distance = (*it)->getIntersectionDistance(init_photon->getPhotonRay());
            photonintersection.insert(std::make_pair(distance,(*it)));
        }
    }
    if(photonintersection.size()>0){
        double choice = mt_rand()*0.5/INT_MAX;
        shape* s = photonintersection.begin()->second;
        raytrace rt = s->getIntersection(init_photon->getPhotonRay());
        if(choice < s->getReflection()){
            photon* p = new photon(rt.getReflected(),init_photon->getPhotonColor()*rt.getColor());
            if(init_photon->getCaustic()){
                p->setCaustic();
            }
            choice = mt_rand()*0.5/INT_MAX;
            if(choice < s->getDiffuse()){
                p->setBounce(MAXDEPTH+1);
                p->setFlag();
                return p;
            }else{
                p->setBounce(init_photon->getBounce());
                p->incrementBounce();
                if(p->getBounce()>MAXDEPTH){
                    p->setFlag();
                    return p;
                }else{
                    return getBounce(p);
                }
            }
        }else if(s->getRefractiveIndex()>0){
            photon* p = new photon(rt.getRefracted(),init_photon->getPhotonColor()*rt.getColor());
            p->setCaustic();
            p->setBounce(init_photon->getBounce());
            p->incrementBounce();
            if(p->getBounce()>MAXDEPTH){
                p->setFlag();
                return p;
            }else{
                return getBounce(p);
            }            
        }else{
            photon* p = new photon(ray(vec3(0,0,0),vec3(0,0,0)));
            return p;
        }
    }else{
        photon* p = new photon(ray(vec3(0,0,0),vec3(0,0,0)));
        return p;
    }
}

void scene::balancePhoton(int start,int end,int axis,bool caustic){
    if(!caustic){
        if(start>=end){
            return;
        }else{
            sort(global_photonmap.begin()+start,global_photonmap.begin()+end,
            [axis](const photon* a, const photon* b) -> bool { 
                return a->getPhotonRay().getOrigin().getAxis(axis) < b->getPhotonRay().getOrigin().getAxis(axis);
            });
            int mid = (start+end)/2;
            balancePhoton(start,mid-1,(axis+1)%3,caustic);
            balancePhoton(mid+1,end,(axis+1)%3,caustic);
        }
    }else{
        if(start>=end){
            return;
        }else{
            sort(caustic_photonmap.begin()+start,caustic_photonmap.begin()+end,
            [axis](const photon* a, const photon* b) -> bool { 
                return a->getPhotonRay().getOrigin().getAxis(axis) < b->getPhotonRay().getOrigin().getAxis(axis);
            });
            int mid = (start+end)/2;
            balancePhoton(start,mid-1,(axis+1)%3,caustic);
            balancePhoton(mid+1,end,(axis+1)%3,caustic);
        }
    }
}

color scene::searchPhoton(int start,int end,int axis, vec3 intersection,bool caustic) const{
    if(caustic){
        if(start>end){
            return color(0,0,0);
        }else if(start==end){
            if(intersection.inBox(caustic_photonmap[start]->getPhotonRay().getOrigin(),BOUNDING_BOX)){
                return caustic_photonmap[start]->getPhotonColor();
            }else{
                return color(0,0,0);
            }
        }else{
            int mid = (start+end)/2;
            vec3 photonO = caustic_photonmap[mid]->getPhotonRay().getOrigin();
            if(intersection.inBox(photonO,BOUNDING_BOX)){
                return caustic_photonmap[mid]->getPhotonColor() + searchPhoton(start,mid-1,(axis+1)%3,intersection,caustic) + searchPhoton(mid+1,end,(axis+1)%3,intersection,caustic);
            }else if(fabs(photonO.getAxis(axis)-intersection.getAxis(axis))>BOUNDING_BOX){
                if(intersection.getAxis(axis) < photonO.getAxis(axis)){
                    return searchPhoton(start,mid-1,(axis+1)%3,intersection,caustic);
                }else{
                    return searchPhoton(mid+1,end,(axis+1)%3,intersection,caustic);
                }
            }else{
                return searchPhoton(start,mid-1,(axis+1)%3,intersection,caustic) + searchPhoton(mid+1,end,(axis+1)%3,intersection,caustic);
            }
        }
    }else{
        if(start>end){
            return color(0,0,0);
        }else if(start==end){
            if(intersection.inBox(global_photonmap[start]->getPhotonRay().getOrigin(),BOUNDING_BOX)){
                return global_photonmap[start]->getPhotonColor();
            }else{
                return color(0,0,0);
            }
        }else{
            int mid = (start+end)/2;
            vec3 photonO = global_photonmap[mid]->getPhotonRay().getOrigin();
            if(intersection.inBox(photonO,BOUNDING_BOX)){
                return global_photonmap[mid]->getPhotonColor() + searchPhoton(start,mid-1,(axis+1)%3,intersection,caustic) + searchPhoton(mid+1,end,(axis+1)%3,intersection,caustic);
            }else if(fabs(photonO.getAxis(axis)-intersection.getAxis(axis))>BOUNDING_BOX){
                if(intersection.getAxis(axis) < photonO.getAxis(axis)){
                    return searchPhoton(start,mid-1,(axis+1)%3,intersection,caustic);
                }else{
                    return searchPhoton(mid+1,end,(axis+1)%3,intersection,caustic);
                }
            }else{
                return searchPhoton(start,mid-1,(axis+1)%3,intersection,caustic) + searchPhoton(mid+1,end,(axis+1)%3,intersection,caustic);
            }
        }
    }
}

