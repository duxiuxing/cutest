#include "stdafx.h"
#include "dynamic_layout_info.h"

#include "cdxCDynamicWnd.h"

DynamicLayoutInfo::DynamicLayoutInfo()
	: ControlCount(0)
	, UseScrollPos(false)
	, ScrollPos(0)
{}

DynamicLayoutInfo::DynamicLayoutInfo(cdxCDynamicWnd* wnd)
	: ControlCount(0)
	, UseScrollPos(false)
	, ScrollPos(0)
{
	operator = (wnd);
}

bool DynamicLayoutInfo::operator=(cdxCDynamicWnd* wnd)
{
	if (!wnd || !wnd->IsUp())
	{
		return false;
	}

	CurrentSize = wnd->GetCurrentClientSize();
	InitialSize = wnd->m_szInitial;
	DeltaSize = CurrentSize - InitialSize;
	ControlCount = wnd->GetCtrlCount();

	if (UseScrollPos == wnd->m_bUseScrollPos)
	{
		ScrollPos.x = wnd->GetWnd()->GetScrollPos(SB_HORZ);
		ScrollPos.y = wnd->GetWnd()->GetScrollPos(SB_VERT);
	}

	return true;
}

bool DynamicLayoutInfo::IsInitial() const
{
	return !DeltaSize.cx
		   && !DeltaSize.cy
		   && (!UseScrollPos || (!ScrollPos.x && !ScrollPos.y));
}
