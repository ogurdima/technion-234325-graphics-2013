#pragma once

#include "Rgb.h"

// MColorDialog dialog

typedef Rgb Dlgrgb;

typedef enum {DIFFUSE, EMISSIVE, SPECULAR, AMBIENT} ColorType;

class MColorDialog : public CDialog
{
	DECLARE_DYNAMIC(MColorDialog)

public:
	MColorDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~MColorDialog();

// Dialog Data
	enum { IDD = IDD_COLOR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	
	void colorDataToVar(ColorType t);
	void colorDataToWidget(ColorType t);
	void setColorPointers(ColorType t, CMFCColorButton** w, Dlgrgb** v);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPreview();
	CMFCColorButton m_diffuse;
	CMFCColorButton m_emissive;
	CMFCColorButton m_specular;
	CMFCColorButton m_ambient;

	Dlgrgb m_clr_diffuse;
	Dlgrgb m_clr_emissive;
	Dlgrgb m_clr_specular;
	Dlgrgb m_clr_ambient;
	
};
