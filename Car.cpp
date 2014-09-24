#include "math.h"
#include <iostream>
#include "Texture.h"
#include "Car.h"
#include "Tools.h"

#define CAR_SCALE 0.35

#define ACCELERATION 0.002
#define BREAK -0.004
#define FS 0.0001

#define ENGINFORCE 50.0
#define BREAKING 30.0
#define REVERSE 8.0
#define AIR_DRAG  0.3257
#define ROLLING_DRAG 20.8
#define MASS 6000

#define STEER_ANGLE 45

Car::Car(Point center) 
	:Movable(center,7*CAR_SCALE,13.5*CAR_SCALE)
{
	speed = 0;
	w = new Wheel*[4];

	w[0] = new Wheel(Point(-4.4, 0, 3), 1.15, 1);
	w[1] = new Wheel(Point(-4.4, 0, -3), 1.15, 1);
	w[2] = new Wheel(Point(4.64, 0, 3), 1.15, 1);
	w[3] = new Wheel(Point(4.64, 0, -3), 1.15, 1);

	SF3dVector axes = SF3dVector(w[0]->GetCenter(), w[2]->GetCenter());
	wheelBase = axes.GetMagnitude();

	Texture tex = Texture::GetInstance();
	side = tex.carSide;
}

double Car::GetSpeed()
{
	if(speed < 0)
		return -speed;
	return speed;
}

void Car::Accelerate()
{
	fTraction = viewDir * ENGINFORCE;
}
void Car::Break()
{
	fTraction = viewDir * -BREAKING;
}
void Car::Reverse()
{
	if(speed>0)
	{
		Break();
		return;
	}
	fTraction = viewDir * -REVERSE;
}

void Car::turn(double steerAngle)
{
	w[0]->IncrementAngle(steerAngle);
	w[1]->IncrementAngle(steerAngle);

	SF3dVector carLocation = SF3dVector(center.x, center.y, center.z);

	SF3dVector frontWheel = carLocation + viewDir * (wheelBase / 2);
	SF3dVector backWheel = carLocation + viewDir * (-wheelBase / 2);

	backWheel = backWheel + viewDir * speed;
	double steeringAngle=angle + steerAngle * PIdiv180 + PI/2;
	frontWheel = frontWheel + SF3dVector(cos(steeringAngle), 0, -sin(steeringAngle)) * speed;
	
	carLocation = (frontWheel + backWheel) / 2;
	angle = atan2( frontWheel.z - backWheel.z , -frontWheel.x + backWheel.x ) + PI/2;
	
	center.x = carLocation.x;
	center.z = carLocation.z;

	computeViewDir();

	//compute velocity vector
	velocity.x = speed *  cos(angle+PI/2);
	velocity.z = speed * -sin(angle+PI/2);
}
void Car::TurnLeft()
{
	turn(STEER_ANGLE);
}

void Car::TurnRight()
{
	turn(-STEER_ANGLE);
}

void Car::Update()
{
	SF3dVector fAirDrag = velocity * -AIR_DRAG * speed;
	SF3dVector fRollDrag = velocity * -ROLLING_DRAG;
	SF3dVector fTotal = fTraction+fAirDrag+fRollDrag;
	SF3dVector a = fTotal/MASS;
	velocity = velocity + a;
	speed = velocity.GetMagnitude() * Tools::Sign(viewDir*velocity);
	if(speed>0.01 || speed<-0.01)
	{
		MoveWith(velocity);
		for(int i=0; i<4; i++)
		{
			w[i]->IncrementRotationAngle( speed * 6 / 0.1 );
		}
	}
	fTraction=SF3dVector();
}

