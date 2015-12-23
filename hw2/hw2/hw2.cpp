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
*		2015 NCTU CG hw2
*/


#include "stdafx.h"
#include "mesh.h"
#include "glut.h"
#include "Object.h"
#include "View.h"
#include "Light.h"
#include "Vector3D.h"
using namespace MathTool;

#define	pass1 0x01
#define	pass2 0x02
#define	pass3 0x03
#define	pass4 0x04
#define	Depth_With_Field 0x01
#define	Depth_Without_Field 0x00

mesh *object;
Scene* scene;
View* view;
Lights* lights;
int windowSize[2];
int selectedObj = 0;
int offsetX, offsetY;
float curDegree = 0;
bool depthfield = true;

const string localpath = "TestScenes\\";
const string sceneName = localpath + "Scene3.scene";
const string viewName = localpath + "Scene3.view";
const string lightName = localpath + "Scene3.light";
unsigned *textureObj;

void light();
void lightAmbientOnly();

void display();
void reshape(GLsizei, GLsizei);
void keyboard(unsigned char key, int x, int y);
void mouse(int, int, int, int);
void motion(int x, int y);

void drawObject();
void drawShadowVol();
void WithoutField();
void WithField();
void drawpoly(float* pos);

int main(int argc, char** argv)
{
	scene = new Scene(sceneName.c_str(), localpath.c_str());
	view = new View(viewName.c_str());
	lights = new Lights(lightName.c_str());
	//view->show();
	//object = new mesh("box.obj");

	glutInit(&argc, argv);
	glutInitWindowSize(view->viewport[2], view->viewport[3]);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutCreateWindow("CG_HW2");
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
	glShadeModel(GL_SMOOTH);

	// z buffer enable
	glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property

	for (int i = 0; i < lights->lightNum; i++){
		glEnable(GL_LIGHT0 + i);
		glLightfv(GL_LIGHT0 + i, GL_POSITION, lights->lightgroup[i]->position);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, lights->lightgroup[i]->diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, lights->lightgroup[i]->specular);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, lights->lightgroup[i]->ambient);
	}

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lights->envambient);
}

void lightAmbientOnly(){
	glShadeModel(GL_SMOOTH);

	// z buffer enable
	glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	float zeroArray[4];
	for (int i = 0; i < 4; i++)
		zeroArray[i] = 0;
	for (int i = 0; i < lights->lightNum; i++){
		glEnable(GL_LIGHT0 + i);
		glLightfv(GL_LIGHT0 + i, GL_POSITION, lights->lightgroup[i]->position);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, zeroArray);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, zeroArray);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, lights->lightgroup[i]->ambient);
	}

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lights->envambient);
}

void display()
{
	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BACK);
	glAlphaFunc(GL_GREATER, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );//這行把畫面清成黑色並且清除z buffer


	glViewport(0, 0, windowSize[0], windowSize[1]);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(view->fovy, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], view->dnear, view->dfar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(view->eye[0], view->eye[1], view->eye[2],
		view->vat[0], view->vat[1], view->vat[2],
		view->vup[0], view->vup[1], view->vup[2]);

	glRotatef(curDegree, 0, 1, 0);

	//注意light位置的設定，要在gluLookAt之後
	if (depthfield)
		WithField();
	else
		WithoutField();

	glutSwapBuffers();
}

