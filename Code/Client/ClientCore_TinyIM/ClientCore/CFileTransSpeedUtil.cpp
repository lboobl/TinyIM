#include "CFileTransSpeedUtil.h"
#include <time.h>
#include <string>
CSpeedBase::CSpeedBase(const int nDisTime, const long nTransBytes)
{
	if (nDisTime == 0)
	{
		m_nDisTime = nDisTime;
		m_nDisTime = 1;
	}

	m_lTransBytes = nTransBytes;

}


int CSpeedBase::GetSpeedValue() const
{
	return m_lTransBytes / m_nDisTime;
}
std::string CSpeedBase::GetSpeed() const
{
	double dSpeed = double(m_lTransBytes) / double(m_nDisTime);
	return std::to_string(dSpeed)+" kb/s";
}

CSpeed_Ptr CFileTransSpeedUtil::GetFileTransSpeed(const int nFileId)
{
	if (FileExist(nFileId))
	{
		int nDisTime = static_cast<int>(time(nullptr)-m_FileTransMap[nFileId].m_tStartTime);
		long nTransBytes = static_cast<long>(m_FileTransMap[nFileId].m_llTransBytes);
		auto result = std::make_shared<CSpeedBase>(nDisTime,nTransBytes);
		return result;
	}
	else
	{
		auto result = std::make_shared<CSpeedBase>(1, 0);
		return result;
	}
}

bool CFileTransSpeedUtil::UpdateFileTrans(const int nFileId, const long nTransBytes)
{
	if (FileExist(nFileId))
	{
		m_FileTransMap[nFileId].m_llTransBytes += nTransBytes;
		return true;
	}
	else
	{
		return false;
	}
}

bool CFileTransSpeedUtil::StartFileTrans(const int nFileId)
{
	if (FileExist(nFileId))
	{
		return false;
	}
	else
	{
		FileTransBytes_ST value;
		value.m_tStartTime = time(nullptr);
		value.m_llTransBytes = 0;
		m_FileTransMap.insert({ nFileId,value });
		return true;
	}
}

bool CFileTransSpeedUtil::StopFileTrans(const int nFileId)
{
	if (FileExist(nFileId))
	{
		m_FileTransMap.erase(nFileId);
		return true;
	}
	else
	{
		return false;
	}
}


bool CFileTransSpeedUtil::FileExist(const int nFileId)
{
	if (m_FileTransMap.empty())
	{
		return false;
	}
	auto item = m_FileTransMap.find(nFileId);
	return item == m_FileTransMap.end();
}