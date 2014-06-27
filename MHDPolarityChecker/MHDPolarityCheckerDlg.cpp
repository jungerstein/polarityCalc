
// MHDPolarityCheckerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MHDPolarityChecker.h"
#include "MHDPolarityCheckerDlg.h"
#include "afxdialogex.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum waveType {fast, slow, aw};

double factorTable[8];
double vgPara, vgPerp, vp_Gross;
waveType typ; 

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMHDPolarityCheckerDlg dialog

void recompute(double cs, double va, double rhoZero, double thetaDeg)
{
	double th, vp, delta; 
	th = thetaDeg / 180 * 3.141592653589793l; 
	delta = sqrt((cs*cs + va*va)*(cs*cs + va*va) - 4 * cs * cs * va * va * cos(th));

	switch(typ)
	{
	case fast:
		vp = sqrt((cs*cs + va*va + delta) / 2); 
		break;
	case slow:
		vp = sqrt((cs*cs + va*va - delta) / 2); 
		break;
	case aw:
		vp = va * cos(th); 
		break;
	default:
		break;
	}

	switch(typ)
	{
	case fast:
	case slow:
		factorTable[0] = rhoZero;				// deltaRho
		factorTable[1] = vp / (vp * vp - cos(th) * cos(th) * va * va) * (vp * vp * cos(th) - cos(th) * va * va); // dvPara
	    factorTable[2] = vp / (vp * vp - cos(th) * cos(th) * va * va) * (vp * vp * sin(th)); // dvPerp
		factorTable[3] = 0; // dv3
		factorTable[4] = cs * cs * rhoZero; //deltaP
		factorTable[5] = vp / (vp * vp - cos(th) * cos(th) * va * va) * vp * va * sqrt(rhoZero) * (1 - cos(th) * cos(th)); // dBPara
		factorTable[6] = vp / (vp * vp - cos(th) * cos(th) * va * va) * vp * va * sqrt(rhoZero) * (- cos(th) * sin(th)); // dBPerp
		factorTable[7] = 0; // dB3
		break;
	case aw:
		factorTable[0] = 0;				// deltaRho
		factorTable[1] = 0; // dvPara
	    factorTable[2] = 0; // dvPerp
		factorTable[3] = - cos(th) / abs(cos(th)); // dv3
		factorTable[4] = 0; //deltaP
		factorTable[5] = 0; // dBPara
		factorTable[6] = 0; // dBPerp
		factorTable[7] = 1; // dB3
		break;
	default:
		break;
	}
	vp_Gross = vp; 
}


CMHDPolarityCheckerDlg::CMHDPolarityCheckerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMHDPolarityCheckerDlg::IDD, pParent)
	, cs(1)
	, va(2)
	, rhoZero(1)
	, theta(2)
	, dRho(0)
	, dvPara(0)
	, dvPerp(0)
	, dv3(0)
	, dp(0)
	, dBPara(0)
	, dBPerp(0)
	, dB3(0)
	, vp(0)
	, vg(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMHDPolarityCheckerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CS, cs);
	DDX_Text(pDX, IDC_EDIT_VA, va);
	DDX_Text(pDX, IDC_EDIT_RHOZERO, rhoZero);
	DDX_Text(pDX, IDC_EDIT_THETA, theta);
	DDX_Text(pDX, IDC_EDIT_RHO, dRho);
	DDX_Text(pDX, IDC_EDIT_VPARA, dvPara);
	DDX_Text(pDX, IDC_EDIT_VPERP, dvPerp);
	DDX_Text(pDX, IDC_EDIT_V3, dv3);
	DDX_Text(pDX, IDC_EDIT_P, dp);
	DDX_Text(pDX, IDC_EDIT_BPARA, dBPara);
	DDX_Text(pDX, IDC_EDIT_BPERP, dBPerp);
	DDX_Text(pDX, IDC_EDIT_B3, dB3);
	DDX_Text(pDX, IDC_EDIT_VP, vp);
	DDX_Text(pDX, IDC_EDIT_VG, vg);
}

