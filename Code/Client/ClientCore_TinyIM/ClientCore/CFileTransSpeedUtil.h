

#ifndef _TINY_IM_C_FILE_TRANS_SPEED_UTIL_H_
#define _TINY_IM_C_FILE_TRANS_SPEED_UTIL_H_
#include <map>
#include <memory>
class CSpeedBase
{
public:
	CSpeedBase(const int nDisTime, const long nTransBytes);
	virtual std::string GetSpeed() const;
	virtual int GetSpeedValue() const ;
private:
	int m_nDisTime;
	long m_lTransBytes;

};
using CSpeed_Ptr = std::shared_ptr<CSpeedBase>;
//class CSpeedByte_Second:public CSpeedBase
//{
//public:
//	virtual std::string GetSpeed() const override;
//};
//
//class CSpeedKB_Second :public CSpeedBase
//{
//public:
//	virtual std::string GetSpeed() const override;
//};
//
//class CSpeedMB_Second :public CSpeedBase
//{
//public:
//	virtual std::string GetSpeed() const override;
//};


struct FileTransBytes_ST
{
public:
	FileTransBytes_ST()
	{
		m_tStartTime = 0;
		m_llTransBytes = 0;
	}
	time_t m_tStartTime;
	long long m_llTransBytes;
};

class CFileTransSpeedUtil
{
public:
	CSpeed_Ptr GetFileTransSpeed(const int nFileId);
	bool UpdateFileTrans(const int nFileId, const long nTransBytes);
	bool StartFileTrans(const int nFileId);
	bool StopFileTrans(const int nFileId);
private:
	bool FileExist(const int nFileId);
	std::map<int, FileTransBytes_ST> m_FileTransMap;
};
#endif