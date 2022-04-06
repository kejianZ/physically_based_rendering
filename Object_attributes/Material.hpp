// Termm--Fall 2020

#pragma once

class Material {
public:
  virtual ~Material();
  /* Types:
  0 - Specular material
  1 - Dielectric
  2 - Diffuse material
  3 - Texture (Assuming diffuse)
  4 - Light
  */
  int type;
protected:
  Material();
};