void WithField(){
	glClear(GL_ACCUM_BUFFER_BIT);
	for (int ptr1 = -5; ptr1 < 5; ptr1++)
	{
		for (int ptr2 = -5; ptr2 < 5; ptr2++)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt(view->eye[0] + 0.1*ptr1, view->eye[1] + 0.1*ptr2, view->eye[2],
				view->vat[0], view->vat[1], view->vat[2],
				view->vup[0], view->vup[1], view->vup[2]);
			glRotatef(curDegree, 0, 1, 0);

			glEnable(GL_STENCIL_TEST);
			glClearStencil(0);

			for (int i = pass1; i <= pass4; i++){
				switch (i){
				case pass1:
					glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
					lightAmbientOnly();
					drawObject();
					break;
				case pass2:
					glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
					glDepthMask(GL_FALSE);
					glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK);
					glStencilFunc(GL_ALWAYS, 0, 0xff);
					glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
					drawpoly(lights->lightgroup[0]->position);
					glDisable(GL_CULL_FACE);
					break;
				case pass3:
					glEnable(GL_CULL_FACE);
					glCullFace(GL_FRONT);
					glStencilFunc(GL_ALWAYS, 0, 0xff);
					glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
					drawpoly(lights->lightgroup[0]->position);
					glDisable(GL_CULL_FACE);
					break;
				case pass4:
					glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
					glDepthMask(GL_TRUE);
					glStencilFunc(GL_EQUAL, 0, 0xff);
					glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
					light();
					drawObject();
					break;
				}
			}
			glAccum(GL_ACCUM, 0.01);
		}
	}
	glAccum(GL_RETURN, 1.0);
}

