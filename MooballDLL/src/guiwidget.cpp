#include "guiwidget.h"

CGuiWidget::CGuiWidget()
{
	m_v2f_Pos.x = m_v2f_Pos.y = 0;
	m_v2f_Dim.x = m_v2f_Pos.y = 0;
	
	m_Color.R = 153;
	m_Color.G = 204;
	m_Color.B = 255;
	m_Color.A = 50;
}

CGuiWidget::~CGuiWidget()
{
	//
}
