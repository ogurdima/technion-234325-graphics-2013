// ColorSelector.cpp : implementation file
//

#include "stdafx.h"
#include "CG_skel_w_MFC.h"
#include "ColorSelector.h"
#include "afxdialogex.h"


// ColorSelector dialog

IMPLEMENT_DYNAMIC(ColorSelector, CDialogEx)

ColorSelector::ColorSelector(CWnd* pParent /*=NULL*/)
	: CDialogEx(ColorSelector::IDD, pParent)
{

}

ColorSelector::~ColorSelector()
{
}

void ColorSelector::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON1, m_colorWidget);
}


BEGIN_MESSAGE_MAP(ColorSelector, CDialogEx)
END_MESSAGE_MAP()


// ColorSelector message handlers

Rgb ColorSelector::GetColor()
{
	COLORREF clr = m_colorWidget.GetColor();
	return Rgb(((float)GetRValue(clr)/255), ((float)GetGValue(clr)/255), ((float)GetBValue(clr)/255));
}