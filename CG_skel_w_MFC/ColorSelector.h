#pragma once

#include "Rgb.h"
// ColorSelector dialog

class ColorSelector : public CDialogEx
{
	DECLARE_DYNAMIC(ColorSelector)

public:
	ColorSelector(CWnd* pParent = NULL);   // standard constructor
	virtual ~ColorSelector();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CMFCColorButton m_colorWidget;
	Rgb GetColor();
};
