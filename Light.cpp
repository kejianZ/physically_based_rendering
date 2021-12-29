// Termm--Fall 2020

#include <iostream>

#include <glm/ext.hpp>

#include "Light.hpp"

Light::Light()
  : colour(0.0, 0.0, 0.0),
    position(0.0, 0.0, 0.0, 1.0)
{
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;

	uniform = std::uniform_real_distribution<double>(0, 1);
  x = glm::vec4(10.0, 0.0, 0.0, 0.0);
  y = glm::vec4(0.0, 0.0, 10.0, 0.0);
}

glm::vec4 Light::random_pos()
{
  return position + uniform(generator) * x + uniform(generator) * y;
}

std::ostream& operator<<(std::ostream& out, const Light& l)
{
  out << "L[" << glm::to_string(l.colour) 
  	  << ", " << glm::to_string(l.position) << ", ";
  for (int i = 0; i < 3; i++) {
    if (i > 0) out << ", ";
    out << l.falloff[i];
  }
  out << "]";
  return out;
}
