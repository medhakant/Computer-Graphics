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
        scene();

        void addLight(light* l);

        void addSphere(shape* s);

        void addPlane(plane* p);

        void addVisible(shape* s);

        void render(GLubyte* data,double xoffset,double yoffset) const;

        bool isLightBlocked(ray r) const;

        color getIntersectionColor(ray r,int depth) const;

        color getLightIntersection(raytrace rt,shape* s) const;

};