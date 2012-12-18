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

	setDiffuseColors();

	UpdateData(FALSE);
    return TRUE;
}


MColorDialog::~MColorDialog()
{
}

void MColorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLR_DIFFUSE, m_diffuse);
	DDX_Control(pDX, IDC_CLR_EMMUSIVE, m_emmusive);
	DDX_Control(pDX, IDC_CLR_SPECULAR, m_specular);
}


BEGIN_MESSAGE_MAP(MColorDialog, CDialog)
	ON_BN_CLICKED(IDC_PREVIEW, &MColorDialog::OnBnClickedPreview)
END_MESSAGE_MAP()


// MColorDialog message handlers


void MColorDialog::OnBnClickedPreview()
{
	getDiffuseColors();
	
	CString tmp;

	tmp.Format("Diffuse color: R %3d G %3d B %3d", m_clr_diffuse.r, m_clr_diffuse.g, m_clr_diffuse.b);

	AfxMessageBox(LPCTSTR(tmp));
}


void MColorDialog::getDiffuseColors()
{
	COLORREF clr = m_diffuse.GetColor();
	m_clr_diffuse.r = GetRValue(clr);
	m_clr_diffuse.g = GetGValue(clr);
	m_clr_diffuse.b = GetBValue(clr);
}
void MColorDialog::setDiffuseColors()
{
	byte br = (m_clr_diffuse.r % 256);
	byte bg = (m_clr_diffuse.g % 256);
	byte bb = (m_clr_diffuse.b % 256);
	COLORREF clr = RGB(br, bg, bb);
	m_diffuse.SetColor(clr);
}