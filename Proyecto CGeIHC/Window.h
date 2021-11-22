#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	GLfloat getmuevez() { return muevez; }
	GLfloat getllantaz() { return llantaz; }
	GLfloat gethelicesy() { return helicesy; }
	GLfloat getmuevehx() { return muevehx; }
	GLfloat getmuevehy() { return muevehy; }
	GLfloat getluzPrendida() { return luzPrendida; }
	GLfloat getmueveshrekX() { return mueveshrekX; }
	GLfloat getmueveshrekZ() { return mueveshrekZ; }
	GLint getanimacion() { return animacion; }
	void getanimacionr() { animacion = 0; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX,mueveshrekX,mueveshrekZ;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat muevez;
	GLfloat	llantaz;
	GLfloat	helicesy;
	GLfloat	luzPrendida;
	GLfloat	muevehx;
	GLfloat	muevehy;
	GLint animacion = 0;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

