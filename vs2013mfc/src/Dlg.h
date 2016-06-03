
// Dlg.h : header file
//

#pragma once
#include "stdafx.h"
#include "afxwin.h"
#include "afxcmn.h"

// CDlg dialog
class CDlg : public CDialogEx
{
// Construction
public:
  CDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
  enum { IDD = IDD_APP };

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

private:
  bool started;
  CWinThread* thread;
  CvCapture* capture;
  IBGS* bgs;
  CString methodName;
  CString fileType;
  bool useWebCam;
  bool useImgSeq;
  int webCamIndex;
  int startIdx;
  int stopIdx;
  int extern_i;
  std::string extern_input_filename;
  cv::Mat extern_input_img;
  std::string extern_fg_filename;
  cv::Mat extern_fg_img;
  std::string extern_bg_filename;
  cv::Mat extern_bg_img;

  CString filePath;

public:
  afx_msg void ThreadProcess();
  static DWORD Thread(LPVOID *x);
  afx_msg void StopThread();
  afx_msg bool ipDraw(HDC hdc, IplImage* img, int xoffset=0, int yoffset=0);

public:
  void addBgsList();
  bool getBgsMethodName();
  CComboBox m_bgslist;
  afx_msg void OnBnClickedStart();
  CStatic m_log;
  CMFCEditBrowseCtrl m_inputVideo;
  bool getInputVideoFilePath();
  CButton m_useWebCam;
  afx_msg void OnBnClickedStop();
  CSpinButtonCtrl m_spinWebCamIndex;
  CEdit m_webCamIndex;
  CStatic m_frameNumber;
  CButton m_saveFrame;
  CButton m_saveMask;
  CButton m_saveBkg;
  CButton m_isImgSeq;
  CEdit m_startIdx;
  CEdit m_stopIdx;
  CComboBox m_fileTypeList;
  bool getFileType();
  CSpinButtonCtrl m_spinStartIdx;
  CSpinButtonCtrl m_spinStopIdx;
  afx_msg void OnBnClickedUseWebcam();
  afx_msg void OnBnClickedImgSeq();
  CEdit m_delay;
  CStatic m_execTime;
  CButton m_medianFilter;
  afx_msg void OnBnClickedSave();
};
