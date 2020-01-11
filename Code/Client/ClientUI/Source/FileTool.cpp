#include "stdafx.h"
#include "FileTool.h"
#include "MiniBuffer.h"
#include "Path.h"
#include "EncodingUtil.h"
#include "net/IUProtocolData.h"
#include "UI_USER_INFO.h"
#include <stdint.h>
#include "UIWndMsgDef.h"



CTinyImFile::CTinyImFile()
{
	m_hFile = INVALID_HANDLE_VALUE;
}

CTinyImFile::~CTinyImFile()
{
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hFile);
		//m_Buffer.Release();
	}
}

BOOL CTinyImFile::Open(LPCTSTR lpszFileName, BOOL bCreateAlways/* = TRUE*/)
{
	DWORD dwCreationFlag = (bCreateAlways ? CREATE_ALWAYS : OPEN_EXISTING);
	m_hFile = ::CreateFile(lpszFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, dwCreationFlag, FILE_ATTRIBUTE_NORMAL, NULL);
	
	return (m_hFile!=INVALID_HANDLE_VALUE);
}

long CTinyImFile::GetSize()
{
	if(m_hFile == INVALID_HANDLE_VALUE)
		return 0;
	else 
		return ::GetFileSize(m_hFile, NULL);
}

const char* CTinyImFile::Read()
{
	if(m_hFile == INVALID_HANDLE_VALUE)
		return NULL;

	//m_Buffer.Release();

	long nSize = GetSize();
	if(nSize <= 0)
		return NULL;

	char* pBuffer = new char[nSize+1];
	memset(pBuffer, 0, nSize+1);
	DWORD dwBytesRead = 0;
	DWORD dwTotalBytes = 0;
	do{
		if(!::ReadFile(m_hFile, pBuffer+dwTotalBytes, nSize-dwTotalBytes, &dwBytesRead, NULL))
			return NULL;
		
		dwTotalBytes += dwBytesRead;
		if(dwTotalBytes >= (DWORD)nSize)
			break;

		::SetFilePointer(m_hFile, dwTotalBytes, NULL, FILE_BEGIN);

	}while(TRUE);
	return pBuffer;
}

BOOL CTinyImFile::Write(const char* pBuffer, long nSize)
{
	if(m_hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD dwBytesWritten = 0;
	DWORD dwBytesTotal = 0;
	DWORD dwSizeToWriteEachTime = 4*1024;
	//写文件的次数
	if((DWORD)nSize <= dwSizeToWriteEachTime)
		dwSizeToWriteEachTime = nSize;
	
	do 
	{
		//为了防止文件较大一次性写失败，这里每次只写4K
		if(!::WriteFile(m_hFile, pBuffer+dwBytesTotal, dwSizeToWriteEachTime, &dwBytesWritten, NULL) || dwBytesWritten!=dwSizeToWriteEachTime)
			return FALSE;

		dwBytesTotal += dwBytesWritten;
		if(dwBytesTotal >= (DWORD)nSize)
			break;
		else if(nSize-dwBytesTotal <  dwSizeToWriteEachTime)		//剩下的字节数已经小于4K
			dwSizeToWriteEachTime = nSize-dwBytesTotal;

		::SetFilePointer(m_hFile, dwBytesTotal, NULL, FILE_BEGIN);
	} while (TRUE);

	return TRUE;
}

void CTinyImFile::Close()
{
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}


