/*
Semestre 2022-1
Práctica: Animación Sencilla y animación compleja
*/
//para cargar imagen
//cometario commit

#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include <irrklang/irrKlang.h>
//using namespace irrklang;


//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include"Model.h"
#include "Skybox.h"
#include"SpotLight.h"

const float toRadians = 3.14159265f / 180.0f;
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture pisoTexture;
Texture Tagave;
//materiales
Material Material_brillante;
Material Material_opaco;
//luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Model Jengibre;
Model Pinocchio;
Model House;
Model puerta;
Model Letrina;
Model Cartel;
Model Burro;
Model Silla;
Model puss;
Model fiona;
Model interior;
Model Pinonevado;


Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
//comentario
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";
//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount, 
						unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);
		
		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects() 
{
	unsigned int indices[] = {		
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
	//	x      y      z			u	  v			nx	  ny    nz
		-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};


	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh *obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.27f,  0.35f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.48f,	0.35f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.48f,	0.64f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.27f,	0.64f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.52f,  0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.73f,	0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.73f,	0.64f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.52f,	0.64f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.77f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.98f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.98f,	0.64f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.77f,	0.64f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.23f,  0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.23f,	0.64f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	0.64f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.27f,	0.02f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.48f,  0.02f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.48f,	0.31f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.27f,	0.31f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.27f,	0.68f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.48f,  0.68f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.48f,	0.98f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.27f,	0.98f,		0.0f,	-1.0f,	0.0f,

	};
	
	Mesh *cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);

}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
int main() 
{
	
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	
	CreateObjects();
	CrearCubo();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);


	pisoTexture= Texture("Textures/pisonevado.tga");
	pisoTexture.LoadTextureA();
	Tagave = Texture("Textures/Agave.tga");
	Tagave.LoadTextureA();
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	Jengibre = Model();
	Jengibre.LoadModel("Models/GingerbreadMan.obj");
	Pinocchio = Model();
	Pinocchio.LoadModel("Models/pinocchio.obj");
	Letrina = Model();
	Letrina.LoadModel("Models/letrina.obj");
	House = Model();
	House.LoadModel("Models/casafinal.obj");
	Cartel = Model();
	Cartel.LoadModel("Models/cartel.obj");
	Burro = Model();
	Burro.LoadModel("Models/Donkey.obj");
	Silla = Model();
	Silla.LoadModel("Models/Chair.obj");
	puss = Model();
	puss.LoadModel("Models/puss.obj");
	fiona = Model();
	fiona.LoadModel("Models/fiona.obj");
	puerta = Model();
	puerta.LoadModel("Models/casapuerta.obj");
	interior = Model();
	interior.LoadModel("Models/interior.obj");
	Pinonevado = Model();
	Pinonevado.LoadModel("Models/pinonevado.obj");




	


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 
								0.3f, 0.3f,
								0.0f, 0.0f, -1.0f);
