#ifndef	__GUIMANAGER_H_
#define	__GUIMANAGER_H_

#include <vector>

#include "windowsvc.h"
#include "guiwidget.h"

class CGuiManager
{
	public:
		CGuiManager()
		{
			m_bEnabled = false;
		}
		~CGuiManager()
		{
			m_bEnabled = false;

			for(unsigned int i = 0; i < m_vWidgets.size(); ++i)
			{
				delete m_vWidgets[i];
				m_vWidgets[i] = NULL;
			}
		}
			
		void GetClientMousePosition(int& x, int& y);
		void GetAbsoluteMousePosition(int& x, int& y);

	private:
		bool m_bEnabled;

		std::vector<CGuiWidget*> m_vWidgets;
};

#endif
