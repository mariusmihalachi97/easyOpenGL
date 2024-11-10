#pragma once
#include "SceneManager.h"

enum class object_type : int
{
	undefined = -1,
	normal = 1,
	instanced,
	planet
};

object_type load_object_type(char * const &type)
{
	if (type[0] == 'n')
		return object_type::normal;
	if (type[0] == 'i')
		return object_type::instanced;
	if (type[0] == 'p')
		return object_type::planet;
	return object_type::undefined;
}

SceneManager* SceneManager::uniqueSManager = NULL;
float const SceneObject::tau = 2 * M_PI;
SceneManager* SceneManager::getInstance()
{
	if (uniqueSManager == NULL)
		uniqueSManager = new SceneManager;
	return uniqueSManager;
}

void SceneManager::InitBackgroundColor(xml_node<>* const &pRoot)
{
	xml_node<> *RootBackgroundColor = pRoot->first_node("backgroundColor");
	xml_node<> *color = RootBackgroundColor->first_node();
	background_color.r = atof(color->value());
	color = color->next_sibling();
	background_color.g = atof(color->value());;
	color = color->next_sibling();
	background_color.b = atof(color->value());
	glClearColor(background_color.r, background_color.g, background_color.b, 0.0f);
}

void SceneManager::InitCameras(xml_node<>* const & pRoot)
{
	xml_node<> *RootCameras = pRoot->first_node("cameras"), *cam_node;
	int id;
	for (xml_node<> *Cameras = RootCameras->first_node(); Cameras; Cameras = Cameras->next_sibling())
	{
		id = atoi(Cameras->first_attribute("id")->value());
		cam_node = Cameras->first_node("position");
		vec3 _pos(atof(cam_node->first_node("x")->value()), atof(cam_node->first_node("y")->value()), atof(cam_node->first_node("z")->value()));
		cam_node = Cameras->first_node("target");
		vec3 _targ(atof(cam_node->first_node("x")->value()), atof(cam_node->first_node("y")->value()), atof(cam_node->first_node("z")->value()));
		cam_node = Cameras->first_node("up");
		vec3 _up(atof(cam_node->first_node("x")->value()), atof(cam_node->first_node("y")->value()), atof(cam_node->first_node("z")->value()));
		cam_node = Cameras->first_node("translationSpeed");
		GLfloat _moveSpeed = atof(cam_node->value());
		cam_node = Cameras->first_node("rotationSpeed");
		GLfloat _rotateSpeed = atof(cam_node->value());
		cam_node = Cameras->first_node("fov");
		GLfloat _fov = atof(cam_node->value());
		cam_node = Cameras->first_node("near");
		GLfloat _near = atof(cam_node->value());
		cam_node = Cameras->first_node("far");
		GLfloat _far = atof(cam_node->value());
		cameras.try_emplace(id, _pos, _targ, _up, _moveSpeed, _rotateSpeed, _near, _far, _fov);
	}
	active_cameraId = atoi(pRoot->first_node("activeCamera")->value());
	changeActiveCamera(active_cameraId);
}

