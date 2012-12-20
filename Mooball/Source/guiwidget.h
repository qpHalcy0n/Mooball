#ifndef	__GUIWIDGET_H_
#define	__GUIWIDGET_H_

#include "quickmath.h"

struct GuiColor
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};

class CGuiWidget
{
	public:
		CGuiWidget();
		~CGuiWidget();

		virtual void Render() = 0;

	protected:
		vec2f m_v2f_Pos;
		vec2f m_v2f_Dim;

		GuiColor m_Color;
};

#endif
