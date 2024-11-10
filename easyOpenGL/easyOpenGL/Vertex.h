#pragma once
#include "glm/glm.hpp"
using glm::vec3;
using glm::vec2;

struct Vertex 
{
	vec3 pos;
	vec3 color;
	vec3 norm;
	vec3 binorm;
	vec3 tgt;
	vec2 uv;
	vec2 uvBlend;
};