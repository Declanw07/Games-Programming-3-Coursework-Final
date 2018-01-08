#ifndef _GAMECONSTANTS_H
#define _GAMECONSTANTS_H

#define GLM_FORCE_RADIANS
//Define the string to appear in the top left corner of the window
#define WINDOW_TITLE "GP3 Coursework Declan Wylie"

struct lightColour4
{
	lightColour4(float _r = 0.0f, float _g = 0.0f, float _b = 0.0f, float _a = 0.0f) : r(_r), g(_g), b(_b), a(_a) {}

	float r;	// Red
	float g;	// Green
	float b;	// Blue
	float a;	// Alpha
};


// Windows & OpenGL 
#include <stdlib.h>
#include <Windows.h>
#include "GL\glut.h"
#include <iostream>
#include <string>
#include <ctime>
#include <MMSystem.h>

// Image Texture loading library
#include "IL\il.h"
#include "IL\ilu.h"
#include "IL\ilut.h"

// Font
#include "FTGL\ftgl.h"

// Maths functions
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\geometric.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtx\quaternion.hpp"

// STL Container & Algorithms
#include <vector>
#include <map>
#include <algorithm>

// Textures
#include "cTexture.h"

#include "cCamera.h"

extern int drawMode;
extern float rotationAngle;
extern bool soundPlaying;
extern GLfloat cameraRotationAngle;
extern glm::vec3 currentCameraPos;
extern bool isOrtho;

extern float translationZ;


//extern cCamera theCamera;

#endif