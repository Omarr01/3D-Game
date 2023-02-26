//  Includes
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <irrKlang.h>
using namespace irrklang;
//-------------------------------------------------------------------------------------

//  Constants
GLuint texDarkSky;
GLuint texBlueSky;
#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)
//-------------------------------------------------------------------------------------

//  Classes
class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;
	bool topView = false;
	bool frontView = false;
	bool sideView = false;

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f,
		float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f,
		float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f
	)
	{
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		if (!topView) {
			Vector3f view = (center - eye).unit();
			eye = eye + view * d;
			center = center + view * d;
		}
		else {
			eye.x += d;
			center.x += d;
		}

	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;
Camera camera2;
//-------------------------------------------------------------------------------------

// Camera and Lights and Assets
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

void setupCamera2() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera2.look();
}

void setupLightsScene1White() {
	glEnable(GL_LIGHT0);
	GLfloat l0Diffuse[] = { 1.0f, 0.0f, 1.0f, 1.0f };
	GLfloat l0Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l0Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l0Position[] = { -10.0f, 5.0f, 1.75f, 1.0f };
	GLfloat l0Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0Diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0Ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0Spec);
	glLightfv(GL_LIGHT0, GL_POSITION, l0Position);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, l0Direction);

	glEnable(GL_LIGHT1);
	GLfloat l1Diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l1Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l1Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l1Position[] = { -43.0f, 5.0f, 1.75f, 1.0f };
	GLfloat l1Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1Diffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, l1Ambient);
	glLightfv(GL_LIGHT1, GL_SPECULAR, l1Spec);
	glLightfv(GL_LIGHT1, GL_POSITION, l1Position);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l1Direction);

	glEnable(GL_LIGHT2);
	GLfloat l2Diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l2Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l2Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l2Position[] = { -76.0f, 5.0f, 1.75f, 1.0f };
	GLfloat l2Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT2, GL_DIFFUSE, l2Diffuse);
	glLightfv(GL_LIGHT2, GL_AMBIENT, l2Ambient);
	glLightfv(GL_LIGHT2, GL_SPECULAR, l2Spec);
	glLightfv(GL_LIGHT2, GL_POSITION, l2Position);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, l2Direction);

	glEnable(GL_LIGHT3);
	GLfloat l3Diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l3Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l3Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l3Position[] = { -109.0f, 5.0f, 1.75f, 1.0f };
	GLfloat l3Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT3, GL_DIFFUSE, l3Diffuse);
	glLightfv(GL_LIGHT3, GL_AMBIENT, l3Ambient);
	glLightfv(GL_LIGHT3, GL_SPECULAR, l3Spec);
	glLightfv(GL_LIGHT3, GL_POSITION, l3Position);
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, l3Direction);

	glEnable(GL_LIGHT4);
	GLfloat l4Diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l4Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l4Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l4Position[] = { -26.5f, 5.0f, -1.05f, 1.0f };
	GLfloat l4Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT4, GL_DIFFUSE, l4Diffuse);
	glLightfv(GL_LIGHT4, GL_AMBIENT, l4Ambient);
	glLightfv(GL_LIGHT4, GL_SPECULAR, l4Spec);
	glLightfv(GL_LIGHT4, GL_POSITION, l4Position);
	glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, l4Direction);

	glEnable(GL_LIGHT5);
	GLfloat l5Diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l5Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l5Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l5Position[] = { -59.5f, 5.0f, -1.05f, 1.0f };
	GLfloat l5Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT5, GL_DIFFUSE, l5Diffuse);
	glLightfv(GL_LIGHT5, GL_AMBIENT, l5Ambient);
	glLightfv(GL_LIGHT5, GL_SPECULAR, l5Spec);
	glLightfv(GL_LIGHT5, GL_POSITION, l5Position);
	glLightf(GL_LIGHT5, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT5, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, l5Direction);

	glEnable(GL_LIGHT6);
	GLfloat l6Diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l6Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l6Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l6Position[] = { -92.5f, 5.0f, -1.05f, 1.0f };
	GLfloat l6Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT6, GL_DIFFUSE, l6Diffuse);
	glLightfv(GL_LIGHT6, GL_AMBIENT, l6Ambient);
	glLightfv(GL_LIGHT6, GL_SPECULAR, l6Spec);
	glLightfv(GL_LIGHT6, GL_POSITION, l6Position);
	glLightf(GL_LIGHT6, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT6, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, l6Direction);

	glEnable(GL_LIGHT7);
	GLfloat l7Diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l7Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l7Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l7Position[] = { -125.5f, 5.0f, -1.05f, 1.0f };
	GLfloat l7Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT7, GL_DIFFUSE, l7Diffuse);
	glLightfv(GL_LIGHT7, GL_AMBIENT, l7Ambient);
	glLightfv(GL_LIGHT7, GL_SPECULAR, l7Spec);
	glLightfv(GL_LIGHT7, GL_POSITION, l7Position);
	glLightf(GL_LIGHT7, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT7, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT7, GL_SPOT_DIRECTION, l7Direction);
}

