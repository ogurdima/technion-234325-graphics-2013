// MColorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CG_skel_w_MFC.h"
#include "MColorDialog.h"
#include "afxdialogex.h"

// MColorDialog dialog

IMPLEMENT_DYNAMIC(MColorDialog, CDialog)

MColorDialog::MColorDialog(CWnd* pParent /*=NULL*/)
	: CDialog(MColorDialog::IDD, pParent)
{
}


BOOL MColorDialog::OnInitDialog()
{
    CDialog::OnInitDialog();
    SetWindowText("Pick a Color");
	colorDataToWidget(DIFFUSE);
	colorDataToWidget(EMISSIVE);
	colorDataToWidget(SPECULAR);
	colorDataToWidget(AMBIENT);
	
    return TRUE;
}


MColorDialog::~MColorDialog()
{
}

void MColorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLR_DIFFUSE, m_diffuse);
	DDX_Control(pDX, IDC_CLR_EMISSIVE, m_emissive);
	DDX_Control(pDX, IDC_CLR_SPECULAR, m_specular);
	DDX_Control(pDX, IDC_CLR_AMBIENT, m_ambient);
	colorDataToVar(DIFFUSE);
	colorDataToVar(EMISSIVE);
	colorDataToVar(SPECULAR);
	colorDataToVar(AMBIENT);
	
}


BEGIN_MESSAGE_MAP(MColorDialog, CDialog)
	ON_BN_CLICKED(IDC_PREVIEW, &MColorDialog::OnBnClickedPreview)
END_MESSAGE_MAP()


// MColorDialog message handlers


void MColorDialog::OnBnClickedPreview()
{
	colorDataToVar(DIFFUSE);
	
	CString tmp;

	tmp.Format("Diffuse color: R %3d G %3d B %3d", m_clr_diffuse.r, m_clr_diffuse.g, m_clr_diffuse.b);

	AfxMessageBox(LPCTSTR(tmp));
}


void MColorDialog::colorDataToVar(ColorType t)
{
	CMFCColorButton* clrWidget = NULL;
	Dlgrgb* clrVar = NULL;
	setColorPointers(t, &clrWidget, &clrVar);
	COLORREF clr = clrWidget->GetColor();
	clrVar->r = ((double)GetRValue(clr))/255;
	clrVar->g = ((double)GetGValue(clr))/255;
	clrVar->b = ((double)GetBValue(clr))/255;
}

void MColorDialog::colorDataToWidget(ColorType t)
{
	CMFCColorButton* clrWidget = NULL;
	Dlgrgb* clrVar = NULL;
	setColorPointers(t, &clrWidget, &clrVar);
	byte br = ((int)clrVar->r * 256);
	byte bg = ((int)clrVar->g * 256);
	byte bb = ((int)clrVar->b * 256);
	COLORREF clr = RGB(br, bg, bb);
	clrWidget->SetColor(clr);
}

void MColorDialog::setColorPointers(ColorType t, CMFCColorButton** w, Dlgrgb** v)
{
	CMFCColorButton* clrWidget = NULL;
	Dlgrgb* clrVar = NULL;
	switch (t)
	{
		case DIFFUSE:
			clrWidget = &m_diffuse;
			clrVar = &m_clr_diffuse;
			break;
		case EMISSIVE:
			clrWidget = &m_emissive;
			clrVar = &m_clr_emissive;
			break;
		case SPECULAR:
			clrWidget = &m_specular;
			clrVar = &m_clr_specular;
			break;
		case AMBIENT:
			clrWidget = &m_ambient;
			clrVar = &m_clr_ambient;
	}
	*w = clrWidget;
	*v = clrVar;
}