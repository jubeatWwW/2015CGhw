#include "stdafx.h"
#include "Object.h"

Object::Object(const char* meshName, const char* localPath){
	object = new mesh(meshName, localPath);
};

Scene::Scene(const char* sceneName, const char* localPath){
	FILE* scene;
	char str[100];
	int curTexMed=0;
	int curTexID;
	char filename[100];
	char textureName[100];
	objectNum = 0;
	texture_size = 1;
	scene = fopen(sceneName, "r");
	while (!feof(scene)){
		fscanf(scene, "%s", str);
		if (strcmp(str, "model") == 0){
			if (feof(scene)) break;
			fscanf(scene, "%s", filename);
			objects[objectNum] = new Object(filename, localPath);
			for (int i = 0; i < 3; i++)
				fscanf(scene, "%f", &objects[objectNum]->scale[i]);

			for (int i = 0; i < 4; i++)
				fscanf(scene, "%f", &objects[objectNum]->rotate[i]);

			for (int i = 0; i < 3; i++)
				fscanf(scene, "%f", &objects[objectNum]->transform[i]);

			objects[objectNum]->textureID = curTexID;
			objects[objectNum]->textureMethod = curTexMed;
			objectNum++;
		}
		else{
			if (feof(scene)) break;
			if (strcmp(str, "single-texture") == 0){
				curTexMed = 1;
				curTexID = texture_size;
				fscanf(scene, "%s", textureName);
				strcpy(textures[texture_size], textureName);
				cube[texture_size++] = -1;
			}
			else if (strcmp(str, "multi-texture") == 0){
				curTexMed = 2;
				curTexID = texture_size;
				for (int i = 0; i < 2; i++){
					fscanf(scene, "%s", textureName);
					strcpy(textures[texture_size], textureName);
					cube[texture_size++] = -1;
				}
			}
			else if (strcmp(str, "cube-map") == 0){
				curTexMed = 3;
				curTexID = texture_size;
				for (int i = 0; i < 6; i++){
					fscanf(scene, "%s", textureName);
					strcpy(textures[texture_size], textureName);
					cube[texture_size++] = i;
				}
			}
		}
	}
};

