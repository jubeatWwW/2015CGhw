#include "stdafx.h"
#include "View.h"
#include "Object.h"

View::View(const char* viewfile){
	FILE* view;
	view = fopen(viewfile, "r");
	char component[100];
	
	while (!feof(view)){ 
		fscanf(view, "%s", component); 
		//cout << component << endl;
		if (strcmp(component, "eye")==0){
			for (int i = 0; i < 3; i++){
				fscanf(view, "%f", &eye[i]);
			}
		}
		else if (strcmp(component, "vat") == 0){
			for (int i = 0; i < 3; i++){
				fscanf(view, "%f", &vat[i]);
			}
		}
		else if (strcmp(component, "vup") == 0){
			for (int i = 0; i < 3; i++){
				fscanf(view, "%f", &vup[i]);
			}
		}
		else if (strcmp(component, "fovy") == 0){
			fscanf(view, "%f", &fovy);
		}
		else if (strcmp(component, "dnear") == 0){
			fscanf(view, "%f", &dnear);
		}
		else if (strcmp(component, "dfar") == 0){
			fscanf(view, "%f", &dfar);
		}
		else if (strcmp(component, "viewport") == 0){
			for (int i = 0; i < 4; i++){
				fscanf(view, "%f", &viewport[i]);
			}
		}
		else{
			cout << "WTF" << endl;
		}
	}

};

void View::show(){
	cout << "eye: " << eye[0] << " " << eye[1] << " " << eye[2] << endl;
	cout << "vat: " << vat[0] << " " << vat[1] << " " << vat[2] << endl;
	cout << "vup: " << vup[0] << " " << vup[1] << " " << vup[2] << endl;
	cout << "fovy: " << fovy<< endl;
	cout << "dnear: " << dnear<< endl;
	cout << "dfar: " << dfar << endl;
	cout << "viewport: " << viewport[0] << " " << viewport[1] << " " << viewport[2] << " " << viewport[3] << endl;
};