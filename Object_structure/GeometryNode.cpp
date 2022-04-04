// Termm--Fall 2020

#include "GeometryNode.hpp"
#include <iostream>
#include <glm/ext.hpp>
//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

bool GeometryNode::hit(Ray ray, float t0, float t1, Record& record, bool this_hit)
{
	bool intersect = m_primitive->hit(ray.transform(invtrans), t0, t1, record, m_material);
	if(ray.Type == RayType::ShadowRay && record.hit) return true;
	if(intersect) record.hit_node = m_nodeId;
	return SceneNode::hit(ray, t0, t1, record, intersect);
}

void GeometryNode::divide_patch(Radiosity_Kernel &rd_kernel, mat4 cumulative_trans)
{
	m_primitive->divide_patch(rd_kernel, trans * cumulative_trans);
	SceneNode::divide_patch(rd_kernel, trans);
}