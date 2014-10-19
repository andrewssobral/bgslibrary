
// Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "App.h"
#include "Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CDlg dialog




CDlg::CDlg(CWnd* pParent /*=NULL*/)
  : CDialogEx(CDlg::IDD, pParent)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_COMBO_BGSLIST, m_bgslist);
  DDX_Control(pDX, IDC_LOG, m_log);
  DDX_Control(pDX, IDC_INPUT_VIDEO, m_inputVideo);
  DDX_Control(pDX, IDC_USE_WEBCAM, m_useWebCam);
  DDX_Control(pDX, IDC_SPIN_WEBCAM_INDEX, m_spinWebCamIndex);
  DDX_Control(pDX, IDC_EDIT_WEBCAM_INDEX, m_webCamIndex);
  DDX_Control(pDX, IDC_FRAME_NUMBER, m_frameNumber);
  DDX_Control(pDX, IDC_SAVE_FRAME, m_saveFrame);
  DDX_Control(pDX, IDC_SAVE_MASK, m_saveMask);
  DDX_Control(pDX, IDC_SAVE_BKG, m_saveBkg);
  DDX_Control(pDX, IDC_IMG_SEQ, m_isImgSeq);
  DDX_Control(pDX, IDC_EDIT_START_IDX, m_startIdx);
  DDX_Control(pDX, IDC_EDIT_STOP_IDX, m_stopIdx);
  DDX_Control(pDX, IDC_COMBO_FILE_TYPE, m_fileTypeList);
  DDX_Control(pDX, IDC_SPIN_START_IDX, m_spinStartIdx);
  DDX_Control(pDX, IDC_SPIN_STOP_IDX, m_spinStopIdx);
  DDX_Control(pDX, IDC_EDIT_DELAY, m_delay);
  DDX_Control(pDX, IDC_EXEC_TIME, m_execTime);
  DDX_Control(pDX, IDC_MEDIAN_FILTER, m_medianFilter);
}

BEGIN_MESSAGE_MAP(CDlg, CDialogEx)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDSTART, &CDlg::OnBnClickedStart)
  ON_BN_CLICKED(IDSTOP, &CDlg::OnBnClickedStop)
  ON_BN_CLICKED(IDC_USE_WEBCAM, &CDlg::OnBnClickedUseWebcam)
  ON_BN_CLICKED(IDC_IMG_SEQ, &CDlg::OnBnClickedImgSeq)
  ON_BN_CLICKED(IDSAVE, &CDlg::OnBnClickedSave)
END_MESSAGE_MAP()


// CDlg message handlers

BOOL CDlg::OnInitDialog()
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
  methodName = L"";
  useWebCam = false;
  useImgSeq = false;
  filePath = L"";
  bgs = NULL;
  webCamIndex = 0;
  m_spinWebCamIndex.SetRange(0, 9); 
  m_spinStartIdx.SetRange(0, 999999);
  m_spinStopIdx.SetRange(0, 999999);
  m_frameNumber.SetWindowTextW(L"0");
  
  addBgsList();
  m_fileTypeList.AddString(L"PNG");
  m_fileTypeList.AddString(L"JPG");
  m_fileTypeList.AddString(L"JPEG");
  m_fileTypeList.AddString(L"JPE");
  m_fileTypeList.AddString(L"JP2");
  m_fileTypeList.AddString(L"BMP");
  m_fileTypeList.AddString(L"DIB");
  m_fileTypeList.AddString(L"PBM");
  m_fileTypeList.AddString(L"PGM");
  m_fileTypeList.AddString(L"PPM");
  m_fileTypeList.AddString(L"SR");
  m_fileTypeList.AddString(L"RAS");
  m_fileTypeList.AddString(L"TIFF");
  m_fileTypeList.AddString(L"TIF");
  m_fileTypeList.SelectString(0,L"PNG");
  m_inputVideo.SetWindowTextW(L"../dataset/video.avi");
  m_delay.SetWindowTextW(L"1");
  m_execTime.SetWindowTextW(L"0");
  
  started = false;
  if(started == false)
  {
    cv::namedWindow("INPUT", 1);
    HWND hWnd = (HWND) cvGetWindowHandle("INPUT");
    HWND hParent = ::GetParent(hWnd);
    ::SetParent(hWnd, GetDlgItem(IDC_FRAME_INPUT)->m_hWnd);
    ::ShowWindow(hParent, SW_HIDE);
  }

  if(started == false)
  {
    cv::namedWindow("MASK", 1);
    HWND hWnd = (HWND) cvGetWindowHandle("MASK");
    HWND hParent = ::GetParent(hWnd);
    ::SetParent(hWnd, GetDlgItem(IDC_FRAME_MASK)->m_hWnd);
    ::ShowWindow(hParent, SW_HIDE);
  }

  if(started == false)
  {
    cv::namedWindow("BKG", 1);
    HWND hWnd = (HWND) cvGetWindowHandle("BKG");
    HWND hParent = ::GetParent(hWnd);
    ::SetParent(hWnd, GetDlgItem(IDC_FRAME_BKG)->m_hWnd);
    ::ShowWindow(hParent, SW_HIDE);
  }

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDlg::OnPaint()
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
HCURSOR CDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