void SceneManager::InitSceneObjects(xml_node<>* const & pRoot)
{
	xml_node<> *RootObjects = pRoot->first_node("objects");
	//CommonAttributes
	GLuint _Id; 
	object_type _type;
	char * _name;
	vec3 _position, _rotation, _scaling;
	int _id_model, 
		_nr_textures, *_vec_id_texture,
		_id_shader;
	bool _blend, _useDepthTest;
	//InstancedAttributes
	GLuint _nr_instances;
	//PlanetAttributes
	vec3 _revolutionAxis; float _revolutionSpeed;
	vec3 _rotationAxis; float _rotationSpeed;
	int _id_parent;
	for (xml_node<> *Objects = RootObjects->first_node(); Objects; Objects = Objects->next_sibling())
	{
		LoadCommonAttributes(Objects, _Id, _type, _name, _position, _rotation, _scaling, _id_model, _nr_textures, _vec_id_texture, _id_shader, _blend, _useDepthTest);
		switch (_type)
		{
		case object_type::normal:
			objects.emplace_back(new SceneObject(_Id, _type, _name, _position, _rotation, _scaling, _id_model, _nr_textures, _vec_id_texture, _id_shader, _blend, _useDepthTest));
			break;
		case object_type::instanced:
			LoadInstancedAttributes(Objects, _nr_instances);
			objects.emplace_back(new InstancedObject(_Id, _type, _name, _position, _rotation, _scaling, _id_model, _nr_textures, _vec_id_texture, _id_shader, _blend, _useDepthTest, _nr_instances));
			break;
		case object_type::planet:
			LoadPlanetAttributes(Objects, _revolutionAxis, _revolutionSpeed, _rotationAxis, _rotationSpeed, _id_parent);
			objects.emplace_back(new PlanetObject(_Id, _type, _name, _position, _rotation, _scaling, _id_model, _nr_textures, _vec_id_texture, _id_shader, _blend, _useDepthTest, _revolutionAxis, _revolutionSpeed, _rotationAxis, _rotationSpeed, _id_parent));
			break;
		default:
			cout << "Obiect necunoscut";
		}
			
	}
}

void SceneManager::LoadCommonAttributes(xml_node<> * const &Objects,
	GLuint &_Id, object_type &_type, char * &_name,
	vec3 &_position, vec3 &_rotation, vec3 &_scaling,
	int &_id_model,
	int &_nr_textures, int * &_vec_id_texture,
	int &_id_shader,
	bool &_blend, bool &_useDepthTest)
{
	//
	xml_node<> *obj_node;
	//
	_Id = atoi(Objects->first_attribute("id")->value());
	//
	obj_node = Objects->first_node("position");
	_position.x = atof(obj_node->first_node("x")->value());
	_position.y = atof(obj_node->first_node("y")->value());
	_position.z = atof(obj_node->first_node("z")->value());
	//
	obj_node = Objects->first_node("rotation");
	_rotation.x = atof(obj_node->first_node("x")->value());
	_rotation.y = atof(obj_node->first_node("y")->value());
	_rotation.z = atof(obj_node->first_node("z")->value());
	//
	obj_node = Objects->first_node("scale");
	_scaling.x = atof(obj_node->first_node("x")->value());
	_scaling.y = atof(obj_node->first_node("y")->value());
	_scaling.z = atof(obj_node->first_node("z")->value());
	//	
	obj_node = Objects->first_node("name");
	_name = new char[obj_node->value_size() + 1]; 
	strcpy(_name, obj_node->value());
	//
	obj_node = Objects->first_node("model");
	char *string_id_model = obj_node->value();
	_id_model = isdigit(string_id_model[0]) ? atoi(string_id_model) : -1;
	//
	obj_node = Objects->first_node("shader");
	_id_shader = atoi(obj_node->value());
	//
	obj_node = Objects->first_node("blend");
	_blend = obj_node->value()[0] == 't' ? true : false;
	//
	obj_node = Objects->first_node("type");
	_type = load_object_type(obj_node->value());
	//
	obj_node = Objects->first_node("textures");
	xml_node<> *textures;
	_nr_textures = 0;
	for (textures = obj_node->first_node(); textures; textures = textures->next_sibling(), _nr_textures++);
	if (_nr_textures)
	{
		_vec_id_texture = new int[_nr_textures];
		int i = 0;
		for (textures = obj_node->first_node(); textures; textures = textures->next_sibling(), i++)
			_vec_id_texture[i] = atoi(textures->first_attribute("id")->value());
	}
	else _vec_id_texture = NULL;
	//

	_useDepthTest = true;
}

void SceneManager::LoadInstancedAttributes(xml_node<>* const & Objects, GLuint & _nr_instances)
{
	xml_node<> *obj_node;
	obj_node = Objects->first_node("instances");
	_nr_instances = atoi(obj_node->value());
}

