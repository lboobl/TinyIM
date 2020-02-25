/**
 * @file CMediumServer.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 总控类
 * @version 0.1
 * @date 2019-08-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _MEDIUM_SERVER_C_MEDIUM_SERVER_H_
#define _MEDIUM_SERVER_C_MEDIUM_SERVER_H_

#include <functional>

#include <map>

#include "CommonMsg.h"
#include "Log.h"
#include "asio_common.h"
#include "json11.hpp"

#include "CClientSess.h"
#include "CServerSess.h"
#include "CHttpServer.h"
#include "CUdpClient.h"
#include "planform.h"
#include "CMsgPersistentUtil.h"
#include "CFileUtil.h"
namespace ClientCore
{
using tcp = asio::ip::tcp;
class CClientSess;
class CMediumServer : public std::enable_shared_from_this<CMediumServer>
{
  protected:
    asio::io_service &m_ioService;

    tcp::socket m_socket;

    tcp::acceptor m_acceptor;

    //服务器的IP端口设置
    IpPortCfg m_serverCfg;
	//所有的客户端的连接客户端的IP端口设置
	std::vector<IpPortCfg> m_clientCfgVec;

	IpPortCfg m_httpCfg;
	
	IpPortCfg m_udpCfg;

	CFileUtil m_fileUtil;

    
	//std::vector<std::shared_ptr<CServerSess>> m_GuiSessList; //监听的套接字的列表
	//std::vector<std::shared_ptr<CClientSess>> m_ConnectSessList; //连接到服务器的套接字列表


	std::map<std::shared_ptr<CServerSess>, std::shared_ptr<CClientSess>> m_ForwardSessMap;
	std::map<std::shared_ptr<CClientSess>, std::shared_ptr<CServerSess>> m_BackSessMap;


	std::map<std::string, std::shared_ptr<CClientSess>> m_userId_ClientSessMap;
	std::map<std::string, std::shared_ptr<CServerSess>> m_userId_ServerSessMap;

	std::map<std::string, FriendChatRecvTxtReqMsg> m_waitImageMsgMap;
	std::map<std::string, RecvGroupTextMsgReqMsg> m_groupWaitImageMsgMap;

    std::shared_ptr<asio::high_resolution_timer> m_timer = nullptr;

	std::shared_ptr<CHttpServer> m_httpServer;
	CClientSess_SHARED_PTR m_freeClientSess;
	std::map<std::string, std::vector<std::string>> m_userFriendListMap;
	std::map<std::string, time_t> m_userKeepAliveMap;
    void SetTimer(int nSeconds);
    void OnTimer();
	void CheckWaitMsgVec();
    std::error_code m_ec;

    void do_accept();
	void OnHttpRsp(const std::shared_ptr<CClientSess>& pClientSess,std::shared_ptr<TransBaseMsg_t> pMsg);

	void HandleFileDataSendRsp(const FileDataSendRspMsg& rspMsg);

	void HandleFileDataRecvReq(const FileDataRecvReqMsg& reqMsg);

	void HandleFileVerifyReq(const FileVerifyReqMsg& msg);
	void HandleFriendNotifyFileMsgReq(const FriendNotifyFileMsgReqMsg& reqMsg);
  public:
    static std::shared_ptr<spdlog::logger> ms_loger;
    inline IpPortCfg &config() { return m_serverCfg; }

	bool HandleHttpMsg(const BaseMsg* pMsg);
    void SendBack(const std::shared_ptr<CClientSess>& pClientSess,const TransBaseMsg_t& msg);
	void SendFoward(const std::shared_ptr<CServerSess>& pServerSess,const TransBaseMsg_t& msg);

	void Handle_RecvFileOnlineRsp(const FriendRecvFileMsgRspMsg& rspMsg);
	void DispatchUdpMultiCastReciverMsg(const asio::ip::udp::endpoint endPt, TransBaseMsg_t* pMsg);
	void DispatchUdpMultiCastSenderMsg(const asio::ip::udp::endpoint endPt, TransBaseMsg_t* pMsg);
	void DispatchUdpMsg(const asio::ip::udp::endpoint endPt, TransBaseMsg_t* pMsg);
	void Handle_UdpMsg(const asio::ip::udp::endpoint endPt, const FileDataSendRspMsg& Msg);
	void Handle_UdpMsg(const asio::ip::udp::endpoint endPt, const KeepAliveRspMsg& Msg);
	void Handle_UdpMsg(const asio::ip::udp::endpoint endPt, const FileDataSendReqMsg& reqMsg);
	void Handle_UdpMsg(const asio::ip::udp::endpoint endPt, const FileDataRecvReqMsg& reqMsg);
	void Handle_UdpMsg(const asio::ip::udp::endpoint endPt, const UdpP2pStartReqMsg& reqMsg);
	void Handle_UdpMsg(const asio::ip::udp::endpoint endPt, const UdpP2pStartRspMsg& reqMsg);

	bool DoSF_FriendChatSendTxtReq(FriendChatSendTxtReqMsg& reqMsg);
	void CheckAllConnect();
	void CheckFriendP2PConnect();

	CClientSess_SHARED_PTR GetClientSess(const std::string strUserId);
	CClientSess_SHARED_PTR CreateClientSess();
	//CClientSess_SHARED_PTR CreateClientSess(const std::string strUserName);
	CUdpClient_PTR CreateUdpSess();
	CUdpClient_PTR GetUdpSess(const std::string strUserId);

	CMsgPersistentUtil_SHARED_PTR GetMsgPersisUtil(const std::string strUserId) {
		if (strUserId.empty())
		{
			return nullptr;
		}
		auto item = m_UserId_MsgPersistentUtilMap.find(strUserId);
		if (item != m_UserId_MsgPersistentUtilMap.end())
		{
			return item->second;
		}
		else
		{
			return nullptr;
		}
	}

	std::string GetUserId(const std::string strUserName);
    CMediumServer(asio::io_service &io_service)
        : m_ioService(io_service), m_socket(io_service), m_acceptor(io_service)
    {
        if (!m_timer)
        {
            m_timer = std::make_shared<asio::high_resolution_timer>(m_ioService);
        }
		m_timeCount = 0;
		m_nNoSessTimeCount = 0;
    }

	void ServerSessClose(const CServerSess_SHARED_PTR pSess);

	void start(const std::function<void(const std::error_code &)> &callback);
	void stop();
    void loadConfig(const json11::Json &cfg, std::error_code &ec);
    //获取server的ip和端口,
    std::string getServerIpPort();
	GetFriendChatHistoryRsp DoFriendChatHistoryReq(const GetFriendChatHistoryReq& reqMsg);
	GetGroupChatHistoryRsp DoFriendChatHistoryReq(const GetGroupChatHistoryReq& reqMsg);
	SearchChatHistoryRsp DoSearchChatHistoryReq(const SearchChatHistoryReq& reqMsg);
	TransBaseMsg_S_PTR DoSendBackFileDataSendRsp(const FileDataSendRspMsg& rspMsg);
private:
	std::vector<std::string> GetLocalAllIp();
	CServerSess_SHARED_PTR Get_GUI_Sess(const std::string strUserId);
	bool HandleSendBack(const std::shared_ptr<CClientSess>& pClientSess, const TransBaseMsg_t& msg);
	bool HSB_GetFriendListRsp(const std::shared_ptr<CClientSess>& pClientSess, const GetFriendListRspMsg& msg);
	void HSB_FriendChatRecvTxtReq(const std::shared_ptr<CClientSess>& pClientSess, const FriendChatRecvTxtReqMsg reqMsg);
	void HSB_FriendChatSendTxtRsp(const std::shared_ptr<CClientSess>& pClientSess, const FriendChatSendTxtRspMsg reqMsg);
	void HSB_FileSendDataBeginReq(const std::shared_ptr<CClientSess>& pClientSess, const FileSendDataBeginReq reqMsg);
	void HSB_FileSendDataBeginRsp(const std::shared_ptr<CClientSess>& pClientSess, const FileSendDataBeginRsp reqMsg);
	void HSB_UserLoginRsp(const std::shared_ptr<CClientSess>& pClientSess, const UserLoginRspMsg reqMsg);
	void HSB_UserLogoutRsp(const std::shared_ptr<CClientSess>& pClientSess, const UserLogoutRspMsg rspMsg);
	void HSB_NetFailed(const std::shared_ptr<CClientSess>& pClientSess);
	void HSB_QueryUserUdpAddrRsp(const std::shared_ptr<CClientSess>& pClientSess, const QueryUserUdpAddrRspMsg rspMsg);
	void HSB_FileDownLoadRsp(const std::shared_ptr<CClientSess>& pClientSess, const FileDownLoadRspMsg rspMsg);
	void HSB_FileDataSendRsp(const std::shared_ptr<CClientSess>& pClientSess, const FileDataSendRspMsg& rspMsg);
	void HSB_FileVerifyRsp(const std::shared_ptr<CClientSess>& pClientSess, const FileVerifyRspMsg& rspMsg);
	void HSB_FriendRecvFileMsgReq(const std::shared_ptr<CClientSess>& pClientSess, const FriendRecvFileMsgReqMsg reqMsg);
	void HSB_RecvGroupTextMsgReqMsg(const std::shared_ptr<CClientSess>& pClientSess, const RecvGroupTextMsgReqMsg& reqMsg);
	void HSB_SendGroupTextMsgRspMsg(const std::shared_ptr<CClientSess>& pClientSess, const SendGroupTextMsgRspMsg& rspMsg);
	void HSB_NotifyGroupMsgReqMsg(const std::shared_ptr<CClientSess>& pClientSess, const NotifyGroupMsgReqMsg& reqMsg);

	bool HandleSendForward(const std::shared_ptr<CServerSess>& pServerSess, const TransBaseMsg_t& msg);
	void HSF_GetFriendChatHistoryReq(const std::shared_ptr<CServerSess>& pServerSess, const GetFriendChatHistoryReq& msg);
	void HSF_GetGroupChatHistoryReq(const std::shared_ptr<CServerSess>& pServerSess, const GetGroupChatHistoryReq& msg);
	void HSF_FileSendDataBeginReq(const std::shared_ptr<CServerSess>& pServerSess,FileSendDataBeginReq& msg);
	void HSF_FriendChatSendTxtReqMsg(const std::shared_ptr<CServerSess>& pServerSess,FriendChatSendTxtReqMsg& msg);
	void HSF_UserLoginReq(const std::shared_ptr<CServerSess>& pServerSess, UserLoginReqMsg& reqMsg);
	void HSF_NetFailedReq(const std::shared_ptr<CServerSess>& pServerSess);
	void HSF_SendGroupTextMsgReqMsg(const std::shared_ptr<CServerSess>& pServerSess, SendGroupTextMsgReqMsg& reqMsg);

	void Handle_TcpMsg(const std::shared_ptr<CClientSess>& pClientSess, const FileDataRecvReqMsg& reqMsg);


	void HandleFriendChatSendTextMsgRsp(const FriendChatSendTxtRspMsg& rspMsg);
	void HandleFriendChatRecvTextMsgRsp(const FriendChatRecvTxtReqMsg& reqMsg);
	

	std::map<std::string, std::string> m_userName_UserIdMap;

	void SetUserIdUserName(const std::string strUserId, const std::string strUserName);
	std::string GetUserNameById(const std::string strUserId);
	std::string GetUserMainFolder(const std::string strUserId);
	std::string GetUserImageDir(const std::string strUserId);
	std::string GetUserFileDir(const std::string strUserId);
	std::string GetUserDataBaseFileName(const std::string strUserId);
	std::map<std::string, std::string> m_userId_UserNameMap;


	std::map<std::string, CLIENT_SESS_STATE>  m_userStateMap;
	std::map<std::string, UserLoginReqMsg> m_userLoginMsgMap;
	std::map<std::string, CUdpClient_PTR> m_userUdpSessMap;
	std::map<std::string, IpPortCfg> m_userIdUdpAddrMap;
	std::map<std::string, std::string> m_fileHashMsgIdMap;
	std::vector<std::string> m_fileHashTransVec;

	long long m_timeCount;
	int m_nNoSessTimeCount;
	std::map<std::string, CMsgPersistentUtil_SHARED_PTR> m_UserId_MsgPersistentUtilMap;
	std::map<std::string, std::vector<std::shared_ptr<BaseMsg>> > m_RecvWaitMsgMap;
	
	std::map<std::string, FriendChatSendTxtReqMsg>  m_SendWaitMsgMap;
	
	std::map<std::string, SendGroupTextMsgReqMsg>  m_groupSendWaitMsgMap;

	//
	std::map<std::string, std::string> m_AddFriendMsgIdMap;

	std::map<CClientSess_SHARED_PTR, time_t> m_reConnectSessMap;
};
} // namespace MediumServer

#endif // HTTP2T1_CAGENTSERVER_H
