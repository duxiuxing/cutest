#include "stdafx.h"
#include "cdxCSizeIconCtrl.h"

#include <winuser.h>

#ifndef OBM_SIZE
  #define OBM_SIZE 32766
  #pragma message("*** NOTE[cdxCSizeIconCtrl.cpp]: Please define OEMRESOURCE in your project settings !")
#endif

#ifdef _DEBUG
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

// #pragma warning(disable: 4100)

cdxCSizeIconCtrl::OEMImageList::OEMImageList( UINT nBitmapID, COLORREF crMask )
{
  CBitmap cbmp;
  BITMAP bmp = { 0 };
  VERIFY( cbmp.LoadOEMBitmap( nBitmapID ) );
  VERIFY( cbmp.GetBitmap( &bmp ) );

  this->image_size.cx  = bmp.bmWidth;
  this->image_size.cy  = bmp.bmHeight;

  InitCommonControls();

  VERIFY( Create( bmp.bmWidth, bmp.bmHeight, ILC_COLOR16 | ILC_MASK, 0, 1 ) );
  int i = Add( &cbmp, crMask );
  ASSERT( i == 0 );
}

cdxCSizeIconCtrl::OEMImageList::~OEMImageList()
{}

const CSize &
cdxCSizeIconCtrl::OEMImageList::GetImageSize() const
{
  return this->image_size;
}

cdxCSizeIconCtrl::OEMImageList cdxCSizeIconCtrl::oem_image_list(
  OBM_SIZE,
  ::GetSysColor( COLOR_BTNFACE )
);

HCURSOR cdxCSizeIconCtrl::cursor  = ::LoadCursor( NULL, IDC_SIZENWSE );

cdxCSizeIconCtrl::cdxCSizeIconCtrl( bool reflect_parent_state_in )
  : reflect_parent_state( reflect_parent_state_in )
{}

cdxCSizeIconCtrl::~cdxCSizeIconCtrl()
{}

BEGIN_MESSAGE_MAP( cdxCSizeIconCtrl, CScrollBar )
  //{{AFX_MSG_MAP(cdxCSizeIconCtrl)
  ON_WM_PAINT()
  ON_WM_SETCURSOR()
  ON_WM_LBUTTONDBLCLK()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL
cdxCSizeIconCtrl::Create( CWnd *pParentWnd, UINT nID )
{
  ASSERT( pParentWnd != NULL );
  CRect rect;
  pParentWnd->GetClientRect( &rect );
  if ( !CScrollBar::Create(
         SBS_SIZEBOX | SBS_SIZEBOXBOTTOMRIGHTALIGN | WS_CHILD,
         rect,
         pParentWnd, nID ) )
  {
    return FALSE;
  }

  VERIFY( ModifyStyleEx( 0, WS_EX_TRANSPARENT ) );
  return TRUE;
}

void
cdxCSizeIconCtrl::OnPaint()
{
  CPaintDC dc( this ); // device context for painting

  if ( GetParent() && ( !GetParent()->IsZoomed() || !this->reflect_parent_state ) )
  {
    CRect rect;
    GetClientRect( &rect );
    CSize sz  = oem_image_list.GetImageSize();

    VERIFY( oem_image_list.Draw( &dc,
                                 0,
                                 CPoint( rect.right - sz.cx, rect.bottom - sz.cy ),
                                 ILD_NORMAL | ILD_TRANSPARENT ) );
  }
}

BOOL
cdxCSizeIconCtrl::OnSetCursor( CWnd *pWnd, UINT nHitTest, UINT message )
{
  if ( GetParent() && ( !GetParent()->IsZoomed() || !this->reflect_parent_state ) )
  {
    ::SetCursor( ( nHitTest == HTCLIENT ) ? cursor : NULL );
  }
  return TRUE;
}

/*
   catch Doubleclick - if you don't do that,
   the window will be maximized if you double-blick
   the control.
   Don't know why, but it's annoying.
*/
void
cdxCSizeIconCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
  // CScrollBar::OnLButtonDblClk(nFlags, point);
}
