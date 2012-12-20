#include "quickcam.h"




CCameraObject::CCameraObject()
{
    memset(m_curPosition, 0, sizeof(float) * 3);
    memset(m_curLookAt, 0, sizeof(float) * 3);
    memset(m_curUpVec, 0, sizeof(float) * 3);

    m_curViewVec[0] = 0.0F;
    m_curViewVec[1] = 0.0F;
    m_curViewVec[2] = -1.0F;
    m_curStrafeVec[0] = 1.0F;
    m_curStrafeVec[1] = 0.0F;
    m_curStrafeVec[2] = 0.0F;

    m_curFOV = 0.0F;
    m_curAspect = 0.0F;
    m_zNear = 0.0F;
    m_zFar = 0.0F;
}

CCameraObject::~CCameraObject()
{
    memset(m_curPosition, 0, sizeof(float) * 3);
    memset(m_curLookAt, 0, sizeof(float) * 3);
    memset(m_curUpVec, 0, sizeof(float) * 3);
    memset(m_curViewVec, 0, sizeof(float) * 3);
    memset(m_curStrafeVec, 0, sizeof(float) * 3);

    m_curFOV = 0.0F;
    m_curAspect = 0.0F;
    m_zNear = 0.0F;
    m_zFar = 0.0F;
}



void CCameraObject::SetCamera(vec3 camPos, vec3 lookAt, vec3 up)
{
    vec3_copy(m_curPosition, camPos);
    vec3_copy(m_curLookAt, lookAt);
    vec3_copy(m_curUpVec, up);

    vec3_sub(m_curLookAt, m_curPosition, m_curViewVec);
    vec3_normalize(m_curViewVec);

    vec3_cross(m_curViewVec, m_curUpVec, m_curStrafeVec);
    vec3_normalize(m_curStrafeVec);
}

void CCameraObject::SetCamera(vec3 camPos)
{
    // Get the delta vector between old cam pos and new one //
    vec3 delta_pos;
    vec3_sub(camPos, m_curPosition, delta_pos);
    vec3_copy(m_curPosition, camPos);

    // Move look at position according to dV //
    vec3_sub_replace(m_curLookAt, delta_pos);
}




/*
    rotate
    Internally used helper for CCameraObject::RotateByMouse
*/
static void rotate(float* camPos, float* lookAtPos, float* upVec, float xDir)
{
	quat qRot, qView, qNewView;
	quat_rotate(qRot, deg_2_rad(xDir), upVec);

	qView[0] = lookAtPos[0] - camPos[0];
	qView[1] = lookAtPos[1] - camPos[1];
	qView[2] = lookAtPos[2] - camPos[2];
	qView[3] = 0.0f;

	quat_mul(qRot, qView, qNewView);
	quat_conjugate(qRot);
	quat_mul(qNewView, qRot, qNewView);

	lookAtPos[0] = camPos[0] + qNewView[0];
	lookAtPos[1] = camPos[1] + qNewView[1];
	lookAtPos[2] = camPos[2] + qNewView[2];
}


void CCameraObject::RotateByMouse(int mouseX, int mouseY, int midScreenX, int midScreenY)
{
	vec3 tmpLook;
	vec3_copy(tmpLook, m_curLookAt);

	float xDir, yRot;
	xDir = yRot = 0.0f;

	if((mouseX == midScreenX) && (mouseY == midScreenY))
		return;

	// Get direction vectors in screen space //
	xDir = -float(mouseX) / 10.0f * 4.0f;         // 4.0F sensitivity coef.
	yRot = -float(mouseY) / 10.0f * 4.0f;		  // Invert for GL

	vec3 up = {0.0, 1.0, 0.0};
	rotate(m_curPosition, m_curLookAt, up, xDir);
	rotate(m_curPosition, m_curLookAt, m_curStrafeVec, yRot);

    // Update view and strafe vectors, since view vector has changed //
    vec3_sub(m_curLookAt, m_curPosition, m_curViewVec);
    vec3_normalize(m_curViewVec);

    vec3_cross(m_curViewVec, m_curUpVec, m_curStrafeVec);
    vec3_normalize(m_curStrafeVec);

	// Check that we dont pivot too far by obtaining the new view vector and
	// Checking the angle between it and the view vector on the x/z plane
	vec3 projected_vec = {m_curViewVec[0], 0.0F, m_curViewVec[2]};
	vec3_normalize(projected_vec);

	float ang = rad_2_deg(acos(vec3_dot(m_curViewVec, projected_vec)));
}

void CCameraObject::SetPerspective(float fovy, float aspect, float nearClip, float farClip)
{
    m_curFOV = fovy;
    m_curAspect = aspect;
    m_zNear = nearClip;
    m_zFar = farClip;
}


void CCameraObject::MoveForward(float fMul)
{
    vec3 fVec;
    vec3_copy(fVec, m_curViewVec);
    vec3_scale(fVec, fMul);
    vec3_add_replace(m_curPosition, fVec);
    vec3_add_replace(m_curLookAt, fVec);
}

void CCameraObject::MoveBack(float fMul)
{
    vec3 fVec;
    vec3_copy(fVec, m_curViewVec);
    vec3_scale(fVec, fMul);
    vec3_sub_replace(m_curPosition, fVec);
    vec3_sub_replace(m_curLookAt, fVec);
}

void CCameraObject::MoveRight(float fMul)
{
    vec3 sVec;
    vec3_copy(sVec, m_curStrafeVec);
    vec3_scale(sVec, fMul);
    vec3_add_replace(m_curPosition, sVec);
    vec3_add_replace(m_curLookAt, sVec);
}

void CCameraObject::MoveLeft(float fMul)
{
    vec3 sVec;
    vec3_copy(sVec, m_curStrafeVec);
    vec3_scale(sVec, fMul);
    vec3_sub_replace(m_curPosition, sVec);
    vec3_sub_replace(m_curLookAt, sVec);
}



void CCameraObject::Update()
{
	glMatrixMode(GL_MODELVIEW);
	mat4 la;
	mat4_load_lookat_rh(m_curPosition, m_curLookAt, m_curUpVec, la);
    glLoadMatrixf(la);

	glMatrixMode(GL_PROJECTION);
	mat4 p;
	mat4_load_perspective_rh(m_curFOV, m_curAspect, m_zNear, m_zFar, p);
	glLoadMatrixf(p);
}


