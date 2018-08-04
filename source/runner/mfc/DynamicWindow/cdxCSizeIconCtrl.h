#pragma once

class cdxCSizeIconCtrl : public CScrollBar
{
public:
  class OEMImageList : public CImageList
  {
  public:
    OEMImageList( UINT nBitmapID, COLORREF crMask );
    virtual ~OEMImageList();

    const CSize &GetImageSize() const;

  private:
    CSize image_size;
  };

public:
  cdxCSizeIconCtrl( bool reflect_parent_state_in = true );
  virtual ~cdxCSizeIconCtrl();

  virtual BOOL Create( CWnd *pParentWnd, UINT nID = AFX_IDW_SIZE_BOX );

protected:
  //{{AFX_MSG(cdxCSizeIconCtrl)
  afx_msg void OnPaint();
  afx_msg BOOL OnSetCursor( CWnd *pWnd, UINT nHitTest, UINT message );
  afx_msg void OnLButtonDblClk( UINT nFlags, CPoint point );
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP();

public:
  static OEMImageList oem_image_list;
  static HCURSOR cursor;

  bool  reflect_parent_state;
};
