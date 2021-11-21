/*
Semestre 2022-1
Proyeco final
*/

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

//Biblioteca de audio 
#include <irrklang/irrKlang.h> 

//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"
#include"SpotLight.h"

const float toRadians = 3.14159265f / 180.0f;
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
bool dia  = false;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;
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
Model shrekcuepo;
Model shrekmanoder;
Model shrekmanoizq;
Model shrekpantbajo;
Model shrekpieizq;
Model shrekpieder;
Model regalos;


Skybox skybox;
Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat Tiempo = 0.0f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

static double limitFPS = 1.0 / 60.0;

//void my_input(GLFWwindow *window);
void inputKeyframes(bool* keys);


// Vertex Shader
//comentario
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";
//c�lculo del promedio de las normales para sombreado de Phong
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

int banderaDia;
int DiaNoche()
{
	if (dia == false) {
		glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);
		std::vector<std::string> skyboxFaces;
		skyboxFaces.push_back("Textures/Skybox/posx-n.tga");
		skyboxFaces.push_back("Textures/Skybox/negx-n.tga");
		skyboxFaces.push_back("Textures/Skybox/negy-n.tga");
		skyboxFaces.push_back("Textures/Skybox/posy-n.tga");
		skyboxFaces.push_back("Textures/Skybox/posz-n.tga");
		skyboxFaces.push_back("Textures/Skybox/negz-n.tga");
		skybox = Skybox(skyboxFaces);
		dia = true;
		banderaDia = 1;
	}
	else {
		glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);
		std::vector<std::string> skyboxFaces;
		skyboxFaces.push_back("Textures/Skybox/posx.tga");
		skyboxFaces.push_back("Textures/Skybox/negx.tga");
		skyboxFaces.push_back("Textures/Skybox/negy.tga");
		skyboxFaces.push_back("Textures/Skybox/posy.tga");
		skyboxFaces.push_back("Textures/Skybox/posz.tga");
		skyboxFaces.push_back("Textures/Skybox/negz.tga");
		skybox = Skybox(skyboxFaces);
		dia = false;
		banderaDia = -1;
	}
	return banderaDia;
}
///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;



//NEW// Keyframes
float posXburro = 2.0, posYburro = -2.0, posZburro = -1.0;
float	movBurro_x = 0.0f, movBurro_y = 0.0f, movBurro_z = 0.0f;
float giroBurro = 0;

#define MAX_FRAMES 30
int i_max_steps = 90;
int i_curr_steps = 28;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movBurro_x;		//Variable para PosicionX
	float movBurro_y;		//Variable para PosicionY
	float movBurro_z;

	float movBurro_xInc;		//Variable para IncrementoX
	float movBurro_yInc;		//Variable para IncrementoY
	float movBurro_zInc;		//Variable para IncrementoZ
	float giroBurro;
	float giroBurroInc;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 28;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);


	KeyFrame[FrameIndex].movBurro_x = movBurro_x;
	KeyFrame[FrameIndex].movBurro_y = movBurro_y;
	KeyFrame[FrameIndex].movBurro_z = movBurro_z;
	KeyFrame[FrameIndex].giroBurro = giroBurro;

	FrameIndex++;
}

void resetElements(void)
{

	movBurro_x = KeyFrame[0].movBurro_x;
	movBurro_y = KeyFrame[0].movBurro_y;
	movBurro_z = KeyFrame[0].movBurro_z;
	giroBurro = KeyFrame[0].giroBurro;
}

void interpolation(void)
{
	KeyFrame[playIndex].movBurro_xInc = (KeyFrame[playIndex + 1].movBurro_x - KeyFrame[playIndex].movBurro_x) / i_max_steps;
	KeyFrame[playIndex].movBurro_yInc = (KeyFrame[playIndex + 1].movBurro_y - KeyFrame[playIndex].movBurro_y) / i_max_steps;
	KeyFrame[playIndex].movBurro_zInc = (KeyFrame[playIndex + 1].movBurro_z - KeyFrame[playIndex].movBurro_z) / i_max_steps;
	KeyFrame[playIndex].giroBurroInc = (KeyFrame[playIndex + 1].giroBurro - KeyFrame[playIndex].giroBurro) / i_max_steps;

}


void animate(void)
{
	//Movimiento del objeto
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				//printf("entro aqu�\n");
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//printf("se qued� aqui\n");
			//printf("max steps: %f", i_max_steps);
			//Draw animation
			movBurro_x += KeyFrame[playIndex].movBurro_xInc;
			movBurro_y += KeyFrame[playIndex].movBurro_yInc;
			movBurro_z += KeyFrame[playIndex].movBurro_zInc;
			giroBurro += KeyFrame[playIndex].giroBurroInc;
			i_curr_steps++;
		}

	}
}

