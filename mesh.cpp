#include "mesh.h"
#include <fstream>
#include <limits>
#include <string>
#include <algorithm>

// Consider a triangle to intersect a ray if the ray intersects the plane of the
// triangle with barycentric weights in [-weight_tolerance, 1+weight_tolerance]
static const double weight_tolerance = 1e-4;

Mesh::Mesh(const Parse* parse, std::istream& in)
{
    std::string file;
    in>>name>>file;
    Read_Obj(file.c_str());
}

// Read in a mesh from an obj file.  Populates the bounding box and registers
// one part per triangle (by setting number_parts).
void Mesh::Read_Obj(const char* file)
{
    std::ifstream fin(file);
    if(!fin)
    {
        exit(EXIT_FAILURE);
    }
    std::string line;
    ivec3 e, t;
    vec3 v;
    vec2 u;
    while(fin)
    {
        getline(fin,line);

        if(sscanf(line.c_str(), "v %lg %lg %lg", &v[0], &v[1], &v[2]) == 3)
        {
            vertices.push_back(v);
        }

        if(sscanf(line.c_str(), "f %d %d %d", &e[0], &e[1], &e[2]) == 3)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
        }

        if(sscanf(line.c_str(), "vt %lg %lg", &u[0], &u[1]) == 2)
        {
            uvs.push_back(u);
        }

        if(sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &e[0], &t[0], &e[1], &t[1], &e[2], &t[2]) == 6)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
            for(int i=0;i<3;i++) t[i]--;
            triangle_texture_index.push_back(t);
        }
    }
    num_parts=triangles.size();
}

// Check for an intersection against the ray.  See the base class for details.
Hit Mesh::Intersection(const Ray& ray, int part) const
{
    Debug_Scope debug;
    Hit h;
    double min_t = std::numeric_limits<double>::max();
    for(int i = 0; i < triangles.size(); ++i)
    {
        Hit curr = Intersect_Triangle(ray, i);
        if (curr.dist <= min_t && curr.dist >= small_t)
        {
            min_t = curr.dist;
            h = curr;
        }

    }
    return h;
}

// Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const Ray& ray, const Hit& hit) const
{
    assert(hit.triangle>=0);
    vec3 A = vertices[triangles[hit.triangle][0]];
    vec3 B = vertices[triangles[hit.triangle][1]];
    vec3 C = vertices[triangles[hit.triangle][2]];
    vec3 n = cross(A - B, A - C).normalized();
    return n;
}

// This is a helper routine whose purpose is to simplify the implementation
// of the Intersection routine.  It should test for an intersection between
// the ray and the triangle with index tri.  If an intersection exists,
// record the distance and return true.  Otherwise, return false.
// This intersection should be computed by determining the intersection of
// the ray and the plane of the triangle.  From this, determine (1) where
// along the ray the intersection point occurs (dist) and (2) the barycentric
// coordinates within the triangle where the intersection occurs.  The
// triangle intersects the ray if dist>small_t and the barycentric weights are
// larger than -weight_tolerance.  The use of small_t avoid the self-shadowing
// bug, and the use of weight_tolerance prevents rays from passing in between
// two triangles.
Hit Mesh::Intersect_Triangle(const Ray& ray, int tri) const
{
    vec3 A = vertices[triangles[tri][0]];
    vec3 B = vertices[triangles[tri][1]];
    vec3 C = vertices[triangles[tri][2]];

    vec3 u = ray.direction;
    vec3 v = B - A;
    vec3 w = C - A;

    vec3 n = cross(v, w);

    if(dot(u, n) == 0)
    {
        return {};
    }
    float t = dot((A - ray.endpoint), n) / dot(u, n);
    if (t < small_t)
    {
        return {};
    }
    vec3 p = ray.Point(t);
    vec3 y = p - A;
    double d = dot(cross(u, v), w);

    double gamma = dot(cross(u, v), y) / d;
    double beta = dot(cross(w, u), y) / d;
    double alpha = 1 - beta - gamma;

    if(alpha < -weight_tolerance || beta < -weight_tolerance || gamma < -weight_tolerance)
    {
        return {};
    }

    Hit h;
    h.dist = t;
    h.triangle = tri;
    return h;
}

std::pair<Box,bool> Mesh::Bounding_Box(int part) const
{
    if(part<0)
    {
        Box box;
        box.Make_Empty();
        for(const auto& v:vertices)
            box.Include_Point(v);
        return {box,false};
    }

    ivec3 e=triangles[part];
    vec3 A=vertices[e[0]];
    Box b={A,A};
    b.Include_Point(vertices[e[1]]);
    b.Include_Point(vertices[e[2]]);
    return {b,false};
}
