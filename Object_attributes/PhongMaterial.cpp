// Termm--Fall 2020

#include "PhongMaterial.hpp"
#include <iostream>
using namespace std;

PhongMaterial::PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, int t)
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
{
	type = t;
}

PhongMaterial::PhongMaterial(std::string filename, uint width, uint height, const glm::vec3& ks, double shininess, int t)
	: m_ks(ks)
	, m_shininess(shininess)
	, texture(filename, width, height)
{
	type = 3;
}

PhongMaterial::PhongMaterial(const glm::vec3& kd, const glm::vec3& emis, int t)
	: m_kd(kd)
	, m_emis(emis)
{
	type = t;
}

PhongMaterial::~PhongMaterial() {}

glm::vec3 PhongMaterial::spectular() { return m_ks; }
double PhongMaterial::shininess() { return m_shininess; }
glm::vec3 PhongMaterial::diffuse() { return m_kd; }
glm::vec3 PhongMaterial::emission() { return type == 4? m_emis:glm::vec3(0,0,0); }
glm::vec3 PhongMaterial::diffuse(double x, double y) { return texture.texture_col(x, y);  }

Dielectric::Dielectric(const glm::vec3& kd, const glm::vec3& ks, double shininess, const glm::vec3& kr, double ri)
	: PhongMaterial(kd, ks, shininess, 1)
	, m_kr(kr)
	, refractive_index(ri)
{
	m_reflectance = std::pow(refractive_index - 1, 2) / std::pow(refractive_index + 1, 2);
}

double Dielectric::refractive() { return refractive_index; }
glm::vec3 Dielectric::kr() { return m_kr; }
double Dielectric::reflectance() { return m_reflectance; }