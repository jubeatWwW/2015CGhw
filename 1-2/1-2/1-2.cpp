/***
*                 _      _                  _               _            _                 _            _             _             _
*                /\ \   /\_\               / /\            /\ \         / /\              /\ \         / /\      _   / /\      _   / /\      _
*                \ \ \ / / /         _    / /  \          /  \ \       / /  \             \_\ \       / / /    / /\ / / /    / /\ / / /    / /\
*                /\ \_\\ \ \__      /\_\ / / /\ \        / /\ \ \     / / /\ \            /\__ \     / / /    / / // / /    / / // / /    / / /
*               / /\/_/ \ \___\    / / // / /\ \ \      / / /\ \_\   / / /\ \ \          / /_ \ \   / / /_   / / // / /_   / / // / /_   / / /
*      _       / / /     \__  /   / / // / /\ \_\ \    / /_/_ \/_/  / / /  \ \ \        / / /\ \ \ / /_//_/\/ / // /_//_/\/ / // /_//_/\/ / /
*     /\ \    / / /      / / /   / / // / /\ \ \___\  / /____/\    / / /___/ /\ \      / / /  \/_// _______/\/ // _______/\/ // _______/\/ /
*     \ \_\  / / /      / / /   / / // / /  \ \ \__/ / /\____\/   / / /_____/ /\ \    / / /      / /  \____\  // /  \____\  // /  \____\  /
*     / / /_/ / /      / / /___/ / // / /____\_\ \  / / /______  / /_________/\ \ \  / / /      /_/ /\ \ /\ \//_/ /\ \ /\ \//_/ /\ \ /\ \/
*    / / /__\/ /      / / /____\/ // / /__________\/ / /_______\/ / /_       __\ \_\/_/ /       \_\//_/ /_/ / \_\//_/ /_/ / \_\//_/ /_/ /
*    \/_______/       \/_________/ \/_____________/\/__________/\_\___\     /____/_/\_\/            \_\/\_\/      \_\/\_\/      \_\/\_\/
*
*
*		2015 NCTU CG hw1-2
*/


#include "stdafx.h"
#include "mesh.h"
#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "Object.h"
#include "View.h"
#include "Light.h"
mesh *object;
Scene* scene;
View* view;
Lights* lights;
int windowSize[2];
int selectedObj = 0;
int offsetX, offsetY;
float curDegree = 0;

const string localpath = "park\\";
const string sceneName = localpath + "park.scene";
const string viewName = localpath + "park.view";
const string lightName = localpath + "park.light";
unsigned *textureObj;

void light();
void display();
void textureRead(const char* filename, unsigned textureIndex);
void reshape(GLsizei, GLsizei);
void keyboard(unsigned char key, int x, int y);
void mouse(int, int, int, int);
void motion(int x, int y);

int main(int argc, char** argv)
{
	scene = new Scene(sceneName.c_str(), localpath.c_str());
	view = new View(viewName.c_str());
	lights = new Lights(lightName.c_str());
	textureObj = new unsigned[scene->texture_size - 1];
	//view->show();
	//object = new mesh("box.obj");

	for (int i = 1; i < scene->texture_size; i++){
		cout << scene->textures[i] << " " << scene->cube[i] << endl;
	}

	glutInit(&argc, argv);
	glutInitWindowSize(view->viewport[2], view->viewport[3]);
	glutInitWindowPosition(view->viewport[0], view->viewport[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("CG_HW1");

	glewInit();
	FreeImage_Initialise();
	glGenTextures(scene->texture_size - 1, textureObj);
	for (int i = 1; i < scene->texture_size; i++) {
		textureRead(scene->textures[i], i);
	}
	FreeImage_DeInitialise();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutMainLoop();

	return 0;
}

void light()
{
	/*GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat light_position[] = {150.0, 150.0, 150.0, 1.0};*/

	glShadeModel(GL_SMOOTH);

	// z buffer enable
	glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property


	/*
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	*/

	for (int i = 0; i < lights->lightNum; i++){
		glEnable(GL_LIGHT0 + i);
		glLightfv(GL_LIGHT0 + i, GL_POSITION, lights->lightgroup[i]->position);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, lights->lightgroup[i]->diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, lights->lightgroup[i]->specular);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, lights->lightgroup[i]->ambient);
	}

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lights->envambient);
}