void WithoutField(){
	
	glEnable(GL_STENCIL_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearStencil(0);

	for (int i = pass1; i <= pass4; i++){
		switch (i){
		case pass1:
			glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			lightAmbientOnly();
			drawObject();
			break;
		case pass2:
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glDepthMask(GL_FALSE);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glStencilFunc(GL_ALWAYS, 0, 0xff);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			drawpoly(lights->lightgroup[0]->position);
			glDisable(GL_CULL_FACE);
			break;
		case pass3:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glStencilFunc(GL_ALWAYS, 0, 0xff);
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			drawpoly(lights->lightgroup[0]->position);
			glDisable(GL_CULL_FACE);
			break;
		case pass4:
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDepthMask(GL_TRUE);
			glStencilFunc(GL_EQUAL, 0, 0xff);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			light();
			drawObject();
			break;
		}
	}
}

void drawObject(){
	for (int i = 0; i < scene->objectNum; i++){
		object = scene->objects[i]->object;

		glPushMatrix();
		glTranslatef(scene->objects[i]->transform[0], scene->objects[i]->transform[1], scene->objects[i]->transform[2]);
		glRotatef(scene->objects[i]->rotate[0], scene->objects[i]->rotate[1], scene->objects[i]->rotate[2], scene->objects[i]->rotate[3]);
		glScalef(scene->objects[i]->scale[0], scene->objects[i]->scale[1], scene->objects[i]->scale[2]);


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
}

void drawShadowVol(){
	for (int i = 0; i < scene->objectNum; i++){
		object = scene->objects[i]->object;

		glPushMatrix();
		glTranslatef(scene->objects[i]->transform[0], scene->objects[i]->transform[1], scene->objects[i]->transform[2]);
		glRotatef(scene->objects[i]->rotate[0], scene->objects[i]->rotate[1], scene->objects[i]->rotate[2], scene->objects[i]->rotate[3]);
		glScalef(scene->objects[i]->scale[0], scene->objects[i]->scale[1], scene->objects[i]->scale[2]);


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

			Vector3D LightVec, vec1, vec2, FaceNormal;

			LightVec = Vector3D(lights->lightgroup[0]->position) - Vector3D(object->vList[object->faceList[i][0].v].ptr);
			vec1 = Vector3D(object->vList[object->faceList[i][1].v].ptr) - Vector3D(object->vList[object->faceList[i][0].v].ptr);
			vec2 = Vector3D(object->vList[object->faceList[i][2].v].ptr) - Vector3D(object->vList[object->faceList[i][0].v].ptr);
			
			FaceNormal = vec1.Cross(vec2);

			Vector3D VertexToLight[3];
			Vector3D QuadVertex[4];

			for (int j = 0; j < 3; j++)
				VertexToLight[i] = Vector3D(object->vList[object->faceList[i][j].v].ptr) - Vector3D(lights->lightgroup[0]->position);

			if (FaceNormal.Dot(LightVec) >= 0){

				//Quad0~1
				glBegin(GL_QUADS);
				QuadVertex[0] = Vector3D(object->vList[object->faceList[i][0].v].ptr);
				QuadVertex[1] = Vector3D(object->vList[object->faceList[i][0].v].ptr) + (VertexToLight[0] * 4);
				QuadVertex[2] = Vector3D(object->vList[object->faceList[i][1].v].ptr) + (VertexToLight[1] * 4);
				QuadVertex[3] = Vector3D(object->vList[object->faceList[i][1].v].ptr);

				for (int j = 0; j < 3; j++){
					float tmp[3];
					tmp[0] = QuadVertex[i].x;
					tmp[1] = QuadVertex[i].y;
					tmp[2] = QuadVertex[i].z;
					glVertex3fv(tmp);
				}
				glEnd();

				//Quad1~2
				glBegin(GL_QUADS);
				QuadVertex[0] = Vector3D(object->vList[object->faceList[i][1].v].ptr);
				QuadVertex[1] = Vector3D(object->vList[object->faceList[i][1].v].ptr) + (VertexToLight[1] * 4);
				QuadVertex[2] = Vector3D(object->vList[object->faceList[i][2].v].ptr) + (VertexToLight[2] * 4);
				QuadVertex[3] = Vector3D(object->vList[object->faceList[i][2].v].ptr);

				for (int j = 0; j < 3; j++){
					float tmp[3];
					tmp[0] = QuadVertex[i].x;
					tmp[1] = QuadVertex[i].y;
					tmp[2] = QuadVertex[i].z;
					glVertex3fv(tmp);
				}
				glEnd();

				//Quad2~0
				glBegin(GL_QUADS);
				QuadVertex[0] = Vector3D(object->vList[object->faceList[i][2].v].ptr);
				QuadVertex[1] = Vector3D(object->vList[object->faceList[i][2].v].ptr) + (VertexToLight[2] * 4);
				QuadVertex[2] = Vector3D(object->vList[object->faceList[i][0].v].ptr) + (VertexToLight[0] * 4);
				QuadVertex[3] = Vector3D(object->vList[object->faceList[i][0].v].ptr);

				for (int j = 0; j < 3; j++){
					float tmp[3];
					tmp[0] = QuadVertex[i].x;
					tmp[1] = QuadVertex[i].y;
					tmp[2] = QuadVertex[i].z;
					glVertex3fv(tmp);
				}
				glEnd();

			}
			else{
				//Quad0~1
				glBegin(GL_QUADS);
				QuadVertex[0] = Vector3D(object->vList[object->faceList[i][0].v].ptr);
				QuadVertex[1] = Vector3D(object->vList[object->faceList[i][1].v].ptr);
				QuadVertex[2] = Vector3D(object->vList[object->faceList[i][1].v].ptr) + (VertexToLight[1] * 4);
				QuadVertex[3] = Vector3D(object->vList[object->faceList[i][0].v].ptr) + (VertexToLight[0] * 4);

				for (int j = 0; j < 3; j++){
					float tmp[3];
					tmp[0] = QuadVertex[i].x;
					tmp[1] = QuadVertex[i].y;
					tmp[2] = QuadVertex[i].z;
					glVertex3fv(tmp);
				}
				glEnd();

				//Quad1~2
				glBegin(GL_QUADS);
				QuadVertex[0] = Vector3D(object->vList[object->faceList[i][1].v].ptr);
				QuadVertex[1] = Vector3D(object->vList[object->faceList[i][2].v].ptr);
				QuadVertex[2] = Vector3D(object->vList[object->faceList[i][2].v].ptr) + (VertexToLight[2] * 4);
				QuadVertex[3] = Vector3D(object->vList[object->faceList[i][1].v].ptr) + (VertexToLight[1] * 4);

				for (int j = 0; j < 3; j++){
					float tmp[3];
					tmp[0] = QuadVertex[i].x;
					tmp[1] = QuadVertex[i].y;
					tmp[2] = QuadVertex[i].z;
					glVertex3fv(tmp);
				}
				glEnd();

				//Quad2~0
				glBegin(GL_QUADS);
				QuadVertex[0] = Vector3D(object->vList[object->faceList[i][2].v].ptr);
				QuadVertex[1] = Vector3D(object->vList[object->faceList[i][0].v].ptr);
				QuadVertex[2] = Vector3D(object->vList[object->faceList[i][0].v].ptr) + (VertexToLight[0] * 4);
				QuadVertex[3] = Vector3D(object->vList[object->faceList[i][2].v].ptr) + (VertexToLight[2] * 4);

				for (int j = 0; j < 3; j++){
					float tmp[3];
					tmp[0] = QuadVertex[i].x;
					tmp[1] = QuadVertex[i].y;
					tmp[2] = QuadVertex[i].z;
					glVertex3fv(tmp);
				}
				glEnd();
			}
			
		}
		glPopMatrix();
	}

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
	case 'c':
		depthfield = depthfield ? Depth_Without_Field : Depth_With_Field;
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

void drawpoly(float* LightPos)
{
	mesh *obj;
	for (int ID = 0; ID < scene->objectNum; ID++)
	{
		obj = scene->objects[ID]->object;//object
		glPushMatrix();
		glTranslatef(scene->objects[ID]->transform[0], scene->objects[ID]->transform[1], scene->objects[ID]->transform[2]);
		glRotatef(scene->objects[ID]->rotate[0], scene->objects[ID]->rotate[1], scene->objects[ID]->rotate[2], scene->objects[ID]->rotate[3]);
		glScalef(scene->objects[ID]->scale[0], scene->objects[ID]->scale[1], scene->objects[ID]->scale[2]);//object

		int lastMaterial = -1;//object
		for (size_t i = 0; i < obj->fTotal; ++i)
		{
			// set material property if this face used different material
			if (lastMaterial != obj->faceList[i].m)
			{
				lastMaterial = (int)obj->faceList[i].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT, obj->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, obj->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR, obj->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &obj->mList[lastMaterial].Ns);
			}
			float VertexLightVec[2][3];
			for (int ptr1 = 0; ptr1 < 2; ptr1++)
			for (int ptr2 = 0; ptr2 < 3; ptr2++)
			{
				VertexLightVec[ptr1][ptr2] = obj->vList[obj->faceList[i][0].v].ptr[ptr2] - obj->vList[obj->faceList[i][ptr1 + 1].v].ptr[ptr2];
			}//邊的向量
			float VecCross[2][3];
			VecCross[0][0] = VertexLightVec[0][1] * VertexLightVec[1][2] - VertexLightVec[0][2] * VertexLightVec[1][1];
			VecCross[0][1] = VertexLightVec[0][2] * VertexLightVec[1][0] - VertexLightVec[0][0] * VertexLightVec[1][2];
			VecCross[0][2] = VertexLightVec[0][0] * VertexLightVec[1][1] - VertexLightVec[0][1] * VertexLightVec[1][0];

			for (int ptr2 = 0; ptr2 < 3; ptr2++)
				VecCross[1][ptr2] = obj->vList[obj->faceList[i][0].v].ptr[ptr2] - LightPos[ptr2];

			if (VecCross[0][0] * VecCross[1][0] + VecCross[0][1] * VecCross[1][1] + VecCross[0][2] * VecCross[1][2] < 0)
			{
				glBegin(GL_POLYGON);
				glNormal3fv(obj->nList[obj->faceList[i][1].n].ptr);
				glVertex3fv(obj->vList[obj->faceList[i][1].v].ptr);
				glNormal3fv(obj->nList[obj->faceList[i][0].n].ptr);
				glVertex3fv(obj->vList[obj->faceList[i][0].v].ptr);
				glNormal3fv(obj->nList[obj->faceList[i][0].n].ptr);
				glVertex3f(obj->vList[obj->faceList[i][0].v].ptr[0] + 4 * (obj->vList[obj->faceList[i][0].v].ptr[0] - LightPos[0]),
					obj->vList[obj->faceList[i][0].v].ptr[1] + 4 * (obj->vList[obj->faceList[i][0].v].ptr[1] - LightPos[1]),
					obj->vList[obj->faceList[i][0].v].ptr[2] + 4 * (obj->vList[obj->faceList[i][0].v].ptr[2] - LightPos[2]));
				glNormal3fv(obj->nList[obj->faceList[i][1].n].ptr);
				glVertex3f(obj->vList[obj->faceList[i][1].v].ptr[0] + 4 * (obj->vList[obj->faceList[i][1].v].ptr[0] - LightPos[0])
					, obj->vList[obj->faceList[i][1].v].ptr[1] + 4 * (obj->vList[obj->faceList[i][1].v].ptr[1] - LightPos[1])
					, obj->vList[obj->faceList[i][1].v].ptr[2] + 4 * (obj->vList[obj->faceList[i][1].v].ptr[2] - LightPos[2]));
				glEnd();

				glBegin(GL_POLYGON);
				glNormal3fv(obj->nList[obj->faceList[i][2].n].ptr);
				glVertex3fv(obj->vList[obj->faceList[i][2].v].ptr);
				glNormal3fv(obj->nList[obj->faceList[i][1].n].ptr);
				glVertex3fv(obj->vList[obj->faceList[i][1].v].ptr);
				glNormal3fv(obj->nList[obj->faceList[i][1].n].ptr);
				glVertex3f(obj->vList[obj->faceList[i][1].v].ptr[0] + 4 * (obj->vList[obj->faceList[i][1].v].ptr[0] - LightPos[0]),
					obj->vList[obj->faceList[i][1].v].ptr[1] + 4 * (obj->vList[obj->faceList[i][1].v].ptr[1] - LightPos[1]),
					obj->vList[obj->faceList[i][1].v].ptr[2] + 4 * (obj->vList[obj->faceList[i][1].v].ptr[2] - LightPos[2]));
				glNormal3fv(obj->nList[obj->faceList[i][2].n].ptr);
				glVertex3f(obj->vList[obj->faceList[i][2].v].ptr[0] + 4 * (obj->vList[obj->faceList[i][2].v].ptr[0] - LightPos[0]),
					obj->vList[obj->faceList[i][2].v].ptr[1] + 4 * (obj->vList[obj->faceList[i][2].v].ptr[1] - LightPos[1]),
					obj->vList[obj->faceList[i][2].v].ptr[2] + 4 * (obj->vList[obj->faceList[i][2].v].ptr[2] - LightPos[2]));
				glEnd();

				glBegin(GL_POLYGON);
				glNormal3fv(obj->nList[obj->faceList[i][0].n].ptr);
				glVertex3fv(obj->vList[obj->faceList[i][0].v].ptr);
				glNormal3fv(obj->nList[obj->faceList[i][2].n].ptr);
				glVertex3fv(obj->vList[obj->faceList[i][2].v].ptr);
				glNormal3fv(obj->nList[obj->faceList[i][2].n].ptr);
				glVertex3f(obj->vList[obj->faceList[i][2].v].ptr[0] + 4 * (obj->vList[obj->faceList[i][2].v].ptr[0] - LightPos[0]),
					obj->vList[obj->faceList[i][2].v].ptr[1] + 4 * (obj->vList[obj->faceList[i][2].v].ptr[1] - LightPos[1]),
					obj->vList[obj->faceList[i][2].v].ptr[2] + 4 * (obj->vList[obj->faceList[i][2].v].ptr[2] - LightPos[2]));
				glNormal3fv(obj->nList[obj->faceList[i][0].n].ptr);
				glVertex3f(obj->vList[obj->faceList[i][0].v].ptr[0] + 4 * (obj->vList[obj->faceList[i][0].v].ptr[0] - LightPos[0]),
					obj->vList[obj->faceList[i][0].v].ptr[1] + 4 * (obj->vList[obj->faceList[i][0].v].ptr[1] - LightPos[1]),
					obj->vList[obj->faceList[i][0].v].ptr[2] + 4 * (obj->vList[obj->faceList[i][0].v].ptr[2] - LightPos[2]));
				glEnd();
			}

		}

		glPopMatrix();
	}//object
}