void Car::Draw()
{
	glPushMatrix();
	glTranslated( center.x, center.y, center.z );
	glRotated( angle*180/PI , 0.0, 1.0, 0.0);
	glScaled( CAR_SCALE, CAR_SCALE, CAR_SCALE);

	for (int i = 0; i < 4; i++)
		w[i]->Draw();
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, side);
	glTranslatef(3.5, 0, 0);
	//bottom panel
	//behind the back wheel
	glBegin(GL_POLYGON);
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		
		glTexCoord2f(0.08, 0.18); glVertex3f(0, 0.92, 5.88);
		glTexCoord2f(0.05, 0.20); glVertex3f(0, 0.98, 6.27);
		glTexCoord2f(0.03, 0.23); glVertex3f(0, 1.15, 6.56);
		glTexCoord2f(0.02, 0.27); glVertex3f(0, 1.34, 6.77);
		glTexCoord2f(0.01, 0.31); glVertex3f(0, 1.54, 6.83);
		glTexCoord2f(0.01, 0.43); glVertex3f(0, 2.13, 6.85);
		glTexCoord2f(0.03, 0.48); glVertex3f(0, 2.39, 6.57);
		glTexCoord2f(0.08, 0.48); glVertex3f(0, 2.39, 5.88);		
	}
	glEnd();
	//between the wheels
	glBegin(GL_POLYGON);
	{
		glTexCoord2f(0.26, 0.17); glVertex3f(0, 0.84, 3.35);
		glTexCoord2f(0.26, 0.48); glVertex3f(0, 2.38, 3.35);
		glTexCoord2f(0.72, 0.48); glVertex3f(0, 2.38, -3.10);
		glTexCoord2f(0.72, 0.15); glVertex3f(0, 0.75, -3.10);
	}
	glEnd();

	
	//in front of the front wheel
	glBegin(GL_POLYGON);
	{
		glTexCoord2f(0.90, 0.15); glVertex3f(0, 0.75, -5.63);
		glTexCoord2f(0.90,0.48); glVertex3f(0, 2.38, -5.63);
		glTexCoord2f(0.97,0.48); glVertex3f(0, 2.38, -6.54);
		glTexCoord2f(0.98,0.43); glVertex3f(0, 2.17, -6.65);
		glTexCoord2f(0.99, 0.39); glVertex3f(0, 1.95, -6.84);
		glTexCoord2f(0.99, 0.32); glVertex3f(0, 1.58, -6.86);
		glTexCoord2f(0.99, 0.20); glVertex3f(0, 0.99, -6.86);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//middle panel
	glBegin(GL_POLYGON);
	{
		glVertex3f(0, 2.38, 6.57);//rear bumper
		glVertex3f(-0.1, 3, 6.56);//tail light
		glVertex3f(-0.2, 3.24, 6.49);
		glVertex3f(-0.2, 3.47, 6.32);
		glVertex3f(-0.2, 3.47, 1);
		glVertex3f(-0.2, 3.47, -2.99);
		glVertex3f(-0.2, 3.39, -3.14);
		glVertex3f(-0.15, 3.19, -4.77);
		glVertex3f(-0.1, 2.92, -5.82);
		glVertex3f(-0.05, 2.77, -6.13);
		glVertex3f(-0.025, 2.64, -6.34);
		glVertex3f(0, 2.38, -6.54);		
	}
	glEnd();

	//top panel
	glBegin(GL_POLYGON);
	{
		glVertex3f(-0.2, 3.47, 6.32);
		glVertex3f(-0.25, 3.93, 5.94);
		glVertex3f(-0.3, 4.29, 5.58);
		glVertex3f(-0.35, 4.46, 5.39);

		glVertex3f(-0.4, 4.58, 5.48);//tail

		glVertex3f(-0.4, 4.68, 4.86);
		glVertex3f(-0.4, 4.77, 3.78);
		glVertex3f(-0.4, 4.83, 2.59);
		glVertex3f(-0.4, 4.83, 1.28);
		glVertex3f(-0.4, 4.76, 0.12);
		glVertex3f(-0.4, 4.57, -0.85);//windscreen
		glVertex3f(-0.3, 4.10, -1.87);
		glVertex3f(-0.2, 3.47, -2.99);
	}
	glEnd();

	glTranslatef(-7, 0, 0);
	//left side
	//bottom panel
	//behind the back wheel
	glBegin(GL_POLYGON);
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0, 0.92, 5.88);
		glVertex3f(0, 0.98, 6.27);
		glVertex3f(0, 1.15, 6.56);
		glVertex3f(0, 1.34, 6.77);
		glVertex3f(0, 1.54, 6.83);
		glVertex3f(0, 2.13, 6.85);
		glVertex3f(0, 2.39, 6.57);
		glVertex3f(0, 2.39, 5.88);
	}
	glEnd();
	//between the wheels
	glBegin(GL_POLYGON);
	{
		glVertex3f(0, 0.84, 3.35);
		glVertex3f(0, 2.38, 3.35);
		glVertex3f(0, 2.38, -3.10);
		glVertex3f(0, 0.75, -3.10);
	}
	glEnd();
	//in front of the front wheel
	glBegin(GL_POLYGON);
	{
		glVertex3f(0, 0.75, -5.63);
		glVertex3f(0, 2.38, -5.63);
		glVertex3f(0, 2.38, -6.54);
		glVertex3f(0, 2.17, -6.65);
		glVertex3f(0, 1.95, -6.84);
		glVertex3f(0, 1.58, -6.86);
		glVertex3f(0, 0.99, -6.86);
	}
	glEnd();

	//middle panel
	glBegin(GL_POLYGON);
	{
		glVertex3f(0, 2.38, 6.57);//rear bumper
		glVertex3f(0.1, 3, 6.56);//tail light
		glVertex3f(0.2, 3.24, 6.49);
		glVertex3f(0.2, 3.47, 6.32);
		glVertex3f(0.2, 3.47, 1);
		glVertex3f(0.2, 3.47, -2.99);
		glVertex3f(0.2, 3.39, -3.14);
		glVertex3f(0.15, 3.19, -4.77);
		glVertex3f(0.1, 2.92, -5.82);
		glVertex3f(0.05, 2.77, -6.13);
		glVertex3f(0.025, 2.64, -6.34);
		glVertex3f(0, 2.38, -6.54);
	}
	glEnd();

	//top panel
	glBegin(GL_POLYGON);
	{
		glVertex3f(0.2, 3.47, 6.32);
		glVertex3f(0.25, 3.93, 5.94);
		glVertex3f(0.3, 4.29, 5.58);
		glVertex3f(0.35, 4.46, 5.39);

		glVertex3f(0.4, 4.58, 5.48);//tail

		glVertex3f(0.4, 4.68, 4.86);
		glVertex3f(0.4, 4.77, 3.78);
		glVertex3f(0.4, 4.83, 2.59);
		glVertex3f(0.4, 4.83, 1.28);
		glVertex3f(0.4, 4.76, 0.12);
		glVertex3f(0.4, 4.57, -0.85);//windscreen
		glVertex3f(0.3, 4.10, -1.87);
		glVertex3f(0.2, 3.47, -2.99);
	}
	glEnd();

	//test
	/*glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, side);
	glTranslatef(7.5, 0, 0);
	glBegin(GL_POLYGON);
	{
		glTexCoord2f(0, 0);glVertex3f(0, 0.75, 6.85);
		glTexCoord2f(0, 1); glVertex3f(0, 4.83, 6.85);
		glTexCoord2f(1, 1); glVertex3f(0, 4.83, -6.86);
		glTexCoord2f(1, 0);glVertex3f(0, 0.75, -6.86);
	}
	glEnd();*/

	//911 porsche
	//glBegin(GL_QUADS);
	//{
	//	glColor3f(1.0f, 0.0f, 0.0f);
	//	//behind the back wheel
	//	glVertex3f(0, 1, 7); //a
	//	glVertex3f(0, 3, 7); //b
	//	glVertex3f(0, 3, 5.6);  //c
	//	glVertex3f(0, 0.8, 5.6);  //d

	//	//between the wheels
	//	glVertex3f(0, 0.8, 2.4); //a
	//	glVertex3f(0, 3, 2.4); //b
	//	glVertex3f(0, 3, -2.4);  //c
	//	glVertex3f(0, 0.8, -2.4);  //

	//	//in front of the front wheel
	//	glVertex3f(0, 0.8, -5.6); //a
	//	glVertex3f(0, 3, -5.6); //b
	//	glVertex3f(0, 3, -7.3);  //c
	//	glVertex3f(0, 1, -8);  //

	//	//1st part middle panel
	//	glVertex3f(0, 3, 7); //a
	//	glVertex3f(-0.4, 3.25, 6.8); //b
	//	glVertex3f(-0.4, 3.25, -6.8);  //c
	//	glVertex3f(0, 3, -7);  //

	//	//2nd part middle panel
	//	glVertex3f(-0.4, 3.25, 6.8); //a
	//	glVertex3f(-0.7, 3.65, 6.7); //b
	//	glVertex3f(-0.7, 3.65, -5.7);  //c
	//	glVertex3f(-0.4, 3.25, -6.8);  //

	//	//3rd part middle panel
	//	glVertex3f(-0.7, 3.65, 6.7); //a
	//	glVertex3f(-0.9, 4, 6.5); //b
	//	glVertex3f(-0.9, 4, -5.5);  //c
	//	glVertex3f(-0.7, 3.65, -5.7);  //

	//	//upper panel
	//	glColor3f(1.0f, 0.0f, 0.0f);
	//	glVertex3f(-0.9, 4, 6.5); //a
	//	glVertex3f(-1.2, 6, 1); //b
	//	glVertex3f(-1.2, 6, -2);  //c
	//	glVertex3f(-0.9, 4, -4);  //
	//}
	//glEnd();

	//glTranslatef(-7, 0, 0);
	//glBegin(GL_QUADS);
	//{
	//	glColor3f(1.0f, 0.0f, 0.0f);
	//	//behind the back wheel
	//	glVertex3f(0, 1, 7); //a
	//	glVertex3f(0, 3, 7); //b
	//	glVertex3f(0, 3, 5.6);  //c
	//	glVertex3f(0, 0.8, 5.6);  //d

	//	//between the wheels		
	//	glVertex3f(0, 0.8, 2.4); //a
	//	glVertex3f(0, 3, 2.4); //b
	//	glVertex3f(0, 3, -2.4);  //c
	//	glVertex3f(0, 0.8, -2.4);  //

	//	//in front of the front wheel	
	//	glVertex3f(0, 0.8, -5.6); //a
	//	glVertex3f(0, 3, -5.6); //b
	//	glVertex3f(0, 3, -7.3);  //c
	//	glVertex3f(0, 1, -8);  //

	//	//1st part middle panel
	//	glVertex3f(0, 3, 7); //a
	//	glVertex3f(0.4, 3.25, 6.8); //b
	//	glVertex3f(0.4, 3.25, -6.8);  //c
	//	glVertex3f(0, 3, -7);  //

	//	//2nd part middle panel
	//	glVertex3f(0.4, 3.25, 6.8); //a
	//	glVertex3f(0.7, 3.65, 6.7); //b
	//	glVertex3f(0.7, 3.65, -5.7);  //c
	//	glVertex3f(0.4, 3.25, -6.8);  //

	//	//3rd part middle panel
	//	glVertex3f(0.7, 3.65, 6.7); //a
	//	glVertex3f(0.9, 4, 6.5); //b
	//	glVertex3f(0.9, 4, -5.5);  //c
	//	glVertex3f(0.7, 3.65, -5.7);  //

	//	//upper panel
	//	glVertex3f(0.9, 4, 6.5); //a
	//	glVertex3f(1.2, 6, 1); //b
	//	glVertex3f(1.2, 6, -2);  //c
	//	glVertex3f(0.9, 4, -4);  //
	//}
	//glEnd();

	////top
	//glBegin(GL_QUADS);
	//{
	//	glColor3f(0.0f, 0.0f, 1.0f);

	//	//lower back
	//	glVertex3f(7, 1, 7); //a
	//	glVertex3f(0, 1, 7); //a
	//	glVertex3f(0, 3, 7); //b
	//	glVertex3f(7, 3, 7); //b

	//	//1st part middle panel back
	//	glVertex3f(7, 3, 7); //a
	//	glVertex3f(0, 3, 7); //b
	//	glVertex3f(0.4, 3.25, 6.8);  //c
	//	glVertex3f(6.6, 3.25, 6.8);  //

	//	//2nd part middle panel back
	//	glVertex3f(6.6, 3.25, 6.8); //a
	//	glVertex3f(0.4, 3.25, 6.8); //b
	//	glVertex3f(0.7, 3.65, 6.7);  //c
	//	glVertex3f(6.3, 3.65, 6.7);  //

	//	//3rd part middle panel
	//	glVertex3f(6.3, 3.65, 6.7); //a
	//	glVertex3f(0.7, 3.65, 6.7); //b
	//	glVertex3f(0.9, 4, 6.5);  //c
	//	glVertex3f(6.1, 4, 6.5);  //

	//	//top back
	//	glVertex3f(6.1, 4, 6.5); //a
	//	glVertex3f(0.9, 4, 6.5); //b
	//	glVertex3f(1.2, 6, 1);  //c
	//	glVertex3f(5.8, 6, 1);  //

	//	//top
	//	glVertex3f(5.8, 6, 1); //a
	//	glVertex3f(1.2, 6, 1); //b
	//	glVertex3f(1.2, 6, -2);  //c
	//	glVertex3f(5.8, 6, -2);  //

	//	//windshield
	//	glColor3f(0.5f, 0.5f, 0.5f);
	//	glVertex3f(5.8, 6, -2);  //c
	//	glVertex3f(1.2, 6, -2);  //c
	//	glVertex3f(0.9, 4, -4);  //
	//	glVertex3f(6.1, 4, -4);  //

	//	glColor3f(0.0f, 0.0f, 1.0f);
	//	//bonet
	//	glVertex3f(6.1, 4, -4);  //
	//	glVertex3f(0.9, 4, -4);  //
	//	glVertex3f(0.9, 4, -5.5);  //
	//	glVertex3f(6.1, 4, -5.5);

	//	//upper front
	//	glVertex3f(6.1, 4, -5.5);  //
	//	glVertex3f(0.9, 4, -5.5);  //
	//	glVertex3f(0.7, 3.65, -5.7);  //
	//	glVertex3f(6.3, 3.65, -5.7);

	//	//middle front
	//	glVertex3f(6.3, 3.65, -5.7);  //
	//	glVertex3f(0.7, 3.65, -5.7);  //
	//	glVertex3f(0.4, 3.25, -6.8);  //
	//	glVertex3f(6.6, 3.25, -6.8);

	//	//lower front
	//	glVertex3f(6.6, 3.25, -6.8);  //
	//	glVertex3f(0.4, 3.25, -6.8);  //
	//	glVertex3f(0, 3, -7);  //
	//	glVertex3f(7, 3, -7);

	//	//low
	//	glVertex3f(7, 3, -7);  //
	//	glVertex3f(0, 3, -7);  //
	//	glVertex3f(0, 3, -7.3);  //
	//	glVertex3f(7, 3, -7.3);

	//	//front
	//	glVertex3f(7, 3, -7.3);  //
	//	glVertex3f(0, 3, -7.3);  //
	//	glVertex3f(0, 1, -8);  //
	//	glVertex3f(7, 1, -8);
	//}
	//glEnd();


	glPopMatrix();
}
Car::~Car()
{
}

