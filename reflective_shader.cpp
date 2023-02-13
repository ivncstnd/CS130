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
    vec3 color;
    if (recursion_depth >= render_world.recursion_depth_limit)
    {
        return color = vec3(0, 0, 0);
    }

    vec3 reflection_direction = (ray.direction - 2 * dot(ray.direction, normal) * normal).normalized();
    Ray reflection(intersection_point, reflection_direction);

    color = render_world.Cast_Ray(reflection, recursion_depth + 1);

    return color;
}
