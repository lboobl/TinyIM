/**
 * @file File2.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 文件扩充类头文件
 * @version 0.1
 * @date 2019-08-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _DENNIS_THINK_C_FILE_2_H_ 
#define _DENNIS_THINK_C_FILE_2_H_


//#include "Buffer.h"
#include <stdint.h>
#include "UICommonDef.h"
//////////////////////////////////////
////文件系列的函数
//////////////////////////////////////
////打开一个文件并获得该文件的句柄
//HANDLE GetFileHandle(PCTSTR pszFileName);
////获取文件大小
//DWORD IUGetFileSize(PCTSTR pszFileName);		//不支持超过4G的文件
//DWORD IUGetFileSize(HANDLE hFile);				//不支持超过4G的文件
//UINT64 IUGetFileSize2(PCTSTR pszFileName);		//支持超过4G的文件
//
////获取文件md5值
//E_UI_GET_FILE_MD5_RETCODE GetFileMd5ValueA(PCTSTR pszFileName, char* pszMd5, long nMd5Length, int64_t& nFileSize, HWND hwndReflection=NULL, HANDLE hCancelEvent=NULL);
//BOOL GetFileMd5ValueW(PCTSTR pszFileName, TCHAR* pszMd5, long nMd5Length);
////获取文件的上传名称：大小|md5值
//BOOL GetFileUploadName(PCTSTR pszFileName, char* pszUploadName, long nUploadNameLength);
////获取文件指定部分
//BOOL GetFileData(HANDLE hFile, DWORD dwOffset, DWORD dwLength, char* buffer);
//BOOL GetFileData(PCTSTR pszFileName, DWORD dwOffset, DWORD dwLength, char* buffer);

//class CAutoFileHandle
//{
//public:
//	CAutoFileHandle(HANDLE hFile);
//	~CAutoFileHandle();
//
//public:
//	operator HANDLE();
//	void Release();
//
//private:
//	HANDLE m_hFile;
//};

class CFile
{
public:
	CFile();
	~CFile();

	BOOL Open(LPCTSTR lpszFileName, BOOL bCreateAlways = TRUE);
	long GetSize();
	
	const char* Read();
	BOOL Write(const char* pBuffer, long nSize);
	HANDLE GetFileHandle(){return m_hFile;}

	void Close();

private:
	HANDLE	m_hFile;
	//CBuffer	m_Buffer;
};

#endif