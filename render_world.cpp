// Student Name: Ivan Castaneda
// Student ID: 861311076
#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"

extern bool enable_acceleration;

Render_World::~Render_World()
{
    for(auto a:all_objects) delete a;
    for(auto a:all_shaders) delete a;
    for(auto a:all_colors) delete a;
    for(auto a:lights) delete a;
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
std::pair<Shaded_Object,Hit> Render_World::Closest_Intersection(const Ray& ray) const
{
    Debug_Scope debug;
    std::pair<Shaded_Object, Hit> p;
    double min_t = std::numeric_limits<double>::max();
    for(auto o : objects)
    {
        Hit curr = o.object->Intersection(ray, 0);
        if(curr.dist < 0)
            Pixel_Print("no intersection with ", o.object->name);
        else
            Pixel_Print("intersect test with ", o.object->name,"; hit: ", curr);
        if(curr.dist <= min_t && curr.dist >= small_t)
        {
            min_t = curr.dist;
            p.first = o;
            p.second = curr;
        }
    }
    if(p.second.Valid())
        Pixel_Print("closest intersection; obj: ", p.first.object->name,"; hit: ", p.second);
    else
        Pixel_Print("closest intersection; none");
    return p;
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    Ray ray;
    ray.endpoint = camera.position;
    ray.direction = (camera.World_Position(pixel_index) - ray.endpoint).normalized();
    vec3 color = Cast_Ray(ray, 1);
    camera.Set_Pixel(pixel_index,Pixel_Color(color));
}

void Render_World::Render()
{
    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++)
            Render_Pixel(ivec2(i,j));
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth) const
{
    vec3 color;
    Debug_Scope debug;
    Pixel_Print("cast ray ", ray);
    std::pair<Shaded_Object, Hit> p = Closest_Intersection(ray);
    if(p.second.Valid())
    {
        Hit closest_hit = p.second; 
        Pixel_Print("ray: ", ray,"; closest_hit.dist: ", closest_hit.dist);
        vec3 intersection_point = ray.Point(closest_hit.dist);
        vec3 normal = p.first.object->Normal(ray, closest_hit);
        Pixel_Print("call Shade_Surface with location ", intersection_point,"; normal: ", normal);
        color = p.first.shader->Shade_Surface(*this, ray, closest_hit, intersection_point, normal, recursion_depth);
    }
    else
    {
        if(background_shader)
        {
            color = background_shader->Shade_Surface(*this, ray, p.second, vec3(0, 0, 0), vec3(0, 0, 0), recursion_depth);
            Pixel_Print("call Shader_Surface with background_shader ", background_shader->name,"; color: ", color);
        }
        else
        {
            color = vec3(0, 0, 0);
        }
    }
    return color;
}
