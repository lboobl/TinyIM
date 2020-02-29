
/**
 * @file FaceList.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 头像列表控件
 * @version 0.1
 * @date 2019-08-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_C_FACE_LIST_H_
#define _TINY_IM_C_FACE_LIST_H_
#include <vector>
#include "Utils.h"
#include "Path.h"
#include "XmlDocument.h"

class CFaceInfo
{
public:
	CFaceInfo(void);
	~CFaceInfo(void);

public:
	int m_nId;
	int m_nIndex;
	WString m_strTip;
	WString m_strFileName;
};

class CFaceList
{
public:
	CFaceList(void);
	~CFaceList(void);

public:
	void Reset();
	BOOL LoadConfigFile(LPCTSTR lpszFileName);
	CFaceInfo* GetFaceInfo(int nIndex);
	CFaceInfo* GetFaceInfoById(int nFaceId);
	CFaceInfo* GetFaceInfoByIndex(int nFaceIndex);

public:
	int m_nItemWidth, m_nItemHeight;
	int m_nZoomWidth, m_nZoomHeight;
	int m_nRow, m_nCol;
	std::vector<CFaceInfo*> m_arrFaceInfo;
};

#endif