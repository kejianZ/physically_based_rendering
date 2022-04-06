// Termm--Fall 2020

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"
#include "PhongMaterial.hpp"

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

Surface::Surface()
	: hit_vertices()
{
	hit_vertices.push_back(glm::vec3(0.5, 0, 0.5));
	hit_vertices.push_back(glm::vec3(-0.5, 0, 0.5));
	hit_vertices.push_back(glm::vec3(0.5, 0, -0.5));
	hit_vertices.push_back(glm::vec3(-0.5, 0, -0.5));
}

bool Surface::hit(Ray ray, float t0, float t1, Record& record, Material *m)
{
	bool rough_hit = false;
	float res;
	if(tri_hit(ray, hit_vertices[0], hit_vertices[1], hit_vertices[2], res))
	{
		if(res > t0 && res < t1)
		{
			record.over_write(res, vec4(0,1,0,0), ray.pos_at(res), m);
			rough_hit = true;
		}
	}
	if(tri_hit(ray, hit_vertices[1], hit_vertices[2], hit_vertices[3], res))
	{
		if(res > t0 && res < t1)
		{
			record.over_write(res, vec4(0,1,0,0), ray.pos_at(res), m);
			rough_hit = true;
		}
	}
	if(!rough_hit) return false;

	bool hit_one = false;
	for(int i = 0; i < m_faces.size(); i++)
	{
		Triangle tri = m_faces[i];
		float res;
		if(tri_hit(ray, m_vertices[tri.v1], m_vertices[tri.v2], m_vertices[tri.v3], res))
		{
			if(res > t0 && res < t1)
			{
				record.over_write(res, vec4(0,0,0,0), ray.pos_at(res), m);
				record.radiosity_color = vec3(0, 0.01 * i, 0.01 * i);
			}
			break;
		}
	}
	return true;
}

void Surface::divide_patch(Radiosity_Kernel &rd_kernel, mat4 trans, Material* mat)
{
	p_trans = trans;
	p_mat = mat;
	if(mat->type == 4) divide_patch(1.0, 1.0, rd_kernel, trans, mat);
	else divide_patch(0.2, 0.2, rd_kernel, trans, mat);
}

void Surface::draw_primitive(Rasterization *rt_kernel, vec3 *patches)
{
	if(p_mat->type == 4) draw_primitive(1.0, 1.0, rt_kernel, patches);
	else draw_primitive(0.2, 0.2, rt_kernel, patches);
}

void Surface::draw_primitive(float delt_x, float delt_y, Rasterization *rt_kernel, vec3 *patches)
{
	int row_patch_count = 1 / delt_x, col_patch_count = 1 / delt_y;
	int vertexs_count = (row_patch_count + 1) * (col_patch_count + 1);	// number of vertexs on a rectangle surface
	int patch_count = row_patch_count * col_patch_count;				// number of rectangle patches on the surface
	
	// ------------------------------------------------------------GPU needed data---------------------------------------------------------------
	float vertexs[ vertexs_count * 3];									
	int lower_tri_id = 0, upper_tri_id = patch_count;
	int index[ patch_count * 6 ];										// The index array teaches OpenGL how to draw triangles
	vec3 vec_colors[vertexs_count];
	int vert_patch_count[vertexs_count] = {0};

	for(int i = 0; i < col_patch_count + 1; i++)
	{
		for(int j = 0; j < row_patch_count + 1; j++)
		{
			vec4 tri_vertex = p_trans * vec4(j * delt_x - 0.5, 0, - i * delt_y + 0.5, 1);
			int ver_ind = i * (row_patch_count + 1) + j;
			vertexs[ver_ind * 3] = tri_vertex[0];
			vertexs[ver_ind * 3 + 1] = tri_vertex[1];
			vertexs[ver_ind * 3 + 2] = tri_vertex[2];
			if(i != col_patch_count && j != row_patch_count) 
			{
				index[lower_tri_id * 3] = ver_ind;
				index[lower_tri_id * 3 + 1] = ver_ind + 1;
				index[lower_tri_id * 3 + 2] = ver_ind + row_patch_count + 1;
				vec3 patch_color = patches[lower_tri_id];
				vec_colors[ver_ind] += patch_color; 					vert_patch_count[ver_ind] += 1;
				vec_colors[ver_ind+1] += patch_color;					vert_patch_count[ver_ind+1] += 1;
				vec_colors[ver_ind+row_patch_count+1] += patch_color;	vert_patch_count[ver_ind+row_patch_count+1] += 1;
				lower_tri_id++;				
			}

			if(i != 0 && j != 0) 
			{
				index[upper_tri_id * 3] =  ver_ind;
				index[upper_tri_id * 3 + 1] = ver_ind - 1;
				index[upper_tri_id * 3 + 2] = ver_ind - row_patch_count - 1;
				vec3 patch_color = patches[upper_tri_id];
				vec_colors[ver_ind] += patch_color;						vert_patch_count[ver_ind] += 1;
				vec_colors[ver_ind-1] += patch_color;					vert_patch_count[ver_ind-1] += 1;
				vec_colors[ver_ind-row_patch_count-1] += patch_color;	vert_patch_count[ver_ind-row_patch_count-1] += 1;
				upper_tri_id++;				
			}
		}
	}
	float colors[3 * vertexs_count] = {0};
	for(int i = 0; i < vertexs_count; i++)
	{
		colors[3 * i] = vec_colors[i][0] / vert_patch_count[i]; 
		colors[3 * i+1] = vec_colors[i][1] / vert_patch_count[i]; 
		colors[3 * i+2] = vec_colors[i][2] / vert_patch_count[i];
	}
	rt_kernel->add_patch(vertexs, NULL, colors, index, vertexs_count, patch_count * 6, 1);
}

