/**
 * @file FaceCtrl.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 头像控件实现文件
 * @version 0.1
 * @date 2020-02-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "stdafx.h"
#include "FaceCtrl.h"

/**
 * @brief Construct a new CFaceCtrl::CFaceCtrl object
 * 
 */
CFaceCtrl::CFaceCtrl(void)
{
	m_clrBg = RGB(255, 255, 255);
	m_clrLine = RGB(223, 230, 246);
	m_clrFocusBorder = RGB(0, 0, 255);
	m_clrZoomBorder = RGB(0, 138, 255);
	m_nRow = 8;
	m_nCol = 15;
	m_nItemWidth = m_nItemHeight = 28;
	m_nZoomWidth = m_nZoomHeight = 84;
	m_nCurPage = m_nPageCnt = 0;
	m_bMouseTracking = FALSE;
	m_nHoverIndex = -1;
	m_nFramePos = 0;
	m_rcZoomImg.SetRectEmpty();
	m_lpFaceList = NULL;
	m_dwTimerId = NULL;
}

/**
 * @brief Destroy the CFaceCtrl::CFaceCtrl object
 * 
 */
CFaceCtrl::~CFaceCtrl(void)
{
	DestroyImage();
}

/**
 * @brief 设置背景色
 * 
 * @param color 
 */
void CFaceCtrl::SetBgColor(COLORREF color)
{
	m_clrBg = color;
}

/**
 * @brief 设置线条颜色
 * 
 * @param color 
 */
void CFaceCtrl::SetLineColor(COLORREF color)
{
	m_clrLine = color;
}

/**
 * @brief 设置选中时的边框颜色
 * 
 * @param color 
 */
void CFaceCtrl::SetFocusBorderColor(COLORREF color)
{
	m_clrFocusBorder = color;
}

/**
 * @brief 设置缩放的边框颜色
 * 
 * @param color 
 */
void CFaceCtrl::SetZoomBorderColor(COLORREF color)
{
	m_clrZoomBorder = color;
}

/**
 * @brief 设置行数和列数
 * 
 * @param nRow 
 * @param nCol 
 */
void CFaceCtrl::SetRowAndCol(int nRow, int nCol)
{
	m_nRow = nRow;
	m_nCol = nCol;
}

/**
 * @brief 设置每一项的大小
 * 
 * @param nWidth 每个头像的宽度
 * @param nHeight 头像高度
 */
void CFaceCtrl::SetItemSize(int nWidth, int nHeight)
{
	m_nItemWidth = nWidth;
	m_nItemHeight = nHeight;
}

/**
 * @brief 设置缩放大小
 * 
 * @param nWidth 
 * @param nHeight 
 */
void CFaceCtrl::SetZoomSize(int nWidth, int nHeight)
{
	m_nZoomWidth = nWidth;
	m_nZoomHeight = nHeight;
}

/**
 * @brief 设置头像列表
 * 
 * @param lpFaceList 
 */
void CFaceCtrl::SetFaceList(CFaceList* lpFaceList)
{
	m_lpFaceList = lpFaceList;
}

/**
 * @brief 设置当前页
 * 
 * @param nPageIndex 当前页索引 
 */
void CFaceCtrl::SetCurPage(int nPageIndex)
{
	if (NULL == m_lpFaceList)
		return;

	int nCount = (int)m_lpFaceList->m_arrFaceInfo.size();
	int nOnePage = m_nRow* m_nCol;
	if (nCount > 0 && nOnePage > 0)
	{
		if (nCount % nOnePage == 0)
			m_nPageCnt = nCount / nOnePage;
		else
			m_nPageCnt = nCount / nOnePage + 1;
	}
	else
		m_nPageCnt = 0;

	if (nPageIndex < 0 || nPageIndex >= m_nPageCnt)
		return;

	m_nCurPage = nPageIndex;
	LoadImage(m_nCurPage);
}

/**
 * @brief 获取头像信息
 * 
 * @param nItemIndex 头像索引
 * @return CFaceInfo* 头像信息
 */
CFaceInfo* CFaceCtrl::GetFaceInfo(int nItemIndex)
{
	if (m_lpFaceList != NULL)
	{
		int nCount = (int)m_lpFaceList->m_arrFaceInfo.size();
		int nOnePage = m_nRow* m_nCol;
		int nStart = m_nCurPage* nOnePage;
		int nIndex = nStart + m_nHoverIndex;
		if (nIndex >= 0 && nIndex < nCount)
			return m_lpFaceList->m_arrFaceInfo[nIndex];
	}
	return NULL;
}

/**
 * @brief 响应擦除背景
 * 
 * @param dc 
 * @return BOOL 
 */
BOOL CFaceCtrl::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

/**
 * @brief 响应界面绘制
 * 
 * @param dc 
 */
