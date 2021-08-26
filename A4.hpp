// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"
using namespace glm;

void A4_Render(
	// What to render
	SceneNode *root,

	// Image to write to, set to a given width and height
	Image & image,

	// Viewing parameters
	const glm::vec3 &eye,
	const glm::vec3 &view,
	const glm::vec3 &up,
	double Fovy,

	// Lighting parameters
	const glm::vec3 &ambient,
	const std::list<Light *> &lights);

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
};
