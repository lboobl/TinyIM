/**
 * @file Proto.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 用于处理UI和网络协议的交互
 * @version 0.1
 * @date 2019-07-28
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _DENNIS_THINK_IM_PROTO_H_
#define _DENNIS_THINK_IM_PROTO_H_
#include <string>
#include <memory>
#include "CommonDef.h"
#include "CommonMsg.h"
//#include "asioNet/CSessManager.h"
#include <Windows.h>
#include <map>
#include "UserMgr.h"
#include "net/IUProtocolData.h"
#include "UICommonDef.h"
#include "IULog.h"
#include "UI_USER_INFO.h"
using ID_HANDLE_MAP = std::map<E_MsgType, HWND>;
//历史消息记录使用
struct LogMsgPair {
	std::string m_strFirstMsgId;
	std::string m_strLastMsgId;
};
ChatMsgElemVec UiToCore(const RichEditMsgList& richMsg);
RichEditMsgList CoreToUi(const ChatMsgElemVec& coreMsgVec);
class CMsgProto:public std::enable_shared_from_this<CMsgProto>
{
protected:
	CMsgProto();

	void Init();

public:
	static void Initialize();
	static void UnInstialize();
	static std::shared_ptr<CMsgProto> GetInstance();
	static std::shared_ptr<spdlog::logger> ms_loger;
	void SetIpPort(const std::string strIp, const int port);
	void StartConnect();
	UserRegisterRspMsg SendRegisterSync(const std::string strUserName, const std::string strPassword, const std::string strNickName, const int nSeconds = 3);
	
	UserLoginRspMsg SendLoginSync(const std::string strUserName, const std::string strPassword, const int nSeconds = 5);
	
	bool OnMsgBack(const TransBaseMsg_t* pMsg);

	void OnTimeOut();
	/**
	 * @brief 增加消息类型和窗口句柄的对应关系，以便使用PostMessage进行分发
	 * 
	 * @param type 消息类型
	 * @param winHandle 窗口的句柄
	 */
	void AddMap(const E_MsgType type, const HWND winHandle) {
		EraseMap(type);
		m_msgMap.insert(std::pair<E_MsgType, HWND>(type, winHandle));
	}

	//UI Begin
	E_UI_ONLINE_STATUS GetStatus();
	//UI End


	//Core Begin
	bool SendUserRegister(const std::string strUserName, const std::string strPassword, const std::string strNickName);
	bool SendUserLogin(const std::string strUserName, const std::string strPassword);
	bool SendUserLogout();
	bool SendFindFriendAsnyc(const std::string strUserName, const std::string friendName, HWND winHandle);
	bool SendAddFriendAsnyc(const std::string strUserName, const std::string friendName, HWND winHandle);
	bool SendAddFriendRecvAsnyc(const std::string friendName,const std::string msgId,const E_UI_OPERATE_FRIEND option);
	bool SendAddFriendNotifyRsp(const std::string strMsgId);
	bool SendChatTxtMsg(const std::string strFriendName, const std::string strContext,const C_UI_FontInfo font=C_UI_FontInfo());
	bool SendChatTxtMsg(const std::string strFriendName, RichEditMsgList msgList, const C_UI_FontInfo font);

	bool SendRecvChatTxtRsp(const std::string strUserName,const std::string strFriendName,const std::string strMsgId);
	bool GetFriendList();
	
	bool SendRemoveFriend(const std::string strFriendId);
	bool SendFriendOnLineFileP2PMode(const std::string strFriendId, std::string strFileName);
	bool SendFriendOnLineFileMediumTransMode(const std::string strFriendId, std::string strFileName);
	bool SendFriendOffLineFile(const std::string strFriendId, std::string strFileName);
	bool SendFriendRecvFileRsp(const C_WND_MSG_FileRecvReq& reqMsg, const E_FRIEND_OPTION eOption);
	bool SendFileDataBeginReq(const std::string strFriendId, const std::string strFileName);
	//bool SendFriendOffLineFile(const std::string strFileName);

	bool SendFriendNotifyFileRsp(const C_WND_MSG_FileNotifyReq& reqMsg);

	bool SendAddTeamReq(const std::string strTeamName);
	bool SendRemoveTeamReq(const std::string strTeamId);
	bool MoveFriendToTeam(const std::string strUserName, const std::string strFriendName, const std::string strSrcTeamId, const std::string strDstTeamId);
	bool GetChatHistoryReq(const std::string strFriendId, const std::string strChatMsgId, const HISTORY_DIRECTION eDirection);
	//Group
	bool SendCreateGroupReq(const std::string strUserName, const std::string strGroupName);
	
	bool SendDestroyGroupReq(const std::string strUserName, const std::string strGroupId);

	bool SendGetGroupList();
	bool SendGroupChatTextMsg(const std::string strGroupId, RichEditMsgList msgList, const C_UI_FontInfo font);
	
	

	bool SendFindGroupReq(const std::string strGroupId);

	bool SendAddToGroupReq(const std::string strGroupId);

	bool SendQuitFromGroupReq(const std::string strGroupId);

	bool GetGroupHistoryReq(const std::string strGroupId, const HISTORY_DIRECTION eDirection);
	//Core End

	CGroupList* GetGroupList() {
		return &m_GroupList;
	}

	C_UI_BuddyList m_BuddyList;//好友列表
	CGroupList m_GroupList;//群组列表
	C_WND_MSG_LoginResult m_loginResult;//用户登录结果
	std::string UserName() const { return m_strUserName; }
	std::string UserId() const { return m_strUserId;  }
	bool SendKeepAliveReq();


	//Friend Info Begin
	UserBaseInfo GetFriendInfoById(const std::string strFriendId);
	//Friend Info End

	CBuddyChatUiMsgVector GetBuddyMsgList(const std::string strFriendId);
	bool IsFriend(const std::string strFriendId);
