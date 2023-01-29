// Student Name: Ivan Castaneda
// Student ID: 861311076
#include "plane.h"
#include "hit.h"
#include "ray.h"
#include <cfloat>
#include <limits>

Plane::Plane(const Parse* parse,std::istream& in)
{
    in>>name>>x>>normal;
    normal=normal.normalized();
}

// Intersect with the plane.  The plane's normal points outside.
Hit Plane::Intersection(const Ray& ray, int part) const
{
    Hit intersect;

    if (dot(ray.direction, normal) != 0)
    {
        float t = dot((x - ray.endpoint), normal) / dot(ray.direction, normal);
        if (t >= small_t)
        {
            intersect.dist = t;
        }
    }
    return intersect;
}

vec3 Plane::Normal(const Ray& ray, const Hit& hit) const
{
    return normal;
}

std::pair<Box,bool> Plane::Bounding_Box(int part) const
{
    Box b;
    b.Make_Full();
    return {b,true};
}
