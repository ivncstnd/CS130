// Student Name: Ivan Castaneda
// Student ID: 861311076
#include "sphere.h"
#include "ray.h"

Sphere::Sphere(const Parse* parse, std::istream& in)
{
    in>>name>>center>>radius;
}

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    Hit intersect;
    double b = 2 * dot(ray.endpoint - center, ray.direction);
    double c = dot(ray.endpoint - center, ray.endpoint - center) - radius * radius;
    double discriminant = b * b - 4 * c;
    if (discriminant <= 0)
    {
        intersect.dist = -1; // did not intersect
    }
    else
    {
        double t1 = (-b + sqrt(discriminant)) / 2;
        double t2 = (-b - sqrt(discriminant)) / 2;
        if(t1 >= small_t && t2 >= small_t)
        {
            intersect.dist = std::min(t1, t2);
        }
        else if(t1 < small_t && t2 < small_t)   
        {
            intersect.dist = -1;
        }
        else
        {
            intersect.dist = std::max(t1, t2);
        }
    }
    return intersect;
}

vec3 Sphere::Normal(const Ray& ray, const Hit& hit) const
{
    vec3 normal;
    normal = (ray.Point(hit.dist) - center).normalized();
    return normal;
}


std::pair<Box,bool> Sphere::Bounding_Box(int part) const
{
    return {{center-radius,center+radius},false};
}
