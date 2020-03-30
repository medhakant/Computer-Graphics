#include<cmath>
#include<sstream>
#include<string>
#include<iostream>

class vec3{
    private:
        double x,y,z;
        bool ispoint;
    public:
        vec3(){
            this->x = 0;
            this->y = 0;
            this->z = 0;
            this->ispoint = true;
        }

        vec3(double x,double y,double z){
            this->x = x;
            this->y = y;
            this->z = z;
            this->ispoint = true;
        }

        void isDirection(){
            this->ispoint = false;
        }

        double getX() const{
            return x;
        }

        double getY() const{
            return y;
        }

        double getZ() const{
            return z;
        }

        double getAxis(int i) const{
            if(i==0){
                return getX();
            }else if(i==1){
                return getY();
            }else{
                return getZ();
            }
        }

        vec3 operator + (vec3 const &obj) const{
            return vec3(x+obj.x,y+obj.y,z+obj.z);
        }

        vec3 operator - (vec3 const &obj) const{
            return vec3(x-obj.x,y-obj.y,z-obj.z);
        }

        vec3 operator / (double d) const{
            return vec3(x/d,y/d,z/d);
        }

        vec3 operator * (vec3 const &obj) const{
            double icomp = y*obj.z - z*obj.y;
            double jcomp = z*obj.x - x*obj.z;
            double kcomp = x*obj.y - y*obj.x;
            return vec3(icomp,jcomp,kcomp);
        }

        vec3 operator * (double alpha) const{
            return vec3(x*alpha,y*alpha,z*alpha);
        }

        bool operator < (double alpha) const{
            return (x<alpha) && (y<alpha) && (z<alpha);
        }

        bool operator <= (double alpha) const{
            return (x<=alpha) && (y<=alpha) && (z<=alpha);
        }

        bool operator > (double alpha) const{
            return (x>alpha) && (y>alpha) && (z>alpha);
        }

        bool operator >= (double alpha) const{
            return (x>=alpha) && (y>=alpha) && (z>=alpha);
        }

        bool operator != (vec3 const &obj) const{
            return (x!=obj.x) || (y!=obj.y) || (z!=obj.z);
        }

        bool operator == (vec3 const &obj) const{
            return (x==obj.x) && (y==obj.y) && (z==obj.z);
        }

        bool inBox(vec3 const &obj,double box) const{
            return (fabs(x-obj.x) <= box) && (fabs(y-obj.y) <= box) && (fabs(z-obj.z) <= box);
        }

        double dot(vec3 const &obj) const{
            return ((x*obj.x)+(y*obj.y)+(z*obj.z));
        }

        double magnitude() const{
            return sqrt(x*x + y*y + z*z);
        }

        vec3 getUnitVector() const{
            double d = magnitude();
            return vec3(x/d,y/d,z/d);
        }

        vec3 componentWiseMult(vec3 const &obj) const{
            return vec3(x*obj.x,y*obj.y,z*obj.z);
        }

        vec3 absolute() const{
            return vec3(fabs(x),fabs(y),fabs(z));
        }

        std::string toString() const{
            std::ostringstream ss;
            ss << "(" << x << ", " << y << ", " << z << ")";
            return ss.str();
        }
};

class ray{
    private:
        vec3 origin,direction;
        std::string medium; 

    public:
        ray(){
            origin = vec3(0,0,0);
            direction = vec3(0,0,0);
            medium = "air";
        }

        ray(vec3 o,vec3 d){
            origin = o;
            direction = d;
            medium = "air";
        }

        void setMediumGlass(){
            medium = "glass";
        }

        void setMediumAir(){
            medium = "air";
        }

        std::string getMedium() const{
            return medium;
        }

        vec3 getOrigin() const{
            return origin;
        }

        vec3 getDirection() const{
            return direction;
        }

        vec3 getPoint(const double a){
            return direction*a + origin;
        }

        std::string toString() const{
            std::ostringstream ss;
            ss << "origin: " << origin.toString() << "\n" << "direction: " << direction.toString();
            return ss.str();
        }
};

class color{
    private:
        int red;
        int green;
        int blue;
    public:
        color(){
            red = 0;
            green = 0;
            blue = 0;
        }
        color(int r,int g,int b){
            red = r;
            green = g;
            blue = b;            
        }

        int getRed() const{
            return red;
        }

        int getGreen() const{
            return green;
        }

        int getBlue() const{
            return blue;
        }

        color operator * (double d) const{
            return color(red*d,green*d,blue*d);
        }

        color operator + (color const &obj) const{
            return color(red+obj.getRed(),green+obj.getGreen(),blue+obj.getBlue());
        }

        color operator / (double d) const{
            return color(red/d,green/d,blue/d);
        }

        color operator * (color const &obj){
            return color((red*obj.red)/255,(green*obj.green)/255,(blue*obj.blue)/255);
        }

        std::string toString() const{
            std::ostringstream ss;
            ss << "(R:" <<red << ", G:" << green << ", B:" << blue << ")";
            return ss.str();
        }
};

class photon{
    private:
        ray photon_ray;
        color photon_color;
        int num_bounce;
        bool flag;
    public:
        photon(ray r){
            photon_ray = r;
            photon_color = color(255,255,255);
            num_bounce = 0;
            flag = false;
        }

        photon(ray r,color c){
            photon_ray = r;
            photon_color = c;
            num_bounce = 0;
        }

        int getBounce() const{
            return num_bounce;
        }

        ray getPhotonRay() const{
            return photon_ray;
        }

        color getPhotonColor() const{
            return photon_color;
        }

        void setFlag(){
            flag = true;
        }

        bool getFlag(){
            return flag;
        }

        void setPhotonColor(color c){
            photon_color = c;
        }

        void setPhotonRay(ray r){
            photon_ray = r;
        }

        void setBounce(int b){
            num_bounce = b;
        }

        void incrementBounce(){
            num_bounce++;
        }

        std::string toString() const{
            std::ostringstream ss;
            ss << photon_ray.toString() << "\n" << photon_color.toString();
            return ss.str();
        }
}; 