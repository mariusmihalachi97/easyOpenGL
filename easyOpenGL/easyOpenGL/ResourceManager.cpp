#include "ResourceManager.h"
#include "SOIL.h"


GLenum load_texture_type(char * const &type)
{
	if (type[0] == '2')return GL_TEXTURE_2D;
	return -1;
}

GLenum load_texture_filter(char * const &filter)
{
	if (filter[0] == 'L')return GL_LINEAR;
	if (filter[0] == 'N')return GL_NEAREST;
	return -1;
}

GLenum load_texture_wrap(char * const &wrap)
{
	if (wrap[0] == 'C')return GL_CLAMP_TO_EDGE;
	if (wrap[0] == 'R')return GL_REPEAT;
	if (wrap[0] == 'M')return GL_MIRRORED_REPEAT;
	return -1;
}

ResourceManager* ResourceManager::uniqueRManager = NULL;

ResourceManager* ResourceManager::getInstance()
{
	if (uniqueRManager == NULL)
		uniqueRManager = new ResourceManager();
	return uniqueRManager;
}

Model * ResourceManager::reqModel(int const & id_model)
{
	//unordered_map<int, Model>::iterator 
	auto models_iterator = (models.find(id_model));
	if (models_iterator != models.end())
		return &models_iterator->second;
	auto _modelPtr = &models.try_emplace(id_model, &modelResources[id_model]
	).first->second;
	_modelPtr->Load();
	return _modelPtr;
}

Texture * ResourceManager::reqTexture(int const & id_texture)
{
	//unordered_map<int, Texture>::iterator 
	auto textures_iterator = textures.find(id_texture);
	if (textures_iterator != textures.end())
		return &textures_iterator->second;
	auto _TexturePtr = &textures.try_emplace(id_texture, &textureResources[id_texture]
	).first->second;
	_TexturePtr->Load();
	return _TexturePtr;
}

Shader * ResourceManager::reqShader(int const & id_shader)
{
	//unordered_map<int, Shader>::iterator 
	auto shaders_iterator = shaders.find(id_shader);
	if (shaders_iterator != shaders.end())
		return &shaders_iterator->second;
	auto shaderPtr = &shaders.try_emplace(id_shader, &shaderResources[id_shader]
	).first->second;
	shaderPtr->Load();
	return shaderPtr;
}

void ResourceManager::InitModelResources(xml_node<>* const & pRoot)
{
	xml_node<> *RootModels = pRoot->first_node("models");
	xml_node<> *path; int id, size;
	file_type type;
	for (xml_node<> *Models = RootModels->first_node();
		Models; Models = Models->next_sibling())
	{
		id = atoi(Models->first_attribute("id")->value());
		path = Models->first_node("path");
		size = path->value_size();
		char* modelRes_path = new char[size + 1];
		strcpy(modelRes_path, path->value());
		if ((Models->first_node("type")->value())[0] == 'o')
			type = obj_file;
		else
			type = obj_file_no_texture;
		modelResources.try_emplace(id, modelRes_path, type);
	}
}

void ResourceManager::InitShaderResources(xml_node<>* const & pRoot)
{
	xml_node<> *RootShaders = pRoot->first_node("shaders");
	xml_node<> *vs, *fs; int id;
	for (xml_node<> *Shaders = RootShaders->first_node(); Shaders; Shaders = Shaders->next_sibling())
	{
		id = atoi(Shaders->first_attribute("id")->value());
		vs = Shaders->first_node("vs");
		fs = Shaders->first_node("fs");
		char* _vertexShader = new char[vs->value_size() + 1];
		char* _fragmentShader = new char[fs->value_size() + 1];
		strcpy(_vertexShader, vs->value());
		strcpy(_fragmentShader, fs->value());
		shaderResources.try_emplace(id, _vertexShader, _fragmentShader);
	}
}