void SceneManager::LoadPlanetAttributes(xml_node<> * const &Objects,
	vec3 &_revolutionAxis, float &_revolutionSpeed,
	vec3 &_rotationAxis, float &_rotationSpeed,
	int &_id_parent)
{
	xml_node<> *obj_node;
	obj_node = Objects->first_node("revolutionaxis");
	_revolutionAxis.x = atof(obj_node->first_node("x")->value());
	_revolutionAxis.y = atof(obj_node->first_node("y")->value());
	_revolutionAxis.z = atof(obj_node->first_node("z")->value());
	obj_node = Objects->first_node("revolutionspeed");
	_revolutionSpeed = atof(obj_node->value());

	obj_node = Objects->first_node("rotationaxis");
	_rotationAxis.x = atof(obj_node->first_node("x")->value());
	_rotationAxis.y = atof(obj_node->first_node("y")->value());
	_rotationAxis.z = atof(obj_node->first_node("z")->value());
	obj_node = Objects->first_node("rotationspeed");
	_rotationSpeed = atof(obj_node->value());

	obj_node = Objects->first_node("parent");
	_id_parent = atoi(obj_node->value());
}

void SceneManager::Init()
{	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	xml_document<> doc;
	ifstream file("sceneManager.xml");
	stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	string content(buffer.str());
	doc.parse<0>(&content[0]);
	xml_node<> *pRoot = doc.first_node();

	InitBackgroundColor(pRoot);
	InitCameras(pRoot);

	InitSceneObjects(pRoot);
	
	content.clear();
	doc.clear();

	return;
}

void SceneManager::Draw()
{
	static GLclampf c = 0.0f;
	glClearColor(c, c, c, c);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	c += 0.01;
	if (c >= 1.0)
		c = 0.0;
	Sleep(100);
	/*for (auto& x : objects)
		x->Draw(active_cameraPtr);*/
	
}

void SceneManager::Update()
{
	/*for (auto& x : objects)
		x->Update();*/
}

void SceneObject::Init(GLuint const & _Id, object_type const & _type, char * const & _name, vec3 const & _position, vec3 const & _rotation, vec3 const & _scaling, int const & _id_model, int const & _nr_textures, int * const & _vec_id_texture, int const & _id_shader, bool const & _blend, bool const & _useDepthTest)
{
	Id = _Id; type = _type; name = _name; 
	position = _position; rotation = _rotation; scaling = _scaling;
	id_model = _id_model;
	nr_textures = _nr_textures; vec_id_texture = _vec_id_texture;
	id_shader = _id_shader;
	blend =_blend; useDepthTest = _useDepthTest;
	time = 0;
	defineModelMatrix();
}

void SceneObject::defineModelMatrix()
{
	modelMatrix = translate(position)/* * rotate(time, Globals::OY) * 
		rotate(rotation.z, Globals::OZ) * rotate(rotation.y, Globals::OY) * 
		rotate(rotation.x, Globals::OX) * scale(scaling)*/;
}

void SceneObject::LoadModel(ResourceManager * const &ReqResources)
{
	modelPtr = ReqResources->reqModel(id_model);	
}

void SceneObject::LoadTextures(ResourceManager * const &ReqResources)
{
	texturePtrs = new Texture*[nr_textures];
	for (int i = 0; i < nr_textures; i++)
		texturePtrs[i] = ReqResources->reqTexture(vec_id_texture[i]);
}

void SceneObject::LoadShader(ResourceManager * const &ReqResources)
{
	shaderPtr = ReqResources->reqShader(id_shader);
}

inline void SceneObject::Load()
{
	ResourceManager *ReqResources = ResourceManager::getInstance();
	LoadModel(ReqResources);
	LoadTextures(ReqResources);
	LoadShader(ReqResources);
}

void SceneObject::Bind()
{
	modelPtr->Bind();
	for (int i = 0; i < nr_textures; i++)
		texturePtrs[i]->Bind(i);
	shaderPtr->Use();
}