BEGIN_MESSAGE_MAP(CMHDPolarityCheckerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_FAST, &CMHDPolarityCheckerDlg::OnBnClickedRadioFast)
	ON_BN_CLICKED(IDC_RADIO_SLOW, &CMHDPolarityCheckerDlg::OnBnClickedRadioSlow)
	ON_BN_CLICKED(IDC_RADIO_ALFVEN, &CMHDPolarityCheckerDlg::OnBnClickedRadioAlfven)
	ON_EN_CHANGE(IDC_EDIT_RHO, &CMHDPolarityCheckerDlg::OnEnChangeEditRho)
	ON_EN_CHANGE(IDC_EDIT_CS, &CMHDPolarityCheckerDlg::OnEnChangeEditCs)
	ON_EN_CHANGE(IDC_EDIT_VA, &CMHDPolarityCheckerDlg::OnEnChangeEditVa)
	ON_EN_CHANGE(IDC_EDIT_THETA, &CMHDPolarityCheckerDlg::OnEnChangeEditTheta)
	ON_EN_CHANGE(IDC_EDIT_RHOZERO, &CMHDPolarityCheckerDlg::OnEnChangeEditRhozero)
	ON_EN_CHANGE(IDC_EDIT_VPARA, &CMHDPolarityCheckerDlg::OnEnChangeEditVpara)
	ON_EN_CHANGE(IDC_EDIT_VPERP, &CMHDPolarityCheckerDlg::OnEnChangeEditVperp)
	ON_EN_CHANGE(IDC_EDIT_V3, &CMHDPolarityCheckerDlg::OnEnChangeEditV3)
	ON_EN_CHANGE(IDC_EDIT_P, &CMHDPolarityCheckerDlg::OnEnChangeEditP)
	ON_EN_CHANGE(IDC_EDIT_BPARA, &CMHDPolarityCheckerDlg::OnEnChangeEditBpara)
	ON_EN_CHANGE(IDC_EDIT_BPERP, &CMHDPolarityCheckerDlg::OnEnChangeEditBperp)
	ON_EN_CHANGE(IDC_EDIT_B3, &CMHDPolarityCheckerDlg::OnEnChangeEditB3)
END_MESSAGE_MAP()


// CMHDPolarityCheckerDlg message handlers

BOOL CMHDPolarityCheckerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMHDPolarityCheckerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMHDPolarityCheckerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMHDPolarityCheckerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMHDPolarityCheckerDlg::updateAll(changed what)
{
	double rate = 1.0;
	UpdateData(1);
	switch(what)
	{
		case idType:
		case idCs:
		case idVa:
		case idRhoZero:
		case idTheta:
			recompute(cs, va, rhoZero, theta);
			rate = 1.0;
			break;
		case idRho:
			rate = dRho / factorTable[0];
			break;
		case idVPara:
			rate = dvPara / factorTable[1];
			break;
		case idVPerp:
			rate = dvPerp / factorTable[2];
			break;
		case idV3:
			rate = dv3 / factorTable[3];
		    break;
		case idP:
			rate = dp / factorTable[4];
		    break;
		case idBPara:
			rate = dBPara / factorTable[5];
		    break;
		case idBPerp:
			rate = dBPerp / factorTable[6];
			break;
		case idB3:
			rate = dB3 / factorTable[7];
			break;
	    default:
			break;
	}
	
	dRho = rate * factorTable[0];
	dvPara = rate * factorTable[1];
	dvPerp = rate * factorTable[2];
	dv3 = rate * factorTable[3];
	dp = rate * factorTable[4];
	dBPara = rate * factorTable[5];
	dBPerp = rate * factorTable[6];
	dB3 = rate * factorTable[7];
	vp = vp_Gross;
	UpdateData(0);
}


void CMHDPolarityCheckerDlg::OnBnClickedRadioFast()
{
	// TODO: Add your control notification handler code here
	typ = fast; 
	updateAll(idType);
}


void CMHDPolarityCheckerDlg::OnBnClickedRadioSlow()
{
	// TODO: Add your control notification handler code here
	typ = slow; 
	updateAll(idType);
}


void CMHDPolarityCheckerDlg::OnBnClickedRadioAlfven()
{
	// TODO: Add your control notification handler code here
	typ = aw; 
	updateAll(idType);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditRho()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	updateAll(idRho);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditCs()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	updateAll(idCs);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditVa()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	updateAll(idVa);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditTheta()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	updateAll(idTheta);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditRhozero()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	updateAll(idRhoZero);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditVpara()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	updateAll(idVPara);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditVperp()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	updateAll(idVPerp);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditV3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	updateAll(idV3);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditP()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	updateAll(idP);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditBpara()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	updateAll(idBPara);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditBperp()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	updateAll(idBPerp);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditB3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	updateAll(idB3);
}
