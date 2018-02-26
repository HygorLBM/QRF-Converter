
// QRConverterFinalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QRConverterFinal.h"
#include "QRConverterFinalDlg.h"
#include "afxdialogex.h"

//General purposes
#include <string>
#include <Windows.h>
#include <iterator>
#include <fstream>
#include <wtypes.h>

//OpenCVIncludes
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\core.hpp>

//Zbar library and functions
#include <zbar.h>

//Encryption
#include <sstream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <functional>
#include <Windows.h>

//Systray
#include "TrayIcon.h"

//Namespasce
using namespace std;
using namespace cv;
using namespace zbar;




#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//defines to QRCode image 
#define OUT_FILE_PIXEL_PRESCALER	8											// Prescaler (number of pixels in bmp file for each QRCode pixel, on each dimension)
#define PIXEL_COLOR_R				0											// Color of bmp pixels
#define PIXEL_COLOR_G				0
#define PIXEL_COLOR_B				0

const int e_key = 47;
const int d_key = 15;
const int n = 391;

//To the systray
static const UINT WMU_NOTIFY_TASKBAR_ICON 
   = ::RegisterWindowMessage( _T("WMU_NOTIFY_TASKBAR_ICON") );

//The encryption
struct crypt : std::binary_function<int, int, int> {
    int operator()(int input, int key) const { 
        int result = 1;
        for (int i=0; i<key; i++) {
            result *= input;
            result %= n;
        }
        return result;
    }
};


CString decrypt(){
	std::ifstream temp("StrToRead.txt");
	//std::string encryptedstring((std::istreambuf_iterator<char>(temp)),
                 //std::istreambuf_iterator<char>());

    vector<int> encryptednumbers;
	int eachletter;
	string encryptedstring;
	stringstream plmdds;
	std::string::size_type sz;   // alias of size_t

	while(getline(temp, encryptedstring, ' ' ) )
  {
	eachletter =  stoi(encryptedstring,&sz); 
	encryptednumbers.push_back(eachletter);
  }

    std::transform(encryptednumbers.begin(), encryptednumbers.end(), 
        std::ostream_iterator<char>(plmdds, ""), 
        std::bind2nd(crypt(), d_key));
	
	encryptedstring = plmdds.str();
	CString strfromfilebox(encryptedstring.c_str());
	return strfromfilebox;
}

CString decryptimg(CString todecrypt){

	int eachletter;
	CT2CA pszConvertedAnsiString (todecrypt);
	string simg(pszConvertedAnsiString);
	string encryptedstring;
	stringstream plmdds;
	vector<int> encryptednumbers;
	std::string::size_type sz;   // alias of size_t
	
	for (int i=0; i<simg.size(); i++){
		if (!(isspace(simg.at(i)))){
		   encryptedstring += simg[i];
		}  
		else{
			eachletter= stoi(encryptedstring,&sz);
			encryptednumbers.push_back(eachletter);
			encryptedstring.clear();
		}
	}

	std::transform(encryptednumbers.begin(), encryptednumbers.end(), 
        std::ostream_iterator<char>(plmdds, ""), 
        std::bind2nd(crypt(), d_key));
	
	encryptedstring = plmdds.str();
	CString strfromfilebox(encryptedstring.c_str());
	return strfromfilebox;
}


//The function to take a screenshot from the program
void GetScreenShot()
{
    int x1, y1, x2, y2, w, h;

    // get screen dimensions
    x1  = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y1  = GetSystemMetrics(SM_YVIRTUALSCREEN);
    x2  = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    y2  = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    w   = x2 - x1;
    h   = y2 - y1;

    // copy screen to bitmap
    HDC     hScreen = GetDC(NULL);
    HDC     hDC     = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    BOOL    bRet    = BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);

    // save bitmap to clipboard
    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard();   

    // clean up
    SelectObject(hDC, old_obj);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);
}

//Function that returns a input taken (as a click) even out of the program
int keyPressed(int key){
    return (GetAsyncKeyState(key) & 0x8000 != 0);
}