SceneObject::SceneObject(
	GLuint const &_Id, object_type const &_type, char * const &_name,
	vec3 const &_position, vec3 const &_rotation, vec3 const &_scaling,
	int const &_id_model,
	int const &_nr_textures, int * const &_vec_id_texture,
	int const &_id_shader,
	bool const &_blend, bool const &_useDepthTest)
	{
	Init(_Id, _type, _name, _position, _rotation, _scaling, _id_model, _nr_textures, _vec_id_texture, _id_shader, _blend, _useDepthTest);	
	Load();
}

void SceneObject::Draw(Camera * const &eye)
{
	Bind();
	
	mat4 MVPmatrix = eye->ViewProjectionMatrix * modelMatrix;
	shaderPtr->SendMVP(&MVPmatrix[0][0]);
	
	mat4 mMatrix = modelMatrix;
	shaderPtr->SendModelMatrix(&mMatrix[0][0]);
	
	shaderPtr->SendViewPos(&eye->position.x);
	float objCol[3] = { 1.0,1.0,1.0 };
	shaderPtr->SendObjectColor(objCol);
	float lightCol[3] = { 30.0,30.0,30.0 };
	shaderPtr->SendLightColor(lightCol);
	float lightPos[3] = { 0.0,0.0,0.0 };
	shaderPtr->SendLightPos(lightPos);
	modelPtr->Draw();	
	
}

InstancedObject::InstancedObject(GLuint const & _Id, object_type const & _type, char * const & _name, vec3 const & _position, vec3 const & _rotation, vec3 const & _scaling, int const & _id_model, int const & _nr_textures, int * const & _vec_id_texture, int const & _id_shader, bool const & _blend, bool const & _useDepthTest, int const & _nr_instances)
{
	Init(_Id, _type, _name, _position, _rotation, _scaling, _id_model, _nr_textures, _vec_id_texture, _id_shader, _blend, _useDepthTest);
	Load();
	nr_instances = _nr_instances;
	translations = new vec3[nr_instances];
	axes = new vec3[nr_instances];
	speeds = new float[nr_instances];
	int i;
	float dist;
	vec3 dir_versor;
	for (i = 0; i < nr_instances; i++)
	{
		dist = 1000 +(2*nr_instances) + (float)(rand() % 1001) / 10;
		dir_versor.x = (float)(rand() % 101) / 100 - 0.5;
		dir_versor.y = (float)(rand() % 101) / 1000 - 0.05;
		dir_versor.z = (float)(rand() % 101) / 100 - 0.5;
			translations[i] = dist * normalize(dir_versor);
			dir_versor.x = (float)(rand() % 1001) / 1000 - 0.5;
			dir_versor.y = (float)(rand() % 1001) / 1000 - 0.5;
			dir_versor.z = (float)(rand() % 1001) / 1000 - 0.5;
			axes[i] = normalize(dir_versor);	
			speeds[i] = (float)(rand() % 3001) / 100 + 1.0;
	}
	time = 0;
	glGenBuffers(1, &MVP_vb_Id);
	glBindBuffer(GL_ARRAY_BUFFER, MVP_vb_Id);
	glBufferData(GL_ARRAY_BUFFER, nr_instances * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);
	glGenBuffers(1, &M_vb_Id);
	glBindBuffer(GL_ARRAY_BUFFER, M_vb_Id);
	glBufferData(GL_ARRAY_BUFFER, nr_instances * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);
}