//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
								0.0f, 1.0f,
								2.0f, 1.5f,1.5f,
								0.3f, 0.2f, 0.1f);
	pointLightCount++;
	
	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		0.0f, 2.0f,
		10.0f, 0.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;

	

	glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/posx-n.tga");
	skyboxFaces.push_back("Textures/Skybox/negx-n.tga");
	skyboxFaces.push_back("Textures/Skybox/negy-n.tga");
	skyboxFaces.push_back("Textures/Skybox/posy-n.tga");
	skyboxFaces.push_back("Textures/Skybox/posz-n.tga");
	skyboxFaces.push_back("Textures/Skybox/negz-n.tga");


	skybox = Skybox(skyboxFaces);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);
	
	movCoche = 0.0f;
	movOffset = 0.5f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;

	SoundEngine->play2D("audio/Shrek.wav", true);

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		
		
		//SoundEngine->play2D("audio/bleep.mp3", true);
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;
		if (movCoche < 5.0f)
		{
			movCoche += movOffset * deltaTime;
			
		}
		rotllanta += rotllantaOffset * deltaTime;
	
		//Recibir eventos del usuario
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0f, 1.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//pisoTexture.UseTexture();
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//modelaux = glm::mat4(1.0);
		model = glm::mat4(1.0);
		modelaux=model = glm::translate(model, glm::vec3(movCoche, -1.5f, 0.2f));
		//modelaux = model;
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Jengibre.RenderModel();
			model = modelaux;
			model = glm::translate(model, glm::vec3(-1.0, -0.1f, 0.2f));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			Pinocchio.RenderModel();
		
		// exterior 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.0f, 0.0f, -10.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, 180* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Letrina.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, -1.0f, 10.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cartel.RenderModel();

		//bosque
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-56.0f, 0.0f, -53.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.0f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-66.0f, 0.0f, -45.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.9f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-63.0f, 0.0f, -39.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.3f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-56.0f, 0.0f, -29.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-60.0f, 0.0f, -34.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.7f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-56.0f, 0.0f, -25.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.9f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-59.0f, 0.0f, -20.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.5f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-61.0f, 0.0f, -14.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.9f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-61.0f, 0.0f, -5.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.7f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-56.0f, 0.0f, 5.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.2f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-62.0f, 0.0f, 11.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.7f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-54.0f, 0.0f, 16.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.4f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-63.0f, 0.0f, 6.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.0f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-55.0f, 0.0f, 22.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.6f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-64.0f, 0.0f, 15.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.1f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-62.0f, 0.0f, 23.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.2f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-56.0f, 0.0f, 29.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.3f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-64.0f, 0.0f, 35.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.9f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-59.0f, 0.0f, 31.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.1f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-63.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.5f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-66.0f, 0.0f, 46.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.7f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		/////
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(56.0f, 0.0f, -53.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.0f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(66.0f, 0.0f, -45.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.9f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(63.0f, 0.0f, -39.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.3f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(66.0f, 0.0f, -29.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(60.0f, 0.0f, -34.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.7f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(56.0f, 0.0f, -25.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.9f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(59.0f, 0.0f, -20.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.5f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(61.0f, 0.0f, -14.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.9f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(61.0f, 0.0f, -5.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.7f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(56.0f, 0.0f, 5.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.2f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(62.0f, 0.0f, 11.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.7f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(54.0f, 0.0f, 16.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.4f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(63.0f, 0.0f, 6.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.0f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(55.0f, 0.0f, 22.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.6f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(64.0f, 0.0f, 15.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.1f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(62.0f, 0.0f, 23.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.2f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(56.0f, 0.0f, 29.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.3f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(64.0f, 0.0f, 35.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.9f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(59.0f, 0.0f, 31.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.1f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(63.0f, 0.0f, 40.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.5f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(66.0f, 0.0f, 46.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.7f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		////////
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, -54.0f));
		model = glm::scale(model, glm::vec3(1.7f, 3.4f, 1.7f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-40.0f, 0.0f, -58.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.0f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-34.0f, 0.0f, -61.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.4f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-36.0f, 0.0f, -48.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.4f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-28.0f, 0.0f, -53.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.6f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-22.0f, 0.0f, -58.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.9f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-16.0f, 0.0f, -55.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.6f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, -50.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.5f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-4.0f, 0.0f, -62.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.8f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, -55.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.0f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(8.0f, 0.0f, -61.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.7f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(14.0f, 0.0f, -51.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.5f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.0f, 0.0f, -57.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.1f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(26.0f, 0.0f, -58.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.5f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(32.0f, 0.0f, -62.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.8f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(38.0f, 0.0f, -49.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.6f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(44.0f, 0.0f, -59.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.0f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(49.0f, 0.0f, -58.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.3f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(54.0f, 0.0f, -60.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.0f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

	    //////////
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-45.0f, 0.0f, 53.0f));
		model = glm::scale(model, glm::vec3(1.7f, 3.4f, 1.7f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-40.0f, 0.0f, 58.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.0f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-34.0f, 0.0f, 61.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.4f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-36.0f, 0.0f, 48.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.4f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-28.0f, 0.0f, 53.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.6f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-22.0f, 0.0f, 58.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.9f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-16.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.6f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, 50.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.5f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-4.0f, 0.0f, 62.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.8f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 55.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.0f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(8.0f, 0.0f, 61.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.7f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(14.0f, 0.0f, 51.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.5f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.0f, 0.0f, 57.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.1f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(26.0f, 0.0f, 58.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.5f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(32.0f, 0.0f, 62.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.8f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(38.0f, 0.0f, 49.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.6f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(44.0f, 0.0f, 59.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.0f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(49.0f, 0.0f, 58.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.3f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(54.0f, 0.0f, 60.0f));
		model = glm::scale(model, glm::vec3(1.5f, 3.0f, 1.5f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pinonevado.RenderModel();
		/////////


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -0.1f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		House.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -0.1f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		puerta.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -0.1f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		interior.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, -1.7f, -0.1f));
		model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Burro.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, -0.7f, -0.1f));
		model = glm::scale(model, glm::vec3(0.0015f, 0.0015f, 0.0015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Silla.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0, 1.1f, -5.5f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puss.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0, 1.1f, -10.5f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		fiona.RenderModel();

		


		//Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, -1.7f, -2.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Tagave.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);
			glUseProgram(0);

		mainWindow.swapBuffers();


	}

	return 0;
}