// CQRConverterFinalDlg dialog
CQRConverterFinalDlg::CQRConverterFinalDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CQRConverterFinalDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQRConverterFinalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, EditBox_StringToConvert);
	DDX_Control(pDX, IDC_BUTTON1, BGenerateCode);
	DDX_Control(pDX, IDC_EDIT2, EditDecodeBOX);
	DDX_Control(pDX, IDC_EDIT3, FromScreenText);
	DDX_Control(pDX, IDC_EDIT5, Encrypted1);
	DDX_Control(pDX, IDC_EDIT4, Encrypted2);
	DDX_Control(pDX, IDC_EDIT6, SizeToScreenRead);
	DDX_Control(pDX, IDC_BUTTON3, MinModeButton);
	DDX_Control(pDX, IDC_BUTTON4, DCFromScreen);
}

BEGIN_MESSAGE_MAP(CQRConverterFinalDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT1, &CQRConverterFinalDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON1, &CQRConverterFinalDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CQRConverterFinalDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CQRConverterFinalDlg::OnBnClickedButton4)
    ON_COMMAND(ID_TEST_ONE, OnTestOne)
	ON_COMMAND(ID_TEST_TWO, OnTestTwo)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(WMU_NOTIFY_TASKBAR_ICON, OnNotifyTaskbarIcon)
	ON_BN_CLICKED(IDC_BUTTON3, &CQRConverterFinalDlg::OnBnClickedButton3)
	ON_STN_CLICKED(IDC_STATIC2, &CQRConverterFinalDlg::OnStnClickedStatic2)
END_MESSAGE_MAP()


// CQRConverterFinalDlg message handlers


//DialogBox initialization
BOOL CQRConverterFinalDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	 m_hAccel = ::LoadAccelerators( AfxGetInstanceHandle(), 
                                  MAKEINTRESOURCE(IDR_ACCELERATOR_MAIN) );
	 AddTaskbarIcon();
	   
	 CWnd *label = GetDlgItem(IDC_STATIC);
	   CWnd *label2 = GetDlgItem(IDC_STATIC2);
	   CWnd *label3 = GetDlgItem(IDC_BUTTON1);
	   CWnd *label4 = GetDlgItem(IDC_EDIT1);
	   CWnd *label5 = GetDlgItem(IDC_EDIT5);
	   CWnd *label6 = GetDlgItem(IDC_BUTTON4);
	   CWnd *label7 = GetDlgItem(IDC_EDIT3);
	   CWnd *label8 = GetDlgItem(IDC_BUTTON3);

	   label3->GetWindowRect(&size_button1);
	   label4->GetWindowRect(&size_edit1);
	   label5->GetWindowRect(&size_edit5);
	   label6->GetWindowRect(&size_button4);
	   label7->GetWindowRect(&size_edit3);
	   label8->GetWindowRect(&size_button3);
	   minimized = false;


	// TODO: Add extra initialization here
	EditBox_StringToConvert.SetWindowTextW(decrypt());
	SizeToScreenRead.SetWindowTextW(_T("150"));

	return TRUE;  // return TRUE  unless you set the focus to a control

}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CQRConverterFinalDlg::OnPaint()
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
HCURSOR CQRConverterFinalDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CQRConverterFinalDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.



	// TODO:  Add your control notification handler code here
}

