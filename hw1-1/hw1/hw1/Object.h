#if !defined OBJECT_H
#define OBJECT_H

#include "mesh.h"
#include<string>
#include<iostream>
using namespace std;
class Object{
public:
	Object(const char* meshName);
	mesh* object;
	float scale[3];
	float rotate[4];
	float transform[3];
};


class Scene{
public:
	Scene(const char* sceneName);
	int objectNum;
	Object* objects[10];
};



#endif