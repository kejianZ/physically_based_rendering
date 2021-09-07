// Termm--Fall 2020

#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, int t)
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
{
	type = t;
}

PhongMaterial::~PhongMaterial()
{}

glm::vec3 PhongMaterial::diffuse() { return m_kd; }
glm::vec3 PhongMaterial::spectular() { return m_ks; }
double PhongMaterial::shininess() { return m_shininess; }


Dielectric::Dielectric(const glm::vec3& kd, const glm::vec3& ks, double shininess, const glm::vec3& kr, double ri)
	: PhongMaterial(kd, ks, shininess, 1)
	, m_kr(kr)
	, refractive_index(ri)
{
}

double Dielectric::refractive() { return refractive_index; }