void CFaceCtrl::OnPaint(CDCHandle dc)
{
	CPaintDC PaintDC(m_hWnd);
	
	CRect rcClient;
	GetClientRect(&rcClient);

	CMemoryDC MemDC(PaintDC.m_hDC, rcClient);

	MemDC.FillSolidRect(&rcClient, m_clrBg);

	DrawLine(MemDC.m_hDC);

	CRect rcItem, rcImage;
	int nRow = 0, nCol = 0;
	int nLeft = 0, nTop = 0;
	for (int i = 0; i < (int)m_arrImage.size(); i++)
	{
		rcItem = CRect(nLeft, nTop, nLeft+m_nItemWidth, nTop+m_nItemHeight);
		
		CGifImage* lpImage = m_arrImage[i];
		if (lpImage != NULL)
		{
			CalcCenterRect(rcItem, lpImage->GetWidth(), lpImage->GetHeight(), rcImage);
			lpImage->Draw(MemDC.m_hDC, rcImage, 0);
		}

		if (m_nHoverIndex == i)
			DrawFocusBorder(MemDC.m_hDC, rcItem);

		nLeft += m_nItemWidth;
		nCol++;
		if (nCol >= m_nCol)
		{
			nCol = 0;
			nRow++;
			if (nRow > m_nRow)
				break;
			nLeft = 0;
			nTop += m_nItemHeight;
		}
	}

	if (m_nHoverIndex != -1)
		DrawZoomImage(MemDC.m_hDC);
}

/**
 * @brief 响应鼠标左键按下
 * 
 * @param nFlags 
 * @param point 
 */
void CFaceCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);

	int nIndex = HitTest(point);
	if (nIndex != -1)
	{
		CFaceInfo* lpFaceInfo = GetFaceInfo(nIndex);
		if (lpFaceInfo != NULL)
			::SendMessage(GetParent(), FACE_CTRL_SEL, 0, nIndex);
	}
}

/**
 * @brief 响应鼠标左键抬起
 * 
 * @param nFlags 
 * @param point 
 */
void CFaceCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);
}

/**
 * @brief 响应鼠标移动
 * 
 * @param nFlags 
 * @param point 
 */
void CFaceCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);

	if (!m_bMouseTracking)
	{
		StartTrackMouseLeave();
		m_bMouseTracking = TRUE;
	}

	int nIndex = HitTest(point);
	if (nIndex != m_nHoverIndex)
	{
		if (m_dwTimerId != NULL)
		{
			KillTimer(m_dwTimerId);
			m_dwTimerId = NULL;
		}
		m_nFramePos = 0;
		m_nHoverIndex = nIndex;
		if (m_nHoverIndex != -1)
		{
			SetItemToolTip(m_nHoverIndex);

			CalcZoomRect(point);
			CGifImage* lpImage = GetZoomImage();
			if (lpImage != NULL)
			{
				if (lpImage->IsAnimatedGif())
					m_dwTimerId = SetTimer(1, lpImage->GetFrameDelay(m_nFramePos), NULL);
			}
		}
		this->Invalidate();
	}
}

/**
 * @brief 响应鼠标离开消息
 * 
 */
void CFaceCtrl::OnMouseLeave()
{
	SetMsgHandled(FALSE);
	m_bMouseTracking = FALSE;
	m_nHoverIndex = -1;
	this->Invalidate();
}

/**
 * @brief 响应鼠标消息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CFaceCtrl::OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);
	MSG msg = { m_hWnd, uMsg, wParam, lParam };
	if (m_ToolTipCtrl.IsWindow())
		m_ToolTipCtrl.RelayEvent(&msg);
	return 1;
}

/**
 * @brief 响应定时器消息
 * 
 * @param nIDEvent 
 */
void CFaceCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_dwTimerId)
	{
		KillTimer(nIDEvent);

		CGifImage* lpImage = GetZoomImage();
		if (lpImage != NULL)
		{
			m_nFramePos++;
			if (m_nFramePos >= (int)lpImage->GetFrameCount())
				m_nFramePos = 0;
			m_dwTimerId = SetTimer(1, lpImage->GetFrameDelay(m_nFramePos), NULL);
		}
		this->Invalidate();
	}
}

/**
 * @brief 响应控件销毁
 * 
 */
void CFaceCtrl::OnDestroy()
{
	SetMsgHandled(FALSE);

	if (m_dwTimerId != NULL)
	{
		KillTimer(m_dwTimerId);
		m_dwTimerId = NULL;
	}

	if (m_ToolTipCtrl.IsWindow())	// ToolTipCtrl早在之前的不知道什么地方已销毁，这里显式置空m_hWnd
		m_ToolTipCtrl.DestroyWindow();
	m_ToolTipCtrl.m_hWnd = NULL;

	DestroyImage();

	m_nHoverIndex = -1;
	m_nFramePos = 0;
}

