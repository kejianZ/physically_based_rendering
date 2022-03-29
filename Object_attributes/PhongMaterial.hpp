// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"
#include "../IO_interface/Image.hpp"
#include <string>

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, int type);
  PhongMaterial(std::string filename, uint width, uint height, const glm::vec3& ks, double shininess, int t);
  virtual ~PhongMaterial();
  glm::vec3 diffuse();
  glm::vec3 spectular();
  glm::vec3 diffuse(double x, double y);
  double shininess();

private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;

  Image texture;
};

class Dielectric : public PhongMaterial {
public:
  Dielectric(const glm::vec3& kd, const glm::vec3& ks, double shininess, const glm::vec3& kr, double ri);
  double refractive();
  double reflectance();
  glm::vec3 kr();

private:
  glm::vec3 m_kr;
  double m_reflectance;
  double refractive_index;
};