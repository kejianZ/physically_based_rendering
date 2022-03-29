// Termm--Fall 2020

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
	, first(true)
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			record_vertice(vx, vy, vz);
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
	bounds[0] = glm::vec3(min_x, min_y, min_z);
	bounds[1] = glm::vec3(max_x, max_y, max_z);
	std::cout << to_string(bounds[0]) << ' ' << to_string(bounds[1]) << std::endl;
	std::cout << fname << ": " << m_vertices.size() << std::endl;
}

void Mesh::record_vertice(double x, double y, double z)
{
	if(first)
	{
		max_x = x;	min_x = x;
		max_y = y;	min_y = y;
		max_z = z;	min_z = z;
		first = false;
		return;
	}

	if(x > max_x) max_x = x;
	else if(x < min_x) min_x = x;

	if(y > max_y) max_y = y;
	else if(y < min_y) min_y = y;

	if(z > max_z) max_z = z;
	else if(z < min_z) min_z = z;
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

// bool in_range(double root, float t0, float t1)
// {
//     return root > t0 && root < t1;
// }

bool Mesh::hit_bb(Ray ray, float t0, float t1)
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

	return in_range(tmin, t0, t1) || in_range(tmin, t0, t1);
} 

bool Mesh::hit(Ray ray, float t0, float t1, Record& record, Material *m)
{
	if(!hit_bb(ray, t0, t1)) return false;
	bool cum_hit = false;
	for(Triangle tri : m_faces)
	{
		float res;
		if(tri_hit(ray, m_vertices[tri.v1], m_vertices[tri.v2], m_vertices[tri.v3], res))
		{
			if(res > t0 && res < t1)
			{
				vec3 face_nor = cross(m_vertices[tri.v2] - m_vertices[tri.v1], m_vertices[tri.v3] - m_vertices[tri.v2]);
				record.over_write(res, vec4(face_nor, 0), ray.pos_at(res), m);
				cum_hit = true;
			}
		}
	}
	return cum_hit;
}

// https://github.com/stharlan/oglwalker/blob/768c3fb65ebf831f20dae0747b72d9f959f1f121/OglWalker/GeometryOperations.cpp
bool Mesh::tri_hit(Ray ray, vec3 v0, vec3 v1, vec3 v2, float &result)
{
	const float EPSILON = 0.0000001f;
	glm::vec3 edge1, edge2, h, s, q;
	float a, f, u, v;
	edge1 = v1 - v0;
	edge2 = v2 - v0;

	vec3 direction = vec3(ray.Direction);

	h = glm::cross(direction, edge2);
	a = glm::dot(edge1, h);
	if (a > -EPSILON && a < EPSILON)
		return false;

	f = 1 / a;
	s = vec3(ray.Origin) - v0;
	u = f * (glm::dot(s, h));
	if (u < 0.0 || u > 1.0)
		return false;

	q = glm::cross(s, edge1);
	v = f * glm::dot(direction, q);
	if (v < 0.0 || u + v > 1.0)
		return false;
	// At this stage we can compute t to find out where the intersection point is on the line.

	float t = f * glm::dot(edge2, q);
	if (t > EPSILON) // ray intersection
	{
		result = t;
		return true;
	}

    return false;
}