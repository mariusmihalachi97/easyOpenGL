#pragma once
#include "Globals.h"
#include <iostream>
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <GL/glew.h>
#include <GL/freeglut.h>
using namespace glm;
class Camera 
{

public:
	vec3 position, target, up, xAxis, yAxis, zAxis;
	GLfloat moveSpeed, rotateSpeed, cam_near, cam_far, cam_fov, deltaTime;
	mat4 viewMatrix, worldMatrix, projectionMatrix, ViewProjectionMatrix;
	
	void moveOx(GLint const &directie);
	void moveOy(GLint const &directie);
	void moveOz(GLint const &directie);
	void rotateOx(GLint const &directie);
	void rotateOy(GLint const &directie);
	void rotateOz(GLint const &directie);
	void updateWorldView();
	Camera(vec3 const &_position, vec3 const &_target, vec3 const &_up,
		   GLfloat const &_moveSpeed, GLfloat const &_rotateSpeed,
		   GLfloat const &_cam_near, GLfloat const &_cam_far, GLfloat const &_cam_fov);
	Camera() { std::cout << "Constructor implicit apelat -Camera:Undefined Behaviour"; }
	~Camera() { }
};