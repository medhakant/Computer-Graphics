#include "shapes.h"
#include "constants.h"
#include <map>
#include <set>
#include <GLFW/glfw3.h>
#include <iterator>
#include <algorithm>
#include <omp.h>

class scene{
    private:
        light * arealight;
        std::set<shape*> objects;
        std::vector<photon*> photonmap;
        int height;
        int width;

    public:
        scene();

        void addLight(light* l);

        void addSphere(shape* s);

        void addPlane(plane* p);

        photon* makePhoton() const;

        photon* getBounce(const photon* init_photon) const;

        void balancePhoton(int start,int end,int axis);

        color searchPhoton(int start,int end,int axis, vec3 intersection) const;

        void castPhoton();

        void render(GLubyte* data,double xoffset,double yoffset);

        color getIntersectionColor(ray r) const;

};