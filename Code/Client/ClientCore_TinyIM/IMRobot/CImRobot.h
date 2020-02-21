
/**
 * @file CImRobot.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief ImRobot的头文件
 * @version 0.1
 * @date 2019-12-18
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _DENNIS_THINK_C_IM_ROBOT_H_
#define _DENNIS_THINK_C_IM_ROBOT_H_
#include <string>
#include <vector>
class CIMRobot
{
public:
	CIMRobot();
	virtual ~CIMRobot();
	bool loadConfig(const std::string strCfg);
	void Run();
private:
	void GetVersion();
	void RegisterUser();
	void UserLogin();
	void UserLogout();

	//Friend Begin
	void GetFriendList();
	void SendMsg();
	void SendFaceMsg();
	void SendSingleImageMsg();
	void GetRecvMsg();

	void GetFriendHistroyFirst();
	void GetFriendHistoryLast();
	void GetFriendHistoryPrev();
	void GetFriendHistoryNext();

	void AddFriendTeam();
	void RemoveFriendTeam();
	void MoveFriendToTeam();

	void GetRandomUserName();

	void SendFindFriendReq();
	void SendAddFriendReq();
	void GetRecvAddFriendReq();
	void GetAddFriendNotify();

	void SendFriendFile();
	void GetRecvFriendFileReq();
	void GetFriendFileNotify();
	//Friend End

	//Group Begin
	void GetGroupList();
	void SendGroupMsg();
	void SendGroupFace();
	void SendGroupImage();
	void GetRecvGroupMsg();


	void GetGroupHistroyFirst();
	void GetGroupHistoryLast();
	void GetGroupHistoryPrev();
	void GetGroupHistoryNext();

	void SearchChatMsg();
	//Group End
	//Load From Client
	std::string m_strHttpIp;
	int m_nHttpPort;
	std::string m_strUserName;
	std::string m_strPassWord;

	//Http Data
	std::string m_strUserId;
	std::vector<std::string> m_strFriendVec;
	std::vector<std::string> m_strGroupVec;
	
	std::string m_strFriendName;
	std::string m_strFriendId;
};
#endif