void textureRead(const char* filename, unsigned textureIndex){
	FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(filename, 0), filename);
	FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
	int bitmapWidth = FreeImage_GetWidth(bitmap32);
	int bitmapHeight = FreeImage_GetHeight(bitmap32);
	int textureType = GL_TEXTURE_2D;
	int textureMode = GL_TEXTURE_2D;

	int cubeNum = scene->cube[textureIndex];
	if (cubeNum > 0){
		textureType = GL_TEXTURE_CUBE_MAP;
		textureIndex -= cubeNum;
		switch (scene->cube[textureIndex]){
		case 0:
			textureMode = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			break;
		case 1:
			textureMode = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
			break;
		case 2:
			textureMode = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
			break;
		case 3:
			textureMode = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
			break;
		case 4:
			textureMode = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			break;
		case 5:
			textureMode = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
			break;
		}
	}

	glBindTexture(textureType, textureObj[textureIndex - 1]);
	glTexImage2D(textureMode, 0, GL_RGBA8, bitmapWidth, bitmapHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(bitmap32));
	glGenerateMipmap(textureType);
	glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	float largest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
	glTexParameterf(textureType, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, largest);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	FreeImage_Unload(bitmap32);
	FreeImage_Unload(bitmap);

}

void display()
{
	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer

	// viewport transformation
	//glViewport(0, 0, windowSize[0], windowSize[1]);
	//cout << view->viewport[0];
	glViewport(view->viewport[0], view->viewport[1], view->viewport[2], view->viewport[3]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(60.0, (GLfloat)windowSize[0]/(GLfloat)windowSize[1], 1.0, 1000.0);
	gluPerspective(view->fovy, (GLfloat)view->viewport[3] / (GLfloat)view->viewport[2], view->dnear, view->dfar);
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/*gluLookAt(	200.0, 300.0, 150.0,// eye
	0.0, 0.0, 0.0,     // center
	0.0, 1.0, 0.0);    // up*/
	gluLookAt(view->eye[0], view->eye[1], view->eye[2],
		view->vat[0], view->vat[1], view->vat[2],
		view->vup[0], view->vup[1], view->vup[2]);

	glRotatef(curDegree, 0, 1, 0);

	//注意light位置的設定，要在gluLookAt之後
	light();
	for (int a = 0; a < scene->objectNum; a++){
		object = scene->objects[a]->object;

		glPushMatrix();
		glRotatef(scene->objects[a]->rotate[0], scene->objects[a]->rotate[1], scene->objects[a]->rotate[2], scene->objects[a]->rotate[3]);
		glScalef(scene->objects[a]->scale[0], scene->objects[a]->scale[1], scene->objects[a]->scale[2]);
		glTranslatef(scene->objects[a]->transform[0], scene->objects[a]->transform[1], scene->objects[a]->transform[2]);

		int lastMaterial = -1;
		for (size_t i = 0; i < object->fTotal; ++i)
		{
			// set material property if this face used different material
			if (lastMaterial != object->faceList[i].m)
			{
				lastMaterial = (int)object->faceList[i].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT, object->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, object->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR, object->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &object->mList[lastMaterial].Ns);

				//you can obtain the texture name by object->mList[lastMaterial].map_Kd
				//load them once in the main function before mainloop
				//bind them in display function here
			}

			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j < 3; ++j)
			{
				//textex corrd. object->tList[object->faceList[i][j].t].ptr
				glNormal3fv(object->nList[object->faceList[i][j].n].ptr);
				glVertex3fv(object->vList[object->faceList[i][j].v].ptr);
			}


			glEnd();
		}
		glPopMatrix();
	}

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y){
	cout << key << endl;
	switch (key){
	case 'w':
		for (int i = 0; i < 3; i++)
			view->eye[i] -= (view->eye[i] - view->vat[i])*0.1;
		glutPostRedisplay();
		break;
	case 's':
		for (int i = 0; i < 3; i++)
			view->eye[i] += (view->eye[i] - view->vat[i])*0.1;
		glutPostRedisplay();
		break;
	case 'a':
		curDegree += 5;
		glutPostRedisplay();
		break;
	case 'd':
		curDegree -= 5;
		glutPostRedisplay();
		break;
	default:
		if (key >= '1'&&key <= '9')
			selectedObj = key - '1';
		break;
	}
}

void mouse(int button, int state, int x, int y){
	int initPosX, initPosY;
	if (state == GLUT_DOWN){
		initPosX = x;
		initPosY = y;
		if (selectedObj >= 0 && selectedObj < scene->objectNum){
			offsetX = x - scene->objects[selectedObj]->transform[0];
			offsetY = y - scene->objects[selectedObj]->transform[1];
		}
		else if (state == GLUT_UP){
			cout << "OnMouseUp" << endl;
		}
	}
}

void motion(int x, int y){
	if (selectedObj >= 0 && selectedObj < scene->objectNum){
		//cout << offsetX << " " << offsetY<<endl;
		scene->objects[selectedObj]->transform[0] += (x - offsetX)*0.005;
		scene->objects[selectedObj]->transform[1] += (offsetY - y)*0.005;
		glutPostRedisplay();
	}
}