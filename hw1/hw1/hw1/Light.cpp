#include "Light.h"
#include "Object.h"
Light::Light(){

};

Lights::Lights(const char* lightfile){
	FILE* light;
	light = fopen(lightfile, "r");
	char title[100];
	lightNum = 0;
	while (!feof(light)){
		fscanf(light,"%s",title);
		if (feof(light)) break;
		if (strcmp(title, "light") == 0){
			lightgroup[lightNum] = new Light();
			for (int i = 0; i < 3; i++)
				fscanf(light, "%f", &lightgroup[lightNum]->position[i]);
			for (int i = 0; i < 3; i++)
				fscanf(light, "%f", &lightgroup[lightNum]->ambient[i]);
			for (int i = 0; i < 3; i++)
				fscanf(light, "%f", &lightgroup[lightNum]->diffuse[i]);
			for (int i = 0; i < 3; i++)
				fscanf(light, "%f", &lightgroup[lightNum]->specular[i]);

			lightNum++;
		}
		else if (strcmp(title, "ambient") == 0){
			for (int i = 0; i < 3; i++)
				fscanf(light, "%f", &envambient[i]);
		}
		else{
			cout << "WTF" << endl;
		}
	}

};