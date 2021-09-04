// Termm--Fall 2020

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
	std::cout << m_vertices.size() << std::endl;
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

bool Mesh::hit(Ray ray, float t0, float t1, Record& record, Material *m)
{
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
