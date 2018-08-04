#include "stdafx.h"
#include "cdxCDynamicLayoutInfo.h"

#include "cdxCDynamicWnd.h"

cdxCDynamicLayoutInfo::cdxCDynamicLayoutInfo()
  : control_count( 0 )
  , use_scroll_pos( false )
  , scroll_pos( 0 )
{}

cdxCDynamicLayoutInfo::cdxCDynamicLayoutInfo( cdxCDynamicWnd *wnd )
  : control_count( 0 )
  , use_scroll_pos( false )
  , scroll_pos( 0 )
{
  operator=( wnd );
}

bool cdxCDynamicLayoutInfo::operator=( cdxCDynamicWnd *wnd )
{
/*  if ( !wnd || !wnd->IsUp() )
  {
    return false;
  }

  this->current_size = wnd->GetCurrentClientSize();
  this->initial_size = wnd->m_szInitial;
  this->delta_size = this->current_size - this->initial_size;
  this->control_count = wnd->GetCtrlCount();

  if ( this->use_scroll_pos == wnd->m_bUseScrollPos )
  {
    scroll_pos.x = wnd->GetWnd()->GetScrollPos( SB_HORZ );
    scroll_pos.y = wnd->GetWnd()->GetScrollPos( SB_VERT );
  }
*/
  return true;
}

bool cdxCDynamicLayoutInfo::IsInitial() const
{
  return !this->delta_size.cx
         && !this->delta_size.cy
         && ( !use_scroll_pos || ( !scroll_pos.x && !scroll_pos.y ) );
}
