#pragma once



// MColorDialog dialog

typedef struct {
	int r;
	int g;
	int b;
} Dlgrgb;

typedef enum {DIFFUSE, EMMUSIVE, SPECULAR} ColorType;

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
	CMFCColorButton m_emmusive;
	CMFCColorButton m_specular;


	Dlgrgb m_clr_diffuse;
	Dlgrgb m_clr_emussive;
	Dlgrgb m_clr_specular;
};
