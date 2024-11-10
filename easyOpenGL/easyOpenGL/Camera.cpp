#include "Camera.h"

void Camera::moveOx(GLint const &directie)
{
	vec3 vectorDeplasare = xAxis * (GLfloat)directie * moveSpeed * deltaTime;
	position += vectorDeplasare;
	target += vectorDeplasare;
	updateWorldView();
}
void Camera::moveOy(GLint const &directie)
{
	vec3 vectorDeplasare = yAxis * (GLfloat)directie * moveSpeed * deltaTime;
	position += vectorDeplasare;
	target += vectorDeplasare;
	updateWorldView();
}
void Camera::moveOz(CONST_INPUT(GLint) directie)
{
	vec3 vectorDeplasare = zAxis * (GLfloat)directie * moveSpeed * deltaTime;
	position += vectorDeplasare;
	target += vectorDeplasare;
	updateWorldView();
}
void Camera::rotateOx(GLint const &directie)
{
	
	vec4 localUp(0.0f, 1.0f, 0.0f, 0.0f);
	mat4 mRotateOX = glm::rotate((float)directie * rotateSpeed * deltaTime, Globals::OX);
	vec4 rotatedLocalUp = mRotateOX * localUp;
	vec4 _up = normalize(worldMatrix*rotatedLocalUp);
	up = _up;
	vec4 localTarget = vec4(0, 0, -length(target - position), 1);
	vec4 rotatedLocalTarget = mRotateOX * localTarget;
	vec4 _target = worldMatrix * rotatedLocalTarget;
	target = _target;
	updateWorldView();
}
void Camera::rotateOy(GLint const &directie)
{
	vec4 localTarget = vec4(0.0f, 0.0f, -length(target - position), 1.0f);
	mat4 mRotateOY = glm::rotate((float)directie * rotateSpeed * deltaTime, Globals::OY);;
	vec4 rotatedLocalTarget = mRotateOY * localTarget;
	vec4 _target = worldMatrix * rotatedLocalTarget;
	target = _target;
	updateWorldView();
}
void Camera::rotateOz(GLint const &directie)
{
	mat4 mRotateOZ = glm::rotate((float)directie * rotateSpeed * deltaTime, Globals::OZ);;
	vec4 localUp = vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vec4 rotatedLocalUp = mRotateOZ * localUp;
	vec4 _up = normalize(worldMatrix * rotatedLocalUp);
	up = _up;
	updateWorldView();
}
void Camera::updateWorldView()
{
	zAxis = normalize(-(target - position));
	xAxis = normalize(cross(up, zAxis));
	yAxis = cross(zAxis, xAxis);
	mat4 R;
	R[0][0] = xAxis.x; R[0][1] = xAxis.y; R[0][2] = xAxis.z; R[0][3] = 0.0f;
	R[1][0] = yAxis.x; R[1][1] = yAxis.y; R[1][2] = yAxis.z; R[1][3] = 0.0f;
	R[2][0] = zAxis.x; R[2][1] = zAxis.y; R[2][2] = zAxis.z; R[2][3] = 0.0f;
	R[3][0] = 0.0f; R[3][1] = 0.0f; R[3][2] = 0.0f; R[3][3] = 1.0f;
	mat4 T;
	T = translate(position);
	worldMatrix = T * R;
	T = translate(-position);
	viewMatrix = transpose(R) * T;

	ViewProjectionMatrix = projectionMatrix * viewMatrix;
}

Camera::Camera(vec3 const &_position, vec3 const &_target, vec3 const &_up,
	GLfloat const &_moveSpeed, GLfloat const &_rotateSpeed,
	GLfloat const &_cam_near, GLfloat const &_cam_far, GLfloat const &_cam_fov)
{
	position = _position;
	target = _target;
	up = _up;
	moveSpeed = _moveSpeed*7;
	rotateSpeed = _rotateSpeed*17;
	cam_near = _cam_near;
	cam_far = _cam_far;
	cam_fov = _cam_fov;
	deltaTime = 0.005f;
	float aspect = Globals::screenWidth / (float)Globals::screenHeight;
	projectionMatrix = perspective(radians(_cam_fov), aspect, _cam_near, _cam_far);
	updateWorldView();
}