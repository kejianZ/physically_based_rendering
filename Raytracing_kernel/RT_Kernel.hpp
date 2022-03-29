// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "../Object_structure/SceneNode.hpp"
#include "Light.hpp"
#include "../IO_interface/Image.hpp"
#include "Frame.hpp"
#include <random>
#include "../Object_attributes/PhongMaterial.hpp"

// #include "Ray.hpp"
using namespace glm;

class Render
{
private:
	SceneNode *Root;
	Image & Graph;

	vec4 Eye;
	vec4 View;
	vec4 Up;
	double Fovy;

	const glm::vec3 &Ambient;
	const std::list<Light *> &Lights;

	bool perspective = true;
	bool ez_mode = true;

	Frame cam_frame;

	std::default_random_engine generator;
  	std::normal_distribution<double> distribution = std::normal_distribution<double>(0,0.33);
	std::uniform_real_distribution<double> uniform = std::uniform_real_distribution<double>(0, 1);

	int max_hit;

	bool silouette = false;
	int sil_ring_num;
	int sil_ring_size;
	int sil_sample_size;
	float sil_ring_rad;
	float unit_r;
	float unit_ang;

	int shading_al;
	bool multi_thread;
	int quality;
	
public:
	Render(SceneNode *root,
		   Image &image,
		   glm::vec3 &eye,
		   glm::vec3 &view,
		   glm::vec3 &up,
		   double fovy,
		   const glm::vec3 &ambient,
		   const std::list<Light *> &lights);
	~Render();

	void set_parameters();
	void run();
	void print_info();
	void shade_pixel(int x, int y, vec3 color);
	vec3 recursive_ray_color(Ray ray, int hit_count);
	vec3 pix_operation(float radius, vec4 center);
	bool cal_silouette(Ray ray, Record record, vec3 &pix_color);
	vec3 gooch_color(Record record, vec4 view, vec3 &cumulative_km);
	vec3 cal_color(Record record, vec4 view, PhongMaterial* pm);
};
