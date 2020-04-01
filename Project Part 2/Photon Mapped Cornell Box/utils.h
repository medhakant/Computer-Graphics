#include<cmath>
#include<sstream>
#include<string>
#include<iostream>

#ifndef UTILS_H
#define UTILS_H

class vec3{
    private:
        double x,y,z;
        bool ispoint;
    public:
        vec3();
        vec3(double x,double y,double z);
        void isDirection();
        double getX() const;
        double getY() const;
        double getZ() const;
        double getAxis(int i) const;
        vec3 operator + (vec3 const &obj) const;
        vec3 operator - (vec3 const &obj) const;
        vec3 operator / (double d) const;
        vec3 operator * (vec3 const &obj) const;
        vec3 operator * (double alpha) const;
        bool operator < (double alpha) const;
        bool operator <= (double alpha) const;
        bool operator > (double alpha) const;
        bool operator >= (double alpha) const;
        bool operator != (vec3 const &obj) const;
        bool operator == (vec3 const &obj) const;
        bool inBox(vec3 const &obj,double box) const;
        double dot(vec3 const &obj) const;
        double magnitude() const;
        vec3 getUnitVector() const;
        vec3 componentWiseMult(vec3 const &obj) const;
        vec3 absolute() const;
        std::string toString() const;
};

class ray{
    private:
        vec3 origin,direction;
        std::string medium; 

    public:
        ray();
        ray(vec3 o,vec3 d);
        void setMediumGlass();
        void setMediumAir();
        std::string getMedium() const;
        vec3 getOrigin() const;
        vec3 getDirection() const;
        vec3 getPoint(const double a);
        std::string toString() const;
};

class color{
    private:
        int red;
        int green;
        int blue;
    public:
        color();
        color(int r,int g,int b);
        int getRed() const;
        int getGreen() const;
        int getBlue() const;
        color operator * (double d) const;
        color operator + (color const &obj) const;
        color operator / (double d) const;
        color operator * (color const &obj);
        std::string toString() const;
};

class photon{
    private:
        ray photon_ray;
        color photon_color;
        int num_bounce;
        bool flag;
        bool caustic;
    public:
        photon(ray r);
        photon(ray r,color c);
        int getBounce() const;
        ray getPhotonRay() const;
        color getPhotonColor() const;
        void setFlag();
        bool getFlag() const;
        void setCaustic();
        bool getCaustic() const;
        void setPhotonColor(color c);
        void setPhotonRay(ray r);
        void setBounce(int b);
        void incrementBounce();
        std::string toString() const;
}; 

#endif