void ResourceManager::InitTextureResources(xml_node<>* const & pRoot)
{
	xml_node<> *RootTextures = pRoot->first_node("textures");
	xml_node<> *path, *min_filter, *mag_filter, *wrap_s, *wrap_t; int id;
	for (xml_node<> *Textures = RootTextures->first_node(); Textures; Textures = Textures->next_sibling())
	{
		id = atoi(Textures->first_attribute("id")->value());
		path = Textures->first_node("path");
		min_filter = Textures->first_node("min_filter");
		mag_filter = Textures->first_node("mag_filter");
		wrap_s = Textures->first_node("wrap_s");
		wrap_t = Textures->first_node("wrap_t");

		GLenum _type = load_texture_type(Textures->first_attribute("type")->value());
		char* _path = new char[path->value_size() + 1];
		strcpy(_path, path->value());
		GLenum _min_filter = load_texture_filter(min_filter->value());
		GLenum _mag_filter = load_texture_filter(mag_filter->value());
		GLenum _wrap_s = load_texture_wrap(wrap_s->value());
		GLenum _wrap_t = load_texture_wrap(wrap_t->value());
		textureResources.try_emplace(id, _type, _path, _min_filter, _mag_filter, _wrap_s, _wrap_t);
	}
}

void ResourceManager::Init()
{
	xml_document<> doc;
	ifstream file("resourceManager.xml");
	stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	string content(buffer.str());
	doc.parse<0>(&content[0]);
	xml_node<> *pRoot = doc.first_node();
	InitModelResources(pRoot);
	InitShaderResources(pRoot);
	InitTextureResources(pRoot);
	content.clear();
	doc.clear();
	return;
}


void Model::Load()
{
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	FILE * file; 
	Vertex *verticesData;
	switch (modelResPtr->type)
	{
	case obj_file:
		file = fopen(modelResPtr->path, "r");
		if (file == NULL) {
			printf("Impossible to open the file !\n");
			getchar();
			return;
		}

		while (1) {

			char lineHeader[128];
			// read the first word of the line
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.

			// else : parse lineHeader

			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) {
				glm::vec2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
				temp_uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					printf("File can't be read by our simple parser :-( Try exporting with other options\n");
					fclose(file);
					return;
				}
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
			else {
				// Probably a comment, eat up the rest of the line
				char stupidBuffer[1000];
				fgets(stupidBuffer, 1000, file);
			}

		}
		nr_vertices = vertexIndices.size();
		verticesData = new Vertex[nr_vertices];
		// For each vertex of each triangle
		for (unsigned int i = 0; i < nr_vertices; i++) {

			// Get the indices of its attributes
			unsigned int vertexIndex = vertexIndices[i];
			unsigned int uvIndex = uvIndices[i];
			unsigned int normalIndex = normalIndices[i];

			// Get the attributes thanks to the index
			glm::vec3 vertex = temp_vertices[vertexIndex - 1];
			glm::vec2 uv = temp_uvs[uvIndex - 1];
			glm::vec3 normal = temp_normals[normalIndex - 1];

			// Put the attributes in buffers
			verticesData[i].pos = vertex;
			verticesData[i].uv = uv;
			verticesData[i].norm = normal;
			verticesData[i].color = normal;

		}
		fclose(file);
		glGenBuffers(1, &vboId);
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferData(GL_ARRAY_BUFFER, nr_vertices * sizeof(Vertex), verticesData, GL_STATIC_DRAW);
		delete[] verticesData;
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		return;
		break;
	case obj_file_no_texture:
		file = fopen(modelResPtr->path, "r");
		if (file == NULL) {
			printf("Impossible to open the file !\n");
			getchar();
			return;
		}

		while (1) {
			char lineHeader[128];
			// read the first word of the line
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.

			// else : parse lineHeader

			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				if (abs(vertex.x) < 0.0001)vertex.x = 0;
				if (abs(vertex.y) < 0.0001)vertex.y = 0;
				if (abs(vertex.z) < 0.0001)vertex.z = 0;
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], normalIndex[3];
				int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
				if (matches != 6) {
					printf("File %s can't be read by our simple parser :-( Try exporting with other options\n", modelResPtr->path);
					fclose(file);
					return;
				}
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
			else {
				// Probably a comment, eat up the rest of the line
				char stupidBuffer[1000];
				fgets(stupidBuffer, 1000, file);
			}

		}

		// For each vertex of each triangle
		nr_vertices = vertexIndices.size();
		verticesData = new Vertex[nr_vertices];
		for (unsigned int i = 0; i < nr_vertices; i++) {

			// Get the indices of its attributes
			unsigned int vertexIndex = vertexIndices[i];
			unsigned int normalIndex = normalIndices[i];

			// Get the attributes thanks to the index
			glm::vec3 vertex = temp_vertices[vertexIndex - 1];
			glm::vec3 normal = temp_normals[normalIndex - 1];

			// Put the attributes in buffers
			verticesData[i].pos = vertex;
			verticesData[i].norm = normal;
			verticesData[i].color = normal;

		}
		fclose(file);
		glGenBuffers(1, &vboId);
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferData(GL_ARRAY_BUFFER, nr_vertices * sizeof(Vertex), verticesData, GL_STATIC_DRAW);
		delete[] verticesData;
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void Texture::Load()
{
	cout << "Texture loader Unfinished\n";
	glGenTextures(1, &textureId);
	glBindTexture(textureResPtr->type, textureId);
	glTexParameteri(textureResPtr->type, GL_TEXTURE_MIN_FILTER, textureResPtr->min_filter);
	glTexParameteri(textureResPtr->type, GL_TEXTURE_MAG_FILTER, textureResPtr->mag_filter);
	glTexParameteri(textureResPtr->type, GL_TEXTURE_WRAP_S, textureResPtr->wrap_s);
	glTexParameteri(textureResPtr->type, GL_TEXTURE_WRAP_T, textureResPtr->wrap_t);
	int width=0, height=0, bpp=0;
	unsigned char* pointer_textura=0;// = LoadTGA(textureResPtr->path, &width, &height, &bpp);
	pointer_textura = SOIL_load_image(textureResPtr->path, &width, &height, &bpp, SOIL_LOAD_RGB);
	//bpp == 24 ?
	glTexImage2D(textureResPtr->type, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pointer_textura);// :
		//glTexImage2D(textureResPtr->type, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pointer_textura);
	//delete[] pointer_textura;
	SOIL_free_image_data(pointer_textura);
	glBindTexture(textureResPtr->type, 0);
}

