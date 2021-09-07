// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, int type);
  virtual ~PhongMaterial();
  glm::vec3 diffuse();
  glm::vec3 spectular();
  double shininess();

private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
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