/* FIN KEYFRAMES*/

//Audio
irrklang:: ISoundEngine* engine = irrklang::createIrrKlangDevice();

int main() 
{
	
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	
	CreateObjects();
	CrearCubo();
	CreateShaders();

	//Audio ambiental
	irrklang::ISound* music = engine->play3D("audio/fondoc.wav",
		irrklang::vec3df(1.0, 1.0, 1.0), true, false, true);
	if (music)
		music->setMinDistance(1.0f);

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
	shrekcuepo = Model();
	shrekcuepo.LoadModel("Models/shrekcuerpo.obj");
	shrekmanoder = Model();
	shrekmanoder.LoadModel("Models/shrekmanoder.obj");
	shrekmanoizq = Model();
	shrekmanoizq.LoadModel("Models/shrekmanoizq.obj");
	shrekpantbajo = Model();
	shrekpantbajo.LoadModel("Models/shrekpantbajo.obj");
	shrekpieizq = Model();
	shrekpieizq.LoadModel("Models/shrekpieizq.obj");
	shrekpieder = Model();
	shrekpieder.LoadModel("Models/shrekpieder.obj");
	regalos = Model();
	regalos.LoadModel("Models/giftboxes.obj");



	


	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 
								0.3f, 0.3f,
								0.0f, 0.0f, -1.0f);
