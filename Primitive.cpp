// Termm--Fall 2020

#include "Primitive.hpp"
#include "polyroots.hpp"
#include <iostream>
#include <glm/ext.hpp>
using namespace std;

bool Primitive::in_range(double root, float t0, float t1)
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

bool Sphere::hit(Ray ray, float t0, float t1, Record& record, Material *m) 
{   
    double qua_a = dot(ray.Direction, ray.Direction);
    vec4 tem = ray.Origin - m_pos;
    double qua_b = 2 *  dot(ray.Direction, tem);
    double qua_c = dot(tem, tem) - 1;
    double roots[2];
    int root_num = quadraticRoots(qua_a, qua_b, qua_c, roots);

    if(root_num == 1)
    {
        if(!in_range(roots[0], t0, t1)) 
            return false;
        if(ray.Type == RayType::ShadowRay)      // shadow ray only check for hits
        {
            record.shadow_write();
            return true;
        }
        vec4 intersection = ray.pos_at(roots[0]);    
        return record.over_write(roots[0], intersection - m_pos, intersection, m);
    }
    else if(root_num == 2)
    {
        int flag = 0;
        if(in_range(roots[0], t0, t1)) flag += 1;
        if(in_range(roots[1], t0, t1)) flag += 2;

        float close;
        if(flag == 0) 
            return false;
        else if(ray.Type == RayType::ShadowRay)
        {
            record.shadow_write();
            return true;
        }
        else if(flag == 1) close = roots[0];
        else if(flag == 2) close = roots[1];
        else close = roots[0] < roots[1]? roots[0]:roots[1];
        
        vec4 intersection = ray.pos_at(close);
        return record.over_write(close, intersection - m_pos, intersection, m);
    }
    return false;
}

vec4 cube_normal(vec4 inter)
{
    float max_abs = -1;
    int index = 0;
    bool positive;
    for(int i = 0; i < 3; i++)
    {
        float dis = inter[i];
        if(abs(dis) > max_abs)
        {
            index = i;
            max_abs = abs(dis);
            positive = dis > 0;
        }
    }
    
    if(index == 0)
        return positive? vec4(1,0,0,0) : vec4(-1,0,0,0);
    else if(index == 1)
        return positive? vec4(0,1,0,0) : vec4(0,-1,0,0);
    else
        return positive? vec4(0,0,1,0) : vec4(0,0,-1,0);
}

bool Cube::hit(Ray ray, float t0, float t1, Record& record, Material *m)
{
    ray.cal_inv();
    float tmin, tmax, tymin, tymax, tzmin, tzmax; 
 
    tmin = (bounds[ray.sign[0]][0] - ray.Origin[0]) * ray.Inv_dir[0]; 
    tmax = (bounds[1-ray.sign[0]][0] - ray.Origin[0]) * ray.Inv_dir[0]; 
    tymin = (bounds[ray.sign[1]][1] - ray.Origin[1]) * ray.Inv_dir[1]; 
    tymax = (bounds[1-ray.sign[1]][1] - ray.Origin[1]) * ray.Inv_dir[1]; 
 
    if ((tmin > tymax) || (tymin > tmax)) 
        return false;
    if (tymin > tmin) 
        tmin = tymin; 
    if (tymax < tmax) 
        tmax = tymax; 
 
    tzmin = (bounds[ray.sign[2]][2] - ray.Origin[2]) * ray.Inv_dir[2];
    tzmax = (bounds[1-ray.sign[2]][2] - ray.Origin[2]) * ray.Inv_dir[2];
 
    if ((tmin > tzmax) || (tzmin > tmax)) 
        return false;
    if (tzmin > tmin) 
        tmin = tzmin; 
    if (tzmax < tmax) 
        tmax = tzmax; 
    
    int flag = 0;
    if(in_range(tmin, t0, t1)) flag += 1;
    if(in_range(tmax, t0, t1)) flag += 2;

    if(flag == 0) return false;
    if(ray.Type == RayType::ShadowRay)
    {
        record.shadow_write();
        return true;
    }
    if(flag == 2) tmin = tmax;

    vec4 intersection = ray.pos_at(tmin);
    return record.over_write(tmin, cube_normal(intersection), intersection, m);
} 