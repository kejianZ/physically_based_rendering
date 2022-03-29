// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>
#include "../Raytracing_kernel/Record.hpp"
#include "../Raytracing_kernel/Ray.hpp"
#include "Material.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual bool hit(Ray ray, float t0, float t1, Record& record, Material *m) { return false; }
  bool in_range(double root, float t0, float t1);
};

class Sphere : public Primitive {
public:
  Sphere()
  {
  }
  virtual ~Sphere();
  bool hit(Ray ray, float t0, float t1, Record& record, Material *m) override;
private:
  glm::vec4 m_pos = vec4(0, 0, 0, 1);
};

class Cube : public Primitive {
public:
  virtual ~Cube();
  bool hit(Ray ray, float t0, float t1, Record& record, Material *m) override;
private:
  glm::vec4 bounds[2] = {glm::vec4(-0.5f, -0.5f, -0.5f, 1), glm::vec4(0.5f, 0.5f, 0.5f, 1)};
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
};
