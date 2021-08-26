// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"
// #include "Ray.hpp"
using namespace glm;

class Render
{
private:
	SceneNode *Root;
	Image & Graph;

	const vec3 Eye;
	const vec3 View;
	const vec3 Up;
	double Fovy;

	const glm::vec3 &Ambient;
	const std::list<Light *> &Lights;

	bool perspective = true;

public:
	Render(SceneNode *root,
		   Image &image,
		   const glm::vec3 &eye,
		   const glm::vec3 &view,
		   const glm::vec3 &up,
		   double fovy,
		   const glm::vec3 &ambient,
		   const std::list<Light *> &lights);
	~Render();

	void run();
	void print_info();
	// Ray view_ray(uint x, uint y);
};
