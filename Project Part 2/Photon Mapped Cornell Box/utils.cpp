#include"utils.h"

//vec3
vec3::vec3(){
    this->x = 0;
    this->y = 0;
    this->z = 0;
    this->ispoint = true;
}

vec3::vec3(double x,double y,double z){
    this->x = x;
    this->y = y;
    this->z = z;
    this->ispoint = true;
}

void vec3::isDirection(){
    this->ispoint = false;
}

double vec3::getX() const{
    return x;
}

double vec3::getY() const{
    return y;
}

double vec3::getZ() const{
    return z;
}

double vec3::getAxis(int i) const{
    if(i==0){
        return getX();
    }else if(i==1){
        return getY();
    }else{
        return getZ();
    }
}

vec3 vec3::operator + (vec3 const &obj) const{
    return vec3(x+obj.x,y+obj.y,z+obj.z);
}

vec3 vec3::operator - (vec3 const &obj) const{
    return vec3(x-obj.x,y-obj.y,z-obj.z);
}

vec3 vec3::operator / (double d) const{
    return vec3(x/d,y/d,z/d);
}

vec3 vec3::operator * (vec3 const &obj) const{
    double icomp = y*obj.z - z*obj.y;
    double jcomp = z*obj.x - x*obj.z;
    double kcomp = x*obj.y - y*obj.x;
    return vec3(icomp,jcomp,kcomp);
}

vec3 vec3::operator * (double alpha) const{
    return vec3(x*alpha,y*alpha,z*alpha);
}

bool vec3::operator < (double alpha) const{
    return (x<alpha) && (y<alpha) && (z<alpha);
}

bool vec3::operator <= (double alpha) const{
    return (x<=alpha) && (y<=alpha) && (z<=alpha);
}

bool vec3::operator > (double alpha) const{
    return (x>alpha) && (y>alpha) && (z>alpha);
}

bool vec3::operator >= (double alpha) const{
    return (x>=alpha) && (y>=alpha) && (z>=alpha);
}

bool vec3::operator != (vec3 const &obj) const{
    return (x!=obj.x) || (y!=obj.y) || (z!=obj.z);
}

bool vec3::operator == (vec3 const &obj) const{
    return (x==obj.x) && (y==obj.y) && (z==obj.z);
}

bool vec3::inBox(vec3 const &obj,double box) const{
    return (fabs(x-obj.x) <= box) && (fabs(y-obj.y) <= box) && (fabs(z-obj.z) <= box);
}

double vec3::dot(vec3 const &obj) const{
    return ((x*obj.x)+(y*obj.y)+(z*obj.z));
}

double vec3::magnitude() const{
    return sqrt(x*x + y*y + z*z);
}

vec3 vec3::getUnitVector() const{
    double d = magnitude();
    return vec3(x/d,y/d,z/d);
}

vec3 vec3::componentWiseMult(vec3 const &obj) const{
    return vec3(x*obj.x,y*obj.y,z*obj.z);
}

vec3 vec3::absolute() const{
    return vec3(fabs(x),fabs(y),fabs(z));
}

std::string vec3::toString() const{
    std::ostringstream ss;
    ss << "(" << x << ", " << y << ", " << z << ")";
    return ss.str();
}



//ray
ray::ray(){
    origin = vec3(0,0,0);
    direction = vec3(0,0,0);
    medium = "air";
}

ray::ray(vec3 o,vec3 d){
    origin = o;
    direction = d;
    medium = "air";
}

void ray::setMediumGlass(){
    medium = "glass";
}

void ray::setMediumAir(){
    medium = "air";
}

std::string ray::getMedium() const{
    return medium;
}

vec3 ray::getOrigin() const{
    return origin;
}

vec3 ray::getDirection() const{
    return direction;
}

vec3 ray::getPoint(const double a){
    return direction*a + origin;
}

std::string ray::toString() const{
    std::ostringstream ss;
    ss << "origin: " << origin.toString() << "\n" << "direction: " << direction.toString() << "Medium: " << medium;
    return ss.str();
}



//color
color::color(){
    red = 0;
    green = 0;
    blue = 0;
}

color::color(int r,int g,int b){
    red = r;
    green = g;
    blue = b;            
}

int color::getRed() const{
    return red;
}

int color::getGreen() const{
    return green;
}

int color::getBlue() const{
    return blue;
}

color color::operator * (double d) const{
    return color(red*d,green*d,blue*d);
}

color color::operator + (color const &obj) const{
    return color(red+obj.getRed(),green+obj.getGreen(),blue+obj.getBlue());
}

color color::operator / (double d) const{
    return color(red/d,green/d,blue/d);
}

color color::operator * (color const &obj){
    return color((red*obj.red)/255,(green*obj.green)/255,(blue*obj.blue)/255);
}

std::string color::toString() const{
    std::ostringstream ss;
    ss << "(R:" <<red << ", G:" << green << ", B:" << blue << ")";
    return ss.str();
}


//photon
photon::photon(ray r){
    photon_ray = r;
    photon_color = color(255,255,255);
    num_bounce = 0;
    flag = false;
    caustic = false;
}

photon::photon(ray r,color c){
    photon_ray = r;
    photon_color = c;
    num_bounce = 0;
    flag = false;
    caustic = false;
}

int photon::getBounce() const{
    return num_bounce;
}

ray photon::getPhotonRay() const{
    return photon_ray;
}

color photon::getPhotonColor() const{
    return photon_color;
}

void photon::setFlag(){
    flag = true;
}

bool photon::getFlag() const{
    return flag;
}

void photon::setCaustic(){
    caustic = true;
}

bool photon::getCaustic() const{
    return caustic;
}

void photon::setPhotonColor(color c){
    photon_color = c;
}

void photon::setPhotonRay(ray r){
    photon_ray = r;
}

void photon::setBounce(int b){
    num_bounce = b;
}

void photon::incrementBounce(){
    num_bounce++;
}

std::string photon::toString() const{
    std::ostringstream ss;
    ss << photon_ray.toString() << "\n" << photon_color.toString();
    return ss.str();
}