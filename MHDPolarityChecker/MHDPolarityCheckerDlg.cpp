
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
waveType typ = aw; 

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
	delta = sqrt((cs*cs + va*va)*(cs*cs + va*va) - 4 * cs * cs * va * va * cos(th) * cos(th));

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
	DDX_Control(pDX, IDC_PIC_POLARITY, zoneToPlot);
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
	((CButton*) GetDlgItem(IDC_RADIO_ALFVEN)) -> SetCheck(TRUE); 

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
		CWnd * arena; 
		CPen penBlack(PS_SOLID, 2, RGB(0, 0, 0)); 
		CPen penBlue(PS_SOLID, 2, RGB(0, 0, 255)); 
		CPen penRed(PS_SOLID, 2, RGB(255, 0, 0));
		CPen penOrange(PS_SOLID, 2, RGB(127, 64, 127));
		CPen penGreen(PS_SOLID, 2, RGB(0, 127, 0)); 
		CPen penCyan(PS_SOLID, 2, RGB(0, 127, 127));
		CPen penWhite(PS_SOLID, 2, RGB(255, 255, 255));
		
		arena = GetDlgItem(IDC_PIC_POLARITY);
		CPaintDC * arenaDC; 
		CPaintDC etwasDC(arena);
		arenaDC = &etwasDC; 
		int xLeft, yLeft, xRight, yRight, width, height; 
		int radius, centerX, centerY; 

		RECT theRect, theCirc;

		arena->GetClientRect(&theRect); 
		// Blyad window$! 
		yRight = theRect.bottom; 
		yLeft = theRect.top; 
		xLeft = theRect.left;
		xRight = theRect.right; 
		width = xRight - xLeft; 
		height = yRight - yLeft;
		centerX = ((xRight + xLeft) >> 1); 
		centerY = ((yRight + yLeft) >> 1);
		radius = min(width, height) * 19 / 40; 
		theCirc.left = centerX - radius; 
		theCirc.right = centerX + radius; 
		theCirc.bottom = centerY + radius; 
		theCirc.top = centerY - radius; 

		arenaDC->SelectObject(&penWhite); 
		arenaDC->Rectangle(&theRect); 
		arenaDC->SelectObject(&penBlack);
		arenaDC->Ellipse(&theCirc); 
		drawArrow(centerX, centerY, radius, 0, (void*)&penBlack, (void*)arenaDC);
		drawArrow(centerX, centerY, radius, theta, (void*)&penGreen, (void*)arenaDC);
		float dvfpa, dvfpe, dBfpa, dBfpe, maxD; 
		dvfpa = dvPara; 
		dvfpe = dvPerp; 
		dBfpa = dBPara; 
		dBfpe = dBPerp; 
		int thetaVF, thetaBF;
		float rad; 
		int rVf, rBf;
		int magV, magB; 
		rad = 180 / (355.0 / 113);
		thetaVF = atan2(dvfpe, dvfpa) * rad; 
		thetaBF = atan2(dBfpe, dBfpa) * rad; 
		magV = sqrt(dvfpe * dvfpe + dvfpa * dvfpa);
		magB = sqrt(dBfpe * dBfpe + dBfpa * dBfpa);
		maxD = max(magV, magB); 
		magV /= maxD; 
		magB /= maxD; 
		rVf = magV * radius; 
		rBf = magB * radius; 
		if (typ == fast)
		{
			drawArrow(centerX, centerY, rVf, thetaVF, (void*)&penBlue, (void*)arenaDC);
			drawArrow(centerX, centerY, rBf, thetaBF, (void*)&penCyan, (void*)arenaDC);
		}
		else if (typ == slow)
		{
			drawArrow(centerX, centerY, rVf, thetaVF, (void*)&penRed, (void*)arenaDC);
			drawArrow(centerX, centerY, rBf, thetaBF, (void*)&penOrange, (void*)arenaDC);
		}
		arenaDC->CloseFigure(); 
		ReleaseDC(arenaDC); 
		CDialogEx::OnPaint();
	}
}

void CMHDPolarityCheckerDlg::drawArrow(int centerX, int centerY, int len, int rechtung, void* penIn, void* woIn)
{
	int pinX, pinY; 
	int leftTailX, leftTailY, rightTailX, rightTailY, finX, finY; 
	float thetaTailLeft, thetaTailRight, thetaLine;
	int lenTail; 
	float deg = 355.0 / 113 / 180; 
	int arrowSize; 
	CPen * pen; 
	CPaintDC * wo; 

	pen = (CPen *) penIn; 
	wo = (CPaintDC *) woIn; 

	lenTail = len * 1 / 4; 
	arrowSize = 18; 
	wo->SelectObject(pen);
	thetaLine = (rechtung + 90) * deg;
	thetaTailLeft = (rechtung - 90 - arrowSize) * deg; 
	thetaTailRight = (rechtung - 90 + arrowSize) * deg; 

	wo->MoveTo(centerX, centerY); 
	finX = centerX + cos(thetaLine) * len; 
	finY = centerY - sin(thetaLine) * len; // Window$ Ar$chloch! 
	wo->LineTo(finX, finY); 
	wo->MoveTo(finX, finY); 
	leftTailX = finX + cos(thetaTailLeft) * lenTail; 
	leftTailY = finY - sin(thetaTailLeft) * lenTail;
	wo->LineTo(leftTailX, leftTailY); 
	wo->MoveTo(finX, finY); 
	rightTailX = finX + cos(thetaTailRight) * lenTail; 
	rightTailY = finY - sin(thetaTailRight) * lenTail;
	wo->LineTo(rightTailX, rightTailY);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMHDPolarityCheckerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMHDPolarityCheckerDlg::plotGraph()
{
	Invalidate(TRUE);
	OnPaint();
	Invalidate(FALSE);
	UpdateWindow();
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
	// OnPaint();
	plotGraph(); 
	UpdateData(0);
}


void CMHDPolarityCheckerDlg::OnBnClickedRadioFast()
{
	typ = fast; 
	updateAll(idType);
}


void CMHDPolarityCheckerDlg::OnBnClickedRadioSlow()
{
	typ = slow; 
	updateAll(idType);
}


void CMHDPolarityCheckerDlg::OnBnClickedRadioAlfven()
{
	typ = aw; 
	updateAll(idType);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditRho()
{
	updateAll(idRho);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditCs()
{
	updateAll(idCs);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditVa()
{
	updateAll(idVa);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditTheta()
{
	updateAll(idTheta);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditRhozero()
{
	updateAll(idRhoZero);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditVpara()
{
	updateAll(idVPara);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditVperp()
{
	updateAll(idVPerp);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditV3()
{
	updateAll(idV3);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditP()
{
	updateAll(idP);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditBpara()
{
	updateAll(idBPara);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditBperp()
{
	updateAll(idBPerp);
}


void CMHDPolarityCheckerDlg::OnEnChangeEditB3()
{
	updateAll(idB3);
}
