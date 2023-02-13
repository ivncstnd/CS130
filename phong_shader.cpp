// Student Name: Ivan Castaneda
// Student ID: 861311076
#include "light.h"
#include "parse.h"
#include "object.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"

Phong_Shader::Phong_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    color_ambient=parse->Get_Color(in);
    color_diffuse=parse->Get_Color(in);
    color_specular=parse->Get_Color(in);
    in>>specular_power;
}

vec3 Phong_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    Debug_Scope debug;
    vec3 color;
    vec3 ambient;
    if(render_world.ambient_color)
        color = ambient = render_world.ambient_color->Get_Color(hit.uv) * render_world.ambient_intensity * this->color_ambient->Get_Color(hit.uv);
    Pixel_Print("ambient: ", ambient);
    vec3 diffuse, specular;
    for(auto light : render_world.lights)
    {   
        vec3 l = light->position - intersection_point;
        vec3 r = -l + 2.0 * dot(l, normal) * normal;
        diffuse = color_diffuse->Get_Color(hit.uv) * light->Emitted_Light(l) * std::max(dot(normal, l.normalized()), 0.0);
        specular = color_specular->Get_Color(hit.uv) * light->Emitted_Light(l) * pow(std::max(dot(-ray.direction, r.normalized()), 0.0), specular_power);
        if(!render_world.enable_shadows)
        {
            Pixel_Print("shading for light ", light->name, ": diffuse: ", diffuse,"; specular: ", specular);
            color += diffuse + specular;
            continue;
        }
        Ray shadow_ray(intersection_point, l);
        std::pair<Shaded_Object, Hit> shadow = render_world.Closest_Intersection(shadow_ray);
        if(!shadow.first.object || shadow.second.dist > l.magnitude())
        {
            color += diffuse + specular;
        }
        if(!shadow.first.object)
        {
            Pixel_Print("light ", light->name," not visible; obscured by object ", shadow.first.object->name," at location ", shadow_ray.Point(shadow.second.dist));
        }
        else
        {
            Pixel_Print("light ", light->name," visible; closest object on ray too far away (light dist: ", l.magnitude(), "; object dist: ", shadow.second.dist,")");
            Pixel_Print("shading for light ", light->name, ": diffuse: ", diffuse,"; specular: ", specular);
        }
    }
    Pixel_Print("final color ", color);
    return color;
}
