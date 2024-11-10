#pragma once
#include "glm/glm.hpp"
using namespace glm;
class Globals
{
public:
	static const int screenWidth;
	static const int screenHeight;
	static const vec3 OX;
	static const vec3 OY;
	static const vec3 OZ;
};

#define CONSTT(TYPE) TYPE const

#define VAR(TYPE) TYPE
#define CONST_VAR(TYPE) VAR(CONSTT(TYPE))
#define INPUT(TYPE) TYPE &
#define CONST_INPUT(TYPE) INPUT(CONSTT(TYPE))
#define ADR(TYPE) TYPE *
#define ADR_OF_CONST(TYPE) ADR(CONST_VAR(TYPE))
#define CONST_ADR(TYPE) CONSTT(ADR(TYPE))
#define CONST_ADR_OF_CONST(TYPE) CONSTT(ADR_OF_CONST(TYPE))
#define INPUT_ADR(TYPE) INPUT(ADR(TYPE))
#define INPUT_CONST_ADR(TYPE) INPUT(CONST_ADR(TYPE))
#define INPUT_ADR_OF_CONST(TYPE) INPUT(ADR_OF_CONST(TYPE))
#define INPUT_CONST_ADR_OF_CONST(TYPE) INPUT(CONST_ADR_OF_CONST(TYPE))