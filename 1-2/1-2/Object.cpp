#include "stdafx.h"
#include "Object.h"
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>

Texture::Texture(int iid, string iname, int icube){
	id = iid;
	name = iname;
	cube = icube;
};

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
	bool newTexture = false;
	int startid = 0;
	int endid = 0;

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
			vector<Texture*> tmp;

			if (newTexture){
				startid = endid;
				int cubetmp = -1;
				for (string a : texts){
					if (strcmp(str, "cube-map") == 0)
						cubetmp++;
					//cout << endid << " " << a << " " << cubetmp << endl;
					textures[endid++] = new Texture(endid, a, cubetmp);
					//cout << textures[endid - 1]->id << " " << textures[endid - 1]->name << " " << textures[endid - 1]->cube << endl;
				}
			}
			//cout << startid << " " << endid << endl;
			/*for (int i = startid; i < endid; i++){
				tmp.push_back(textures[i]);
				//cout << tmp.back().name << endl;
			}

			objects[objectNum]->texture = tmp;
			objects[objectNum]->textureType = string(curTextureType);*/
			for (int i = 0; i < endid; i++){
				cout << textures[i]->id << " " << textures[i]->name << " " << textures[i]->cube << endl;
			}
			tmp.clear();
			objectNum++;
			newTexture = false;
		}
		else {
			if (feof(scene)) break;
			if (strcmp(str, "single-texture") == 0 || strcmp(str, "multi-texture") == 0 || strcmp(str, "cube-map") == 0){
				char tmp[100];
				fgets(tmp, sizeof(tmp), scene);
				strcpy(textureStr, tmp);
				strcpy(curTextureType, str);
				newTexture = true;
			}
			else if (strcmp(str, "no-texture") == 0){
				strcpy(textureStr, "");
				strcpy(curTextureType, str);
			}
		}
	}
	texture_num = endid;
	for (int i = 0; i < texture_num; i++){
		cout << textures[i]->id << " " << textures[i]->name << " " << textures[i]->cube << endl;
	}
};

