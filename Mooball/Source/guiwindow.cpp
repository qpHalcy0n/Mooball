#include "guiwindow.h"

#include "windowsvc.h"

/*
CGuiWindow::CGuiWindow()
{
}

void CGuiWindow::Initialize(vec2f& pos, vec2f& dim, std::string& caption)
{
	m_v2f_Pos = pos;
	m_v2f_Dim = dim;
	m_sCaption = caption;
}

void CGuiWindow::Initialize(vec2f& pos, vec2f& dim, std::string& caption, GuiColor& color)
{
	m_v2f_Pos = pos;
	m_v2f_Dim = dim;
	m_sCaption = caption;
	m_Color = color;
}

void CGuiWindow::Render()
{
	int winX, winY;
	winX = winY = 0;
	int mouseX, mouseY;
	mouseX = mouseY = 0;
	GetScreenDims(winX, winY);
	GetMousePosition(mouseX, mouseY);
	m_v2f_Pos.x = mouseX;
	m_v2f_Pos.y = mouseY;

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glColor4ub(m_Color.R, m_Color.G, m_Color.B, m_Color.A);
	glViewport(m_v2f_Pos.x, winY-m_v2f_Pos.y-m_v2f_Dim.y, m_v2f_Dim.x, m_v2f_Dim.y);
	
	//	Quad
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-1.0, -1.0);
	glTexCoord2f(0, 1);
	glVertex2f(1.0, -1.0);
	glTexCoord2f(1, 1);
	glVertex2f(1.0, 1.0);
	glTexCoord2f(1, 0);
	glVertex2f(-1.0, 1.0);
	glEnd();
}

void CGuiWindow::Dispose()
{
	
}
*/