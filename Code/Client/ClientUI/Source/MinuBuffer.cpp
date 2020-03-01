/**
 * @file MinuBuffer.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief MiniBuffer的实现类,考虑后期将这个类优化掉
 * TODO: 优化掉这个类
 * @version 0.1
 * @date 2020-03-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "stdafx.h"
#include "MiniBuffer.h"
#include "Utils.h"

CMiniBuffer::CMiniBuffer(int64_t nSize, BOOL bAutoRelease)
{
	//assert(nSize > 0);
	
	m_nSize = nSize;
	m_bAutoRelease = bAutoRelease;

	m_pData = new char[(int)nSize];
	memset(m_pData, 0, (size_t)nSize);
}

CMiniBuffer::~CMiniBuffer()
{
	if(m_bAutoRelease)
		DEL_ARR(m_pData);
}

void CMiniBuffer::Release()
{
	DEL_ARR(m_pData);
	m_nSize = 0;
}

int64_t CMiniBuffer::GetSize()
{
	return m_nSize;
}

char* CMiniBuffer::GetBuffer()
{
	return m_pData;
}

CMiniBuffer::operator PSTR()
{
	return m_pData;
}
	
void CMiniBuffer::EnableAutoRelease(BOOL bAutoRelease)
{
	m_bAutoRelease = bAutoRelease;
}

BOOL CMiniBuffer::IsAutoRelease()
{
	return m_bAutoRelease;
}