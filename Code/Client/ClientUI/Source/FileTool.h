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
#ifndef _TINY_IM_C_FILE_2_H_ 
#define _TINY_IM_C_FILE_2_H_

#include <stdint.h>
#include "UICommonDef.h"

class CTinyImFile
{
public:
	CTinyImFile();
	~CTinyImFile();

	BOOL Open(LPCTSTR lpszFileName, BOOL bCreateAlways = TRUE);
	long GetSize();
	
	const char* Read();
	BOOL Write(const char* pBuffer, long nSize);
	HANDLE GetFileHandle(){return m_hFile;}

	void Close();

private:
	HANDLE	m_hFile;
};

#endif