private:
	int m_startTime;//某个操作的开始时间
	std::string m_strRecvMsgId;
	std::map<std::string, UserBaseInfo> m_friendInfoMap;//好友ID和好友信息的对应关系
private:


	void EraseMap(const E_MsgType type) {
		m_msgMap.erase(type);
	}
	void HandleMsg(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);

	void HandleUserRegisterRsp(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	void HandleUserLoginRsp(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	void HandleFriendUnReadNotifyReq(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	void HandleUserKickOffReq(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	void HandleFindFriendRsp(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	void HandleGetFriendListRsp(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	void HandleAddFriendRecvReq(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	void HandleAddFriendNotifyReq(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	void HandleSendChatTxtRsp(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	void HandleRecvChatTxtReq(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	void HandleNetRecoverReportMsg(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	void HandleNetFailedMsg(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);

	void HandleFileTransProcessNotifyReq(const FileTransProgressNotifyReqMsg& reqMsg);
	
	void HandleCreateGroupRspMsg(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);

	void HandleDestroyGroupRspMsg(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);

	void HandleGetGroupListRspMsg(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	
	void HandleFindGroupRsp(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);

	void HandleLogoutRspMsg(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);

	void HandleSendGroupTextRspMsg(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);

	void HandleAddToGroupRspMsg(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);

	void HandleRecvGroupTextMsgReq(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);

	void HandleFriendSendFileRsp(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);

	void HandleFriendRecvFileReq(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	
	void HandleFriendNotifyFileReq(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	
	void HandleFileSendDataRsp(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	void HandleUpdateFriendListNotifyReq(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	
	void HandleGetFriendChatHistory(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);

	void HandleGetGroupChatHistory(const std::shared_ptr<TransBaseMsg_t> pOrgMsg);
	ID_HANDLE_MAP m_msgMap;      //消息类型和窗口句柄的对应关系表,收到消息的时候进行PostMessage

	bool m_bRecvRegisterRsp;     //是否收到注册消息回复
	UserRegisterRspMsg m_userRegisterRsp; //收到注册消息回复

	bool m_bRecLoginRsp;  //是否收到登录消息回复
	UserLoginRspMsg m_userLoginRsp;//收到登录消息回复

	std::string m_strUserId;   //用户ID
	std::string m_strUserName; //用户名
	std::string m_strPassword; //用户密码(重新登录时使用)

	//CoreMsg To UI
	CBuddyChatUiMsg CoreMsgToUiMsg(FriendChatMsg_s reqMsg);
	//CBuddyChatUiMsg CoreMsgToUiMsg(FriendChatSendTxtRspMsg rspMsg);
	std::string GetFriendName(const std::string strFriendId);
private:
	std::vector<std::string> m_friendIdVec;
	E_UI_ONLINE_STATUS m_eOnLineStatus;//用户在线状态
	std::map<std::string, CBuddyChatUiMsgVector> m_friendMsgMap;
	std::map<std::string, LogMsgPair> m_friendChatLogMap;
};

#endif