// Termm--Fall 2020

#pragma once

#include "SceneNode.hpp"
#include "../Object_attributes/Primitive.hpp"
#include "../Object_attributes/Material.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	void setMaterial( Material *material );

	Material *m_material;
	Primitive *m_primitive;

	bool hit(Ray ray, float t0, float t1, Record& record, bool this_hit) override;
	void divide_patch(Rasterization &raster, mat4 cumulative) override;
};
