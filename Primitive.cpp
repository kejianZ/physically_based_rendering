// Termm--Fall 2020

#include "Primitive.hpp"
#include "polyroots.hpp"
#include <iostream>
using namespace std;

bool in_range(double root, float t0, float t1)
{
    return root > t0 && root < t1;
}

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

NonhierBox::~NonhierBox()
{
}

void Sphere::hit(Ray ray, float t0, float t1, Record& record, Material *m) 
{   
    double qua_a = dot(ray.Direction, ray.Direction);
    vec3 tem = ray.Origin - m_pos;
    double qua_b = 2 *  dot(ray.Direction, tem);
    double qua_c = dot(tem, tem) - m_radius * m_radius;
    double roots[2];
    int root_num = quadraticRoots(qua_a, qua_b, qua_c, roots);

    if(root_num == 1)
    {
        if(!in_range(roots[0], t0, t1)) return;
        if(ray.Type == RayType::ShadowRay)      // shadow ray only check for hits
        {
            record.shadow_write();
            return;
        }
        vec3 intersection = ray.pos_at(roots[0]);    
        record.over_write(roots[0], intersection - m_pos, intersection, m);
    }
    else if(root_num == 2)
    {
        int flag = 0;
        if(in_range(roots[0], t0, t1)) flag += 1;
        if(in_range(roots[1], t0, t1)) flag += 2;

        float close;
        if(flag == 0) return;
        else if(ray.Type == RayType::ShadowRay)
        {
            record.shadow_write();
            return;
        }
        else if(flag == 1) close = roots[0];
        else if(flag == 2) close = roots[1];
        else close = roots[0] < roots[1]? roots[0]:roots[1];
        
        vec3 intersection = ray.pos_at(close);
        record.over_write(close, intersection - m_pos, intersection, m);
    }
}