void CDlg::addBgsList()
{
  // 36 algorithms (37 with PBAS)
  m_bgslist.AddString(L"FrameDifferenceBGS");
  m_bgslist.AddString(L"StaticFrameDifferenceBGS");
  m_bgslist.AddString(L"WeightedMovingMeanBGS");
  m_bgslist.AddString(L"WeightedMovingVarianceBGS");
  m_bgslist.AddString(L"MixtureOfGaussianV1BGS");
  m_bgslist.AddString(L"MixtureOfGaussianV2BGS");
  m_bgslist.AddString(L"AdaptiveBackgroundLearning");
  m_bgslist.AddString(L"AdaptiveSelectiveBackgroundLearning");
  m_bgslist.AddString(L"GMG");
  m_bgslist.AddString(L"DPAdaptiveMedianBGS");
  m_bgslist.AddString(L"DPGrimsonGMMBGS");
  m_bgslist.AddString(L"DPZivkovicAGMMBGS");
  m_bgslist.AddString(L"DPMeanBGS");
  m_bgslist.AddString(L"DPWrenGABGS");
  m_bgslist.AddString(L"DPPratiMediodBGS");
  m_bgslist.AddString(L"DPEigenbackgroundBGS");
  m_bgslist.AddString(L"DPTextureBGS");
  m_bgslist.AddString(L"T2FGMM_UM");
  m_bgslist.AddString(L"T2FGMM_UV");
  m_bgslist.AddString(L"T2FMRF_UM");
  m_bgslist.AddString(L"T2FMRF_UV");
  m_bgslist.AddString(L"FuzzySugenoIntegral");
  m_bgslist.AddString(L"FuzzyChoquetIntegral");
  m_bgslist.AddString(L"LBSimpleGaussian");
  m_bgslist.AddString(L"LBFuzzyGaussian");
  m_bgslist.AddString(L"LBMixtureOfGaussians");
  m_bgslist.AddString(L"LBAdaptiveSOM");
  m_bgslist.AddString(L"LBFuzzyAdaptiveSOM");
  m_bgslist.AddString(L"MultiLayerBGS");
  //m_bgslist.AddString(L"PBAS");
  m_bgslist.AddString(L"VuMeter");
  m_bgslist.AddString(L"KDE");
  m_bgslist.AddString(L"IndependentMultimodalBGS");
  m_bgslist.AddString(L"SJN_MultiCueBGS");
  m_bgslist.AddString(L"SigmaDeltaBGS");
  m_bgslist.AddString(L"SuBSENSEBGS");
  m_bgslist.AddString(L"LOBSTERBGS");
}

bool CDlg::getBgsMethodName()
{
  int nIndex = m_bgslist.GetCurSel();
  if(nIndex != CB_ERR)
  {
    m_bgslist.GetLBText(nIndex, methodName);
    return true;
  }
  else
  {
    AfxMessageBox(L"Please, select one background subtraction method!");
    return false;
  }
}

bool CDlg::getFileType()
{
  int nIndex = m_fileTypeList.GetCurSel();
  if(nIndex != CB_ERR)
  {
    m_fileTypeList.GetLBText(nIndex, fileType);
    return true;
  }
  else
  {
    AfxMessageBox(L"Please, select one file type or extension!");
    return false;
  }
}

bool CDlg::getInputVideoFilePath()
{
  m_inputVideo.GetWindowTextW(filePath);

  if(filePath.GetLength() > 0)
    return true;
  else
  {
    AfxMessageBox(L"Please, select one video file or image sequence folder!");
    return false;
  }
}