//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaraci�n de primer luz puntual
	pointLights[0] = PointLight(1.0f, 1.0f, 0.0f,
								1.0f, 1.0f,
								-2.0f, 1.0f,-10.5f,
								0.3f, 0.2f, 0.1f);
	pointLightCount++;
	pointLights[1] = PointLight(1.0f, 1.0f, 0.0f,
		0.0f, 1.0f,
		20.0f, 1.5f, -15.5f,
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
	spotLights[1] = SpotLight(1.0f, 0.0f, 1.0f,
		0.0f, 2.0f,
		10.0f, 0.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;

	

	glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/posx.tga");
	skyboxFaces.push_back("Textures/Skybox/negx.tga");
	skyboxFaces.push_back("Textures/Skybox/negy.tga");
	skyboxFaces.push_back("Textures/Skybox/posy.tga");
	skyboxFaces.push_back("Textures/Skybox/posz.tga");
	skyboxFaces.push_back("Textures/Skybox/negz.tga");


	skybox = Skybox(skyboxFaces);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);
	
	movCoche = 0.0f;
	movOffset = 0.5f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;

	//KEYFRAMES DECLARADOS INICIALES

	KeyFrame[0].movBurro_x = 0.0f;
	KeyFrame[0].movBurro_y = 0.0f;
	KeyFrame[0].movBurro_z = 0.0f;
	KeyFrame[0].giroBurro = 0;


	KeyFrame[1].movBurro_x = 4.0f;
	KeyFrame[1].movBurro_y = 0.0f;
	KeyFrame[1].movBurro_z = 0.0f;
	KeyFrame[1].giroBurro = 0;


	KeyFrame[2].movBurro_x = 8.0f;
	KeyFrame[2].movBurro_y = 0.0f;
	KeyFrame[2].movBurro_z = 0.0f;
	KeyFrame[2].giroBurro = 0;

	KeyFrame[3].movBurro_x = 10.0f;
	KeyFrame[3].movBurro_y = 0.0f;
	KeyFrame[3].movBurro_z = 0.0f;
	KeyFrame[3].giroBurro =  0.0f;

	KeyFrame[4].movBurro_x = 12.0f;
	KeyFrame[4].movBurro_y = 0.0f;
	KeyFrame[4].movBurro_z = -2.0f;
	KeyFrame[4].giroBurro = 30.0f;

	KeyFrame[5].movBurro_x = 14.0f;
	KeyFrame[5].movBurro_y = 0.0f;
	KeyFrame[5].movBurro_z = -4.0f;
	KeyFrame[5].giroBurro = 60.0f;

	KeyFrame[6].movBurro_x = 16.0f;
	KeyFrame[6].movBurro_y = 0.0f;
	KeyFrame[6].movBurro_z = -6.0f;
	KeyFrame[6].giroBurro = 90.0f;

	KeyFrame[7].movBurro_x = 16.0f;
	KeyFrame[7].movBurro_y = 0.0f;
	KeyFrame[7].movBurro_z = -10.0f;
	KeyFrame[7].giroBurro =  90.0f;

	KeyFrame[8].movBurro_x = 16.0f;
	KeyFrame[8].movBurro_y = 0.0f;
	KeyFrame[8].movBurro_z = -14.0f;
	KeyFrame[8].giroBurro = 90.0f;

	KeyFrame[9].movBurro_x = 14.0f;
	KeyFrame[9].movBurro_y = 0.0f;
	KeyFrame[9].movBurro_z = -16.0f;
	KeyFrame[9].giroBurro = 120.0f;

	KeyFrame[10].movBurro_x = 12.0f;
	KeyFrame[10].movBurro_y = 0.0f;
	KeyFrame[10].movBurro_z = -18.0f;
	KeyFrame[10].giroBurro = 150.0f;

	KeyFrame[11].movBurro_x = 10.0f;
	KeyFrame[11].movBurro_y = 0.0f;
	KeyFrame[11].movBurro_z = -20.0f;
	KeyFrame[11].giroBurro = 180.0f;

	KeyFrame[12].movBurro_x = 6.0f;
	KeyFrame[12].movBurro_y = 0.0f;
	KeyFrame[12].movBurro_z = -20.0f;
	KeyFrame[12].giroBurro = 180.0f;

	KeyFrame[13].movBurro_x = 2.0f;
	KeyFrame[13].movBurro_y = 0.0f;
	KeyFrame[13].movBurro_z = -20.0f;
	KeyFrame[13].giroBurro = 180.0f;

	KeyFrame[14].movBurro_x = -2.0f;
	KeyFrame[14].movBurro_y = 0.0f;
	KeyFrame[14].movBurro_z = -20.0f;
	KeyFrame[14].giroBurro = 180.0f;

	KeyFrame[15].movBurro_x = -6.0f;
	KeyFrame[15].movBurro_y = 0.0f;
	KeyFrame[15].movBurro_z = -20.0f;
	KeyFrame[15].giroBurro = 180.0f;

	KeyFrame[16].movBurro_x = -10.0f;
	KeyFrame[16].movBurro_y = 0.0f;
	KeyFrame[16].movBurro_z = -20.0f;
	KeyFrame[16].giroBurro = 180.0f;

	KeyFrame[17].movBurro_x = -14.0f;
	KeyFrame[17].movBurro_y = 0.0f;
	KeyFrame[17].movBurro_z = -20.0f;
	KeyFrame[17].giroBurro = 180.0f;

	KeyFrame[18].movBurro_x = -16.0f;
	KeyFrame[18].movBurro_y = 0.0f;
	KeyFrame[18].movBurro_z = -18.0f;
	KeyFrame[18].giroBurro = 210.0f;

	KeyFrame[19].movBurro_x = -18.0f;
	KeyFrame[19].movBurro_y = 0.0f;
	KeyFrame[19].movBurro_z = -16.0f;
	KeyFrame[19].giroBurro = 240.0f;

	KeyFrame[20].movBurro_x = -20.0f;
	KeyFrame[20].movBurro_y = 0.0f;
	KeyFrame[20].movBurro_z = -14.0f;
	KeyFrame[20].giroBurro = 270.0f;

	KeyFrame[21].movBurro_x = -20.0f;
	KeyFrame[21].movBurro_y = 0.0f;
	KeyFrame[21].movBurro_z = -12.0f;
	KeyFrame[21].giroBurro = 270.0f;

	KeyFrame[22].movBurro_x = -20.0f;
	KeyFrame[22].movBurro_y = 0.0f;
	KeyFrame[22].movBurro_z = -8.0f;
	KeyFrame[22].giroBurro = 270.0f;

	KeyFrame[23].movBurro_x = -20.0f;
	KeyFrame[23].movBurro_y = 0.0f;
	KeyFrame[23].movBurro_z = -6.0f;
	KeyFrame[23].giroBurro = 270.0f;

	KeyFrame[24].movBurro_x = -18.0f;
	KeyFrame[24].movBurro_y = 0.0f;
	KeyFrame[24].movBurro_z = -4.0f;
	KeyFrame[24].giroBurro = 300.0f;

	KeyFrame[25].movBurro_x = -16.0f;
	KeyFrame[25].movBurro_y = 0.0f;
	KeyFrame[25].movBurro_z = -2.0f;
	KeyFrame[25].giroBurro = 330.0f;

	KeyFrame[26].movBurro_x = -14.0f;
	KeyFrame[26].movBurro_y = 0.0f;
	KeyFrame[26].movBurro_z = 0.0f;
	KeyFrame[26].giroBurro = 360.0f;

	KeyFrame[27].movBurro_x = -8.0f;
	KeyFrame[27].movBurro_y = 0.0f;
	KeyFrame[27].movBurro_z = 0.0f;
	KeyFrame[27].giroBurro = 360.0f;

	KeyFrame[27].movBurro_x = 0.0f;
	KeyFrame[27].movBurro_y = 0.0f;
	KeyFrame[27].movBurro_z = -0.0f;
	KeyFrame[27].giroBurro = 360.0f;

	
	//Agregar Kefyrame[5] para que el avi�n regrese al inicio


	//SoundEngine->play2D("audio/Shrek.wav", true);

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{	
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;
		Tiempo += deltaTime;

		if (Tiempo > 30) {
			DiaNoche();
			Tiempo = 0;

		}

		if (movCoche < 5.0f)
		{
			movCoche += movOffset * deltaTime;
			
		}
		rotllanta += rotllantaOffset * deltaTime;

		//Luces tipo puntual con prendido y apagado automatico
		if (banderaDia > 0)
		{
			pointLightCount = 2;
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		}
		else
		{
			pointLightCount = 0;
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		}

		//SpotLight prendido y apagado con teclado
		if (mainWindow.getluzPrendida() > 0)
		{
			spotLightCount = 2;
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
		}
		else
		{
			spotLightCount = 0;
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
		}
	
		//Recibir eventos del usuario
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		//para keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();
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
		posblackhawk = glm::vec3(posXburro + movBurro_x, posYburro + movBurro_y, posZburro + movBurro_z);
		model = glm::translate(model, posblackhawk);
		//model = glm::translate(model, glm::vec3(-10.0f, -1.7f, -0.1f));
		model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
		model = glm::rotate(model, giroBurro * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
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
/*
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0, 0.25f, -10.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		regalos.RenderModel();

		//SHREK jerarquizado
		//Cuerpo
		model = glm::mat4(1.0);
		model = matrizauxiliarshrek;
		model = glm::translate(model, glm::vec3(25.0f + mainWindow.getmueveshrekX(), 5.1f, 
												-15.5f + mainWindow.getmueveshrekZ()));
		matrizauxiliarshrek = model;
		model = glm::scale(model, glm::vec3(3.1f, 3.1f, 3.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shrekcuepo.RenderModel();

		//Brazo derecho
		model = glm::mat4(1.0);
		model = matrizauxiliarshrek;
		model = glm::translate(model, glm::vec3(0.0, 0.0f, 0.0f));
		matrizauxiliarshrek = model;
		model = glm::scale(model, glm::vec3(3.1f, 3.1f, 3.1f));
	/*	model = glm::rotate(model, 270 * toRadians + glm::radians(mainWindow.getrotabrazoder()),
			glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shrekmanoder.RenderModel();

		//Brazo izquierdo
		model = glm::mat4(1.0);
		model = matrizauxiliarshrek;
		model = glm::translate(model, glm::vec3(0.0, 0.0f, 0.0f));
		matrizauxiliarshrek = model;
		model = glm::scale(model, glm::vec3(3.1f, 3.1f, 3.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shrekmanoizq.RenderModel();

		//Parte baja del pantalon
		model = glm::mat4(1.0);
		model = matrizauxiliarshrek;
		model = glm::translate(model, glm::vec3(0.0, 0.0f, 0.0f));
		matrizauxiliarshrek = model;
		model = glm::scale(model, glm::vec3(3.1f, 3.1f, 3.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shrekpantbajo.RenderModel();

		//Pierna derecha
		model = glm::mat4(1.0);
		model = matrizauxiliarshrek;
		model = glm::translate(model, glm::vec3(0.0, 0.0f, 0.0f));
		matrizauxiliarshrek = model;
		model = glm::scale(model, glm::vec3(3.1f, 3.1f, 3.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shrekpieder.RenderModel();

		//Pierna izquierda
		model = glm::mat4(1.0);
		model = matrizauxiliarshrek;
		model = glm::translate(model, glm::vec3(0.0, 0.0f, 0.0f));
		matrizauxiliarshrek = model;
		model = glm::scale(model, glm::vec3(3.1f, 3.1f, 3.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		shrekpieizq.RenderModel(); */


		//Agave �qu� sucede si lo renderizan antes del coche y de la pista?
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

void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("\n presiona 0 para habilitar reproducir de nuevo la animaci�n'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;
			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1)
		{
			reproduciranimacion = 0;
		}
	}

	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("movBurro_x es: %f\n", movBurro_x);
			//printf("movBurro_y es: %f\n", movBurro_y);
			printf(" \npresiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_P])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
		}
	}


	if (keys[GLFW_KEY_1])
	{
		if (ciclo < 1)
		{
			//printf("movBurro_x es: %f\n", movBurro_x);
			movBurro_x += 1.0f;
			printf("\n movBurro_x es: %f\n", movBurro_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n reinicia con 2\n");
		}

	}
	if (keys[GLFW_KEY_2])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
		}
	}

}