void setupLightsScene1Orange() {
	glEnable(GL_LIGHT0);
	GLfloat l0Diffuse[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l0Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l0Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l0Position[] = { -10.0f, 5.0f, 1.75f, 1.0f };
	GLfloat l0Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0Diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0Ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0Spec);
	glLightfv(GL_LIGHT0, GL_POSITION, l0Position);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, l0Direction);

	glEnable(GL_LIGHT1);
	GLfloat l1Diffuse[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l1Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l1Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l1Position[] = { -43.0f, 5.0f, 1.75f, 1.0f };
	GLfloat l1Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1Diffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, l1Ambient);
	glLightfv(GL_LIGHT1, GL_SPECULAR, l1Spec);
	glLightfv(GL_LIGHT1, GL_POSITION, l1Position);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l1Direction);

	glEnable(GL_LIGHT2);
	GLfloat l2Diffuse[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l2Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l2Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l2Position[] = { -76.0f, 5.0f, 1.75f, 1.0f };
	GLfloat l2Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT2, GL_DIFFUSE, l2Diffuse);
	glLightfv(GL_LIGHT2, GL_AMBIENT, l2Ambient);
	glLightfv(GL_LIGHT2, GL_SPECULAR, l2Spec);
	glLightfv(GL_LIGHT2, GL_POSITION, l2Position);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, l2Direction);

	glEnable(GL_LIGHT3);
	GLfloat l3Diffuse[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l3Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l3Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l3Position[] = { -109.0f, 5.0f, 1.75f, 1.0f };
	GLfloat l3Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT3, GL_DIFFUSE, l3Diffuse);
	glLightfv(GL_LIGHT3, GL_AMBIENT, l3Ambient);
	glLightfv(GL_LIGHT3, GL_SPECULAR, l3Spec);
	glLightfv(GL_LIGHT3, GL_POSITION, l3Position);
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, l3Direction);

	glEnable(GL_LIGHT4);
	GLfloat l4Diffuse[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l4Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l4Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l4Position[] = { -26.5f, 5.0f, -1.05f, 1.0f };
	GLfloat l4Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT4, GL_DIFFUSE, l4Diffuse);
	glLightfv(GL_LIGHT4, GL_AMBIENT, l4Ambient);
	glLightfv(GL_LIGHT4, GL_SPECULAR, l4Spec);
	glLightfv(GL_LIGHT4, GL_POSITION, l4Position);
	glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, l4Direction);

	glEnable(GL_LIGHT5);
	GLfloat l5Diffuse[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l5Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l5Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l5Position[] = { -59.5f, 5.0f, -1.05f, 1.0f };
	GLfloat l5Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT5, GL_DIFFUSE, l5Diffuse);
	glLightfv(GL_LIGHT5, GL_AMBIENT, l5Ambient);
	glLightfv(GL_LIGHT5, GL_SPECULAR, l5Spec);
	glLightfv(GL_LIGHT5, GL_POSITION, l5Position);
	glLightf(GL_LIGHT5, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT5, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, l5Direction);

	glEnable(GL_LIGHT6);
	GLfloat l6Diffuse[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l6Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l6Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l6Position[] = { -92.5f, 5.0f, -1.05f, 1.0f };
	GLfloat l6Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT6, GL_DIFFUSE, l6Diffuse);
	glLightfv(GL_LIGHT6, GL_AMBIENT, l6Ambient);
	glLightfv(GL_LIGHT6, GL_SPECULAR, l6Spec);
	glLightfv(GL_LIGHT6, GL_POSITION, l6Position);
	glLightf(GL_LIGHT6, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT6, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, l6Direction);

	glEnable(GL_LIGHT7);
	GLfloat l7Diffuse[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l7Spec[] = { 1.0f, 0.5f, 0.0f, 1.0f };
	GLfloat l7Ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat l7Position[] = { -125.5f, 5.0f, -1.05f, 1.0f };
	GLfloat l7Direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT7, GL_DIFFUSE, l7Diffuse);
	glLightfv(GL_LIGHT7, GL_AMBIENT, l7Ambient);
	glLightfv(GL_LIGHT7, GL_SPECULAR, l7Spec);
	glLightfv(GL_LIGHT7, GL_POSITION, l7Position);
	glLightf(GL_LIGHT7, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT7, GL_SPOT_EXPONENT, 10.0);
	glLightfv(GL_LIGHT7, GL_SPOT_DIRECTION, l7Direction);
}

void setupLightsScene2Morning() {
	glEnable(GL_LIGHT0);
	GLfloat l0Diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat l0Spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l0Ambient[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat l0Position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0Diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0Ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0Spec);
	glLightfv(GL_LIGHT0, GL_POSITION, l0Position);
}

void setupLightsScene2Sunset() {
	glEnable(GL_LIGHT1);
	GLfloat l1Diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat l1Spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l1Ambient[] = { 1.0f, 0.0f, 0.0, 1.0f };
	GLfloat l1Position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1Diffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, l1Ambient);
	glLightfv(GL_LIGHT1, GL_SPECULAR, l1Spec);
	glLightfv(GL_LIGHT1, GL_POSITION, l1Position);
}

Model_3DS model_car;
// Scene 1
Model_3DS model_cone;
Model_3DS model_lamp;
Model_3DS model_building;
GLTexture tex_asphalt;
GLTexture tex_sidewalk;
GLTexture tex_sky;
Model_3DS model_tire;
Model_3DS model_can;
Model_3DS model_trashcan;
Model_3DS model_coin;
Model_3DS model_powerup;
// Scene 2
GLTexture tex_desertroad;
GLTexture tex_ground;
Model_3DS model_cactus;
Model_3DS model_rock;
void loadAssets() {
	model_cone.Load("models/cone/cone.3DS");
	model_car.Load("models/car/car.3DS");
	model_lamp.Load("models/lamp/lamp.3DS");
	tex_asphalt.Load("textures/asphalt.bmp");
	tex_sidewalk.Load("textures/sidewalk.bmp");
	loadBMP(&texDarkSky, "textures/sky.bmp", true);
	loadBMP(&texBlueSky, "textures/blu-sky-3.bmp", true);
	model_building.Load("models/building/Build11_3ds.3DS");
	model_tire.Load("models/tire/tire.3DS");
	model_can.Load("models/can/can.3DS");
	model_trashcan.Load("models/can/bure.3DS");
	model_coin.Load("models/coin/coin.3DS");
	model_powerup.Load("models/powerup/powerup.3DS");

	tex_desertroad.Load("textures/RoadsDirt0035_1_350.bmp");
	tex_ground.Load("textures/desert.bmp");
	model_cactus.Load("models/cactus/cactus.3DS");
	model_rock.Load("models/rock/rock.3DS");
}