void CDlg::OnBnClickedStart()
{
  if(started == false)
  {
    m_log.SetWindowTextW(L"Checking...");

    if(!getBgsMethodName())
    {
      m_log.SetWindowTextW(L"Stopped...");
      return;
    }
    
    useWebCam = false;
    if(m_useWebCam.GetCheck() == BST_CHECKED)
      useWebCam = true;
    
    useImgSeq = false;
    if(m_isImgSeq.GetCheck() == BST_CHECKED)
    {
      useImgSeq = true;

      if(!getFileType())
      {
        m_log.SetWindowTextW(L"Stopped...");
        return;
      }
    }

    if(useWebCam == false)
      if(!getInputVideoFilePath())
      {
        m_log.SetWindowTextW(L"Stopped...");
        return;
      }
  
    m_log.SetWindowTextW(L"Starting...");
    thread = AfxBeginThread((AFX_THREADPROC) CDlg::Thread, (LPVOID) this);
  }
  else
  {
    AfxMessageBox(L"Thread is already initialized!");
    return;
  }
}

DWORD CDlg::Thread(LPVOID *lpvParam)
{
  CDlg *thr = (CDlg*) lpvParam;
  thr->ThreadProcess();
  return NULL;
}

void CDlg::ThreadProcess()
{
  CString csStartIdx;
  m_startIdx.GetWindowTextW(csStartIdx);
  startIdx = _tstoi(csStartIdx);

  CString csStopIdx;
  m_stopIdx.GetWindowTextW(csStopIdx);
  stopIdx = _tstoi(csStopIdx);

  if(useImgSeq == false)
  {
    if(useWebCam)
    {
      CString strIndex;
      m_webCamIndex.GetWindowTextW(strIndex);
      webCamIndex = _tstoi(strIndex);
      capture = cvCaptureFromCAM(webCamIndex);
    }
    else
    {
      CStringA file_path_aux(filePath);
      capture = cvCaptureFromFile((const char *) file_path_aux);
    }
  
    if(!capture)
    {
      AfxMessageBox(L"ERROR: Cannot initialize video!");
      m_log.SetWindowTextW(L"Stopped...");
      return;
    }
  }
  else
  {
    if(stopIdx == 0)
    {
      AfxMessageBox(L"Stop index not defined!");
      return;
    }

    if(startIdx > stopIdx)
    {
      AfxMessageBox(L"Start index is higher than stop index!");
      return;
    }
  }

  /* Background Subtraction Methods */
  
  if(methodName == "FrameDifferenceBGS")
    bgs = new FrameDifferenceBGS;
  if(methodName == "StaticFrameDifferenceBGS")
    bgs = new StaticFrameDifferenceBGS;
  if(methodName == "WeightedMovingMeanBGS")
    bgs = new WeightedMovingMeanBGS;
  if(methodName == "WeightedMovingVarianceBGS")
    bgs = new WeightedMovingVarianceBGS;
  if(methodName == "MixtureOfGaussianV1BGS")
    bgs = new MixtureOfGaussianV1BGS;
  if(methodName == "MixtureOfGaussianV2BGS")
    bgs = new MixtureOfGaussianV2BGS;
  if(methodName == "AdaptiveBackgroundLearning")
    bgs = new AdaptiveBackgroundLearning;
  if(methodName == "AdaptiveSelectiveBackgroundLearning")
    bgs = new AdaptiveSelectiveBackgroundLearning;
  if(methodName == "GMG")
    bgs = new GMG;
  
  if(methodName == "DPAdaptiveMedianBGS")
    bgs = new DPAdaptiveMedianBGS;
  if(methodName == "DPGrimsonGMMBGS")
    bgs = new DPGrimsonGMMBGS;
  if(methodName == "DPZivkovicAGMMBGS")
    bgs = new DPZivkovicAGMMBGS;
  if(methodName == "DPMeanBGS")
    bgs = new DPMeanBGS;
  if(methodName == "DPWrenGABGS")
    bgs = new DPWrenGABGS;
  if(methodName == "DPPratiMediodBGS")
    bgs = new DPPratiMediodBGS;
  if(methodName == "DPEigenbackgroundBGS")
    bgs = new DPEigenbackgroundBGS;
  if(methodName == "DPTextureBGS")
    bgs = new DPTextureBGS;

  if(methodName == "T2FGMM_UM")
    bgs = new T2FGMM_UM;
  if(methodName == "T2FGMM_UV")
    bgs = new T2FGMM_UV;
  if(methodName == "T2FMRF_UM")
    bgs = new T2FMRF_UM;
  if(methodName == "T2FMRF_UV")
    bgs = new T2FMRF_UV;
  if(methodName == "FuzzySugenoIntegral")
    bgs = new FuzzySugenoIntegral;
  if(methodName == "FuzzyChoquetIntegral")
    bgs = new FuzzyChoquetIntegral;

  if(methodName == "LBSimpleGaussian")
    bgs = new LBSimpleGaussian;
  if(methodName == "LBFuzzyGaussian")
    bgs = new LBFuzzyGaussian;
  if(methodName == "LBMixtureOfGaussians")
    bgs = new LBMixtureOfGaussians;
  if(methodName == "LBAdaptiveSOM")
    bgs = new LBAdaptiveSOM;
  if(methodName == "LBFuzzyAdaptiveSOM")
    bgs = new LBFuzzyAdaptiveSOM;
  if(methodName == "MultiLayerBGS")
    bgs = new MultiLayerBGS;
  // The PBAS algorithm was removed from BGSLibrary because it is based on patented algorithm ViBE
  // if(methodName == "PBAS")
  //  bgs = new PixelBasedAdaptiveSegmenter;
  if(methodName == "VuMeter")
    bgs = new VuMeter;
  if(methodName == "KDE")
    bgs = new KDE;
  if(methodName == "IndependentMultimodalBGS")
    bgs = new IndependentMultimodalBGS;
  if(methodName == "SJN_MultiCueBGS")
    bgs = new SJN_MultiCueBGS;
  if (methodName == "SigmaDeltaBGS")
    bgs = new SigmaDeltaBGS;
  if (methodName == "SuBSENSEBGS")
    bgs = new SuBSENSEBGS;
  if (methodName == "LOBSTERBGS")
    bgs = new LOBSTERBGS;

  if(bgs == NULL)
  {
    AfxMessageBox(L"BGS object not defined!");
    return;
  }

  started = true;
  int i = 0;
  if(useImgSeq == true && startIdx > 0)
       i = startIdx - 1;
  CString strFrameNumber;
  CString strExecTime;
  cv::Size default_size;
  default_size.width = 235;
  default_size.height = 189;
  IplImage* frame;
  std::string input_filename;
  cv::Mat img_input;
  // Convert a TCHAR string to a LPCSTR
  CT2CA fileType2(fileType);
  CT2CA filePath2(filePath);
  // construct a std::string using the LPCSTR input
  std::string str_fileType(fileType2);
  std::string str_filePath(filePath2);
  // delay
  CString csDelay;
  m_delay.GetWindowTextW(csDelay);
  int delay = _tstoi(csDelay);
  
  do
  {
    m_log.SetWindowTextW(L"Running...");

    i++;
    //::Sleep(1);

    if(useImgSeq == true && i == (stopIdx + 1))
      break;

    if(useImgSeq)
    {
      input_filename = str_filePath + "\\" + boost::lexical_cast<std::string>(i) + "." + str_fileType;
      img_input = cv::imread(input_filename);

      CString input_filename2(input_filename.c_str());
      m_log.SetWindowTextW(input_filename2);

      strFrameNumber.Format(L"%d",i);
      m_frameNumber.SetWindowTextW(strFrameNumber);

      if(img_input.data == NULL)
      {
        AfxMessageBox(L"File can not be read!");
        break;
      }
    }
    else
    {
      if(useWebCam == false && stopIdx >= 2 && i > stopIdx)
        break;

      frame = cvQueryFrame(capture);
      if(!frame)
        break;
      
      if(useWebCam == false && startIdx >= 2 && i < startIdx)
        continue;

      img_input = cv::Mat(frame,true);
    }

    cv::Mat img_mask;
    cv::Mat img_bkg;
    Clock::time_point t0 = Clock::now();
    bgs->process(img_input, img_mask, img_bkg);
    Clock::time_point t1 = Clock::now();
    auto d = boost::chrono::duration_cast<milliseconds>(t1 - t0);
    //std::cout << "\nElapsed time: " << d.count() << "ms" << std::endl;
    
    cv::Mat img_input_aux;
    cv::resize(img_input, img_input_aux, default_size);

    cv::Mat img_mask_aux;
    if(img_mask.empty())
      img_mask = cv::Mat::zeros(cv::Size(img_input.size().width, img_input.size().height), img_input.type());
    if(m_medianFilter.GetCheck() == BST_CHECKED)
      cv::medianBlur(img_mask, img_mask, 5);
    cv::resize(img_mask, img_mask_aux, default_size);

    cv::Mat img_bgk_aux;
    if(img_bkg.empty())
      img_bkg = cv::Mat::zeros(cv::Size(img_input.size().width, img_input.size().height), img_input.type());
    cv::resize(img_bkg, img_bgk_aux, default_size);
    
    cv::imshow("INPUT", img_input_aux);
    cv::imshow("MASK", img_mask_aux);
    cv::imshow("BKG", img_bgk_aux);

    extern_input_filename = "./outputs/input/" + boost::lexical_cast<std::string>(i)+".png";
    img_input.copyTo(extern_input_img);
    if(m_saveFrame.GetCheck() == BST_CHECKED)
      cv::imwrite(extern_input_filename, img_input);
    
    extern_fg_filename = "./outputs/foreground/" + boost::lexical_cast<std::string>(i)+".png";
    img_mask.copyTo(extern_fg_img);
    if(m_saveMask.GetCheck() == BST_CHECKED)
      cv::imwrite(extern_fg_filename, img_mask);
    
    extern_bg_filename = "./outputs/background/" + boost::lexical_cast<std::string>(i)+".png";
    img_bkg.copyTo(extern_bg_img);
    if(m_saveBkg.GetCheck() == BST_CHECKED)
      cv::imwrite(extern_bg_filename, img_bkg);
    
    strFrameNumber.Format(L"%d",i);
    m_frameNumber.SetWindowTextW(strFrameNumber);

    //strExecTime.Format(_T("%.2f"), d.count());
    strExecTime.Format(_T("%d"), d.count());
    m_execTime.SetWindowTextW(strExecTime);

    ::Sleep(delay);

  }while(1);
  
  delete bgs;
  
  if(!useImgSeq)
    cvReleaseCapture(&capture);

  //AfxMessageBox(L"Thread is finished!");
  m_log.SetWindowTextW(L"Finished!");
  started = false;
}


