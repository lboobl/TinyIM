
/**
 * @file GifImage.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 显示GIF图像的实现类
 * TODO: 具体的作用需要再次确定
 * @version 0.1
 * @date 2020-03-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "stdafx.h"
#include "GifImage.h"

/**
 * @brief Construct a new CGifImage::CGifImage object
 * 
 */
CGifImage::CGifImage(void)
{
	m_pImage = NULL;
	m_nFrameCnt = 0;
	m_nFramePos = 0;
	m_pFrameDelay = NULL;
}

/**
 * @brief Destroy the CGifImage::CGifImage object
 * 
 */
CGifImage::~CGifImage(void)
{
	Destroy();
}

/**
 * @brief 从文件中加载图片
 * 
 * @param pszFileName 
 * @return BOOL 
 */
BOOL CGifImage::LoadFromFile(LPCTSTR pszFileName)
{
	Destroy();

	if (NULL == pszFileName || NULL ==*pszFileName)
		return FALSE;

	m_pImage = new Gdiplus::Image(pszFileName);
	if (NULL == m_pImage)
		return FALSE;
	
	if (m_pImage->GetLastStatus() != Gdiplus::Ok)
	{
		delete m_pImage;
		m_pImage = NULL;
		return FALSE;
	}

	return ReadGifInfo();
}

/**
 * @brief 从数据流中加载图片
 * 
 * @param pStream 
 * @return BOOL 
 */
BOOL CGifImage::LoadFromIStream(IStream* pStream)
{
	Destroy();

	if (NULL == pStream)
		return FALSE;

	m_pImage = new Gdiplus::Image(pStream);
	if (NULL == m_pImage)
		return FALSE;

	if (m_pImage->GetLastStatus() != Gdiplus::Ok)
	{
		delete m_pImage;
		m_pImage = NULL;
		return FALSE;
	}

	return ReadGifInfo();
}

/**
 * @brief 从内存中加载图片
 * 
 * @param lpBuf 
 * @param dwSize 
 * @return BOOL 
 */
BOOL CGifImage::LoadFromBuffer(const BYTE* lpBuf, DWORD dwSize)
{
	if (NULL == lpBuf || dwSize <= 0)
		return FALSE;

	HGLOBAL hGlobal = ::GlobalAlloc(GHND, dwSize);
	if (NULL == hGlobal)
		return FALSE;

	LPVOID lpBuffer = ::GlobalLock(hGlobal);
	if (NULL == lpBuffer)
	{
		::GlobalFree(hGlobal);
		return FALSE;
	}

	memcpy(lpBuffer, lpBuf, dwSize);
	::GlobalUnlock(hGlobal);

	LPSTREAM lpStream = NULL;
	HRESULT hr = ::CreateStreamOnHGlobal(hGlobal, TRUE, &lpStream);
	if (hr != S_OK)
	{
		::GlobalFree(hGlobal);
		return FALSE;
	}

	BOOL bRet = LoadFromIStream(lpStream);
	lpStream->Release();

	return bRet;
}

/**
 * @brief 从资源中加载图片
 * TODO: 未实现
 * @param hInstance 
 * @param pszResourceName 
 * @param pszResType 
 * @return BOOL 
 */
BOOL CGifImage::LoadFromResource(HINSTANCE hInstance, LPCTSTR pszResourceName, LPCTSTR pszResType)
{
	return TRUE;
}

/**
 * @brief 根据资源ID来加载图片
 * TODO: 未实现
 * @param hInstance 
 * @param nIDResource 
 * @param pszResType 
 * @return BOOL 
 */
BOOL CGifImage::LoadFromResource(HINSTANCE hInstance, UINT nIDResource, LPCTSTR pszResType)
{
	return TRUE;
}

/**
 * @brief 将GIF图片保存为文件
 * 
 * @param pszFileName 
 * @return BOOL 
 */
BOOL CGifImage::SaveAsFile(LPCTSTR pszFileName)
{
	if (NULL == pszFileName || NULL == m_pImage)
		return FALSE;

	LPCTSTR lpExtension = _tcsrchr(pszFileName, _T('.'));
	if (NULL == lpExtension)
		return FALSE;

	CLSID clsid = GetEncoderClsidByExtension(lpExtension);
	if (CLSID_NULL == clsid)
		return FALSE;

	Gdiplus::Status status = m_pImage->Save(pszFileName, &clsid, NULL);
	return (status != Gdiplus::Ok) ? FALSE : TRUE;
}