// For [Surface], delt_x, delt_y must divides 1
void Surface::divide_patch(float delt_x, float delt_y, Radiosity_Kernel &rd_kernel, mat4 trans, Material* mat)
{
	int row_patch_count = 1 / delt_x, col_patch_count = 1 / delt_y;
	int vertexs_count = (row_patch_count + 1) * (col_patch_count + 1);	// number of vertexs on a rectangle surface
	int patch_count = row_patch_count * col_patch_count;				// number of rectangle patches on the surface
	
	// ------------------------------------------------------------GPU needed data---------------------------------------------------------------
	float vertexs[ vertexs_count * 6];									// for each vertexs store twice to assign identical ID for each triangle
	bool *triangle_vertex_mask = new bool[ vertexs_count * 2];			// each rectangle patches is divide into 2 triangle patches
																		// This mask clarifies whether this vertexs is used to identify a patch
	int lower_tri_id = 0, upper_tri_id = patch_count;
	int index[ patch_count * 6 ];										// The index array teaches OpenGL how to draw triangles

	// ------------------------------------------------------------CPU needed data---------------------------------------------------------------
	vec4 *patch_oris = new vec4[ patch_count * 2 ];
	Frame *frames = new Frame[1];
	frames[0] = Frame(trans * vec4(0,0,-1,0), trans * vec4(0,1,0,0));

	for(int i = 0; i < col_patch_count + 1; i++)
	{
		for(int j = 0; j < row_patch_count + 1; j++)
		{
			vec4 tri_vertex = trans * vec4(j * delt_x - 0.5, 0, - i * delt_y + 0.5, 1);
			int ver_ind = i * (row_patch_count + 1) + j;
			vertexs[ver_ind * 3] = tri_vertex[0];
			vertexs[ver_ind * 3 + 1] = tri_vertex[1];
			vertexs[ver_ind * 3 + 2] = tri_vertex[2];
			if(i != col_patch_count && j != row_patch_count) 
			{
				index[lower_tri_id * 3] = ver_ind;
				index[lower_tri_id * 3 + 1] = ver_ind + 1;
				index[lower_tri_id * 3 + 2] = ver_ind + row_patch_count + 1;
				triangle_vertex_mask[ver_ind] = true;
				vec4 patch_ori = trans * vec4(j * delt_x - 0.5 + delt_x / 3, 0, - i * delt_y + 0.5 + delt_y / 3, 1);
				patch_oris[lower_tri_id] = patch_ori;
				lower_tri_id++;				
			}
			else 
			{
				triangle_vertex_mask[ver_ind] = false;
			}

			vertexs[(vertexs_count + ver_ind) * 3] = tri_vertex[0];
			vertexs[(vertexs_count + ver_ind) * 3 + 1] = tri_vertex[1];
			vertexs[(vertexs_count + ver_ind) * 3 + 2] = tri_vertex[2];
			if(i != 0 && j != 0) 
			{
				index[upper_tri_id * 3] =  vertexs_count + ver_ind;
				index[upper_tri_id * 3 + 1] = vertexs_count + ver_ind - 1;
				index[upper_tri_id * 3 + 2] = vertexs_count + ver_ind - row_patch_count - 1;
				triangle_vertex_mask[ver_ind + vertexs_count] = true;
				vec4 patch_ori = trans * vec4(j * delt_x - 0.5 - delt_x / 3, 0, - i * delt_y + 0.5 - delt_y / 3, 1);
				patch_oris[upper_tri_id] = patch_ori;
				upper_tri_id++;				
			}
			else 
			{
				triangle_vertex_mask[ver_ind + vertexs_count] = false;
			}
			// cout << ver_ind << ' ' << triangle_vertex_mask[ver_ind] << ' ' << vertexs[ver_ind * 3] << ' '<< vertexs[ver_ind * 3 + 1] << ' '<< vertexs[ver_ind * 3+2] << endl;
			// cout << ver_ind + vertexs_count << ' ' << triangle_vertex_mask[ver_ind + vertexs_count] << ' ' << vertexs[(vertexs_count + ver_ind) * 3] << ' '<< vertexs[(vertexs_count + ver_ind) * 3 + 1] << ' '<< vertexs[(vertexs_count + ver_ind) * 3 + 2] << endl;
		}
	}
	rd_kernel.primitive_feed_opengl(vertexs, triangle_vertex_mask, index, vertexs_count * 2, patch_count * 6);
	PhongMaterial *pm = static_cast<PhongMaterial *>(mat);
	rd_kernel.primitive_add_patch(patch_count * 2, patch_oris, frames, [] (int x) -> int {return 0;}, pm->diffuse(), pm->emission(), this);
}