//-------------------------------------------------------------------------------------

//  Methods Signatures
void main(int argc, char** argv);
void Key(unsigned char key, int x, int y);
void KeyUp(unsigned char key, int x, int y);
void Special(int key, int x, int y);
void SpecialUp(int key, int x, int y);
void PrintOnScreen(int x, int y, int z, char* string, int size);
void Display();
void Anim();
//-------------------------------------------------------------------------------------

//  Global Variables
bool iPressed = false;
bool kPressed = false;
bool jPressed = false;
bool lPressed = false;
bool uPressed = false;
bool oPressed = false;
bool upPressed = false;
bool downPressed = false;
bool leftPressed = false;
bool rightPressed = false;
double transCarX = 0.0;
double transCarZ = 0.0;
bool dPressed = false;
bool aPressed = false;
double transSkyX = 0.0;
bool thirdPersonView = true;
int coinRotateY = 0;
bool carLane = true;
bool stopCar = false;
bool leftCoins[] = { true, true, true, true };
bool rightCoins[] = { true, true, true };
bool leftCoinsScene2[] = { true, true, true, true, true };
bool rightCoinsScene2[] = { true, true, true, true, true };
bool playCoinSound = true;
bool removeCar = false;
bool removePowerup = false;
ISoundEngine* soundEngine = createIrrKlangDevice();
ISound* carSound;
bool playCarSound = true;
bool playCarAccidentSound = true;
bool invincible = false;
bool playTurnSound = true;
bool scene2 = false;
bool startScene1 = false;
bool startScene2 = false;
bool startCarSound = false;
int numberOfCoinsCollected = 0;
bool youWon = false;
bool youLost = false;
bool countCoinsCollected = false;
bool sunset = false;
//-------------------------------------------------------------------------------------

// Main Function
void main(int argc, char** argv) {
	if (!soundEngine) {
		return;
	}
	// soundEngine->play2D("media/passion-127011.mp3", true);

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(1280, 720);
	glutInitWindowPosition(150, 150);
	glutCreateWindow("Final Project");

	glutDisplayFunc(Display);

	glutKeyboardFunc(Key);
	glutKeyboardUpFunc(KeyUp);

	glutSpecialFunc(Special);
	glutSpecialUpFunc(SpecialUp);

	glutIdleFunc(Anim);


	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	loadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glShadeModel(GL_SMOOTH);

	//Start the game with a 3rd person view
	camera.sideView = true;
	camera.eye.x = 4.0f;
	camera.eye.y = 0.0f;
	camera.eye.z = 0.0f;
	camera.center.x = 0.0f;
	camera.center.y = 0.0f;
	camera.center.z = 0.0f;
	camera.moveX(1.7f);
	camera.moveY(3.0f);
	camera.moveZ(-0.5f);
	camera.rotateX(-20.0f);

	carSound = soundEngine->play2D("media/car-acceleration-7087.mp3", true, false, true);
	carSound->setVolume(0);

	glutMainLoop();
}
//-------------------------------------------------------------------------------------

//  Key Functions
void Key(unsigned char key, int x, int y) {

	switch (key) {
	case 'd':
		dPressed = true;
		aPressed = false;
		playTurnSound = true;
		playCoinSound = true;
		break;
	case 'a':
		aPressed = true;
		dPressed = false;
		playTurnSound = true;
		playCoinSound = true;
		break;
	case 't':
		if (!thirdPersonView) {
			camera.moveZ(-5.2f);
			camera.moveY(-0.3f);
			camera2.moveZ(-5.2f);
			camera2.moveY(-0.3f);
			thirdPersonView = true;
		}
		break;
	case 'f':
		if (thirdPersonView) {
			camera.moveZ(5.2f);
			camera.moveY(0.3f);
			camera2.moveZ(-5.2f);
			camera2.moveY(-0.3f);
			thirdPersonView = false;
		}
		break;
	case 'w':
		if (!scene2) {
			startScene1 = true;
		}
		else {
			startScene2 = true;
		}
		startCarSound = true;
		break;

	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}

void KeyUp(unsigned char key, int x, int y) {
	float d = 0.03;

	switch (key) {

	case 'i':
		iPressed = false;
		break;
	case 'k':
		kPressed = false;
		break;
	case 'j':
		jPressed = false;
		break;
	case 'l':
		lPressed = false;
		break;
	case 'u':
		uPressed = false;
		break;
	case 'o':
		oPressed = false;
		break;

	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}

void Special(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_UP:
		upPressed = true;
		break;
	case GLUT_KEY_DOWN:
		downPressed = true;
		break;
	case GLUT_KEY_LEFT:
		leftPressed = true;
		break;
	case GLUT_KEY_RIGHT:
		rightPressed = true;
		break;
	}

	glutPostRedisplay();
}

void SpecialUp(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_UP:
		upPressed = false;
		break;
	case GLUT_KEY_DOWN:
		downPressed = false;
		break;
	case GLUT_KEY_LEFT:
		leftPressed = false;
		break;
	case GLUT_KEY_RIGHT:
		rightPressed = false;
		break;
	}

	glutPostRedisplay();
}
//-------------------------------------------------------------------------------------

