#include<iostream>
#include"utils.h"
#include"constants.cpp"
#include<vector>

#ifndef SHAPES_H
#define SHAPES_H

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
        raytrace(vec3 point,ray incident,vec3 normal,ray reflected,ray refracted,color colour);
        vec3 getIntersection() const;
        ray getIncident() const;
        color getColor() const;
        vec3 getNormal() const;
        ray getReflected() const;
        ray getRefracted() const;

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

        shape(vec3 c,vec3 n,color co,double d,double s,double reflec,double refrac,double refrac_index);
        vec3 getCenter() const;
        vec3 getNormal() const;
        color getColor() const;
        double getDiffuse() const;
        double getSpecular() const;
        double getReflection() const;
        double getRefraction() const;
        double getRefractiveIndex() const;
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
        sphere(vec3 c,vec3 n,double r,double var,color co,double d,double s,double reflec,double refrac,double refrac_index);
        double getRadius() const;
        virtual bool shapeType(std::string s) const;
        virtual bool willIntersect(ray r) const;
        virtual double getIntersectionDistance(ray r) const;
        virtual raytrace getIntersection(ray r) const;

};

class plane : public shape{
    private:
        double length;
    public:
        plane(vec3 c,vec3 n,double l,color co,double d,double s,double reflec,double refrac,double refrac_index);
        double getLength() const;
        virtual bool shapeType(std::string s) const;
        virtual bool willIntersect(ray r) const;
        virtual double getIntersectionDistance(ray r) const;
        virtual raytrace getIntersection(ray r) const;


};

class light : public shape{
    private:
        double length;
    public:
        light(vec3 c,vec3 n,double l,color co,double d,double s,double reflec,double refrac,double refrac_index);
        double getLength() const;
        virtual bool shapeType(std::string s) const;
        vec3 getMidPoint();
        bool willIntersect(ray r) const;
        double getIntersectionDistance(ray r) const;
        raytrace getIntersection(ray r) const;

};

#endif