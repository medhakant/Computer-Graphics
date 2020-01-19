#include<cmath>
#include<sstream>
#include<string>

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
            return vec3(abs(x),abs(y),abs(z));
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

    public:
        ray(){
            origin = vec3(0,0,0);
            direction = vec3(0,0,0);
        }

        ray(vec3 o,vec3 d){
            origin = o;
            direction = d;
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
};