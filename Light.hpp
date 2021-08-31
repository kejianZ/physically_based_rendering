// Termm--Fall 2020

#pragma once

#include <iosfwd>

#include <glm/glm.hpp>

// Represents a simple point light.
struct Light {
  Light();
  
  glm::vec3 colour;
  glm::vec4 position;
  double falloff[3];
};

std::ostream& operator<<(std::ostream& out, const Light& l);
