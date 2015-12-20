#if !defined LIGHT_H
#define LIGHT_H

class Light{
public:
	Light();
	float position[3];
	float ambient[3];
	float diffuse[3];
	float specular[3];
};


class Lights{
public:
	Lights(const char* lightfile);
	float envambient[3];
	Light* lightgroup[10];
	int lightNum;
};

#endif