// Termm--Fall 2020

#pragma once

#include "../Object_attributes/Material.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>
#include "../Raytracing_kernel/Ray.hpp"
#include "../Raytracing_kernel/Record.hpp"
#include "../Radiosity/Radiosity_Kernel.hpp"

enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode
};

class SceneNode {
public:
    SceneNode(const std::string & name);

	SceneNode(const SceneNode & other);

    virtual ~SceneNode();
    
	int totalSceneNodes() const;
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;
    
    void set_transform(const glm::mat4& m);
    
    void add_child(SceneNode* child);
    
    void remove_child(SceneNode* child);

	//-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);


	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;
    
    std::list<SceneNode*> children;

	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;

    virtual bool hit(Ray ray, float t0, float t1, Record& record, bool this_hit = false);
    virtual void divide_patch(Radiosity_Kernel &rd_kernel, mat4 cumulative_trans=mat4(1));
private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};
