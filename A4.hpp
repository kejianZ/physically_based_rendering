// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "Frame.hpp"

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

	Frame cam_frame;
	int max_hit;

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

	void run();
	void print_info();
	vec3 cal_color(Record record, vec4 view, vec3 &cumulative_km);
	void shade_pixel(int x, int y, vec3 color);
	vec3 pix_operation(float radius, vec4 center, int quality = 0);
	void single_ray_color(Ray v_ray, vec3& pix_color);
};
