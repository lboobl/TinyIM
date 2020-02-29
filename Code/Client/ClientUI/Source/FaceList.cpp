/**
 * @file FaceList.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 头像列表信息的实现文件,数据结构实现
 * @version 0.1
 * @date 2020-02-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "stdafx.h"
#include "FaceList.h"
#include "XmlDocument.h"
#include "Utils.h"
#include "Path.h"

/**
 * @brief Construct a new CFaceInfo::CFaceInfo object
 * 
 */
CFaceInfo::CFaceInfo(void)
{
	m_nId = -1;
	m_nIndex = -1;
	m_strTip = _T("");
	m_strFileName = _T("");
}

/**
 * @brief Destroy the CFaceInfo::CFaceInfo object
 * 
 */
CFaceInfo::~CFaceInfo(void)
{

}

/**
 * @brief Construct a new CFaceList::CFaceList object
 * 
 */
CFaceList::CFaceList(void)
{
	Reset();
}

/**
 * @brief Destroy the CFaceList::CFaceList object
 * 
 */
CFaceList::~CFaceList(void)
{
	Reset();
}

/**
 * @brief 清空列表数据
 * 
 */
void CFaceList::Reset()
{
// 	m_nWidth = 28;
// 	m_nHeight = 28;
// 	m_nZoomWidth = 86;
// 	m_nZoomHeight = 86;
// 	m_nCol = 15;
	m_nItemWidth = m_nItemHeight = 0;
	m_nZoomWidth = m_nZoomHeight = 0;
	m_nRow = m_nCol = 0;
	
	for (int i = 0; i < (int)m_arrFaceInfo.size(); i++)
	{
		CFaceInfo* lpFaceInfo = m_arrFaceInfo[i];
		if (lpFaceInfo != NULL)
			delete lpFaceInfo;
	}
	m_arrFaceInfo.clear();
}


/**
 * @brief 加载配置文件
 * 
 * @param lpszFileName 
 * @return BOOL 
 */
BOOL CFaceList::LoadConfigFile(LPCTSTR lpszFileName)
{
	CXmlDocument xmlDoc;
	CXmlNode xmlNode, xmlSubNode;

	Reset();

	BOOL bRet = xmlDoc.Load(lpszFileName);
	if (!bRet)
		return FALSE;

	bRet = xmlDoc.SelectSingleNode(_T("/faceconfig"), xmlNode);
	if (bRet)
	{
		m_nItemWidth = xmlNode.GetAttributeInt(_T("item_width"));
		m_nItemHeight = xmlNode.GetAttributeInt(_T("item_height"));
		m_nZoomWidth = xmlNode.GetAttributeInt(_T("zoom_width"));
		m_nZoomHeight = xmlNode.GetAttributeInt(_T("zoom_height"));
		m_nRow = xmlNode.GetAttributeInt(_T("row"));
		m_nCol = xmlNode.GetAttributeInt(_T("col"));

		bRet = xmlNode.GetFirstChildNode(_T("face"), xmlSubNode);
		while (bRet)
		{
			CFaceInfo* lpFaceInfo = new CFaceInfo;
			if (lpFaceInfo != NULL)
			{
				lpFaceInfo->m_nId = xmlSubNode.GetAttributeInt(_T("id"));
				lpFaceInfo->m_strTip = xmlSubNode.GetAttribute(_T("tip"));
				lpFaceInfo->m_strFileName = Hootina::CPath::GetAppPath() + xmlSubNode.GetAttribute(_T("file"));
				WString strIndex = Hootina::CPath::GetFileNameWithoutExtension(lpFaceInfo->m_strFileName.c_str());
				if (IsDigit(strIndex.c_str()))
					lpFaceInfo->m_nIndex = _tcstol(strIndex.c_str(), NULL, 10);
				m_arrFaceInfo.push_back(lpFaceInfo);
			}
			
			bRet = xmlSubNode.GetNextSiblingNode(_T("face"), xmlSubNode);
		}
	}

	xmlSubNode.Release();
	xmlNode.Release();
	xmlDoc.Release();

	return TRUE;
}

/**
 * @brief 根据索引获取头像信息
 * 
 * @param nIndex 
 * @return CFaceInfo* 
 */
CFaceInfo* CFaceList::GetFaceInfo(int nIndex)
{
	if (nIndex >= 0 && nIndex < (int)m_arrFaceInfo[nIndex])
		return m_arrFaceInfo[nIndex];
	else
		return NULL;
}

/**
 * @brief 根据头像ID获取头像信息
 * 
 * @param nFaceId 
 * @return CFaceInfo* 
 */
CFaceInfo* CFaceList::GetFaceInfoById(int nFaceId)
{
	for (int i = 0; i < (int)m_arrFaceInfo.size(); i++)
	{
		CFaceInfo* lpFaceInfo = m_arrFaceInfo[i];
		if (lpFaceInfo != NULL && lpFaceInfo->m_nId == nFaceId)
			return lpFaceInfo;
	}

	return NULL;
}

/**
 * @brief 根据头像索引获取头像信息
 * 
 * @param nFaceIndex 
 * @return CFaceInfo* 
 */
CFaceInfo* CFaceList::GetFaceInfoByIndex(int nFaceIndex)
{
	for (int i = 0; i < (int)m_arrFaceInfo.size(); i++)
	{
		CFaceInfo* lpFaceInfo = m_arrFaceInfo[i];
		if (lpFaceInfo != NULL && lpFaceInfo->m_nIndex == nFaceIndex)
			return lpFaceInfo;
	}

	return NULL;
}