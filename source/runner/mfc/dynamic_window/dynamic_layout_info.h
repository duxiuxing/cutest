#pragma once

class cdxCDynamicWnd;

struct DynamicLayoutInfo
{
	CSize CurrentSize;  // current client size
	CSize InitialSize;  // initial client size
	CSize DeltaSize;    // current - initial

	UINT ControlCount;  // number of controls (>=0)

	bool UseScrollPos;  // use scroll pos if m_deltaSize < 0
	CPoint ScrollPos;   // current scrolling position

	DynamicLayoutInfo();
	DynamicLayoutInfo(cdxCDynamicWnd* wnd);

	bool operator=(cdxCDynamicWnd* wnd);
	bool IsInitial() const;
};
