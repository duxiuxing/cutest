#include "stdafx.h"
#include "DynamicLayoutInfo.h"

#include "cdxCDynamicWnd.h"

CDynamicLayoutInfo::CDynamicLayoutInfo()
	: m_controlCount(0)
	, m_useScrollPos(false)
	, m_scrollPos(0)
{}

CDynamicLayoutInfo::CDynamicLayoutInfo(cdxCDynamicWnd* wnd)
	: m_controlCount(0)
	, m_useScrollPos(false)
	, m_scrollPos(0)
{
	operator=(wnd);
}

bool CDynamicLayoutInfo::operator=(cdxCDynamicWnd* wnd)
{
	if (!wnd || !wnd->IsUp())
	{
		return false;
	}

	m_currentSize = wnd->GetCurrentClientSize();
	m_initialSize = wnd->m_szInitial;
	m_deltaSize = m_currentSize - m_initialSize;
	m_controlCount = wnd->GetCtrlCount();

	if (m_useScrollPos == wnd->m_bUseScrollPos)
	{
        // Todo: asam
		// m_scrollPos.x = wnd->GetWnd()->GetScrollPos(SB_HORZ);
		// m_scrollPos.y = wnd->GetWnd()->GetScrollPos(SB_VERT);
	}

	return true;
}

bool CDynamicLayoutInfo::IsInitial() const
{
	return !m_deltaSize.cx
		   && !m_deltaSize.cy
		   && (!m_useScrollPos || (!m_scrollPos.x && !m_scrollPos.y));
}
