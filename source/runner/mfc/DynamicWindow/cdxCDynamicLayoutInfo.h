#pragma once

class cdxCDynamicWnd;

// Layout information class.
class cdxCDynamicLayoutInfo
{
public:
  CSize current_size;  // current client size
  CSize initial_size;  // initial client size
  CSize delta_size;    // current - initial

  UINT control_count;  // number of controls (>=0)

  bool use_scroll_pos; // use scroll pos if m_szDelta < 0
  CPoint scroll_pos;   // current scrolling position

public:
  cdxCDynamicLayoutInfo();
  cdxCDynamicLayoutInfo( cdxCDynamicWnd *wnd );

  bool operator=( cdxCDynamicWnd *wnd );
  bool IsInitial() const;
};