/**
 * @brief 开始追踪鼠标离开
 * 
 * @return BOOL 
 */
BOOL CFaceCtrl::StartTrackMouseLeave()
{
	TRACKMOUSEEVENT tme = { 0 };
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = m_hWnd;
	return _TrackMouseEvent(&tme);
}

/**
 * @brief 计算中心区域矩形
 * 
 * @param rcDest 
 * @param cx 
 * @param cy 
 * @param rcCenter 
 */
void CFaceCtrl::CalcCenterRect(CRect& rcDest, int cx, int cy, CRect& rcCenter)
{
	if (cx > rcDest.Width())
		cx = rcDest.Width() - 2;

	if (cy > rcDest.Height())
		cy = rcDest.Height() - 2;
	
	int x = (rcDest.Width() - cx + 1) / 2;
	int y = (rcDest.Height() - cy + 1) / 2;

	rcCenter = CRect(rcDest.left+x, rcDest.top+y, rcDest.left+x+cx, rcDest.top+y+cy);
}

/**
 * @brief 点击测试
 * 
 * @param pt 
 * @return int 
 */
int CFaceCtrl::HitTest(POINT pt)
{
	CRect rcItem;
	int nRow = 0, nCol = 0;
	int nLeft = 0, nTop = 0;
	for (int i = 0; i < (int)m_arrImage.size(); i++)
	{
		rcItem = CRect(nLeft, nTop, nLeft+m_nItemWidth, nTop+m_nItemHeight);
		if (rcItem.PtInRect(pt))
			return i;

		nLeft += m_nItemWidth;
		nCol++;
		if (nCol >= m_nCol)
		{
			nCol = 0;
			nRow++;
			if (nRow > m_nRow)
				break;
			nLeft = 0;
			nTop += m_nItemHeight;
		}
	}

	return -1;
}

/**
 * @brief 计算缩放矩形
 * 
 * @param point 
 */
void CFaceCtrl::CalcZoomRect(CPoint point)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	int nCenter = rcClient.Width() / 2;
	if (point.x < nCenter)
	{
		m_rcZoomImg.left = rcClient.right - m_nZoomWidth;
		m_rcZoomImg.right = rcClient.right;
		m_rcZoomImg.top = rcClient.top + 1;
		m_rcZoomImg.bottom = m_rcZoomImg.top + m_nZoomHeight;
	}
	else
	{
		m_rcZoomImg.left = rcClient.left + 1;
		m_rcZoomImg.right = m_rcZoomImg.left + m_nZoomWidth;
		m_rcZoomImg.top = rcClient.top + 1;
		m_rcZoomImg.bottom = m_rcZoomImg.top + m_nZoomHeight;
	}
}

/**
 * @brief 获取每一项的矩形区域
 * 
 * @param nItemIndex 
 * @param rect 
 * @return BOOL 
 */
BOOL CFaceCtrl::GetItemRect(int nItemIndex, CRect& rect)
{
	rect = CRect(0,0,0,0);

	if (nItemIndex < 0 || nItemIndex >= (int)m_arrImage.size())
		return FALSE;

	CRect rcItem;
	int nRow = 0, nCol = 0;
	int nLeft = 0, nTop = 0;
	for (int i = 0; i < (int)m_arrImage.size(); i++)
	{
		rcItem = CRect(nLeft, nTop, nLeft+m_nItemWidth, nTop+m_nItemHeight);
		if (i == nItemIndex)
		{
			rect = rcItem;
			return TRUE;
		}

		nLeft += m_nItemWidth;
		nCol++;
		if (nCol >= m_nCol)
		{
			nCol = 0;
			nRow++;
			if (nRow > m_nRow)
				break;
			nLeft = 0;
			nTop += m_nItemHeight;
		}
	}

	return FALSE;
}

/**
 * @brief 获取缩放图
 * 
 * @return CGifImage* 
 */
CGifImage* CFaceCtrl::GetZoomImage()
{
	if (m_nHoverIndex >= 0 && m_nHoverIndex < (int)m_arrImage.size())
		return m_arrImage[m_nHoverIndex];
	else
		return NULL;
}

/**
 * @brief 设置某一项的提示,在该项选中时使用
 * 
 * @param nItemIndex 
 */