void InstancedObject::Draw(Camera * const & eye)
 {
	
	
	 Bind();
	
	 glBindBuffer(GL_ARRAY_BUFFER, MVP_vb_Id);
	 
	 mat4 *ptr = (mat4*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	 
		 for (int i = 0; i < nr_instances; i++)
			 ptr[i] = eye->ViewProjectionMatrix * rotate(time, Globals::OY) * translate(translations[i]) * rotate(-time, Globals::OY) * rotate(time*speeds[i], axes[i])* modelMatrix;
			 //ptr[i] = eye->ViewProjectionMatrix * modelMatrix;
		 glUnmapBuffer(GL_ARRAY_BUFFER);

		 glBindBuffer(GL_ARRAY_BUFFER, M_vb_Id);
		 ptr = (mat4*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		 for (int i = 0; i < nr_instances; i++)
			 ptr[i] = rotate(time, Globals::OY) * translate(translations[i]) * rotate(-time, Globals::OY) * rotate(time*speeds[i], axes[i]) * modelMatrix;
			 //ptr[i] = modelMatrix;
		 glUnmapBuffer(GL_ARRAY_BUFFER);
	shaderPtr->SendMatricesInstanced(MVP_vb_Id, M_vb_Id);

	 shaderPtr->SendViewPos(&eye->position.x);
	 float objCol[3] = { 0.7,0.4,0.9 };
	 shaderPtr->SendObjectColor(objCol);
	 float lightCol[3] = { 10.0,10.0,10.0 };
	 shaderPtr->SendLightColor(lightCol);
	 float lightPos[3] = { 0.0,0.0,0.0 };
	 shaderPtr->SendLightPos(lightPos);
	 
	 modelPtr->DrawInstanced(nr_instances); 

 }

inline void PlanetObject::defineModelMatrix()
{
		modelMatrix = translate(truePosition) * rotate(rotation_value, rotationAxis) *
		rotate(rotation.z, Globals::OZ) * rotate(rotation.y, Globals::OY) *
		rotate(rotation.x, Globals::OX) * scale(scaling);
}

PlanetObject::PlanetObject(GLuint const & _Id, object_type const & _type, char * const & _name, vec3 const & _position, vec3 const & _rotation, vec3 const & _scaling, int const & _id_model, int const & _nr_textures, int * const & _vec_id_texture, int const & _id_shader, bool const & _blend, bool const & _useDepthTest, vec3 const & _revolutionAxis, float const & _revolutionSpeed, vec3 const & _rotationAxis, float const & _rotationSpeed, int const & _id_parent)
{
	Init(_Id, _type, _name, _position, _rotation, _scaling, _id_model, _nr_textures, _vec_id_texture, _id_shader, _blend, _useDepthTest);
	Load();
	revolutionAxis = _revolutionAxis;
	revolutionSpeed = _revolutionSpeed;
	rotationAxis = _rotationAxis;
	rotationSpeed = _rotationSpeed;
	IdParent = _id_parent;
	rotation_value = 0;

}

vec3 getPlanetPosition(int IdPlanet)
{
	if (IdPlanet < 0) return vec3(0.0f, 0.0f, 0.0f);
	auto y = SceneManager::uniqueSManager->objects[IdPlanet-1];
	if (dynamic_cast<PlanetObject*>(y) != NULL)
	{
		auto x = (PlanetObject*)y;
		if (x->IdParent < 0)
			return vec3(0.0f, 0.0f, 0.0f);
		return (vec3(translate(getPlanetPosition(x->IdParent)) * rotate(x->time, x->revolutionAxis) * vec4(x->position, 1.0f)));
	}
	else return vec3(0.0f, 0.0f, 0.0f);
}

vec3 PlanetObject::getRelativePosition()
{
	return getPlanetPosition(Id);
}

void PlanetObject::Draw(Camera * const & eye)
{
	Bind();

	mat4 MVPmatrix = eye->ViewProjectionMatrix * modelMatrix;
	shaderPtr->SendMVP(&MVPmatrix[0][0]);

	mat4 mMatrix = modelMatrix;
	shaderPtr->SendModelMatrix(&mMatrix[0][0]);

	shaderPtr->SendViewPos(&eye->position.x);
	float objCol[3] = { 1.0,1.0,1.0 };
	shaderPtr->SendObjectColor(objCol);
	float lightCol[3] = { 30.0,30.0,30.0 };
	shaderPtr->SendLightColor(lightCol);
	float lightPos[3] = { 0.0,0.0,0.0 };
	shaderPtr->SendLightPos(lightPos);
	modelPtr->Draw();
}