//This is the button that generates a QRCode from the text.
void CQRConverterFinalDlg::OnBnClickedButton1()
{
	//Extra variables needed
	HANDLE hFile;
	LPCWSTR filecreated = _T("StrToRead.txt");
	CString EditBox1Text;
	DWORD nbytesWt = 0;
	unsigned int	unWidth, x, y, l, n, unWidthAdjusted, unDataBytes;
	unsigned char*	pRGBData, *pSourceData, *pDestData;
	HBITMAP QRImage;
	QRcode*			pQRC;
	FILE*			f;
	vector<int>		encrypted;

	// TODO: Add your control notification handler code here
	
	//First thing you must save what is written in the EditBox1 to the .txtfile
	//Opening the file.
	hFile = CreateFile(filecreated,                // name of the write
                       GENERIC_WRITE,          // open for writing
                       0,                      // do not share
                       NULL,                   // default security
                       TRUNCATE_EXISTING,             // create new file only
                       FILE_ATTRIBUTE_NORMAL,  // normal file
                       NULL);                  // no attr. template

	//Taking the text on the editbox 1 and converting to a format wich can be used with WriteFile().
	EditBox_StringToConvert.GetWindowTextW(EditBox1Text);
	const size_t textoFiletam = (EditBox1Text.GetLength() + 1)*2;
	char *textoFile = new char[textoFiletam];
	size_t convertedCharsw = 0;
	wcstombs_s(&convertedCharsw, textoFile, textoFiletam, EditBox1Text, _TRUNCATE );
	if(EditBox1Text.IsEmpty()){
		AfxMessageBox(_T("Can't generate a QRCode for a empty field. Please write something to code into a QR!"));
	}
	else{
		//Warning the user that the QRcode was possible to create and writing the EditBox1 content to the file.
		AfxMessageBox(_T("The QRCode was created, you can check the QRCode clicking in the <>Decode QRCode<> Button"));
		string txttofile(textoFile);
		
		

		//Does the encryption
		//The encryption
		std::transform(txttofile.begin(), txttofile.end(),  
        std::back_inserter(encrypted),
        std::bind2nd(crypt(), e_key));

		string stringencrypted;
		char fromencrypted[5];
		
		for (int i = 0; i < encrypted.size(); i++) {
			_itoa(encrypted[i], fromencrypted, 10);
			stringencrypted += fromencrypted;
			stringencrypted += " ";
		}
		char* textefile = new char[stringencrypted.length()+1];
		strcpy(textefile, stringencrypted.c_str());
		WriteFile( hFile, textefile, strlen(textefile), &nbytesWt, NULL );
		//Show the encrypted version:
		CString toebox3(stringencrypted.c_str());
		Encrypted1.SetWindowTextW(toebox3);



		//The decode itself
		// Compute QRCode
		if (pQRC = QRcode_encodeString(textefile, 0, QR_ECLEVEL_H, QR_MODE_8, 1))
		{
		unWidth = pQRC->width;
		unWidthAdjusted = unWidth * 8 * 3;
		if (unWidthAdjusted % 4)
			unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
		unDataBytes = unWidthAdjusted * unWidth * 8;

			// Allocate pixels buffer
		if (!(pRGBData = (unsigned char*)malloc(unDataBytes)))
			{
			printf("Out of memory");
			exit(-1);
			}	
			// Preset to white
		memset(pRGBData, 0xff, unDataBytes);
			// Prepare bmp headers
		BITMAPFILEHEADER kFileHeader;
		kFileHeader.bfType = 0x4d42;  // "BM"
		kFileHeader.bfSize =	sizeof(BITMAPFILEHEADER) +
								sizeof(BITMAPINFOHEADER) +
								unDataBytes;
		kFileHeader.bfReserved1 = 0;
		kFileHeader.bfReserved2 = 0;
		kFileHeader.bfOffBits =	sizeof(BITMAPFILEHEADER) +
								sizeof(BITMAPINFOHEADER);

		BITMAPINFOHEADER kInfoHeader;
		kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		kInfoHeader.biWidth = unWidth * 8;
		kInfoHeader.biHeight = -((int)unWidth * 8);
		kInfoHeader.biPlanes = 1;
		kInfoHeader.biBitCount = 24;
		kInfoHeader.biCompression = BI_RGB;
		kInfoHeader.biSizeImage = 0;
		kInfoHeader.biXPelsPerMeter = 0;
		kInfoHeader.biYPelsPerMeter = 0;
		kInfoHeader.biClrUsed = 0;
		kInfoHeader.biClrImportant = 0;

			// Convert QrCode bits to bmp pixels
		pSourceData = pQRC->data;
		for(y = 0; y < unWidth; y++)
			{
			pDestData = pRGBData + unWidthAdjusted * y * OUT_FILE_PIXEL_PRESCALER;
			for(x = 0; x < unWidth; x++)
				{
				if (*pSourceData & 1)
					{
					for(l = 0; l < OUT_FILE_PIXEL_PRESCALER; l++)
						{
						for(n = 0; n < OUT_FILE_PIXEL_PRESCALER; n++)
							{
							*(pDestData +		n * 3 + unWidthAdjusted * l) =	PIXEL_COLOR_B;
							*(pDestData + 1 +	n * 3 + unWidthAdjusted * l) =	PIXEL_COLOR_G;
							*(pDestData + 2 +	n * 3 + unWidthAdjusted * l) =	PIXEL_COLOR_R;
							}
						}
					}
				pDestData += 3 * OUT_FILE_PIXEL_PRESCALER;
				pSourceData++;
				}
			}

			// Output the bmp file
		if (!(fopen_s(&f, "QRCode.bmp", "wb")))
			{
			fwrite(&kFileHeader, sizeof(BITMAPFILEHEADER), 1, f);
			fwrite(&kInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);
			fwrite(pRGBData, sizeof(unsigned char), unDataBytes, f);
			
			fclose(f);
			}
		// Free data
		free(pRGBData);
		delete [] textefile;
		QRcode_free(pQRC);
		}
	
		else
			{
			AfxMessageBox(_T("Unable to open file"));
			exit(-1);
			}
	}
	

	}

	void CQRConverterFinalDlg::OnBnClickedButton2()
	{
		//The needed variables
		DWORD nBytesWt2 = 0;
		LPCWSTR filecreated = _T("StrToRead.txt");
		HANDLE hFile2;

		//Scanner functions
		ImageScanner scanner;  
		scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);  
	
		//OpenCV classes
		Mat img = imread("QRCode.bmp",0);  
		CString TheCode;
		
		//Variaveis da imagem
		int width = img.cols;  
		int height = img.rows;  
   
		uchar *raw = (uchar *)img.data;  
		// wrap image data  
		zbar::Image QRIMG(width, height, "Y800", raw, width * height);  
		// scan the image for barcodes  
		int n = scanner.scan(QRIMG);  

		//Show the image content in the EditBox2
		for(zbar::Image::SymbolIterator symbol = QRIMG.symbol_begin(); symbol != QRIMG.symbol_end();  ++symbol)
		{
			CString EditBox2Text(symbol->get_data().c_str());
			Encrypted2.SetWindowTextW(EditBox2Text);
			EditDecodeBOX.SetWindowTextW(decryptimg(EditBox2Text));
		}

		Mat imgout = imread("QRCode.bmp",0);
		imshow("QRCode.bmp",imgout); 
		waitKey(100);

		// TODO: Add your control notification handler code here
	}