void CFaceCtrl::SetItemToolTip(int nItemIndex)
{
	CRect rcItem;
	GetItemRect(nItemIndex, rcItem);

	CFaceInfo* lpFaceInfo;
	CString strTip;

	lpFaceInfo = GetFaceInfo(nItemIndex);
	if (lpFaceInfo != NULL)
		strTip = lpFaceInfo->m_strTip.c_str();

	if (!m_ToolTipCtrl.IsWindow())
	{
		m_ToolTipCtrl.Create(m_hWnd);
		m_ToolTipCtrl.SetMaxTipWidth(200);
	}

	if (m_ToolTipCtrl.IsWindow())
	{
		if (m_ToolTipCtrl.GetToolCount() <= 0)
		{
			m_ToolTipCtrl.Activate(TRUE);
			m_ToolTipCtrl.AddTool(m_hWnd, strTip.GetBuffer(), &rcItem, 1);
		}
		else
		{
			m_ToolTipCtrl.Activate(TRUE);
			m_ToolTipCtrl.UpdateTipText(strTip.GetBuffer(), m_hWnd, 1);
			m_ToolTipCtrl.SetToolRect(m_hWnd, 1, &rcItem);
		}
	}
}

/**
 * @brief 绘制边框线
 * 
 * @param hDC 
 */
void CFaceCtrl::DrawLine(HDC hDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	HPEN hPen = ::CreatePen(PS_SOLID, 1, m_clrLine);
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);

	int x1 = 0, y1 = 0, x2 = rcClient.right, y2 = 0;
	for (int i = 0; i < m_nRow; i++)
	{
		::MoveToEx(hDC, x1, y1, NULL);
		::LineTo(hDC, x2, y2);
		y1 += m_nItemHeight;
		y2 = y1;
	}

	x1 = 0, y1 = 0, x2 = 0, y2 = rcClient.bottom;
	for (int i = 0; i < m_nCol; i++)
	{
		::MoveToEx(hDC, x1, y1, NULL);
		::LineTo(hDC, x2, y2);
		x1 += m_nItemWidth;
		x2 = x1;
	}

	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
}


/**
 * @brief 绘制选中时的边框
 * 
 * @param hDC 
 * @param rect 
 */
void CFaceCtrl::DrawFocusBorder(HDC hDC, const CRect& rect)
{
	HPEN hPen = ::CreatePen(PS_SOLID, 1, m_clrFocusBorder);
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, (HBRUSH)::GetStockObject(NULL_BRUSH));
	::Rectangle(hDC, rect.left+1, rect.top+1, rect.right, rect.bottom);
	::SelectObject(hDC, hOldBrush);
	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
}

/**
 * @brief 绘制缩放图
 * 
 * @param hDC 
 */
void CFaceCtrl::DrawZoomImage(HDC hDC)
{
	HPEN hPen = ::CreatePen(PS_SOLID, 1, m_clrZoomBorder);
	HBRUSH hBrush = ::CreateSolidBrush(m_clrBg);
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, hBrush);
	::Rectangle(hDC, m_rcZoomImg.left, m_rcZoomImg.top, m_rcZoomImg.right, m_rcZoomImg.bottom);
	::SelectObject(hDC, hOldBrush);
	::SelectObject(hDC, hOldPen);
	::DeleteObject(hBrush);
	::DeleteObject(hPen);

	if (m_nHoverIndex >= 0 && m_nHoverIndex < (int)m_arrImage.size())
	{
		CGifImage* lpImage = m_arrImage[m_nHoverIndex];
		if (lpImage != NULL)
		{
			CRect rcZoomImg;
			CalcCenterRect(m_rcZoomImg, lpImage->GetWidth(), 
				lpImage->GetHeight(), rcZoomImg);
			lpImage->Draw(hDC, rcZoomImg, m_nFramePos);
		}
	}
}

/**
 * @brief 加载图片
 * 
 * @param nPageIndex 
 * @return BOOL 
 */
BOOL CFaceCtrl::LoadImage(int nPageIndex)
{
	if (NULL == m_lpFaceList)
		return FALSE;

	DestroyImage();

	int nCount = (int)m_lpFaceList->m_arrFaceInfo.size();
	int nOnePage = m_nRow* m_nCol;

	int nStart = nPageIndex* nOnePage;
	int nEnd = nStart + nOnePage;
	if (nEnd > nCount)
		nEnd = nCount;

	for (int i = nStart; i < nEnd; i++)
	{
		CFaceInfo* lpFaceInfo = m_lpFaceList->m_arrFaceInfo[i];
		if (lpFaceInfo != NULL)
		{
			CGifImage* lpImage = new CGifImage;
			if (lpImage != NULL)
			{
				BOOL bRet = lpImage->LoadFromFile(lpFaceInfo->m_strFileName.c_str());
				if (bRet)
					m_arrImage.push_back(lpImage);
				else
					delete lpImage;
			}
		}
	}

	return TRUE;
}

/**
 * @brief 释放图像
 * 
 */
void CFaceCtrl::DestroyImage()
{
	for (int i = 0; i < (int)m_arrImage.size(); i++)
	{
		CGifImage* lpImage = m_arrImage[i];
		if (lpImage != NULL)
			delete lpImage;
	}
	m_arrImage.clear();
}