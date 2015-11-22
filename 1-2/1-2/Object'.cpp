#include "stdafx.h"
#include "Object.h"
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>

Object::Object(const char* meshName,const char* localPath){
	object = new mesh(meshName, localPath);
};

Scene::Scene(const char* sceneName, const char* localPath){
	FILE* scene;
	char str[100];
	char filename[100];
	char textureStr[100];
	char curTextureType[100];
	objectNum = 0;
	scene = fopen(sceneName, "r");
	while (!feof(scene)){
		fscanf(scene, "%s", str);
		if (strcmp(str, "model") == 0){
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


			string strs = string(textureStr);
			istringstream iss(strs);
			vector<string> texts{ istream_iterator<string>{iss }, istream_iterator<string>{} };
			/*for (string i : texts){
				cout << i << endl;
			}
			cout << "-------------------\n" << endl;*/

			objects[objectNum]->textureName = texts;
			objects[objectNum]->textureType = string(curTextureType);

			objectNum++;
		}
		else {
			if (feof(scene)) break;
			if (strcmp(str, "single-texture") == 0 || strcmp(str, "multi-texture") == 0 || strcmp(str, "cube-map") == 0){
				//cout << "****************************\n" << str << "\n****************************\n" << endl;
				char tmp[100];
				fgets(tmp, sizeof(tmp), scene);
				strcpy(textureStr, tmp);
				strcpy(curTextureType, str);
			}
			else if (strcmp(str, "no-texture") == 0){
				strcpy(textureStr, "");
				strcpy(curTextureType, str);
			}
		}
	}
};

