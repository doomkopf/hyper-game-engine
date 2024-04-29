// HGMapCompilerDlg.h : Headerdatei
//

#pragma once


// CHGMapCompilerDlg Dialogfeld
class CHGMapCompilerDlg : public CDialog
{
// Konstruktion
public:
	CHGMapCompilerDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
	enum { IDD = IDD_HGMAPCOMPILER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString file;
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedCompile();
protected:
	virtual void OnOK();
};