//This function Decodes a QRCode to a EditBox
CString Decoding(char * adress, CEdit &thebox)
{
	CString retornando;
	ImageScanner scanner2;  
	DWORD nBytesWt2 = 0;
	scanner2.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);  
	Mat img = imread(adress,0);  
    Mat imgout;
	CString Codigo;
	cvtColor(img,imgout,CV_GRAY2RGB);  
      int width = img.cols;  
      int height = img.rows;  
   uchar *raw = (uchar *)img.data;  
   // wrap image data  
   zbar::Image imagem2(width, height, "Y800", raw, width * height);  
   // scan the image for barcodes  
   int n = scanner2.scan(imagem2);  

  
   for(zbar::Image::SymbolIterator symbol = imagem2.symbol_begin(); symbol != imagem2.symbol_end();  ++symbol)
   {
	   CString nunca_te_pedi_nada(symbol->get_data().c_str());
	   thebox.SetWindowTextW(nunca_te_pedi_nada);
   }

   thebox.GetWindowTextW(retornando);
   return retornando;
}

	void CQRConverterFinalDlg::OnBnClickedButton4()
	{

		// The needed variables
		int clicker = 0, area_to_scan;
		HBITMAP ScrShot;
		POINT p;
		CImage sera;
		Mat imout;
		CString area;
		SizeToScreenRead.GetWindowTextW(area);
		CT2CA pszConvertedAnsiString(area);
		string area_string(pszConvertedAnsiString);
		std::string::size_type sz;   // alias of size_t
		wchar_t to_change[256];
		DCFromScreen.SetWindowTextW(_T("Capturing from screen"));



		area_to_scan = stoi(area_string,&sz);

		//Take a screenshot
		GetScreenShot();

		//Take the position where you clicked with the mouse.
		while (clicker != 2){
			if(keyPressed(VK_LBUTTON)){
				clicker++;}
			if(clicker == 2){ 
			  if (GetCursorPos(&p))
				{
				//cursor position now in p.x and p.y
				}
			}
		}
		//Open clipboard, where the crtl+c content goes and check the print screen.
		if (OpenClipboard()){
			ScrShot = (HBITMAP) GetClipboardData(CF_BITMAP);
			if( ScrShot == NULL )
				{
					AfxMessageBox(_T("Unable to see anything from the screen :( "));    
					CloseClipboard();
				}
			else
				//There's a screenshot
			{
				HBITMAP handle = (HBITMAP)GetClipboardData(CF_BITMAP); //getting from clipboard
				CBitmap * bm = CBitmap::FromHandle(handle);
				sera.Attach(handle);
				sera.Save(_T("print_cropped.bmp"), Gdiplus::ImageFormatBMP);}} 
				Mat nao_cortada =imread("print_cropped.bmp",1);

				//There's no negative p.x and p.y, so this is needed.
				if(p.x < area_to_scan){
					p.x = area_to_scan;
				}
			 
				if(p.y <area_to_scan){
					p.y = area_to_scan;
			  }
			 
			  if (p.x + area_to_scan > GetSystemMetrics(0)){
				  p.x = GetSystemMetrics(0) - area_to_scan;
			  }

			    if (p.y + area_to_scan > GetSystemMetrics(1)){
				  p.y = GetSystemMetrics(1) - area_to_scan;

			  }
				  if ((area_to_scan * 2 > GetSystemMetrics(0)) || (area_to_scan *2 > GetSystemMetrics(1))){
							AfxMessageBox(_T("This area you're trying to analyze is bigger than the screen itself! \nAdjusting..."));
							area_to_scan = GetSystemMetrics(1)/2 - 1;
							if(p.x < area_to_scan){
								p.x = area_to_scan;
							}
			 
							if(p.y <area_to_scan){
								p.y = area_to_scan;
							}
							wsprintfW(to_change, L"%d", area_to_scan);
							SizeToScreenRead.SetWindowTextW(to_change);
				  }

			  //Created a cropped image with the mouse position on the center
			  Mat ROI(nao_cortada, Rect(p.x-area_to_scan ,p.y-area_to_scan,area_to_scan*2,area_to_scan*2));
			  Mat cortada;
				// Copy the data into new matrix
			  ROI.copyTo(cortada);
			  imwrite("print_cropped.bmp",cortada);
			  //Writes data in the third EditBox.
			  DCFromScreen.SetWindowTextW(_T("Decode From Screen"));
			  FromScreenText.SetWindowTextW(Decoding("print_cropped.bmp",FromScreenText));	
			  CloseClipboard();

		// TODO: Add your control notification handler code here
	}


