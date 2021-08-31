// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>
#include "Record.hpp"
#include "Ray.hpp"
#include "Material.hpp"

class Primitive {
public:
  virtual ~Primitive();
  virtual void hit(Ray ray, float t0, float t1, Record& record, Material *m) {}
};

class Sphere : public Primitive {
public:
  Sphere(const glm::vec3& pos, double radius)
    : m_radius(radius)
  {
    m_pos = vec4(pos, 1);
  }
  virtual ~Sphere();
  void hit(Ray ray, float t0, float t1, Record& record, Material *m) override;
private:
  glm::vec4 m_pos;
  double m_radius;
};

class Cube : public Primitive {
public:
  virtual ~Cube();
  void hit(Ray ray, float t0, float t1, Record& record, Material *m) override;
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
