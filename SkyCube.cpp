#include "SkyCube.h"
#include "Texture.h"

#define DIST_FROM_SKY 10000.0f

SkyCube::SkyCube(void):Object3d(Point())
{
}

void SkyCube::Draw()
{	
	Texture tex=Texture::GetInstance();
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(center.x,3*DIST_FROM_SKY/16,center.z);
	glScalef(DIST_FROM_SKY,DIST_FROM_SKY/4,DIST_FROM_SKY);
	//Front
	glBindTexture(GL_TEXTURE_2D,tex.skyCube[0]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( +1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( -1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( -1.0f, +1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( +1.0f, +1.0f, -1.0f);
	glEnd();

	//Back
	glBindTexture(GL_TEXTURE_2D,tex.skyCube[1]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( -1.0f, -1.0f,  +1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( +1.0f, -1.0f,  +1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( +1.0f, +1.0f,  +1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( -1.0f, +1.0f,  +1.0f);
	glEnd();

	//Left
	glBindTexture(GL_TEXTURE_2D,tex.skyCube[2]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( -1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( -1.0f, -1.0f, +1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( -1.0f, +1.0f, +1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( -1.0f, +1.0f, -1.0f);
	glEnd();

	//Right
	glBindTexture(GL_TEXTURE_2D,tex.skyCube[3]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( +1.0f, -1.0f, +1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( +1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( +1.0f, +1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( +1.0f, +1.0f, +1.0f);
	glEnd();

	//Top
	glBindTexture(GL_TEXTURE_2D,tex.skyCube[4]);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(  -1.0f, 1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(  -1.0f, 1.0f, +1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(  +1.0f, 1.0f, +1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(  +1.0f, 1.0f, -1.0f);
	glEnd();
	glPopMatrix();
}
void SkyCube::SetPoz(Point p)
{
	center.x=p.x;
	center.z=p.z;
}
