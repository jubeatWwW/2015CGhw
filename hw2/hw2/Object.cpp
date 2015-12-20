#include "stdafx.h"
#include "Object.h"

Object::Object(const char* meshName, const char* localPath){
	object = new mesh(meshName, localPath);
};

Scene::Scene(const char* sceneName, const char* localPath){
	FILE* scene;
	char str[100];
	char filename[100];
	objectNum = 0;
	scene = fopen(sceneName, "r");
	while (!feof(scene)){
		fscanf(scene, "%s", str);
		if (feof(scene)) break;
		fscanf(scene, "%s", filename);
		objects[objectNum] = new Object(filename, localPath);
		//cout << filename << endl;
		for (int i = 0; i < 3; i++)
			fscanf(scene, "%f", &objects[objectNum]->scale[i]);
		
		for (int i = 0; i < 4; i++)
			fscanf(scene, "%f", &objects[objectNum]->rotate[i]);
		
		for (int i = 0; i < 3; i++)
			fscanf(scene, "%f", &objects[objectNum]->transform[i]);

		objectNum++;
	}
};

