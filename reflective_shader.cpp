#include "reflective_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Reflective_Shader::Reflective_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    shader=parse->Get_Shader(in);
    in>>reflectivity;
    reflectivity=std::max(0.0,std::min(1.0,reflectivity));
}

vec3 Reflective_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    Debug_Scope debug;
    vec3 object_color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);
    vec3 reflection_direction = ray.direction - 2 * dot(ray.direction, normal) * normal;
    Ray reflected_ray(intersection_point, reflection_direction);
    vec3 reflected_color;
    if (recursion_depth >= render_world.recursion_depth_limit)
    {
        Debug_Scope debug;
        Pixel_Print("cast ray ", reflected_ray);
        Pixel_Print("ray too deep; return black");
        reflected_color = vec3(0, 0, 0);
    }
    else 
    {
        reflected_color = render_world.Cast_Ray(reflected_ray, recursion_depth + 1);
    }
    vec3 color = (1 - reflectivity) * object_color + reflectivity * reflected_color;
    Pixel_Print("reflected ray: ", reflected_ray,"; reflected color: ", reflected_color,"; object color: ", object_color,"; final color: ", color);
    return color;
}