/**
 * @brief 释放保存GIF图片的相关内存
 * 
 */
void CGifImage::Destroy()
{
	m_nFrameCnt = 0;
	m_nFramePos = 0;

	if (m_pFrameDelay != NULL)
	{
		delete []m_pFrameDelay;
		m_pFrameDelay = NULL;
	}

	if (m_pImage != NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}

/**
 * @brief 获取GIF图片的帧数
 * 
 * @return UINT 
 */
UINT CGifImage::GetFrameCount()
{
	return m_nFrameCnt;
}

/**
 * @brief 判断是否为动态的帧数
 * 
 * @return BOOL 
 */
BOOL CGifImage::IsAnimatedGif()
{
	return m_nFrameCnt > 1;
}

/**
 * @brief 获取当前帧的时延
 * 
 * @param nFramePos 
 * @return long 
 */
long CGifImage::GetFrameDelay(int nFramePos/* = -1*/)
{
	if (!IsAnimatedGif() || NULL == m_pFrameDelay)
		return 0;

	int nFramePos2;
	if (nFramePos != -1)
		nFramePos2 = nFramePos;
	else
		nFramePos2 = m_nFramePos;

	if (nFramePos2 >= 0 && nFramePos2 < (int)m_nFrameCnt)
		return m_pFrameDelay[nFramePos2];
	else
		return 0;
}

/**
 * @brief 激活下一帧图像
 * 
 */
void CGifImage::ActiveNextFrame()
{
	if (m_pImage != NULL && IsAnimatedGif())
	{
		m_nFramePos++;
		if (m_nFramePos == m_nFrameCnt)
			m_nFramePos = 0;

		if (m_nFramePos >= 0 && m_nFramePos < m_nFrameCnt)
		{
			static GUID Guid = Gdiplus::FrameDimensionTime;
			Gdiplus::Status status = m_pImage->SelectActiveFrame(&Guid, m_nFramePos);
		}
	}
}

/**
 * @brief 选择当前的激活帧
 * 
 * TODO: 此函数应该被某个函数作为循环体调用了
 * @param nFramePos 
 */
void CGifImage::SelectActiveFrame(int nFramePos)
{
	if (m_pImage != NULL && IsAnimatedGif() 
		&& nFramePos >= 0 && nFramePos < (int)m_nFrameCnt)
	{
		static GUID Guid = Gdiplus::FrameDimensionTime;
		Gdiplus::Status status = m_pImage->SelectActiveFrame(&Guid, nFramePos);
		m_nFramePos = nFramePos;
	}
}

/**
 * @brief 绘制具体的图像
 * 
 * @param hDestDC 
 * @param xDest 
 * @param yDest 
 * @param nFramePos 
 * @return BOOL 
 */
BOOL CGifImage::Draw(HDC hDestDC, int xDest, int yDest, int nFramePos/* = -1*/)
{
 	Gdiplus::Graphics graphics(hDestDC);
 	Gdiplus::Status status = graphics.DrawImage(m_pImage, xDest, yDest);
 	if(status != Gdiplus::Ok)
 		return FALSE;
 	else
 		return TRUE;
}

/**
 * @brief 绘制具体的图像
 * 
 * @param hDestDC 
 * @param rectDest 
 * @param nFramePos 
 * @return BOOL 
 */
BOOL CGifImage::Draw(HDC hDestDC, const RECT& rectDest, int nFramePos/* = -1*/)
{
	if (NULL == m_pImage)
		return FALSE;

	if (nFramePos != -1)
		SelectActiveFrame(nFramePos);

	int nWidth = rectDest.right-rectDest.left;
	int nHeight = rectDest.bottom-rectDest.top;

 	Gdiplus::Graphics graphics(hDestDC);
 	Gdiplus::Status status = graphics.DrawImage(m_pImage, 
 		Gdiplus::Rect(rectDest.left, rectDest.top, nWidth, nHeight));
 	if(status != Gdiplus::Ok)
 		return FALSE;
 	else
 		return TRUE;
}

/**
 * @brief 获取图像的宽度
 * 
 * @return int 
 */
int CGifImage::GetWidth()
{
	if (m_pImage != NULL)
		return m_pImage->GetWidth();
	else
		return 0;
}

/**
 * @brief 获取图像的高度
 * 
 * @return int 
 */
int CGifImage::GetHeight()
{
	if (m_pImage != NULL)
		return m_pImage->GetHeight();
	else
		return 0;
}

/**
 * @brief 获取当前帧的位置
 * 
 * @return UINT 
 */
UINT CGifImage::GetCurFramePos()
{
	return m_nFramePos;
}

/**
 * @brief 获取原始数据格式
 * 
 * @param lpGuid 
 * @return BOOL 
 */
BOOL CGifImage::GetRawFormat(GUID* lpGuid)
{
	Gdiplus::Status status = m_pImage->GetRawFormat(lpGuid);
	return (Gdiplus::Ok == status) ? TRUE : FALSE;
}

/**
 * @brief 
 * TODO: 具体作用不清楚
 * @param lpExtension 
 * @return CLSID 
 */
CLSID CGifImage::GetEncoderClsidByExtension(const WCHAR* lpExtension)
{
	CLSID clsid = CLSID_NULL;

	if (NULL == lpExtension)
		return clsid;

	UINT numEncoders = 0, size = 0;
	Gdiplus::Status status = Gdiplus::GetImageEncodersSize(&numEncoders, &size);  
	if (status != Gdiplus::Ok)
		return clsid;

	Gdiplus::ImageCodecInfo* lpEncoders = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (NULL == lpEncoders)
		return clsid;

	status = Gdiplus::GetImageEncoders(numEncoders, size, lpEncoders);
	if (Gdiplus::Ok == status)
	{
		for (UINT i = 0; i < numEncoders; i++)
		{
			BOOL bFind = FALSE;
			const WCHAR* pStart = lpEncoders[i].FilenameExtension;
			const WCHAR* pEnd = wcschr(pStart, L';');
			do 
			{
				if (NULL == pEnd)
				{
					LPCWSTR lpExt = ::wcsrchr(pStart, L'.');
					if ((lpExt != NULL) && (_wcsicmp(lpExt, lpExtension) == 0))
					{
						clsid = lpEncoders[i].Clsid;
						bFind = TRUE;
					}
					break;
				}

				int nLen = pEnd-pStart;
				if (nLen < MAX_PATH)
				{
					WCHAR cBuf[MAX_PATH] = {0};
					wcsncpy(cBuf, pStart, nLen);
					LPCWSTR lpExt = ::wcsrchr(cBuf, L'.');
					if ((lpExt != NULL) && (_wcsicmp(lpExt, lpExtension) == 0))
					{
						clsid = lpEncoders[i].Clsid;
						bFind = TRUE;
						break;
					}
				}
				pStart = pEnd+1;
				if (L'\0' ==*pStart)
					break;
				pEnd = wcschr(pStart, L';');
			} while (1);
			if (bFind)
				break;
		}
	}

	free(lpEncoders);

	return clsid;
}

/**
 * @brief 读取GIF图片的信息
 * 
 * @return BOOL 
 */
BOOL CGifImage::ReadGifInfo()
{
	if (NULL == m_pImage)
		return FALSE;

	UINT nCount = 0;
	nCount = m_pImage->GetFrameDimensionsCount();
	if (nCount <= 0)
		return FALSE;

	GUID* pDimensionIDs = new GUID[nCount];
	if (NULL == pDimensionIDs)
		return FALSE;
	m_pImage->GetFrameDimensionsList(pDimensionIDs, nCount);
	m_nFrameCnt = m_pImage->GetFrameCount(&pDimensionIDs[0]);
	delete pDimensionIDs;

	if (m_nFrameCnt <= 1)
		return TRUE;

	UINT nSize = m_pImage->GetPropertyItemSize(PropertyTagFrameDelay);
	if (nSize <= 0)
		return FALSE;

	Gdiplus::PropertyItem* pPropertyItem = (Gdiplus::PropertyItem*)malloc(nSize);
	if (NULL == pPropertyItem)
		return FALSE;

	m_pImage->GetPropertyItem(PropertyTagFrameDelay, nSize, pPropertyItem);

	m_pFrameDelay = new long[m_nFrameCnt];
	if (NULL == m_pFrameDelay)
	{
		free(pPropertyItem);
		return FALSE;
	}

	for (int i = 0; i < (int)m_nFrameCnt; i++)
	{
		m_pFrameDelay[i] = ((long*)pPropertyItem->value)[i]* 10;	// 帧切换延迟时间，以1/100秒为单位
		if (m_pFrameDelay[i] < 100)
			m_pFrameDelay[i] = 100;
	}

	free(pPropertyItem);

	return TRUE;
}