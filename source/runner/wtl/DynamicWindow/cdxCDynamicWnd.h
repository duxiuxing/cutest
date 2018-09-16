#pragma once

class cdxCDynamicWnd
{
public:
    bool IsUp()
    {
        return false;
    }

    virtual CSize GetCurrentClientSize() const
    {
        return 0;
    }

    CSize m_szInitial;

    virtual UINT GetCtrlCount() const
    {
        return 0;
    }

    bool m_bUseScrollPos;
};
