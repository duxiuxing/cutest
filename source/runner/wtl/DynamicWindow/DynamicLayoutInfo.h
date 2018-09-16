#pragma once

class cdxCDynamicWnd;

class CDynamicLayoutInfo
{
public:
	CSize m_currentSize;  // current client size
	CSize m_initialSize;  // initial client size
	CSize m_deltaSize;    // current - initial

	UINT m_controlCount;  // number of controls (>=0)

	bool m_useScrollPos;  // use scroll pos if m_deltaSize < 0
	CPoint m_scrollPos;   // current scrolling position

public:
	CDynamicLayoutInfo();
	CDynamicLayoutInfo(cdxCDynamicWnd* wnd);

	bool operator=(cdxCDynamicWnd* wnd);
	bool IsInitial() const;
};
