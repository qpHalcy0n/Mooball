#ifndef __QUICKDRAW_H_
#define __QUICKDRAW_H_


#include "quickrender.h"


static void RenderTexturedFullscreenQuad()
{
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	glBegin( GL_QUADS );
		glTexCoord2f( 0.0F, 0.0F );
		glVertex3f( -1.0F, -1.0F, 1.0F );

		glTexCoord2f( 0.0F, 1.0F );
		glVertex3f( -1.0F, 1.0F, 1.0F );

		glTexCoord2f( 1.0F, 1.0F );
		glVertex3f( 1.0F, 1.0F, 1.0F );

		glTexCoord2f( 1.0F, 0.0F );
		glVertex3f( 1.0F, -1.0F, 1.0F );
	glEnd();

	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

static void RenderFullscreenQuad( unsigned int color )
{
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	unsigned char r, g, b, a;
	r = ( unsigned char )( ( color & 0xFF000000 ) >> 24 );
	g = ( unsigned char )( ( color & 0x00FF0000 ) >> 16 );
	b = ( unsigned char )( ( color & 0x0000FF00 ) >> 8 );
	a = ( unsigned char )( ( color & 0x000000FF ) );

	glBegin( GL_QUADS );
		glColor4ub( r, g, b, a );
		glVertex3f( -1.0F, -1.0F, 1.0F );
		glVertex3f( -1.0F, 1.0F, 1.0F );
		glVertex3f( 1.0F, 1.0F, 1.0F );
		glVertex3f( 1.0F, -1.0F, 1.0F );
	glEnd();

	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}


static void RenderSSAOQuad( float farClipPlane, float camFOV )
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	int dispWidth = GetScreenWidth();
	int dispHeight = GetScreenHeight();

	float delta_plane = farClipPlane;							
	float far_y = tan(deg_2_rad(camFOV) * 0.5F) * delta_plane;	
	float far_x = far_y * (dispWidth / dispHeight);				

	
	glBegin(GL_QUADS);
		glNormal3f(-far_x, -far_y, delta_plane);
		glTexCoord2f(0.0F, 0.0F);
		glVertex3f(-1.0F, -1.0F, 1.0F);

		glNormal3f(-far_x, far_y, delta_plane);
		glTexCoord2f(0.0F, 1.0F);
		glVertex3f(-1.0F, 1.0F, 1.0F);

		glNormal3f(far_x, far_y, delta_plane);
		glTexCoord2f(1.0F, 1.0F);
		glVertex3f(1.0F, 1.0F, 1.0F);

		glNormal3f(far_x, -far_y, delta_plane);
		glTexCoord2f(1.0F, 0.0F);
		glVertex3f(1.0F, -1.0F, 1.0F);
	glEnd();

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}



static void RenderCoordinateAxes()
{
	glBegin( GL_LINES );
		glColor3f( 1.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 5.0f, 0.0f, 0.0f );

		glColor3f( 0.0f, 1.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 5.0f, 0.0f );
		
		glColor3f( 0.0f, 0.0f, 1.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 5.0f );
	glEnd();
}

#endif