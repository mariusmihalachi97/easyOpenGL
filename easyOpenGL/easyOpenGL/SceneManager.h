#pragma once
#include <unordered_map>
#include <map>
#include "ResourceManager.h"
#include "Camera.h"
#include "glm/glm.hpp"
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace glm;

enum class object_type : int;
object_type load_object_type(char * const &type);
static float game_speed = 1.0;
class SceneObject
{
protected:
	float time;

	static float const tau;
	GLuint Id;
	object_type type;
	
	char* name;

	vec3 position;
	vec3 rotation; //specify angle in degrees
	vec3 scaling;

	mat4 modelMatrix;

	int id_model;
	Model* modelPtr;

	int nr_textures;
	int *vec_id_texture;
	Texture** texturePtrs;

	int id_shader;
	Shader* shaderPtr;

	bool blend;
	bool useDepthTest;

	SceneObject() {}
	inline void Init(
		GLuint const &_Id, object_type const &_type, char * const &_name,
		vec3 const &_position, vec3  const &_rotation, vec3 const &_scaling,
		int const &_id_model,
		int const &_nr_textures, int * const &_vec_id_texture,
		int const &_id_shader,
		bool const &_blend, bool const &_useDepthTest);
	inline virtual void defineModelMatrix();
	inline virtual void LoadModel(ResourceManager * const &ReqResources);
	virtual void inline LoadTextures(ResourceManager * const &ReqResources);
	virtual inline void LoadShader(ResourceManager * const &ReqResources);
	inline void Load();
	void Bind();
public:
	SceneObject(
		GLuint const &_Id, object_type const &_type, char * const &_name,
		vec3 const &_position, vec3 const &_rotation, vec3 const &_scaling,
		int const &_id_model, 
		int const &_nr_textures, int * const &_vec_id_texture, 
		int const &_id_shader,
		bool const &_blend, bool const &_useDepthTest);
	SceneObject(SceneObject const &) = delete;
	
	virtual void Draw(Camera * const &eye);
	virtual void Update() {
		/*if (Id == 6)
		{
			time += 0.005;
			if (time > tau)
				time -= tau;
			defineModelMatrix();
		}*/
		
	};
	virtual ~SceneObject() {
			delete[] name;
			delete[] vec_id_texture;
			delete[] texturePtrs;
	}
};

class InstancedObject : public SceneObject
{
	int nr_instances;
	GLuint MVP_vb_Id, M_vb_Id;
	vec3* translations;
	vec3* axes;
	float* speeds;
public:
	InstancedObject(
		GLuint const &_Id, object_type const &_type, char * const &_name,
		vec3 const &_position, vec3  const &_rotation, vec3 const &_scaling,
		int const &_id_model,
		int const &_nr_textures, int * const &_vec_id_texture,
		int const &_id_shader,
		bool const &_blend, bool const &_useDepthTest,
		int const &_nr_instances);
	void Update() { 
		time += 0.001;
		if (time > tau)
			time -= tau;
	}
	void Draw(Camera * const &eye);
	~InstancedObject() {
		delete[] axes;
		delete[] translations;
		delete[] speeds;
	}

};

class PlanetObject : public SceneObject
{
	vec3 revolutionAxis;
	float revolutionSpeed;
	vec3 rotationAxis;
	float rotationSpeed;
	int IdParent;

	float rotation_value;
	vec3 truePosition;

	inline void defineModelMatrix();
public:
	PlanetObject(
		GLuint const &_Id, object_type const &_type, char * const &_name,
		vec3 const &_position, vec3  const &_rotation, vec3 const &_scaling,
		int const &_id_model,
		int const &_nr_textures, int * const &_vec_id_texture,
		int const &_id_shader,
		bool const &_blend, bool const &_useDepthTest,
		vec3 const &_revolutionAxis, float const &_revolutionSpeed,
		vec3 const &_rotationAxis, float const &_rotationSpeed,
		int const &_id_parent);
	friend vec3 getPlanetPosition(int IdPlanet);
	vec3 getRelativePosition();
	void Update() {
		if(game_speed<100)game_speed += 0.01;
		time += game_speed * 0.001 * revolutionSpeed;
		if (time > tau)
			time -= tau;
		truePosition = vec3(0.0f, 0.0f, 0.0f);
		if(revolutionAxis != truePosition)
		truePosition = getRelativePosition();
		rotation_value += 0.001 * rotationSpeed;
		if (rotation_value > tau)
			rotation_value -= tau;
		defineModelMatrix();
	}
	void Draw(Camera * const &eye);
	~PlanetObject() {		
	}

};

class SceneManager
{	
private:
	static SceneManager* uniqueSManager;
	SceneManager() { printf("new SceneManager\n"); }
	vec3 background_color;
	map<int, Camera> cameras;
	vector<SceneObject *> objects;
	//map<int, SceneObject *> objects;
	int active_cameraId;
	Camera* active_cameraPtr;
	void InitBackgroundColor(xml_node<> * const &pRoot);
	void InitCameras(xml_node<> * const &pRoot);
	void InitSceneObjects(xml_node<> * const &pRoot);
	void LoadCommonAttributes(xml_node<> * const &Objects,
		GLuint &_Id, object_type &_type, char * &_name,
		vec3 &_position, vec3 &_rotation, vec3 &_scaling,
		int &_id_model,
		int &_nr_textures, int * &_vec_id_texture,
		int &_id_shader,
		bool &_blend, bool &_useDepthTest);
	void LoadInstancedAttributes(xml_node<> * const &Objects, GLuint &_nr_instances);
	void LoadPlanetAttributes(xml_node<> * const &Objects,
		vec3 &_revolutionAxis, float &_revolutionSpeed,
		vec3 &_rotationAxis, float &_rotationSpeed,
		int &_id_parent);
	friend vec3 getPlanetPosition(int IdPlanet);
public:
	void Init();
	void Draw();
	void Update();
	static SceneManager* getInstance();
	Camera& getActiveCamera() { return *active_cameraPtr; }
	void changeActiveCamera(int Id) { active_cameraPtr = &cameras[Id]; }
	~SceneManager() {
		for (auto &x : objects)delete x;
		uniqueSManager = NULL;
		printf("delete SceneManager\n");
	}
};

