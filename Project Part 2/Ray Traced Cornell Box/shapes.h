#include<iostream>
#include"utils.h"
#include"constants.h"
#include<vector>

class raytrace{
    private:
        vec3 intersection_point;
        ray incident_ray;
        vec3 normal_at_intersection;
        ray reflected_ray;
        ray refracted_ray;
        color color_at_intersection;

    public:

        raytrace();

        raytrace(vec3 point,ray incident,vec3 normal,ray reflected,ray refracted,color colour){
            this->intersection_point = point;
            this->incident_ray = incident;
            this->normal_at_intersection = normal;
            this->reflected_ray = reflected;
            this->refracted_ray = refracted;
            this->color_at_intersection = colour;
        }

        vec3 getIntersection() const{
            return intersection_point;
        }

        ray getIncident() const{
            return incident_ray;
        }

        color getColor() const{
            return color_at_intersection;
        }

        vec3 getNormal() const{
            return normal_at_intersection;
        }

        ray getReflected() const{
            return reflected_ray;
        }

        ray getRefracted() const{
            return refracted_ray;
        }

};

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

        virtual bool shapeType(std::string s) const = 0;

        virtual bool willIntersect(ray r) const = 0;

        virtual double getIntersectionDistance(ray r) const = 0;

        virtual raytrace getIntersection(ray r) const = 0;

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

        virtual bool shapeType(std::string s) const{
            return s=="sphere";
        }

        virtual bool willIntersect(ray r) const{
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

        virtual double getIntersectionDistance(ray r) const{
            double a = pow(r.getDirection().magnitude(),2);
            double b = 2*(r.getOrigin()-getCenter()).dot(r.getDirection());
            double c = pow((r.getOrigin()-getCenter()).magnitude(),2) - pow(getRadius(),2);
            double d = pow(b,2)-4*a*c;
            return (-1*(b+sqrt(d)))/(2*a);
        }

        virtual raytrace getIntersection(ray r) const{
            double a = pow(r.getDirection().magnitude(),2);
            double b = 2*(r.getOrigin()-getCenter()).dot(r.getDirection());
            double c = pow((r.getOrigin()-getCenter()).magnitude(),2) - pow(getRadius(),2);
            double d = pow(b,2)-4*a*c;
            vec3 intersection_point = r.getPoint((-1*(b+sqrt(d)))/(2*a));
            if(r.getMedium()=="glass"){
                intersection_point = r.getPoint(((-1*b) +sqrt(d))/(2*a));
            }
            vec3 normal = (intersection_point - getCenter()).getUnitVector();
            double refrac_index = getRefractiveIndex();
            if(r.getMedium()=="glass"){
                normal = normal*-1;
                refrac_index = 1/refrac_index;
            }
            ray reflected = ray(intersection_point,r.getDirection() - normal*2*(r.getDirection().dot(normal)));
            ray refracted = ray(vec3(0,0,0),vec3(0,0,0));
            if(getRefractiveIndex()>0){
                double theta1 = acos(fabs((normal*-1).dot(r.getDirection().getUnitVector())));
                vec3 m = (r.getDirection().getUnitVector() + normal*cos(theta1))/sin(theta1);
                if(!(r.getMedium()=="glass" && theta1 > asin(1/getRefractiveIndex()))){
                    double theta2 = asin(sin(theta1)/refrac_index);
                    vec3 refrac_dir = m*sin(theta2) - normal*cos(theta2);    
                    refracted = ray(intersection_point,refrac_dir);
                    if(r.getMedium()=="air"){
                        refracted.setMediumGlass();
                    }else{
                        refracted.setMediumAir();
                    }
                }else{
                    refracted = ray(intersection_point,m);
                }                
            }
            return raytrace(intersection_point,r,normal,reflected,refracted,getColor());
        }

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

        virtual bool shapeType(std::string s) const{
            return s=="plane";
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

        double getIntersectionDistance(ray r) const{
            double t = (getCenter()-r.getOrigin()).dot(getNormal())/(r.getDirection().dot(getNormal()));
            return t;
        }

        raytrace getIntersection(ray r) const{
            double t = (getCenter()-r.getOrigin()).dot(getNormal())/(r.getDirection().dot(getNormal()));
            vec3 intersection_point = r.getPoint(t);
            vec3 normal = getNormal().getUnitVector();
            ray reflected = ray(intersection_point,r.getDirection() - normal*2*(r.getDirection().dot(normal)));
            return raytrace(intersection_point,r,normal,reflected,ray(vec3(0,0,0),vec3(0,0,0)),getColor());
        }


};

class light : public shape{
    private:
        double length;
    public:

        light(vec3 c,vec3 n,double l,color co,double d,double s,double reflec,double refrac,double refrac_index)
        :shape(c,n,co,d,s,reflec,refrac,refrac_index){
            length = l;
        }

        double getLength() const{
            return length;
        }

        virtual bool shapeType(std::string s) const{
            return s=="light";
        }

        vec3 getMidPoint(){
            return getCenter();
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

        double getIntersectionDistance(ray r) const{
            double t = (getCenter()-r.getOrigin()).dot(getNormal())/(r.getDirection().dot(getNormal()));
            return t;
        }

        raytrace getIntersection(ray r) const{
            double t = (getCenter()-r.getOrigin()).dot(getNormal())/(r.getDirection().dot(getNormal()));
            vec3 intersection_point = r.getPoint(t);
            vec3 normal = getNormal().getUnitVector();
            ray reflected = ray(intersection_point,r.getDirection() - normal*2*(r.getDirection().dot(normal)));
            return raytrace(intersection_point,r,normal,reflected,ray(vec3(0,0,0),vec3(0,0,0)),getColor());
        }

};