void CQRConverterFinalDlg::OnTestOne(){
  AfxMessageBox( _T("One") );
}
void CQRConverterFinalDlg::OnTestTwo(){
AfxMessageBox( _T("Two") );
}

void CQRConverterFinalDlg::OnDestroy(){
if( NULL != m_hAccel )
   {
      ::DestroyAcceleratorTable( m_hAccel );
   }
   // remove the taskbar icon
   DWORD dwMessage = NIM_DELETE;
   NOTIFYICONDATA   nid;

   nid.cbSize = sizeof(NOTIFYICONDATA);
   nid.uID = 0;
   nid.hWnd = GetSafeHwnd();

   ::Shell_NotifyIcon( dwMessage,  // message identifier 
                       &nid);      // pointer to structure

	CDialog::OnDestroy();	
}

void CQRConverterFinalDlg::OnSize(UINT nType, int cx, int cy){
	CDialog::OnSize(nType, cx, cy);
	UINT fe;
	 CRect rect1, rect2, rect3, rect4, rectS;
	   CWnd *label = GetDlgItem(IDC_STATIC);
	   CWnd *label2 = GetDlgItem(IDC_STATIC2);
	   CWnd *label3 = GetDlgItem(IDC_BUTTON1);
	   CWnd *label4 = GetDlgItem(IDC_EDIT1);
	   CWnd *label5 = GetDlgItem(IDC_EDIT5);
	   CWnd *label6 = GetDlgItem(IDC_BUTTON4);
	   CWnd *label7 = GetDlgItem(IDC_EDIT3);

   if( nType  == SIZE_MINIMIZED )
   {
	 
	   ShowWindow(SW_HIDE);
	   
   }

}

