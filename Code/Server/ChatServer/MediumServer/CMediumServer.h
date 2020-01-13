/**
 * @file CMediumServer.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief ChatServer的核心处理逻辑
 * @version 0.1
 * @date 2019-07-19
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _MEDIUM_SERVER_C_MEDIUM_SERVER_H_
#define _MEDIUM_SERVER_C_MEDIUM_SERVER_H_

#include <functional>

#include <map>
#include <string>
#include "CommonMsg.h"
#include "Log.h"
#include "asio_common.h"
#include "json11.hpp"

#include "CClientSess.h"
#include "CServerSess.h"
#include "CUdpServer.h"
#include "planform.h"
#include "CMySqlConnect.h"
#include "SnowFlake.h"
#include "CFileUtil.h"

struct SendFileInfo_st
{
	std::string m_strUserId;
	int         m_nFileId;
	time_t      m_lastPackageTime;
};
using FILE_ID_RSP_MSG_MAP = std::map<int, FileDataSendRspMsg>;
using USER_FILE_DATA_RSP_MAP=std::map<std::string, FILE_ID_RSP_MSG_MAP>;
namespace ChatServer
{

using tcp = asio::ip::tcp;

class CChatServer : public std::enable_shared_from_this<CChatServer>
{
public:
	void DispatchRecvTcpMsg(const std::shared_ptr<CServerSess> pSess, const TransBaseMsg_t* pMsg);
	void OnSessClose(const std::shared_ptr<CServerSess>& pSess);
	
	
    //void HandleUserUnRegisterReq(const std::shared_ptr<CServerSess>& pSess, const  & reqMsg);
	//void HandleUserLoginReq(const std::shared_ptr<CServerSess>& pSess, const UserLoginReqMsg& reqMsg);
	//void HandleUserLoginReq(const std::shared_ptr<CServerSess>& pSess, const UserLoginReqMsg& reqMsg);
	//void HandleUserLoginReq(const std::shared_ptr<CServerSess>& pSess, const UserLoginReqMsg& reqMsg);
	
    UserLoginRspMsg DoUserLoginReq(const UserLoginReqMsg& reqMsg);
	UserLogoutRspMsg DoUserLogoutReq(const UserLogoutReqMsg& reqMsg);
	UserRegisterRspMsg DoUserRegisterReq(const UserRegisterReqMsg& reqMsg);
	UserUnRegisterRspMsg DoUserUnRegisterReq(const UserUnRegisterReqMsg& reqMsg);
	FriendChatSendTxtRspMsg DoFriendChatSendTxtReq(const FriendChatSendTxtReqMsg& reqMsg);
	AddFriendSendRspMsg DoAddFriendReq(const AddFriendSendReqMsg& reqMsg);

	GetFriendListRspMsg DoGetFriendReq(const GetFriendListReqMsg & reqMsg);

	RemoveFriendRspMsg DoRemoveFriendReq(const RemoveFriendReqMsg& reqMsg);
	FindFriendRspMsg   DoFindFriendReq(const FindFriendReqMsg& reqMsg);
	AddTeamRspMsg DoAddTeamReq(const AddTeamReqMsg& reqMsg);
	RemoveTeamRspMsg DoRemoveTeamReq(const RemoveTeamReqMsg& reqMsg);
	MoveFriendToTeamRspMsg DoMoveFriendToTeamReq(const MoveFriendToTeamReqMsg& reqMsg);

	CreateGroupRspMsg DoCreateGroupReq(const CreateGroupReqMsg& reqMsg);

	DestroyGroupRspMsg DoDestroyGroupReq(const DestroyGroupReqMsg& reqMsg);

	GetGroupListRspMsg DoGetGroupListReq(const GetGroupListReqMsg& reqMsg);

	FindGroupRspMsg DoFindGroupReq(const FindGroupReqMsg& reqMsg);

	SendGroupTextMsgRspMsg DoSendGroupTextMsgReqMsg(const SendGroupTextMsgReqMsg& reqMsg);

	AddToGroupRspMsg DoAddToGroupReqMsg(const AddToGroupReqMsg& reqMsg);
	QuitFromGroupRspMsg DoQuitFromGroup(const QuitFromGroupReqMsg& reqMsg);
	TransBaseMsg_S_PTR DoFileDataRecvRsp(const FileDataRecvRspMsg& rspMsg);
	GetRandomUserRspMsg DoGetRandomUserReqMsg(const GetRandomUserReqMsg& reqMsg);
	FileDataSendRspMsg DoFileDataSendReq(const FileDataSendReqMsg& reqMsg);

	void Handle_UdpFileDataSendReqMsg(const asio::ip::udp::endpoint sendPt,const FileDataSendReqMsg& reqMsg);

	//
	
  protected:
    //asio的主循环类
    asio::io_service &m_ioService;

    //用于接收连接的socket
    tcp::socket m_socket;

    //用于监听的socket
    tcp::acceptor m_acceptor;

    //服务器的IP端口设置
    IpPortCfg m_serverCfg;
    
    //数据库操作类
    CMySqlConnect m_util;
    
    //所有的客户端的连接客户端的IP端口设置
    std::vector<IpPortCfg> m_clientCfgVec;
	MySqlCfg m_dbCfg;
    std::vector<std::shared_ptr<CServerSess>> m_listenList; //监听的套接字的列表

    //定时器
    std::shared_ptr<asio::high_resolution_timer> m_timer = nullptr;
	CFileUtil m_fileUtil;
    void SetTimer(int nSeconds);
    
    void OnTimer();
    
    std::error_code m_ec;

    void do_accept();

	bool OnUserReceiveMsg(const std::string strUser);

	bool OnAddFriendRecvReqMsg(const std::string strUser);

	bool OnAddFriendNotifyReqMsg(const std::string strUser);

	bool OnUserRecvGroupMsg(const std::string strUser);

	bool OnUserRecvGroupMsg(const std::string strUser,const std::string strMsgId, const std::string strGroupId, const std::string strSender, const std::string strMsg);
	bool OnUserRecvGroupMsg(const std::string strUser,const T_GROUP_CHAT_MSG& msg);
	void OnDispatchGroupMsg(const std::string strGroupId);

	void OnUserStateCheck(const std::string strUserId);
	void NotifyUserFriends(const std::string strUserId);


	void CloseUserFile(const std::string strUserId);


private:
	const std::size_t HASH_SALT_LENGTH = 32;//密码的哈希盐值的长度
	std::string CreateMsgId();
	std::string GenerateSalt();
	std::string GetSaltFromPasswd(const std::string strPasswd);
	std::string GeneratePassword(const std::string orgPassWord);
	bool VerifyPassword(const std::string orgPassword, const std::string dataBasePassword);
   
    SnowFlake m_MsgID_Util; //消息的唯一生成器
	
  
    std::map<std::string, std::shared_ptr<CServerSess>> m_UserSessVec;  //保存UserId和Sess的对应关系
	std::map<std::string, CLIENT_SESS_STATE> m_clientStateMap;          //保存客户端的状态

	std::map<std::string, std::shared_ptr<CServerSess>> m_KickOffSessMap;  //保存UserId和Sess的对应关系

	std::shared_ptr<CUdpServer> m_udpServer;
	std::map<std::string, IpPortCfg> m_userIdUdpAddrMap;
	std::map<int, FILE_TRANS_TYPE> m_fileTranModeMap;
	std::string GenerateUserId();

	void HandleReLogin(std::string strUserId, std::shared_ptr<CServerSess> pSess);

	void DispatchRecvUdpMsg(const asio::ip::udp::endpoint sendPt, const TransBaseMsg_t* pMsg);
	void Handle_RecvUdpMsg(const asio::ip::udp::endpoint sendPt, const FileDataRecvRspMsg& pMsg);
	void Handle_RecvUdpMsg(const asio::ip::udp::endpoint sendPt, const UdpP2pStartReqMsg& pMsg);
	void Handle_RecvUdpMsg(const asio::ip::udp::endpoint sendPt, const KeepAliveReqMsg& pMsg);

	void CheckFileDataRsp(const std::string strUserId);

	void CheckFileVerifyReq(const FileVerifyReqMsg& reqMsg);

	void SaveFileDataRsp(const FileDataSendRspMsg& rspMsg);
private:
	USER_FILE_DATA_RSP_MAP m_fileDataRspMap;
	std::vector<std::string> m_strSendFileHashVec;
	std::vector<std::string> m_strRecvFileHashVec;//从客户端上来的Hash的数组
	bool IsFileRecving(const std::string strFileHash);//文件是否在接收状态
	bool SaveRecvingState(const std::string strFileHash);
	bool RemoveRecvingState(const std::string strFileHash);

	bool IsFileSending(const std::string strFileHash);//文件是否在发送状态
	bool SaveSendingState(const std::string strFileHash);
	bool RemoveSendingState(const std::string strFileHash);

	std::string GetFilePathByUserIdAndFileName(const std::string strUserId, const std::string strFileName);
	std::string GetFolderByUserId(const std::string strUserId);

	void HandleUserKeepAliveReq(const std::shared_ptr<CServerSess> pSess);
	void HandleUserKeepAliveReq(const std::shared_ptr<CServerSess> pSess, const KeepAliveReqMsg& reqMsg);
	void HandleUserKeepAliveRsp(const std::shared_ptr<CServerSess> pSess, const KeepAliveRspMsg& rspMsg);
	void HandleUserLoginReq(const std::shared_ptr<CServerSess>& pSess, const UserLoginReqMsg& reqMsg);


	void HandleUserLogoutReq(const std::shared_ptr<CServerSess>& pSess, const UserLogoutReqMsg& reqMsg);

	void HandleUserRegisterReq(const std::shared_ptr<CServerSess>& pSess, const UserRegisterReqMsg& reqMsg);
	void HandleUserUnRegisterReq(const std::shared_ptr<CServerSess>& pSess, const UserUnRegisterReqMsg& reqMsg);

	void HandleFriendChatSendTxtReq(const std::shared_ptr<CServerSess>& pSess, const FriendChatSendTxtReqMsg& reqMsg);
	void HandleFriendChatRecvMsgReq(const std::shared_ptr<CServerSess>& pSess, const FriendChatRecvTxtReqMsg& regMsg);
	void HandleFriendChatRecvMsgRsp(const std::shared_ptr<CServerSess>& pSess, const FriendChatRecvTxtRspMsg& regMsg);

	void HandleAddFriendReq(const std::shared_ptr<CServerSess>& pSess, const AddFriendSendReqMsg& regMsg);

	void HandleRemoveFriendReq(const std::shared_ptr<CServerSess>& pSess, const RemoveFriendReqMsg& rspMsg);

	void HandleFindFriendReq(const std::shared_ptr<CServerSess>& pSess, const FindFriendReqMsg& reqMsg);
	void HandleGetFriendListReq(const std::shared_ptr<CServerSess>& pSess, const GetFriendListReqMsg& reqMsg);


	void HandleAddTeamReq(const std::shared_ptr<CServerSess>& pSess, const AddTeamReqMsg& reqMsg);
	void HandleRemoveTeamReq(const std::shared_ptr<CServerSess>& pSess, const RemoveTeamReqMsg& reqMsg);
	void HandleMoveFriendToTeamReq(const std::shared_ptr<CServerSess>& pSess, const MoveFriendToTeamReqMsg& reqMsg);


	void HandleFriendSendFileReq(const std::shared_ptr<CServerSess>& pSess, const FriendSendFileMsgReqMsg& reqMsg);

	void HandleFriendNotifyFileRsp(const std::shared_ptr<CServerSess>& pSess, const FriendNotifyFileMsgRspMsg& req);

	void HandleFileSendDataBeginReq(const std::shared_ptr<CServerSess>& pSess, const FileSendDataBeginReq& req);
	void HandleFileSendDataBeginRsp(const std::shared_ptr<CServerSess>& pSess, const FileSendDataBeginRsp& req);

	void HandleFileDownLoadReq(const std::shared_ptr<CServerSess>& pSess, const FileDownLoadReqMsg& req);

	void HandleFileVerifyReq(const std::shared_ptr<CServerSess>& pSess, const FileVerifyReqMsg& req);


	void HandleGetRandomUserReq(const std::shared_ptr<CServerSess>& pSess, const GetRandomUserReqMsg& req);
	//Group
	void HandleCreateGroupReq(const std::shared_ptr<CServerSess>& pSess, const CreateGroupReqMsg& reqMsg);

	void HandleDestroyGroupReq(const std::shared_ptr<CServerSess>& pSess, const DestroyGroupReqMsg& reqMsg);

	void HandleGetGroupListReq(const std::shared_ptr<CServerSess>& pSess, const GetGroupListReqMsg& reqMsg);

	void HandleAddToGroupReq(const std::shared_ptr<CServerSess>& pSess, const AddToGroupReqMsg& reqMsg);

	void HandleFindGroupReq(const std::shared_ptr<CServerSess>& pSess, const FindGroupReqMsg& reqMsg);

	void HandleSendGroupTextReq(const std::shared_ptr<CServerSess>& pSess, const SendGroupTextMsgReqMsg& reqMsg);

	void HandleRecvGroupTextMsgRspMsg(const std::shared_ptr<CServerSess>& pSess, const RecvGroupTextMsgRspMsg& reqMsg);
	void HandleQuitGroupReqMsg(const std::shared_ptr<CServerSess>& pSess, const QuitFromGroupReqMsg& reqMsg);
	void HandleFriendUnReadNotifyRspMsg(const std::shared_ptr<CServerSess>& pSess, const FriendUnReadNotifyRspMsg& rspMsg);
	void HandleQueryUserUdpAddr(const std::shared_ptr<CServerSess>& pSess, const QueryUserUdpAddrReqMsg& reqMsg);
	void HandleFileDataSendReq(const std::shared_ptr<CServerSess>& pSess, const FileDataSendReqMsg& reqMsg);
	void HandleFileDataRecvRsp(const std::shared_ptr<CServerSess>& pSess, const FileDataRecvRspMsg& rspMsg);


	void HandleFriendRecvFileRsp(const FriendRecvFileMsgRspMsg& reqMsg);
	void HandleFileVerifyRsp(const FileVerifyRspMsg& req);
	void HandleFileDataRecvRspMsg(const std::shared_ptr<CServerSess>& pSess, const FileDataRecvRspMsg& reqMsg);
	void HandleUpdateFriendListRsp(const UpdateFriendListNotifyRspMsg& reqMsg);

	void HandleUpdateGroupListRsp( const UpdateGroupListNotifyRspMsg& reqMsg);
	void HandleAddFriendRecvRsp(const AddFriendRecvRspMsg& rspMsg);
	void HandleAddFriendNotifyRsp(const AddFriendNotifyRspMsg& rspMsg);
	void HandleUserKickOffRsp(const UserKickOffRspMsg& reqMsg);



public:

    static std::shared_ptr<spdlog::logger> ms_loger; //日志指针
    inline IpPortCfg &config() { return m_serverCfg; }

    void SendBack(const TransBaseMsg_t *msg);
    void CheckAllConnect();
	CChatServer(asio::io_service &io_service);

	void start(const std::function<void(const std::error_code &)> &callback);
	
    void loadConfig(const json11::Json &cfg, std::error_code &ec);
    //获取server的ip和端口,
    std::string getServerIpPort();
};
} // namespace MediumServer

#endif // HTTP2T1_CAGENTSERVER_H