void CDlg::OnBnClickedStop()
{
  if(started)
  {
    m_log.SetWindowTextW(L"Stopping...");
  
    StopThread();

    if(!useImgSeq)
      if(capture)
        cvReleaseCapture(&capture);

    delete bgs;
    bgs = NULL;

    m_log.SetWindowTextW(L"Stopped!");
    started = false;
  }
}

void CDlg::StopThread()
{
  DWORD exit_code = NULL;

  if(thread != NULL)
  {
    GetExitCodeThread(thread->m_hThread, &exit_code);
    
    if(exit_code == STILL_ACTIVE)
    {
      ::TerminateThread(thread->m_hThread, 0);
      CloseHandle(thread->m_hThread);
    }

    thread->m_hThread = NULL;
    thread = NULL;
  }
}

void CDlg::OnBnClickedUseWebcam()
{
  if(m_useWebCam.GetCheck() == BST_CHECKED)
    m_isImgSeq.SetCheck(BST_UNCHECKED);
  
  m_inputVideo.EnableFileBrowseButton();
}

void CDlg::OnBnClickedImgSeq()
{
  if(m_isImgSeq.GetCheck() == BST_CHECKED)
  {
    m_inputVideo.EnableFolderBrowseButton();
    m_useWebCam.SetCheck(BST_UNCHECKED);
  }
  else
  {
    m_startIdx.SetWindowTextW(L"0");
    m_stopIdx.SetWindowTextW(L"0");
    m_inputVideo.EnableFileBrowseButton();
  }
}

void CDlg::OnBnClickedSave()
{
  if(m_saveFrame.GetCheck() == BST_CHECKED)
    cv::imwrite(extern_input_filename, extern_input_img);

  if(m_saveMask.GetCheck() == BST_CHECKED)
    cv::imwrite(extern_fg_filename, extern_fg_img);

  if(m_saveBkg.GetCheck() == BST_CHECKED)
    cv::imwrite(extern_bg_filename, extern_bg_img);

  m_log.SetWindowTextW(L"OK! Saved!");
}
