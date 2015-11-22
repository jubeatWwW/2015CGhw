#if !defined VIEW_H
#define VIEW_H

class View{
public:
	View(const char* viewfile);
	float eye[3];
	float vat[3];
	float vup[3];
	float fovy;
	float dnear;
	float dfar;
	float viewport[4];
	void show();
};

#endif