//  Functions
void drawRoad() {
	glColor3f(0.4, 0.4, 0.4);

	glEnable(GL_TEXTURE_2D);

	if (!scene2) {
		glBindTexture(GL_TEXTURE_2D, tex_asphalt.texture[0]);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, tex_desertroad.texture[0]);
	}
	

	glPushMatrix();
	glTranslated(-245, 0, -1.5);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);

	glTexCoord2f(0, 0);
	glVertex3f(0, 0, 0);

	glTexCoord2f(50, 0);
	glVertex3f(250, 0, 0);

	glTexCoord2f(50, 1);
	glVertex3f(250, 0, 4.3);

	glTexCoord2f(0, 1);
	glVertex3f(0, 0, 4.3);
	glEnd();
	glPopMatrix();

	glColor3f(1, 1, 1);
}

void drawDesert() {
	glColor3f(0.6, 0.6, 0.6);

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-250, -0.05, -60);
	glTexCoord2f(5, 0);
	glVertex3f(10, 0, -60);
	glTexCoord2f(5, 5);
	glVertex3f(10, -0.05, 60);
	glTexCoord2f(0, 5);
	glVertex3f(-250, -0.05, 60);
	glEnd();
	glPopMatrix();

	glColor3f(1, 1, 1);
}

void drawSidewalk() {
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, tex_sidewalk.texture[0]);

	glPushMatrix();
	glTranslated(-245, 0, -2.4);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);

	glTexCoord2f(0, 0);
	glVertex3f(0, 0, 0);

	glTexCoord2f(150, 0);
	glVertex3f(250, 0, 0);

	glTexCoord2f(150, 1);
	glVertex3f(250, 0, 1);

	glTexCoord2f(0, 1);
	glVertex3f(0, 0, 1);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-245, 0, 2.7);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);

	glTexCoord2f(0, 0);
	glVertex3f(0, 0, 0);

	glTexCoord2f(150, 0);
	glVertex3f(250, 0, 0);

	glTexCoord2f(150, 1);
	glVertex3f(250, 0, 1);

	glTexCoord2f(0, 1);
	glVertex3f(0, 0, 1);
	glEnd();
	glPopMatrix();
}

void drawCar() {
	glEnable(GL_LIGHTING);

	glPushMatrix();
	if (invincible) {
		glColor4f(1.0, 1.0, 1.0, 0.3);
	}
	glTranslated(transCarX, 0.7, 1.75 + transCarZ);
	glRotated(270, 0, 1, 0);
	glScaled(0.07, 0.07, 0.07);
	model_car.Draw();

	glPopMatrix();
}

void drawLeftLampPosts() {
	glDisable(GL_LIGHTING);

	int j = -10;
	for (int i = 0; i < 4; i++) {
		glPushMatrix();
		glTranslated(j, -0.05, 3.11);
		glRotated(90, 0, 1, 0);
		glScaled(0.7, 0.7, 0.7);		
		model_lamp.Draw();
		glPopMatrix();
		j = j - 33;
	}

	glEnable(GL_LIGHTING);
}

void drawRightLampPosts() {
	glDisable(GL_LIGHTING);

	int j = -26.5;
	for (int i = 0; i < 4; i++) {
		glPushMatrix();
		glTranslated(j, -0.05, -1.81);
		glRotated(-90, 0, 1, 0);
		glScaled(0.7, 0.7, 0.7);
		model_lamp.Draw();
		glPopMatrix();
		j = j - 33;
	}

	glEnable(GL_LIGHTING);
}

void drawSky() {
	if (!scene2) {
		glDisable(GL_LIGHTING);
	}
	else {
		if (transCarX < -120) {
			glColor4f(1.0, 0.0, 0.0, 0.3);
		}
	}

	glPushMatrix();
	GLUquadricObj* qobj;
	qobj = gluNewQuadric();
	glTranslated(10 + transSkyX, 0, 0);
	if (!scene2) {
		glBindTexture(GL_TEXTURE_2D, texDarkSky);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, texBlueSky);
	}
	gluQuadricTexture(qobj, true);
	gluQuadricNormals(qobj, GL_SMOOTH);
	gluSphere(qobj, 100, 100, 100);
	gluDeleteQuadric(qobj);
	glPopMatrix();

	if (!scene2) {
		glEnable(GL_LIGHTING);
	}
	else {
		if (transCarX < -120) {
			glColor3f(1.0, 1.0, 1.0);
		}
	}
}

void drawBuilding() {
	int j = 0;
	for (int i = 0; i < 49; i++) {
		glPushMatrix();
		glTranslated(j, 0, -4.2);
		glRotated(0, 0, 1, 0);
		glScaled(0.03, 0.04, 0.03);
		model_building.Draw();
		glPopMatrix();
		j -= 5;
	}
	j = 0;
	for (int i = 0; i < 49; i++) {
		glPushMatrix();
		glTranslated(j, 0, 5.5);
		glRotated(180, 0, 1, 0);
		glScaled(0.03, 0.04, 0.03);
		model_building.Draw();
		glPopMatrix();
		j -= 5;
	}
}

void drawCone() {
	glDisable(GL_LIGHTING);

	int conesPosition = 0;
	for (int i = 0; i < 2; i++) {
		glPushMatrix();
		glTranslated(-130 - conesPosition, 0, 0.4);
		glScaled(0.01, 0.015, 0.01);
		model_cone.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(-130 - conesPosition, 0, 0.0);
		glScaled(0.01, 0.015, 0.01);
		model_cone.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(-130 - conesPosition, 0, -0.4);
		glScaled(0.01, 0.015, 0.01);
		model_cone.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(-130 - conesPosition, 0, -0.8);
		glScaled(0.01, 0.015, 0.01);
		model_cone.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(-130 - conesPosition, 0, -1.2);
		glScaled(0.01, 0.015, 0.01);
		model_cone.Draw();
		glPopMatrix();
		conesPosition += 30;
	}

	glEnable(GL_LIGHTING);
}