void Shader::Load()
{
	
	vertexShaderId = LoadShader(GL_VERTEX_SHADER, shaderResPtr->vertexShader);

	if (vertexShaderId == 0)
		return ;
	
	fragmentShaderId = LoadShader(GL_FRAGMENT_SHADER, shaderResPtr->fragmentShader);

	if (fragmentShaderId == 0)
	{
		glDeleteShader(vertexShaderId);
		return ;
	}

	programId = LoadProgram(vertexShaderId, fragmentShaderId);
	glUseProgram(programId);
	positionAttribute = glGetAttribLocation(programId, "a_position");
	colorAttribute = glGetAttribLocation(programId, "a_color"); 
	normAttribute = glGetAttribLocation(programId, "a_norm");
	binormAttribute = glGetAttribLocation(programId, "a_binorm");
	tgtAttribute = glGetAttribLocation(programId, "a_tgt");
	uvAttribute = glGetAttribLocation(programId, "a_uv");
	uvBlendAttribute = glGetAttribLocation(programId, "a_uvBlend");
	transfMatrixUniform = glGetUniformLocation(programId, "transfMatrix");
	texture1Uniform = glGetUniformLocation(programId, "texture1");
	texture2Uniform = glGetUniformLocation(programId, "texture2");
	texture3Uniform = glGetUniformLocation(programId, "texture3");
	texture4Uniform = glGetUniformLocation(programId, "texture4");
	textureBlendUniform = glGetUniformLocation(programId, "textureBlend");
	modelMatrixUniform = glGetUniformLocation(programId, "modelMatrix");
	cameraPosUniform = glGetUniformLocation(programId, "cameraPos");
	objectColorUniform = glGetUniformLocation(programId, "objectColor");
	lightColorUniform = glGetUniformLocation(programId, "lightColor");
	lightPosUniform = glGetUniformLocation(programId, "lightPos");
	viewPosUniform = glGetUniformLocation(programId, "viewPos");
	transfMatrixInstanceAttribute = glGetAttribLocation(programId, "transfMatrixInstance");
	modelMatrixInstanceAttribute = glGetAttribLocation(programId, "modelMatrixInstance");
	int i = 0;
	if (texture1Uniform != -1)glUniform1i(texture1Uniform, i++);
	if (texture2Uniform != -1)glUniform1i(texture2Uniform, i++);
	if (texture3Uniform != -1)glUniform1i(texture3Uniform, i++);
	if (texture4Uniform != -1)glUniform1i(texture4Uniform, i++);
	if (textureBlendUniform != -1)glUniform1i(textureBlendUniform, i);
	glUseProgram(0);
}
