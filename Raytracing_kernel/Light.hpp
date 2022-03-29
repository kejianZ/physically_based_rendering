// Termm--Fall 2020

#pragma once

#include <iosfwd>

#include <glm/glm.hpp>
#include <random>

// Represents a simple point light. Here we assume a light is a 2D parallelogram, position defines one coner of the grame while x and y defines sides (set both of them a 0 gives a point light)
struct Light {
  Light();
  glm::vec4 random_pos();
  
  glm::vec3 colour;
  glm::vec4 position;
  glm::vec4 x;
  glm::vec4 y;
  double falloff[3];

  std::default_random_engine generator;
	std::uniform_real_distribution<double> uniform;
};

std::ostream& operator<<(std::ostream& out, const Light& l);
