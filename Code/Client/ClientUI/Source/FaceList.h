
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



/**
 * @brief 用户头像信息
 * 
 */
class CFaceInfo
{
public:
	CFaceInfo(void);
	~CFaceInfo(void);

public:
	int m_nId; //头像ID
	int m_nIndex;//头像索引
	WString m_strTip;//头像信息提示
	WString m_strFileName;//头像文件名
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
	int m_nItemWidth;//宽度
	int m_nItemHeight;//高度
	int m_nZoomWidth;//缩放宽度
	int m_nZoomHeight;//缩放高度
	int m_nRow;//行数
	int m_nCol;//列数
	std::vector<CFaceInfo*> m_arrFaceInfo;
};

#endif