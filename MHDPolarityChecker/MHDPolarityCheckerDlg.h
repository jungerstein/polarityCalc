
// MHDPolarityCheckerDlg.h : header file
//

#pragma once

enum changed {idType, idCs, idVa, idTheta, idRhoZero, idRho, idP, idVPara, idVPerp, idV3, idBPara, idBPerp, idB3}; 
// CMHDPolarityCheckerDlg dialog
class CMHDPolarityCheckerDlg : public CDialogEx
{
// Construction
public:
	CMHDPolarityCheckerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MHDPOLARITYCHECKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	double cs;
	double va;
	double rhoZero;
	double theta;
	double dRho;
	double dvPara;
	double dvPerp;
	double dv3;
	double dp;
	double dBPara;
	double dBPerp;
	double dB3;
	double vp;
	CString vg;
	afx_msg void OnBnClickedRadioFast();
	afx_msg void updateAll(changed what);
	afx_msg void OnBnClickedRadioSlow();
	afx_msg void OnBnClickedRadioAlfven();
	afx_msg void OnEnChangeEditRho();
	afx_msg void OnEnChangeEditCs();
	afx_msg void OnEnChangeEditVa();
	afx_msg void OnEnChangeEditTheta();
	afx_msg void OnEnChangeEditRhozero();
	afx_msg void OnEnChangeEditVpara();
	afx_msg void OnEnChangeEditVperp();
	afx_msg void OnEnChangeEditV3();
	afx_msg void OnEnChangeEditP();
	afx_msg void OnEnChangeEditBpara();
	afx_msg void OnEnChangeEditBperp();
	afx_msg void OnEnChangeEditB3();
};
