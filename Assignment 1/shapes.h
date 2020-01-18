#include "utils.h"
#include<iostream>
#include"constants.h"

class shape{
    private:
        vec3 center;
        vec3 normal;
        color colour;
        double diffuse;
        double specular;
        double reflection;
        double refraction;
        double refractive_index;

    public:
        shape(vec3 c,vec3 n,color co,double d,double s,double reflec,double refrac,double refrac_index){
            center = c;
            normal = n;
            colour = co;
            diffuse = d;
            specular = s;
            reflection = reflec;
            refraction = refrac;
            refractive_index = refrac_index;
        }

        vec3 getCenter() const{
            return center;
        }

        vec3 getNormal() const{
            return normal;
        }

        color getColor() const{
            return colour;
        }

        double getDiffuse() const{
            return diffuse;
        }

        double getSpecular() const{
            return specular;
        }

        double getReflection() const{
            return reflection;
        }

        double getRefraction() const{
            return refraction;
        }

        double getRefractiveIndex() const{
            return refractive_index;
        }

        virtual bool willIntersect(ray r) const = 0;

        virtual vec3 intersection(ray r) const = 0;

        // virtual ray reflectedRay(ray r) const = 0;

        // virtual ray refractedRay(ray r) const = 0;

};

class sphere : public shape{
    private:
        double radius;
        double vertical_angle_range;
    public:

        sphere(vec3 c,vec3 n,double r,double var,color co,double d,double s,double reflec,double refrac,double refrac_index)
        :shape(c,n,co,d,s,reflec,refrac,refrac_index){
            radius = r;
            vertical_angle_range = var;
        }

        double getRadius() const{
            return radius;
        }

        bool willIntersect(ray r) const{
            vec3 a = (r.getOrigin()-getCenter());
            double t = (getCenter()-r.getOrigin()).dot(r.getDirection())/pow(r.getDirection().magnitude(),2);
            double nearest_distance = (r.getPoint(t)-getCenter()).magnitude();
            
            if(vertical_angle_range==90){
                return (t>0 && nearest_distance <= getRadius())?true:false; 
            }else{
                double axis_length = getRadius()*cos((90-vertical_angle_range)*PI/180);
                if(abs((r.getPoint(t)-getCenter()).dot(getNormal().getUnitVector()))<axis_length && t>0 && nearest_distance <= getRadius()){
                    return true;
                }else{
                    return false;
                }
            }
        }

        vec3 intersection(ray r) const{
            double a = pow(r.getDirection().magnitude(),2);
            double b = 2*(r.getOrigin()-getCenter()).dot(r.getDirection());
            double c = pow((r.getOrigin()-getCenter()).magnitude(),2) - pow(getRadius(),2);
            double d = pow(b,2)-4*a*c;
            return r.getPoint((-1*(b+sqrt(d)))/(2*a));
        }

        // ray reflectedRay(ray r) const{}

        // ray refractedRay(ray r) const{}

};

class plane : public shape{
    private:
        double length;
    public:

        plane(vec3 c,vec3 n,double l,color co,double d,double s,double reflec,double refrac,double refrac_index)
        :shape(c,n,co,d,s,reflec,refrac,refrac_index){
            length = l;
        }

        double getLength() const{
            return length;
        }

        bool willIntersect(ray r) const{
            if(r.getDirection().dot(getNormal())==0){
                return false;
            }else{
                double t = (getCenter()-r.getOrigin()).dot(getNormal())/(r.getDirection().dot(getNormal()));
                vec3 point_on_plane = r.getPoint(t);
                vec3 square_point = (point_on_plane-getCenter()).componentWiseMult(vec3(1,1,1)-getNormal());
                if(square_point.absolute() < (getLength()/2.0) && t>0){
                    return true;
                }else{
                    return false;
                }
            }
        }

        vec3 intersection(ray r) const{
            double t = (getCenter()-r.getOrigin()).dot(getNormal())/(r.getDirection().dot(getNormal()));
            vec3 point_on_plane = r.getPoint(t);
            return point_on_plane;
        }



};

class light : public shape{
    private:
        double radius;
        double height;
    public:

        light(vec3 c,vec3 n,double r,double h,color co,double d,double s,double reflec,double refrac,double refrac_index)
        :shape(c,n,co,d,s,reflec,refrac,refrac_index){
            radius = r;
            height = h;
        }

        double getRadius() const{
            return radius;
        }

        double getHeight() const{
            return height;
        }

        bool willIntersect(ray r) const {
            double shortest_distance = abs((getCenter()-r.getOrigin()).dot((r.getDirection()*getNormal()).getUnitVector()));
            if(shortest_distance > getRadius()){
                return false;
            }else{
                double a = ((getCenter()-r.getOrigin()).dot(r.getDirection()))*(getNormal().dot(r.getDirection()));
                double b = pow(getNormal().dot(r.getDirection()),2);
                double c = ((getCenter()-r.getOrigin()).dot(getNormal()))*(r.getDirection().dot(r.getDirection()));
                double d = (r.getDirection().dot(r.getDirection()))*(getNormal().dot(getNormal()));
                double distance_along_axis = (a-c)/(d-b);
                // std::cout << distance_along_axis << "\n";
                if(distance_along_axis <= getHeight() && distance_along_axis>=0){
                    return true;
                }else{
                    return false;
                }
            }
        }

        vec3 intersection(ray r) const{
            return vec3(0,0,0);
        }

};