LRESULT CQRConverterFinalDlg::OnNotifyTaskbarIcon(WPARAM wParam, LPARAM lParam){
UINT uID = (UINT)wParam;       // this is the ID you assigned to your taskbar icon
	UINT uMouseMsg = (UINT)lParam; // mouse messages

	switch (uMouseMsg)
   {
	case WM_LBUTTONDOWN:
      break;
   case WM_LBUTTONDBLCLK:
	   if( IsIconic() )
      {
         ShowWindow( SW_RESTORE );
      }
      ShowWindow( SW_SHOW );
      SetForegroundWindow();
      break;
	case WM_RBUTTONDOWN:
      {
         CMenu* pMenu = GetMenu();
        if(pMenu){
            CMenu *pSubMenu = NULL;
            pSubMenu = pMenu->GetSubMenu( 0 );
            {
              SetForegroundWindow(); // *** little patch here ***				
			  POINT pointCursor;			   
			  ::GetCursorPos( &pointCursor );			   
			  pSubMenu->TrackPopupMenu( TPM_RIGHTALIGN, 
                                         pointCursor.x, pointCursor.y, 
                                         this );
			}
         }
      }
      break;
	case WM_RBUTTONDBLCLK:
      break;
	case WM_MOUSEMOVE:
      break;
   }
   return 0L;
}

 void CQRConverterFinalDlg::AddTaskbarIcon()
{
   DWORD dwMessage = NIM_ADD;
   NOTIFYICONDATA   nid;

   nid.cbSize = sizeof(NOTIFYICONDATA); 
   nid.hWnd = GetSafeHwnd(); 
   nid.uID = 0; 
   nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
   nid.uCallbackMessage = WMU_NOTIFY_TASKBAR_ICON; 
   nid.hIcon = GetIcon( TRUE );
   lstrcpyn( nid.szTip, AfxGetAppName(), sizeof( nid.szTip ) );

   ::Shell_NotifyIcon( dwMessage,  // message identifier 
                       &nid);      // pointer to structure
}
	


 void CQRConverterFinalDlg::OnBnClickedButton3()
 {
	CRect rect1, rect2, rect3, rect4,rect5, rectS;
	   CWnd *label = GetDlgItem(IDC_STATIC);
	   CWnd *label2 = GetDlgItem(IDC_STATIC2);
	   CWnd *label3 = GetDlgItem(IDC_BUTTON1);
	   CWnd *label4 = GetDlgItem(IDC_EDIT1);
	   CWnd *label5 = GetDlgItem(IDC_EDIT5);
	   CWnd *label6 = GetDlgItem(IDC_BUTTON4);
	   CWnd *label7 = GetDlgItem(IDC_EDIT3);
	   CWnd *label8 = GetDlgItem(IDC_BUTTON3);   


	 if (!minimized){
	   ShowWindow(SW_SHOWNORMAL);
	   //Procedure to create the floating functional window.
	   label->ShowWindow(SW_HIDE);
	   label2->ShowWindow(SW_HIDE);
	   label5->ShowWindow(SW_HIDE);
	

	   label3->GetWindowRect(&rect1);
	   label4->GetWindowRect(&rect2);
	   label5->GetWindowRect(&rect3);
	   label6->GetWindowRect(&rect4);
	   label7->GetWindowRect(&rect5);
	   
	   //Adjusting to the minimized screen
	   label3->SetWindowPos(0,0,0,0,0,SWP_NOSIZE);
	   label4->SetWindowPos(0,0,rect1.Height(),0,0,SWP_NOSIZE);
	   label5->SetWindowPos(0,0,rect1.Height()+rect2.Height(),0,0,SWP_NOSIZE);
	   label6->SetWindowPos(0,0,rect1.Height()+rect2.Height()+rect3.Height(),0,0,SWP_NOSIZE);
	   label7->SetWindowPos(0,0,rect1.Height()+rect2.Height()+rect3.Height()+rect4.Height(),0,0,SWP_NOSIZE);
	   label8->SetWindowPos(0,0,rect1.Height()+rect2.Height()+rect3.Height()+rect4.Height()+rect5.Height(),0,0,SWP_NOSIZE);
	   MinModeButton.SetWindowTextW(_T("Return to GUI"));
	   label8->ShowWindow(SW_SHOW);

	   this->GetWindowRect(&size_screen);
	   this->SetWindowPos(&wndTopMost,10,GetSystemMetrics(1)-250,230,216,SWP_SHOWWINDOW);
	   minimized = true;
	 }

	 else if(minimized){
		
	   
	   label3->SetWindowPos(0,size_button1.left,size_button1.top - 28,size_button1.Width(),size_button1.Height(),SWP_SHOWWINDOW);
	   label4->SetWindowPos(0,size_edit1.left,size_edit1.top-28,size_edit1.Width(),size_edit1.Height(),SWP_SHOWWINDOW);
	   label5->SetWindowPos(0,size_edit5.left,size_edit5.top-28,size_edit5.Width(),size_edit5.Height(),SWP_SHOWWINDOW);
	   label6->SetWindowPos(0,size_button4.left,size_button4.top-28,size_button4.Width(),size_button4.Height(),SWP_SHOWWINDOW);
	   label7->SetWindowPos(0,size_edit3.left,size_edit3.top-28,size_edit3.Width(),size_edit3.Height(),SWP_SHOWWINDOW);
	   label8->SetWindowPos(0,size_button3.left,size_button3.top-30,size_button3.Width(),size_button3.Height(),SWP_SHOWWINDOW);
	   this->SetWindowPos(&wndNoTopMost,size_screen.left,size_screen.top,size_screen.Width(),size_screen.Height(),SWP_SHOWWINDOW);
	   MinModeButton.SetWindowTextW(_T("Minimized mode"));
	   label2->ShowWindow(SW_HIDE);
	   label2->ShowWindow(SW_SHOW);
	   label->ShowWindow(SW_SHOW);
	   label3->ShowWindow(SW_SHOW);
	   label4->ShowWindow(SW_SHOW);
	   label5->ShowWindow(SW_SHOW);
	   this->ShowWindow(SW_SHOW);
	   minimized = false;
	 
	 }

	 // TODO: Add your control notification handler code here
 }


 void CQRConverterFinalDlg::OnStnClickedStatic2()
 {
	 // TODO: Add your control notification handler code here
 }

 void CQRConverterFinalDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if(nID == SC_CLOSE)
    {
		
	}
}