void drawTire() {
	int tiresPosition = 0;
	for (int i = 0; i < 2; i++) {
		glPushMatrix();
		glTranslated(-12 - tiresPosition, 0.465, -1.0);
		glRotated(180, 1, 0, 0);
		glScaled(0.001, 0.001, 0.001);
		model_tire.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(-12 - tiresPosition, 0.465, -0.4);
		glRotated(180, 1, 0, 0);
		glScaled(0.001, 0.001, 0.001);
		model_tire.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(-12 - tiresPosition, 0.465, 0.2);
		glRotated(180, 1, 0, 0);
		glScaled(0.001, 0.001, 0.001);
		model_tire.Draw();
		glPopMatrix();
		tiresPosition += 50;
	}
}

void drawCan() {
	glDisable(GL_LIGHTING);

	int cansPosition = 50;
	for (int i = 0; i < 4; i++) {
		glPushMatrix();
		glTranslated(3 - cansPosition, 0.22, 1.3);
		glScaled(0.0017, 0.0025, 0.0017);
		model_trashcan.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(3 - cansPosition, 0.22, 2);
		glScaled(0.0017, 0.0025, 0.0017);
		model_trashcan.Draw();
		glPopMatrix();
		if (i == 2) {
			cansPosition += 25;
		}
		else {
			cansPosition += 50;
		}
	}

	glEnable(GL_LIGHTING);
}
void drawCoin() {
	glDisable(GL_LIGHTING);

	if (!scene2) {
		int leftCoinsPosition = 0;
		for (int i = 0; i < 4; i++)
		{
			if (leftCoins[i]) {
				glPushMatrix();
				glTranslated(-17 - leftCoinsPosition, 0.4, 1.65);
				glRotated(coinRotateY, 0, 1, 0);
				glRotated(90, 0, 0, 1);
				glScaled(0.1, 0.1, 0.1);
				model_coin.Draw();
				glPopMatrix();
			}
			leftCoinsPosition += 50;
		}

		int rightCoinsPosition = 0;
		for (int i = 0; i < 3; i++)
		{
			if (rightCoins[i]) {
				glPushMatrix();
				glTranslated(-42 - rightCoinsPosition, 0.4, -0.45);
				glRotated(coinRotateY, 0, 1, 0);
				glRotated(90, 0, 0, 1);
				glScaled(0.1, 0.1, 0.1);
				model_coin.Draw();
				glPopMatrix();
			}
			rightCoinsPosition += 50;
		}
	}
	else {
		int leftCoinsPosition = 0;
		for (int i = 0; i < 5; i++)
		{
			if (leftCoinsScene2[i]) {
				glPushMatrix();
				glTranslated(-56 - leftCoinsPosition, 0.4, 1.65);
				glRotated(coinRotateY, 0, 1, 0);
				glRotated(90, 0, 0, 1);
				glScaled(0.1, 0.1, 0.1);
				model_coin.Draw();
				glPopMatrix();
			}
			if (i == 3) {
				leftCoinsPosition += 25;
			}
			else {
				leftCoinsPosition += 50;
			}
		}

		int rightCoinsPosition = 0;
		for (int i = 0; i < 5; i++)
		{
			if (rightCoinsScene2[i]) {
				glPushMatrix();
				glTranslated(-42 - rightCoinsPosition, 0.4, -0.45);
				glRotated(coinRotateY, 0, 1, 0);
				glRotated(90, 0, 0, 1);
				glScaled(0.1, 0.1, 0.1);
				model_coin.Draw();
				glPopMatrix();
			}
			if (i == 3) {
				rightCoinsPosition += 22;
			}
			else {
				rightCoinsPosition += 50;
			}
		}
	}

	glEnable(GL_LIGHTING);
}

void drawPowerup() {
	glDisable(GL_LIGHTING);

	if (!removePowerup) {
		glPushMatrix();
		glTranslated(-135, 0.5, 1.65);
		glRotated(90, 1, 0, 0);
		glRotated(90, 0, 0, 1);
		glScaled(0.2, 0.2, 0.2);
		model_powerup.Draw();
		glPopMatrix();
	}

	glDisable(GL_LIGHTING);
}

void drawCactus() {
	int cactusPosition = 0;
	for (int i = 0; i < 7; i++)
	{
		glPushMatrix();
		glTranslated(-10 - cactusPosition, 0.02, 1.1);
		glScaled(0.02, 0.02, 0.02);
		model_cactus.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(-10 - cactusPosition, 0.02, 2.0);
		glScaled(0.02, 0.02, 0.02);
		model_cactus.Draw();
		glPopMatrix();
		if (i >= 3) {
			cactusPosition += 25;
		}
		else {
			cactusPosition += 50;
		}
	}
}

void drawRock() {
	int rockPositon = 0;
	for (int i = 0; i < 7; i++)
	{
		glPushMatrix();
		glTranslated(-35 - rockPositon, 0.45, -0.8);
		glRotated(270, 0, 0, 1);
		glScaled(0.8, 0.8, 0.8);
		model_rock.Draw();
		glPopMatrix();
		glPushMatrix();
		glTranslated(-35 - rockPositon, 0.45, 0.0);
		glRotated(270, 0, 0, 1);
		glScaled(0.8, 0.8, 0.8);
		model_rock.Draw();
		glPopMatrix();
		if (i == 2) {
			rockPositon = 137;
		}
		else if (i > 2) {
			rockPositon += 23;
		}
		else {
			rockPositon += 50;
		}
	}
	
}

void PrintOnScreen(int x, int y, int z, char* string, int size)
{
	int len, i;

	glRasterPos3f(x, y, z);

	len = (int)strlen(string);

	for (i = 0; i < len; i++)
	{
		if (size == 12) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
		}
		else if (size == 18) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
		}
		else {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
		}
	}
}
//-------------------------------------------------------------------------------------

// Power Up Timer
void powerupTimer(int value) {
	invincible = false;
	glutPostRedisplay();
}

//-------------------------------------------------------------------------------------

//  Display Function
void Display() {

	if (!scene2) {
		setupCamera();
		if (!stopCar) {
			setupLightsScene1White();
		}
		else {
			setupLightsScene1Orange();
		}
	}
	else {
		setupCamera2();
		if (!sunset) {
			setupLightsScene2Morning();
		}
		else {
			setupLightsScene2Sunset();
		}
	}
	

	if (startCarSound) {
		carSound->setVolume(1);
		startCarSound = false;
	}

	if (!playCarSound) {
		carSound->setVolume(0);
		playCarSound = true;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawRoad();

	drawCar();

	drawSky();

	if (!scene2) {
		drawSidewalk();

		drawRightLampPosts();

		drawLeftLampPosts();

		drawBuilding();

		drawTire();

		drawCan();

		drawCone();
	}
	else {
		drawDesert();
		
		drawCactus();

		drawRock();
	}

	drawCoin();

	drawPowerup();

	if (youWon) {
		char* youWonText = "You Won! :)";
		if (!carLane) {
			PrintOnScreen(-250, 2.5, -0.3,(char*)youWonText, 18);
		}
		else {
			PrintOnScreen(-250, 2.5, 2.9, (char*)youWonText, 18);
		}
	}

	if (youLost) {
		char* youLostText = "You Lost! :(";
		if (!carLane) {
			PrintOnScreen(transCarX, 3.0, -0.3, (char*)youLostText, 18);
		}
		else {
			PrintOnScreen(transCarX, 3.0, 2.9, (char*)youLostText, 18);
		}
	}

	if (!scene2) {
		char coins[100];
		sprintf(coins, "Coins Collected: %d / 7", numberOfCoinsCollected);
		if (!carLane) {
			PrintOnScreen(transCarX, 2.2, -0.3, (char*)coins, 18);
		}
		else {
			PrintOnScreen(transCarX, 2.2, 2.9, (char*)coins, 18);
		}
	}
	else {
		char coins[100];
		sprintf(coins, "Coins Collected: %d / 10", numberOfCoinsCollected);
		if (!carLane) {
			PrintOnScreen(transCarX, 2.2, -0.3, (char*)coins, 18);
		}
		else {
			PrintOnScreen(transCarX, 2.2, 2.9, (char*)coins, 18);
		}
	}

	glutSwapBuffers();
}
//-------------------------------------------------------------------------------------

// Idle Function
void Anim() {

	if (!scene2 && transCarX < -180) {
		stopCar = true;
		playCarSound = false;

		bool takenAllLeftCoins = true;
		for each (bool var in leftCoins)
		{
			if (var == true) {
				takenAllLeftCoins = false;
				break;
			}
		}

		bool takenAllRightCoins = true;
		for each (bool var in rightCoins)
		{
			if (var == true) {
				takenAllRightCoins = false;
				break;
			}
		}

		if (takenAllLeftCoins && takenAllRightCoins) {
			transCarX = 0.0;
			transCarZ = 0.0;
			transSkyX = 0.0;
			camera2.sideView = true;
			camera2.eye.x = 4.0f;
			camera2.eye.y = 0.0f;
			camera2.eye.z = 0.0f;
			camera2.center.x = 0.0f;
			camera2.center.y = 0.0f;
			camera2.center.z = 0.0f;
			camera2.moveX(1.7f);
			camera2.moveY(3.0f);
			camera2.moveZ(-0.5f);
			camera2.rotateX(-20.0f);
			scene2 = true;
			stopCar = false;
			invincible = false;
			numberOfCoinsCollected = 0;
			removePowerup = false;
		}
		else {
			removeCar = true;
			youLost = true;
		}
	}

	if (scene2 && transCarX < -120) {
		sunset = true;
	}

	if (scene2 && transCarX < -245) {
		stopCar = true;
		playCarSound = false;

		bool takenAllLeftCoins = true;
		for each (bool var in leftCoinsScene2)
		{
			if (var == true) {
				takenAllLeftCoins = false;
				break;
			}
		}

		bool takenAllRightCoins = true;
		for each (bool var in rightCoinsScene2)
		{
			if (var == true) {
				takenAllRightCoins = false;
				break;
			}
		}

		if (takenAllLeftCoins && takenAllRightCoins) {
			youWon = true;
		}
		else {
			removeCar = true;
			youLost = true;
		}
	}

	float camSpeed = 0.2;
	float carSpeed = 80.0;
	float laneSwitchSpeed = 50.0;
	if (scene2) {
		camSpeed = 0.05;
		carSpeed = 40.0;
	}

	if (!stopCar) {
		if (!scene2) {
			if (startScene1) {
				transCarX -= (0.007 * carSpeed);

				transSkyX -= (0.007 * carSpeed);

				camera.rotateX(20.0f);
				camera.moveZ(0.007 * carSpeed);
				camera.rotateX(-20.0f);
			}	
		}
		else {
			if (startScene2) {
				transCarX -= (0.007 * carSpeed);

				transSkyX -= (0.007 * carSpeed);

				camera2.rotateX(20.0f);
				camera2.moveZ(0.007 * carSpeed);
				camera2.rotateX(-20.0f);
			}
		}
	}

	if (!stopCar && dPressed) {
		if (playTurnSound) {
			soundEngine->play2D("media/car-turn(mp3cut.net).mp3", false);
		}
		playTurnSound = false;
		if (!stopCar && transCarZ > -2.0) {
			transCarZ -= (0.01 * laneSwitchSpeed);
			camera.moveX(-0.01 * laneSwitchSpeed);
			camera2.moveX(-0.01 * laneSwitchSpeed);
		}
		carLane = false;
	}

	if (!stopCar && aPressed) {
		if (playTurnSound) {
			soundEngine->play2D("media/car-turn(mp3cut.net).mp3", false);
		}
		playTurnSound = false;
		if (!stopCar && transCarZ < -0.05) {
			transCarZ += (0.01 * laneSwitchSpeed);
			camera.moveX(0.01 * laneSwitchSpeed);
			camera2.moveX(0.01 * laneSwitchSpeed);
		}
		carLane = true;
	}

	if (coinRotateY >= 360) {
		coinRotateY = 0;
	}
	else {
		coinRotateY += 5;
	}

	if (!scene2) {
		// Scene 1 Collisions

		// Cans
		if (!invincible && carLane && ((ceil(transCarX * 100.0) / 100.0 >= -48.50 && ceil(transCarX * 100.0) / 100.0 <= -44.80) ||
			(ceil(transCarX * 100.0) / 100.0 >= -98.50 && ceil(transCarX * 100.0) / 100.0 <= -94.80) ||
			(ceil(transCarX * 100.0) / 100.0 >= -148.50 && ceil(transCarX * 100.0) / 100.0 <= -144.80) ||
			(ceil(transCarX * 100.0) / 100.0 >= -173.50 && ceil(transCarX * 100.0) / 100.0 <= -169.80))) {
			if (playCarAccidentSound) {
				soundEngine->play2D("media/car-crash.mp3", false);
			}
			playCarAccidentSound = false;
			stopCar = true;
			playCarSound = false;
			youLost = true;
		}

		// Tires
		if (!invincible && !carLane && ((ceil(transCarX * 100.0) / 100.0 >= -13.30 && ceil(transCarX * 100.0) / 100.0 <= -9.60) ||
			(ceil(transCarX * 100.0) / 100.0 >= -63.30 && ceil(transCarX * 100.0) / 100.0 <= -59.60))) {
			if (playCarAccidentSound) {
				soundEngine->play2D("media/car-crash.mp3", false);
			}
			playCarAccidentSound = false;
			stopCar = true;
			playCarSound = false;
			youLost = true;
		}

		// Cones
		if (!invincible && !carLane && ((ceil(transCarX * 100.0) / 100.0 >= -131.40 && ceil(transCarX * 100.0) / 100.0 <= -127.7) ||
			(ceil(transCarX * 100.0) / 100.0 >= -161.40 && ceil(transCarX * 100.0) / 100.0 <= -157.7))) {
			if (playCarAccidentSound) {
				soundEngine->play2D("media/car-crash.mp3", false);
			}
			playCarAccidentSound = false;
			stopCar = true;
			playCarSound = false;
			youLost = true;
		}

		// Left Coins
		if (carLane) {
			if (ceil(transCarX * 100.0) / 100.0 >= -18.50 && ceil(transCarX * 100.0) / 100.0 <= -14.80) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				leftCoins[0] = false;
				countCoinsCollected = true;
			}
			if (ceil(transCarX * 100.0) / 100.0 >= -68.50 && ceil(transCarX * 100.0) / 100.0 <= -64.80) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				leftCoins[1] = false;
				countCoinsCollected = true;
			}
			if (ceil(transCarX * 100.0) / 100.0 >= -118.50 && ceil(transCarX * 100.0) / 100.0 <= -114.80) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				leftCoins[2] = false;
				countCoinsCollected = true;
			}
			if (ceil(transCarX * 100.0) / 100.0 >= -168.50 && ceil(transCarX * 100.0) / 100.0 <= -164.80) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				leftCoins[3] = false;
				countCoinsCollected = true;
			}
		}

		// Right Coins
		if (!carLane) {
			if (ceil(transCarX * 100.0) / 100.0 >= -43.47 && ceil(transCarX * 100.0) / 100.0 <= -39.77) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				rightCoins[0] = false;
				countCoinsCollected = true;
			}
			if (ceil(transCarX * 100.0) / 100.0 >= -93.47 && ceil(transCarX * 100.0) / 100.0 <= -89.77) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				rightCoins[1] = false;
				countCoinsCollected = true;
			}
			if (ceil(transCarX * 100.0) / 100.0 >= -143.47 && ceil(transCarX * 100.0) / 100.0 <= -139.77) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				rightCoins[2] = false;
				countCoinsCollected = true;
			}
		}

		// Power Up
		if (carLane && ceil(transCarX * 100.0) / 100.0 >= -136.50 && ceil(transCarX * 100.0) / 100.0 <= -132.80) {
			removePowerup = true;
			soundEngine->play2D("media/power-on-39172.mp3", false);
			invincible = true;
			glutTimerFunc(3000, powerupTimer, 0);
		}
	}
	else {
		// Scene 2 Collisions

		// Cactus
		if (!invincible && carLane && ((ceil(transCarX * 100.0) / 100.0 >= -11.50 && ceil(transCarX * 100.0) / 100.0 <= -7.80) ||
			(ceil(transCarX * 100.0) / 100.0 >= -61.50 && ceil(transCarX * 100.0) / 100.0 <= -57.80) ||
			(ceil(transCarX * 100.0) / 100.0 >= -111.50 && ceil(transCarX * 100.0) / 100.0 <= -107.80) ||
			(ceil(transCarX * 100.0) / 100.0 >= -161.50 && ceil(transCarX * 100.0) / 100.0 <= -157.80) ||
			(ceil(transCarX * 100.0) / 100.0 >= -186.50 && ceil(transCarX * 100.0) / 100.0 <= -182.80) ||
			(ceil(transCarX * 100.0) / 100.0 >= -211.50 && ceil(transCarX * 100.0) / 100.0 <= -207.80) ||
			(ceil(transCarX * 100.0) / 100.0 >= -236.50 && ceil(transCarX * 100.0) / 100.0 <= -232.80))) {
			if (playCarAccidentSound) {
				soundEngine->play2D("media/car-crash.mp3", false);
			}
			playCarAccidentSound = false;
			stopCar = true;
			playCarSound = false;
			youLost = true;
		}

		//// Rocks
		if (!invincible && !carLane && ((ceil(transCarX * 100.0) / 100.0 >= -36.20 && ceil(transCarX * 100.0) / 100.0 <= -32.50) ||
			(ceil(transCarX * 100.0) / 100.0 >= -86.20 && ceil(transCarX * 100.0) / 100.0 <= -82.50) ||
			(ceil(transCarX * 100.0) / 100.0 >= -136.20 && ceil(transCarX * 100.0) / 100.0 <= -132.50) ||
			(ceil(transCarX * 100.0) / 100.0 >= -173.20 && ceil(transCarX * 100.0) / 100.0 <= -169.50) ||
			(ceil(transCarX * 100.0) / 100.0 >= -196.20 && ceil(transCarX * 100.0) / 100.0 <= -192.50) ||
			(ceil(transCarX * 100.0) / 100.0 >= -219.20 && ceil(transCarX * 100.0) / 100.0 <= -215.50) ||
			(ceil(transCarX * 100.0) / 100.0 >= -242.20 && ceil(transCarX * 100.0) / 100.0 <= -238.50))) {
			if (playCarAccidentSound) {
				soundEngine->play2D("media/car-crash.mp3", false);
			}
			playCarAccidentSound = false;
			stopCar = true;
			playCarSound = false;
			youLost = true;
		}

		// Left Coins
		if (carLane) {
			if (ceil(transCarX * 100.0) / 100.0 >= -57.50 && ceil(transCarX * 100.0) / 100.0 <= -53.80) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				leftCoinsScene2[0] = false;
				countCoinsCollected = true;
			}
			if (ceil(transCarX * 100.0) / 100.0 >= -107.50 && ceil(transCarX * 100.0) / 100.0 <= -103.80) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				leftCoinsScene2[1] = false;
				countCoinsCollected = true;
			}
			if (ceil(transCarX * 100.0) / 100.0 >= -157.50 && ceil(transCarX * 100.0) / 100.0 <= -153.80) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				leftCoinsScene2[2] = false;
				countCoinsCollected = true;
			}
			if (ceil(transCarX * 100.0) / 100.0 >= -207.50 && ceil(transCarX * 100.0) / 100.0 <= -203.80) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				leftCoinsScene2[3] = false;
				countCoinsCollected = true;
			}
			if (ceil(transCarX * 100.0) / 100.0 >= -232.50 && ceil(transCarX * 100.0) / 100.0 <= -228.80) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				leftCoinsScene2[4] = false;
				countCoinsCollected = true;
			}
		}

		// Right Coins
		if (!carLane) {
			if (ceil(transCarX * 100.0) / 100.0 >= -43.50 && ceil(transCarX * 100.0) / 100.0 <= -39.80) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				rightCoinsScene2[0] = false;
				countCoinsCollected = true;
			}
			if (ceil(transCarX * 100.0) / 100.0 >= -93.50 && ceil(transCarX * 100.0) / 100.0 <= -89.80) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				rightCoinsScene2[1] = false;
				countCoinsCollected = true;
			}
			if (ceil(transCarX * 100.0) / 100.0 >= -143.50 && ceil(transCarX * 100.0) / 100.0 <= -139.80) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				rightCoinsScene2[2] = false;
				countCoinsCollected = true;
			}
			if (ceil(transCarX * 100.0) / 100.0 >= -193.50 && ceil(transCarX * 100.0) / 100.0 <= -189.80) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				rightCoinsScene2[3] = false;
				countCoinsCollected = true;
			}
			if (ceil(transCarX * 100.0) / 100.0 >= -215.50 && ceil(transCarX * 100.0) / 100.0 <= -211.80) {
				if (playCoinSound && !stopCar) {
					soundEngine->play2D("media/collectcoin-6075.mp3", false);
					playCoinSound = false;
				}
				rightCoinsScene2[4] = false;
				countCoinsCollected = true;
			}
		}

		// Power Up
		if (carLane && ceil(transCarX * 100.0) / 100.0 >= -136.50 && ceil(transCarX * 100.0) / 100.0 <= -132.80) {
			removePowerup = true;
			soundEngine->play2D("media/power-on-39172.mp3", false);
			invincible = true;
			glutTimerFunc(2000, powerupTimer, 0);
		}

	}

	if (countCoinsCollected) {
		numberOfCoinsCollected = 0;
		if (!scene2) {
			for (int i = 0; i < 4; i++)
			{
				if (!leftCoins[i]) {
					numberOfCoinsCollected += 1;
				}
			}
			for (int i = 0; i < 3; i++)
			{
				if (!rightCoins[i]) {
					numberOfCoinsCollected += 1;
				}
			}
		}
		else {
			for (int i = 0; i < 5; i++)
			{
				if (!leftCoinsScene2[i]) {
					numberOfCoinsCollected += 1;
				}
			}
			for (int i = 0; i < 5; i++)
			{
				if (!rightCoinsScene2[i]) {
					numberOfCoinsCollected += 1;
				}
			}
		}
		countCoinsCollected = false;
	}
	

	// Slow down rerendering
	for (int i = 0; i < 100000; i++);

	glutPostRedisplay();
}
//-------------------------------------------------------------------------------------
