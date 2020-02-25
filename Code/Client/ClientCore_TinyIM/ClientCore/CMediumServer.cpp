/**
 * @file CMediumServer.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 
 * @version 0.1
 * @date 2019-09-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "CMediumServer.h"
#include "json11.hpp"
#include "DaemonSvcApp.h"
#include "Msg.h"
#include "UiMsg.h"
#include "IUProtocolData.h"
#include "CNetUtil.h"
namespace ClientCore
{
std::shared_ptr<spdlog::logger> CMediumServer::ms_loger;

/**
 * @brief 加载配置文件
 * 
 * @param cfg 配置文件的json
 * @param ec 发生的异常
 */
void CMediumServer::loadConfig(const json11::Json &cfg, std::error_code& ec) 
{
	auto serverCfg = cfg["server"];
	LOG_INFO(ms_loger,"loadConfig [{} {}]",__FILENAME__,__LINE__);
    ec.clear();
    m_serverCfg.m_strServerIp=serverCfg["ip"].string_value();
    m_serverCfg.m_nPort=(uint16_t)serverCfg["port"].int_value();
    LOG_INFO(ms_loger,"TCP BIND: {} [{} {}]",m_serverCfg.to_string(),__FILENAME__, __LINE__);
    if(!m_serverCfg.Valid())
	{
		LOG_ERR(ms_loger,"Config Error {} [{} {}]", m_serverCfg.to_string(), __FILENAME__, __LINE__);
        return;
    }
	{
		auto clientsCfg = cfg["clients"];
		if (!clientsCfg.is_array())
		{
			LOG_ERR(ms_loger, "Clients Config Error {}", cfg.string_value());
			return;
		}
		for (auto item : clientsCfg.array_items())
		{
			IpPortCfg clientCfg;
			clientCfg.m_strServerIp = item["ip"].string_value();
			clientCfg.m_nPort = item["port"].int_value();
			LOG_INFO(ms_loger, "TCP Connect: {} [{} {}]", clientCfg.to_string(),__FILENAME__, __LINE__);
			m_clientCfgVec.push_back(clientCfg);
		}
	}

	{
		auto httpCfg = cfg["httpserver"];
		m_httpCfg.m_strServerIp = httpCfg["ip"].string_value();
		m_httpCfg.m_nPort = httpCfg["port"].int_value();
		LOG_INFO(ms_loger, "HTTP BIND:{} [{} {}]", m_httpCfg.to_string(), __FILENAME__, __LINE__);
	}

	{
		auto udpJson = cfg["UdpServer"];
		m_udpCfg.m_strServerIp = udpJson["ip"].string_value();
		m_udpCfg.m_nPort = udpJson["port"].int_value();
		LOG_INFO(ms_loger, "UDP CONNECT:{} [{} {}]", m_udpCfg.to_string(), __FILENAME__, __LINE__);
	}
}

/**
 * @brief UDP消息处理,处理P2P开始的回复消息
 * 
 * @param endPt 远端的UDP地址
 * @param reqMsg P2P开始回复消息
 */
void CMediumServer::Handle_UdpMsg(const asio::ip::udp::endpoint /*endPt*/, const UdpP2pStartRspMsg& reqMsg)
{
	if (m_userKeepAliveMap.find(reqMsg.m_strUserId) != m_userKeepAliveMap.end())
	{
		if (reqMsg.m_strUserId == reqMsg.m_strFriendId)
		{
			LOG_INFO(ms_loger, "User:{} UDP Server Connect Succeed [{} {}]", reqMsg.m_strUserId, __FILENAME__, __LINE__);
		}
		else
		{
			LOG_INFO(ms_loger, "User:{} Friend:{} UDP P2P OK [{} {}]", reqMsg.m_strUserId, reqMsg.m_strFriendId, __FILENAME__, __LINE__);
		}
	}
	else
	{
		m_userKeepAliveMap.insert({ reqMsg.m_strUserId,time(nullptr) });
	}
}


/**
 * @brief UDP消息处理,处理收到的P2P开始消息
 * 
 * @param endPt 发送UDP消息的地址
 * @param reqMsg P2P开始消息
 */
void CMediumServer::Handle_UdpMsg(const asio::ip::udp::endpoint endPt, const UdpP2pStartReqMsg& reqMsg)
{
	UdpP2pStartRspMsg rspMsg;
	rspMsg.m_strMsgId = reqMsg.m_strMsgId;
	rspMsg.m_strUserId = reqMsg.m_strFriendId;
	rspMsg.m_strFriendId = reqMsg.m_strUserId;
	auto pUdpSess = GetUdpSess(reqMsg.m_strFriendId);
	if (pUdpSess)
	{
		pUdpSess->send_msg(endPt,&rspMsg);
		pUdpSess->DoSend();
	}
}

/**
 * @brief UDP消息处理,处理收到的KeepAlive消息
 * 
 * @param endPt 发送UDP消息的远端地址
 * @param Msg 心跳回复消息
 */
void CMediumServer::Handle_UdpMsg(const asio::ip::udp::endpoint endPt, const KeepAliveRspMsg& Msg)
{
	//if (m_userKeepAliveMap.find(Msg.m_strClientId) == m_userKeepAliveMap.end())
	{
		UdpP2pStartReqMsg reqMsg;
		reqMsg.m_strMsgId = m_httpServer->GenerateMsgId();
		reqMsg.m_strUserId = Msg.m_strClientId;
		reqMsg.m_strFriendId = Msg.m_strClientId;
		auto pUdpSess = GetUdpSess(Msg.m_strClientId);
		if (pUdpSess)
		{
			pUdpSess->send_msg(endPt, &reqMsg);
			pUdpSess->DoSend();
		}
		else
		{
			LOG_ERR(ms_loger, "No Udp Sess For {} [{} {}]", Msg.m_strClientId, __FILENAME__, __LINE__);
		}
	}
}

/**
 * @brief 处理从UDP收到的文件数据发送请求消息(点对点的方式发送文件数据)
 * 
 * @param endPt UDP消息的发送者地址
 * @param reqMsg 文件数据发送请求消息
 */
void CMediumServer::Handle_UdpMsg(const asio::ip::udp::endpoint endPt, const FileDataSendReqMsg& reqMsg)
{
	if (reqMsg.m_nDataIndex <= reqMsg.m_nDataTotalCount)
	{
		m_fileUtil.OnWriteData(reqMsg.m_nFileId + 1, reqMsg.m_szData, reqMsg.m_nDataLength);
		FileDataSendRspMsg rspMsg;
		rspMsg.m_strMsgId = reqMsg.m_strMsgId;
		rspMsg.m_nFileId = reqMsg.m_nFileId;
		rspMsg.m_strUserId = reqMsg.m_strUserId;
		rspMsg.m_strFriendId = reqMsg.m_strFriendId;
		rspMsg.m_nDataTotalCount = reqMsg.m_nDataTotalCount;
		rspMsg.m_nDataIndex = reqMsg.m_nDataIndex;
		auto pSess = GetUdpSess(reqMsg.m_strUserId);
		if (pSess)
		{
			pSess->send_msg(endPt, &rspMsg);
			pSess->DoSend();
		}
		

		//文件接收进度
		{
			{
				FileTransProgressNotifyReqMsg notifyMsg;
				notifyMsg.m_strMsgId = m_httpServer->GenerateMsgId();
				notifyMsg.m_strUserId = reqMsg.m_strFriendId;
				notifyMsg.m_strFileName = m_fileUtil.GetFileName(reqMsg.m_nFileId);
				notifyMsg.m_strOtherId = reqMsg.m_strUserId;
				notifyMsg.m_eDirection = FILE_TRANS_DIRECTION::E_RECV_FILE;
				if (reqMsg.m_nDataTotalCount != 0)
				{
					notifyMsg.m_nTransPercent = 100 * reqMsg.m_nDataIndex / reqMsg.m_nDataTotalCount;
				}
				auto pGuiSess = Get_GUI_Sess(notifyMsg.m_strUserId);
				if (pGuiSess)
				{
					pGuiSess->SendMsg(&notifyMsg);
				}
			}
		}
	}
}

/**
 * @brief TCP消息处理,处理收到的文件数据回复消息
 * 
 * @param pClientSess 收到消息的Sess
 * @param rspMsg 文件数据回复消息
 */
void CMediumServer::HSB_FileDataSendRsp(const std::shared_ptr<CClientSess>& pClientSess,const FileDataSendRspMsg& rspMsg)
{
	auto pMsg = DoSendBackFileDataSendRsp(rspMsg);
	pClientSess->SendMsg(pMsg);
}
std::vector<std::string> CMediumServer::GetLocalAllIp()
{
	std::vector<std::string> result;
	asio::ip::tcp::resolver resolver(m_ioService);
	asio::ip::tcp::resolver::query query(asio::ip::host_name(), "");
	asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
	asio::ip::tcp::resolver::iterator end; // End marker.
	while (iter != end)
	{
		tcp::endpoint ep = *iter++;
		if (ep.address().is_v4())
		{
			result.push_back(ep.address().to_string());
		}
	}

	return result;
}
/**
 * @brief 实际处理文件数据发送的回复消息
 * 
 * @param rspMsg 文件数据发送的回复消息
 * @return TransBaseMsg_S_PTR 待发送的消息
 */
TransBaseMsg_S_PTR CMediumServer::DoSendBackFileDataSendRsp(const FileDataSendRspMsg& rspMsg)
{
	if (rspMsg.m_nDataIndex < rspMsg.m_nDataTotalCount)
	{
		FileDataSendReqMsg reqMsg;
		if (m_fileUtil.OnReadData(rspMsg.m_nFileId, reqMsg.m_szData, reqMsg.m_nDataLength, 1024))
		{
			LOG_INFO(ms_loger, "Read Data ", rspMsg.m_nFileId);
			reqMsg.m_strMsgId = m_httpServer->GenerateMsgId();
			reqMsg.m_nFileId = rspMsg.m_nFileId;
			reqMsg.m_strUserId = rspMsg.m_strUserId;
			reqMsg.m_strFriendId = rspMsg.m_strFriendId;
			reqMsg.m_nDataTotalCount = rspMsg.m_nDataTotalCount;
			reqMsg.m_nDataIndex = rspMsg.m_nDataIndex + 1;
		}
		auto pResult = std::make_shared<TransBaseMsg_t>(reqMsg.GetMsgType(), reqMsg.ToString());
		return pResult;
	}
	else
	{
		FileVerifyReqMsg verifyReqMsg;
		verifyReqMsg.m_strMsgId = m_httpServer->GenerateMsgId();
		verifyReqMsg.m_nFileId = rspMsg.m_nFileId;
		verifyReqMsg.m_strUserId = rspMsg.m_strUserId;
		verifyReqMsg.m_strFriendId = rspMsg.m_strFriendId;
		std::string strFileName = m_fileUtil.GetFileName(rspMsg.m_nFileId);
		verifyReqMsg.m_strFileName = m_fileUtil.GetFileNameFromPath(strFileName);
		m_fileUtil.GetFileSize(verifyReqMsg.m_nFileSize, strFileName);
		verifyReqMsg.m_strFileHash = m_fileUtil.CalcHash(strFileName);
		m_fileUtil.OnCloseFile(rspMsg.m_nFileId);
		auto pResult = std::make_shared<TransBaseMsg_t>(verifyReqMsg.GetMsgType(), verifyReqMsg.ToString());
		return pResult;
	}
}

/**
 * @brief UDP消息处理,处理从UDP收到的文件数据发送回复消息
 * 
 * @param endPt UDP消息的来源地址
 * @param rspMsg 文件数据发送回复消息
 */
void CMediumServer::Handle_UdpMsg(const asio::ip::udp::endpoint endPt,const FileDataSendRspMsg& rspMsg)
{
	if (rspMsg.m_nDataIndex < rspMsg.m_nDataTotalCount)
	{
		FileDataSendReqMsg reqMsg;
		if (m_fileUtil.OnReadData(rspMsg.m_nFileId, reqMsg.m_szData, reqMsg.m_nDataLength, 1024))
		{
			LOG_INFO(ms_loger, "Read Data ", rspMsg.m_nFileId);
			reqMsg.m_strMsgId = m_httpServer->GenerateMsgId();
			reqMsg.m_nFileId = rspMsg.m_nFileId;
			reqMsg.m_strUserId = rspMsg.m_strUserId;
			reqMsg.m_strFriendId = rspMsg.m_strFriendId;
			reqMsg.m_nDataTotalCount = rspMsg.m_nDataTotalCount;
			reqMsg.m_nDataIndex = rspMsg.m_nDataIndex + 1;
			auto pUdpSess = GetUdpSess(reqMsg.m_strUserId);
			if(pUdpSess)
			{
				pUdpSess->send_msg(endPt, &reqMsg);
			}
			else
			{
				LOG_ERR(ms_loger, "UDP Sess Failed:{}", reqMsg.m_strUserId);
			}
		}
		//Send File Process
		{
			//File Process
			{
				FileTransProgressNotifyReqMsg notifyMsg;
				notifyMsg.m_strMsgId = m_httpServer->GenerateMsgId();
				notifyMsg.m_strUserId = reqMsg.m_strUserId;
				notifyMsg.m_strFileName = m_fileUtil.GetFileName(reqMsg.m_nFileId);
				notifyMsg.m_strOtherId = reqMsg.m_strFriendId;
				notifyMsg.m_eDirection = FILE_TRANS_DIRECTION::E_SEND_FILE;
				if (reqMsg.m_nDataTotalCount != 0)
				{
					notifyMsg.m_nTransPercent = 100 * reqMsg.m_nDataIndex / reqMsg.m_nDataTotalCount;
				}
				auto pGuiSess = Get_GUI_Sess(notifyMsg.m_strUserId);
				if (pGuiSess)
				{
					pGuiSess->SendMsg(&notifyMsg);
				}
			}
		}
	}
	else
	{
		FileVerifyReqMsg verifyReqMsg;
		verifyReqMsg.m_strMsgId = m_httpServer->GenerateMsgId();
		verifyReqMsg.m_nFileId = rspMsg.m_nFileId;
		verifyReqMsg.m_strUserId = rspMsg.m_strUserId;
		verifyReqMsg.m_strFriendId = rspMsg.m_strFriendId;
		std::string strFileName = m_fileUtil.GetFileName(rspMsg.m_nFileId);
		verifyReqMsg.m_strFileName = m_fileUtil.GetFileNameFromPath(strFileName);
		m_fileUtil.GetFileSize(verifyReqMsg.m_nFileSize, strFileName);
		verifyReqMsg.m_strFileHash = m_fileUtil.CalcHash(strFileName);
		auto pSess = GetClientSess(verifyReqMsg.m_strUserId);
		if (pSess != nullptr)
		{
			auto pSend = std::make_shared<TransBaseMsg_t>(verifyReqMsg.GetMsgType(), verifyReqMsg.ToString());
			pSess->SendMsg(pSend);
		}
		m_fileUtil.OnCloseFile(rspMsg.m_nFileId);
	}
}

/**
 * @brief 处理UDP消息,用于UDP的Client的回调
 * 
 * @param endPt 远端UDP的地址
 * @param pMsg 从UDP收到的消息
 */
void CMediumServer::DispatchUdpMsg(const asio::ip::udp::endpoint endPt, TransBaseMsg_t* pMsg)
{
	if (pMsg)
	{
		switch (pMsg->GetType())
		{
		case E_MsgType::KeepAliveRsp_Type:
		{
			KeepAliveRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				Handle_UdpMsg(endPt, rspMsg);
			}
		}break;
		case E_MsgType::FileSendDataReq_Type:
		{
			FileDataSendReqMsg reqMsg;
			if (reqMsg.FromString(pMsg->to_string())) {
				Handle_UdpMsg(endPt, reqMsg);
			}
		}break;
		case E_MsgType::FileSendDataRsp_Type:
		{
			FileDataSendRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string()))
			{
				Handle_UdpMsg(endPt, rspMsg);
			}
		}break;
		case E_MsgType::FileRecvDataReq_Type:
		{
			FileDataRecvReqMsg reqMsg;
			if (reqMsg.FromString(pMsg->to_string())) {
				Handle_UdpMsg(endPt, reqMsg);
			}
		}break;
		case E_MsgType::UdpP2PStartReq_Type:
		{
			UdpP2pStartReqMsg reqMsg;
			if (reqMsg.FromString(pMsg->to_string())) {
				Handle_UdpMsg(endPt, reqMsg);
			}
		}break;
		case E_MsgType::UdpP2PStartRsp_Type:
		{
			UdpP2pStartRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				Handle_UdpMsg(endPt, rspMsg);
			}
		}break;
		default:
		{
			LOG_ERR(ms_loger, "Unhandle UDP Msg:{} {} [{} {}]", MsgType(pMsg->GetType()), pMsg->to_string(), __FILENAME__, __LINE__);
		}break;
		}
	}
}
/**
 * @brief 启动服务
 * 
 * @param callback 
 */
void CMediumServer::start(const std::function<void(const std::error_code &)> &callback)
{
	if (!m_serverCfg.Valid())
	{
		LOG_ERR(ms_loger, "ServerConfig Is Error {}", m_serverCfg.to_string());
		return;
	}
	LOG_INFO(ms_loger, "Server Start Service [{} {}]",__FILENAME__,__LINE__);
	std::error_code ec;
	asio::ip::tcp::endpoint endpoint;
	if (m_serverCfg.m_strServerIp.length() > 0)
	{
		endpoint = asio::ip::tcp::endpoint(
			asio::ip::address::from_string(m_serverCfg.m_strServerIp),
			m_serverCfg.m_nPort);
	}
	else
	{
		endpoint =
			asio::ip::tcp::endpoint(asio::ip::tcp::v4(), m_serverCfg.m_nPort);
	}
	m_acceptor.open(endpoint.protocol());
	m_acceptor.set_option(asio::socket_base::reuse_address(true));
	m_acceptor.bind(endpoint, ec);
	if (!ec)
	{
		LOG_INFO(ms_loger, "Bind To {} Succeed [{} {}]", m_serverCfg.to_string(),__FILENAME__,__LINE__);
		m_acceptor.listen(asio::socket_base::max_connections, ec);
		if (!ec)
		{
			LOG_INFO(ms_loger, "Listen To {} Succeed [{} {}]", m_serverCfg.to_string(), __FILENAME__, __LINE__);
		}
		else
		{
			LOG_WARN(ms_loger, "Listen To {} Failed, Reason:{} {} [{} {}]",
				m_serverCfg.to_string(), ec.value(), ec.message(), __FILENAME__, __LINE__);
		}
		SetTimer(1);
		do_accept();
		LOG_INFO(ms_loger, "Http Server On :{} [{} {} ]", m_httpCfg.m_nPort,__FILENAME__,__LINE__);
		{
			std::weak_ptr<CMediumServer> pSelf = shared_from_this();
			m_httpServer = std::make_shared<CHttpServer>(m_ioService, [this,pSelf](const BaseMsg* pMsg)->bool {
				if (pSelf.lock())
				{
					return HandleHttpMsg(pMsg);
				}
				else
				{
					return false;
				}
			});
			m_httpServer->Start(m_httpCfg.m_nPort);

		}
		if (!m_clientCfgVec.empty())
		{
			m_freeClientSess = std::make_shared<CClientSess>(m_ioService,
				m_clientCfgVec[0].m_strServerIp,
				m_clientCfgVec[0].m_nPort, this);

			m_freeClientSess->StartConnect();
		}
	}
	else
	{
		LOG_WARN(ms_loger, "Bind To {} Failed [{} {}]", m_serverCfg.to_string(), __FILENAME__, __LINE__);
		callback(ec);
#ifndef _WIN32
		exit(BIND_FAILED_EXIT);
#endif
	}
}



bool CMediumServer::HandleHttpMsg(const BaseMsg* pMsg)
{
	switch (pMsg->GetMsgType())
	{
	case E_MsgType::UserRegisterReq_Type:
	{
		if (m_freeClientSess)
		{
			m_freeClientSess->SendMsg(pMsg);
		}
	}break;
	case E_MsgType::UserLoginReq_Type:
	{
		UserLoginReqMsg* msg = (UserLoginReqMsg*)(pMsg);
		auto pSess = CreateClientSess();
		pSess->SendMsg(pMsg);
	}break;
	case E_MsgType::UserLogoutReq_Type:
	{
		UserLogoutReqMsg* msg = (UserLogoutReqMsg*)(pMsg);
		auto pSess = GetClientSess(GetUserId(msg->m_strUserName));
		if (pSess)
		{
			pSess->SendMsg(pMsg);
		}
		else
		{
			LOG_ERR(ms_loger, "No Sess For {} [{} {}]", msg->m_strUserName, __FILENAME__, __LINE__);
		}
	}break;
	case E_MsgType::FindFriendReq_Type:
	{
		FindFriendReqMsg* pSendMsg = (FindFriendReqMsg*)(pMsg);
		auto pSess = GetClientSess(pSendMsg->m_strUserId);
		if (pSess)
		{
			pSess->SendMsg(pMsg);
		}
	}break;
	case E_MsgType::UserUnRegisterReq_Type:
	{
		if (m_freeClientSess)
		{
			m_freeClientSess->SendMsg(pMsg);
		}
	}break;
	case E_MsgType::CreateGroupReq_Type:
	{
		CreateGroupReqMsg * msg = (CreateGroupReqMsg *)(pMsg);
		auto pSess = GetClientSess(msg->m_strUserId);
		if (pSess)
		{
			pSess->SendMsg(pMsg);
		}
	}break;
	case E_MsgType::FindGroupReq_Type:
	{
		FindGroupReqMsg * msg = (FindGroupReqMsg *)(pMsg);
		auto pSess = GetClientSess(msg->m_strUserId);
		if (pSess)
		{
			pSess->SendMsg(pMsg);
		}
		else
		{
			LOG_ERR(ms_loger, "User:{} No Sess [{} {}]", msg->m_strUserId, __FILENAME__, __LINE__);
		}
	}break;
	case E_MsgType::AddToGroupReq_Type:
	{
		AddToGroupReqMsg * msg = (AddToGroupReqMsg *)(pMsg);
		auto pSess = GetClientSess(msg->m_strUserId);
		if (pSess)
		{
			pSess->SendMsg(pMsg);
		}
	}break;
	case E_MsgType::SendGroupTextMsgReq_Type:
	{
		SendGroupTextMsgReqMsg * msg = (SendGroupTextMsgReqMsg *)(pMsg);
		auto pSess = GetClientSess(msg->m_chatMsg.m_strSenderId);
		if (pSess)
		{
			pSess->SendMsg(pMsg);
		}
	}break;
	case E_MsgType::DestroyGroupReq_Type:
	{
		DestroyGroupReqMsg * msg = (DestroyGroupReqMsg *)(pMsg);
		auto pSess = GetClientSess(msg->m_strUserId);
		if (pSess)
		{
			pSess->SendMsg(pMsg);
		}
	}break;
	case E_MsgType::FriendChatReceiveTxtMsgRsp_Type:
	{
		FriendChatRecvTxtRspMsg * msg = (FriendChatRecvTxtRspMsg*)(pMsg);
		auto pSess = GetClientSess(msg->m_strUserId);
		if (pSess)
		{
			pSess->SendMsg(pMsg);
		}
	}break;
	case E_MsgType::FriendChatReceiveTxtMsgReq_Type:
	{
		FriendChatRecvTxtReqMsg* msg = (FriendChatRecvTxtReqMsg*)(pMsg);
		auto pUtil = GetMsgPersisUtil(msg->m_chatMsg.m_strReceiverId);
		if (pUtil)
		{
			FriendChatRecvTxtReqMsg reqMsg;
			pUtil->Get_FriendChatRecvTxtReqMsg(reqMsg.m_chatMsg);
			reqMsg.m_strMsgId = msg->m_strMsgId;
			m_httpServer->On_FriendChatRecvTxtReqMsg(reqMsg);
		}
	}break;
	case E_MsgType::RemoveFriendReq_Type:
	{
		RemoveFriendReqMsg * msg = (RemoveFriendReqMsg*)(pMsg);
		auto pSess = GetClientSess(msg->m_strUserId);
		if (pSess)
		{
			pSess->SendMsg(pMsg);
		}
		else
		{
			LOG_ERR(ms_loger, "User:{} No Sess [{} {}]", msg->m_strUserId, __FILENAME__, __LINE__);
		}
	}break;
	case E_MsgType::AddFriendSendReq_Type:
	{
		AddFriendSendReqMsg * msg = (AddFriendSendReqMsg*)(pMsg);
		auto pSess = GetClientSess(msg->m_strUserId);
		if (pSess)
		{
			pSess->SendMsg(pMsg);
		}
	}break;
	case E_MsgType::AddFriendRecvReq_Type:
	{
		AddFriendRecvReqMsg * msg = (AddFriendRecvReqMsg*)(pMsg);
		auto pUtil = GetMsgPersisUtil(msg->m_strUserId);
		if (pUtil)
		{
			AddFriendRecvReqMsg newMsg;
			if (pUtil->Get_AddFriendRecvReqMsg(newMsg))
			{
				m_AddFriendMsgIdMap.insert({ msg->m_strMsgId, newMsg.m_strMsgId });
				{
					newMsg.m_strMsgId = msg->m_strMsgId;
					m_httpServer->On_AddFriendRecvReqMsg(newMsg);
				}
			}
		}

	}break;
	case E_MsgType::AddFriendRecvRsp_Type:
	{
		AddFriendRecvRspMsg * msg = (AddFriendRecvRspMsg*)(pMsg);
		auto pSess = GetClientSess(msg->m_strUserId);
		if (pSess)
		{
			AddFriendRecvRspMsg rspMsg = *msg;
			if (m_AddFriendMsgIdMap.end() != m_AddFriendMsgIdMap.find(msg->m_strMsgId))
			{
				rspMsg.m_strMsgId = m_AddFriendMsgIdMap[msg->m_strMsgId];
			}
			m_AddFriendMsgIdMap.erase(msg->m_strMsgId);
			pSess->SendMsg(&rspMsg);
		}
	}break;
	case E_MsgType::AddFriendNotifyReq_Type:
	{
		AddFriendNotifyReqMsg* msg = (AddFriendNotifyReqMsg*)(pMsg);
		auto pUtil = GetMsgPersisUtil(msg->m_strUserId);
		if(pUtil)
		{
			AddFriendNotifyReqMsg newMsg;
			if (pUtil->Get_AddFriendNotifyReqMsg(newMsg))
			{
				{
					newMsg.m_strMsgId = msg->m_strMsgId;
					m_httpServer->On_AddFriendNotifyReqMsg(newMsg);
				}
			}
		}
	}break;
	case E_MsgType::AddFriendNotifyRsp_Type:
	{
		AddFriendNotifyRspMsg * msg = (AddFriendNotifyRspMsg*)(pMsg);
		
	}break;
	case E_MsgType::FriendSendFileMsgReq_Type:
	{
		FriendSendFileMsgReqMsg * msg = (FriendSendFileMsgReqMsg*)(pMsg);
		auto pSess = GetClientSess(msg->m_strUserId);
		if (pSess)
		{
			pSess->SendMsg(pMsg);
		}
	}break;
	case E_MsgType::FriendRecvFileMsgReq_Type:
	{

	}break;
	case E_MsgType::FriendRecvFileMsgRsp_Type:
	{
		FriendSendFileMsgReqMsg * msg = (FriendSendFileMsgReqMsg*)(pMsg);
		auto pSess = GetClientSess(msg->m_strUserId);
		if (pSess)
		{
			pSess->SendMsg(pMsg);
		}
	}break;
	case E_MsgType::FriendChatSendTxtMsgReq_Type:
	{
		FriendChatSendTxtReqMsg * msg = (FriendChatSendTxtReqMsg*)(pMsg);
		auto pSess = GetClientSess(msg->m_strSenderId);
		if (pSess)
		{
			pSess->SendMsg(pMsg);
		}
	}break;
	default:
	{
		LOG_ERR(ms_loger, "Unhandle Msg Type:{} [{} {}]", MsgType(pMsg->GetMsgType()), __FILENAME__, __LINE__);
		return false;
	}
	}
	return true;
}

/**
 * @brief 定时检查UDP的P2P 连接
 * 
 */
void CMediumServer::CheckFriendP2PConnect()
{
	if (m_userFriendListMap.empty())
	{
		LOG_INFO(ms_loger, "CheckFriendP2PConnect No User [{} {}]", __FILENAME__, __LINE__);
	}
	else
	{
		LOG_INFO(ms_loger, "CheckFriendP2PConnect Begin   [{} {}]",__FILENAME__,__LINE__);
	}
	for (auto userItem : m_userFriendListMap)
	{
		auto pUdpSess = GetUdpSess(userItem.first);
		if (pUdpSess)
		{
			for (auto friItem : userItem.second)
			{
				auto item = m_userIdUdpAddrMap.find(friItem);
				if (item != m_userIdUdpAddrMap.end())
				{
					UdpP2pStartReqMsg reqMsg;
					reqMsg.m_strMsgId = m_httpServer->GenerateMsgId();
					reqMsg.m_strUserId = userItem.first;
					reqMsg.m_strFriendId = friItem;
					pUdpSess->send_msg(item->second.m_strServerIp, item->second.m_nPort, &reqMsg);
					pUdpSess->DoSend();
				}
				else
				{
					auto pTcpSess = GetClientSess(userItem.first);
					if (pTcpSess)
					{
						QueryUserUdpAddrReqMsg reqMsg;
						reqMsg.m_strMsgId = m_httpServer->GenerateMsgId();
						reqMsg.m_strUserId = userItem.first;
						reqMsg.m_strUdpUserId = friItem;
						pTcpSess->SendMsg(&reqMsg);
					}
				}
			}
		}
		else
		{

		}
	}
}

/**
 * @brief 处理收到的好友列表回复消息
 * 
 * @param pClientSess 用户连接
 * @param msg 好友列表回复消息
 * @return true 处理成功
 * @return false 处理失败
 */
bool CMediumServer::HSB_GetFriendListRsp(const std::shared_ptr<CClientSess>& pClientSess, const GetFriendListRspMsg& msg)
{
	{
		m_userFriendListMap.erase(pClientSess->UserId());
		std::vector<std::string> strFriendVec;
		for (auto teamItem : msg.m_teamVec)
		{
			for (auto friItem : teamItem.m_teamUsers) {
				strFriendVec.push_back(friItem.m_strUserId);
			}
		}
		m_userFriendListMap.insert({ pClientSess->UserId(),strFriendVec });
	}
	{
		auto pGuiSess = Get_GUI_Sess(pClientSess->UserId());
		if (pGuiSess)
		{
			pGuiSess->SendMsg(&msg);
		}
	}
	return true;
}
/**
 * @brief 检查所有的socket连接
 * 
 */
void CMediumServer::CheckAllConnect()
{
	if (m_timeCount % 30 == 0)
	{
		for (auto& item : m_userId_ClientSessMap)
		{
			item.second->StartConnect();
			item.second->SendKeepAlive();
		}

		for (auto& item : m_userUdpSessMap)
		{
			KeepAliveReqMsg reqMsg;
			reqMsg.m_strClientId = item.first;
			item.second->sendToServer(&reqMsg);
		}
		CheckFriendP2PConnect();
	}
	for (auto& item : m_reConnectSessMap)
	{
		item.first->StartConnect();
	}
}

/**
 * @brief 接受来自客户端的连接
 * 
 */
void CMediumServer::do_accept() 
{
		LOG_INFO(ms_loger,"CMediumServer do_accept [{} {}]", __FILENAME__, __LINE__);
        m_acceptor.async_accept(m_socket, [this](std::error_code ec) {
            if (!ec)
			{
			   LOG_INFO(ms_loger,"Server accept Successed [{} {}]",__FILENAME__, __LINE__);
               

			   if (!m_clientCfgVec.empty() )
			   {
				   //
				   auto clientSess = std::make_shared<CClientSess>(m_ioService, 
					                                               m_clientCfgVec[0].m_strServerIp, 
					                                               m_clientCfgVec[0].m_nPort, this);

				   clientSess->StartConnect();


				   //
				   std::weak_ptr<CMediumServer> pSelf = shared_from_this();
				   auto serverSess = std::make_shared<CServerSess>(std::move(m_socket), [this, pSelf](CServerSess_SHARED_PTR pSess, const TransBaseMsg_t& pMsg)->void {
					   if (pSelf.lock())
					   {
						   HandleSendForward(pSess, pMsg);
					   }
				   });
				   serverSess->Start();

				   //m_GuiSessList.push_back(serverSess);
				   //m_ConnectSessList.push_back(clientSess);
				   //
				   m_ForwardSessMap.insert(std::pair<CServerSess_SHARED_PTR, CClientSess_SHARED_PTR>(serverSess, clientSess));
				   m_BackSessMap.insert(std::pair<CClientSess_SHARED_PTR,CServerSess_SHARED_PTR>(clientSess, serverSess));

			   }
			}
            do_accept();
        });
}

/**
 * @brief 根据用户名获取用户ID
 * 
 * @param strUserName 用户名
 * @return std::string 用户ID
 */
std::string CMediumServer::GetUserId(const std::string strUserName)
{
	auto item = m_userName_UserIdMap.find(strUserName);
	if (item != m_userName_UserIdMap.end())
	{
		return item->second;
	}
	else
	{
		return "";
	}
}
/**
 * @brief 根据用户ID或用户名获取客户端会话连接
 * 
 * @param strUserName 用户名或者用户ID
 * @return CClientSess_SHARED_PTR 
 */
CClientSess_SHARED_PTR CMediumServer::GetClientSess(const std::string strUserId) {
	auto item = m_userId_ClientSessMap.find(strUserId);
	if (item != m_userId_ClientSessMap.end())
	{
		return item->second;
	}
	else
	{
		return nullptr;
	}
}

/**
 * @brief 创建连接到服务器的TCP连接
 * 
 * @return CClientSess_SHARED_PTR 到服务器的TCP连接
 */
	CClientSess_SHARED_PTR CMediumServer::CreateClientSess()
	{
		auto pReturn = m_freeClientSess;
		m_freeClientSess = std::make_shared<CClientSess>(m_ioService,
			m_clientCfgVec[0].m_strServerIp,
			m_clientCfgVec[0].m_nPort, this);

		m_freeClientSess->StartConnect();
		return pReturn;
	}

/**
 * @brief 检查待发送消息
 * 
 */
void CMediumServer::CheckWaitMsgVec()
{
	decltype(m_RecvWaitMsgMap) notSendMap;
	if (m_timeCount % 10 == 0)
	{
		for (auto item : m_RecvWaitMsgMap)
		{
			auto pSess = GetClientSess(item.first);
			if (pSess)
			{
				for (auto msgItem : item.second)
				{
					pSess->SendMsg(msgItem.get());
				}
			}
			else
			{
				notSendMap.insert(item);
			}
		}
		m_RecvWaitMsgMap.clear();
		m_RecvWaitMsgMap = notSendMap;
	}
}


void CMediumServer::stop()
{
	m_httpServer->Stop();
	m_timer->cancel();
	m_acceptor.close();
	m_ioService.stop();
}
/**
 * @brief 响应定时器的任务，一些需要定时处理的任务，可以写成一个函数，在此函数中调用
 * 
 */
void CMediumServer::OnTimer()
{
	if (m_httpServer)
	{
		m_httpServer->OnTimer();
	}
	m_timeCount++;
	CheckAllConnect();
	if (m_timeCount % 60 == 0)
	{
		if (!m_userId_ClientSessMap.empty())
		{
			m_nNoSessTimeCount = 0;
			CheckWaitMsgVec();
		}
		else
		{
			m_nNoSessTimeCount++;
			if (m_nNoSessTimeCount > 3)
			{
				stop();
				LOG_ERR(ms_loger, "Server Closed {} [ {} {} ]", m_timeCount, __FILENAME__, __LINE__);
			}
			else
			{
				LOG_WARN(ms_loger, "No Sess Count {} [ {} {} ]", m_timeCount, __FILENAME__, __LINE__);
			}
		}
	}
}

/**
 * @brief 设置定时器，单位秒
 * 
 * @param nSeconds 定时的秒数
 */
void CMediumServer::SetTimer(int nSeconds)
{
	if (m_timeCount % 30 == 0)
	{
		LOG_INFO(this->ms_loger, "On Timer at MediumServer [{} {}]", __FILENAME__, __LINE__);
	}
	if(m_timer && m_nNoSessTimeCount < 30)
	{
		m_timer->expires_from_now(std::chrono::seconds(nSeconds));
		std::weak_ptr<CMediumServer> pSelf = shared_from_this();
		m_timer->async_wait([this,nSeconds, pSelf](const std::error_code& ec){
			if (pSelf.lock())
			{
				if (!ec)
				{
					this->OnTimer();
					this->SetTimer(nSeconds);
				}
				else
				{
					LOG_WARN(this->ms_loger, "On Timer at MediumServer {} [{} {}]", ec.message(), __FILENAME__, __LINE__);
				}
			}
		});
	}
	else
	{
		try
		{
			m_ioService.stop();
		}
		catch (std::exception ec)
		{
			LOG_ERR(ms_loger, "{} [{} {}]",ec.what(), __FILENAME__, __LINE__);
		}
	}
}

/**
 * @brief 处理好友聊天发送文本消息的回复
 * 
 * @param rspMsg 
 */
void CMediumServer::HandleFriendChatSendTextMsgRsp(const FriendChatSendTxtRspMsg& rspMsg)
{
	LOG_ERR(ms_loger,"{}",rspMsg.ToString());
}

/**
 * @brief 获取Listen的服务器的IP和端口
 * 
 * @return std::string 
 */
std::string CMediumServer::getServerIpPort()
{
	return m_serverCfg.to_string();
}

/**
 * @brief 处理从ClientSess中，回传的消息
 * 
 * @param msg 消息
 */
void CMediumServer::SendBack(const std::shared_ptr<CClientSess>& pClientSess, const TransBaseMsg_t& msg)
{

	auto pMsg = std::make_shared<TransBaseMsg_t>(msg.GetType(), msg.to_string());
	//auto item = m_BackSessMap.find(pClientSess);
	auto pGuiSess = Get_GUI_Sess(pClientSess->UserId());
	if (!pGuiSess)
	{
		pGuiSess = Get_GUI_Sess(pClientSess->UserName());
	}
	if (pGuiSess)
	{
		if (HandleSendBack(pClientSess, msg))
		{
		}
		else
		{
			pGuiSess->SendMsg(pMsg);
		}
	}
	else
	{
		//HandleSendBack(pClientSess, msg);
		OnHttpRsp(pClientSess,pMsg);
	}
}

/**
 * @brief 处理文件校验请求,在发送文件完成以后进行
 * 
 * @param msg 文件校验请求消息
 */
void CMediumServer::HandleFileVerifyReq(const FileVerifyReqMsg& msg)
{
	FileVerifyRspMsg rspMsg;
	m_fileUtil.OnCloseFile(msg.m_nFileId + 1);
	std::string strFileName = m_fileUtil.GetCurDir() + msg.m_strUserId + "\\" + msg.m_strFileName;
	std::string strRecvHash = m_fileUtil.CalcHash(strFileName);
	//
	if (msg.m_strFileHash == strRecvHash)
	{

		auto hashItem = std::find(m_fileHashTransVec.begin(), m_fileHashTransVec.end(), strRecvHash);
		if (hashItem != m_fileHashTransVec.end())
		{
			m_fileHashTransVec.erase(hashItem);
		}

		rspMsg.m_eErrCode = ERROR_CODE_TYPE::E_CODE_SUCCEED;
		{
			auto item = m_fileHashMsgIdMap.find(strRecvHash);
			if (item != m_fileHashMsgIdMap.end()) {
				auto msgItem = m_waitImageMsgMap.find(item->second);
				if (msgItem != m_waitImageMsgMap.end()) {
					ChatMsgElemVec oldVec = MsgElemVec(msgItem->second.m_chatMsg.m_strContext);
					ChatMsgElemVec newVec;
					for (auto item : oldVec) {
						if (item.m_eType == CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE) {
							ChatMsgElem elem;
							elem.m_eType = item.m_eType;
							elem.m_strImageName = strFileName;
							newVec.push_back(elem);
						}
						else {
							newVec.push_back(item);
						}
					}
					msgItem->second.m_chatMsg.m_strContext = MsgElemVec(newVec);
					auto pSess = Get_GUI_Sess(msgItem->second.m_chatMsg.m_strReceiverId);
					if (pSess)
					{
						pSess->SendMsg(&(msgItem->second));
					}
					else
					{
						LOG_ERR(ms_loger, "UserId:{} No GUI Sess [{} {}]", msgItem->second.m_chatMsg.m_strReceiverId, __FILENAME__, __LINE__);
					}

					{
						auto pMsgUtil = GetMsgPersisUtil(msgItem->second.m_chatMsg.m_strReceiverId);
						if (pMsgUtil)
						{
							LOG_ERR(ms_loger, "UserId:{} No Msg Util[ {} {} ]", msgItem->second.m_chatMsg.m_strReceiverId, __FILENAME__, __LINE__);
						}
						else
						{
							//TODO: 数据记录保存到数据库会Crash
							//pMsgUtil->Save_FriendChatSendTxtRspMsg(msgItem->second.m_chatMsg);
						}

					}
					{
						FriendChatRecvTxtRspMsg rspMsg;
						rspMsg.m_strMsgId = msgItem->second.m_strMsgId;
						rspMsg.m_strFriendId = msgItem->second.m_chatMsg.m_strSenderId;
						rspMsg.m_strUserId = msgItem->second.m_chatMsg.m_strReceiverId;
						rspMsg.m_strChatMsgId = msgItem->second.m_chatMsg.m_strChatMsgId;
						auto pClientSess = GetClientSess(rspMsg.m_strUserId);
						if (pClientSess)
						{
							pClientSess->SendMsg(&rspMsg);
						}
					}
					m_waitImageMsgMap.erase(item->second);
					m_fileHashMsgIdMap.erase(strRecvHash);

				}
			}
		}
		auto pMsgUtil = GetMsgPersisUtil(msg.m_strUserId);
		if (pMsgUtil)
		{
			pMsgUtil->Save_FileHash(strFileName, strRecvHash);
		}
		else
		{
			LOG_ERR(ms_loger, "UserId:{} No Msg Util[ {} {} ]", msg.m_strUserId, __FILENAME__, __LINE__);
		}
		LOG_INFO(ms_loger, "Download File {} Finished [{} {}]", strFileName, __FILENAME__, __LINE__);

	}
	else
	{
		m_fileUtil.RemoveFile(strFileName);
		rspMsg.m_eErrCode = ERROR_CODE_TYPE::E_CODE_LOGIN_FAILED;
	}
	rspMsg.m_strMsgId = msg.m_strMsgId;
	rspMsg.m_strFileName = msg.m_strFileName;
	rspMsg.m_strUserId = msg.m_strUserId;
	rspMsg.m_strFriendId = msg.m_strFriendId;
	rspMsg.m_nFileId = msg.m_nFileId;
	auto pSess = GetClientSess(rspMsg.m_strUserId);
	if (pSess != nullptr)
	{
		auto pSend = std::make_shared<TransBaseMsg_t>(rspMsg.GetMsgType(), rspMsg.ToString());
		pSess->SendMsg(pSend);
	}

}

/**
 * @brief TCP消息处理,处理文件校验回复消息
 * 
 * @param pClientSess 用户会话
 * @param rspMsg 文件校验回复消息
 */
void CMediumServer::HSB_FileVerifyRsp(const std::shared_ptr<CClientSess>& pClientSess, const FileVerifyRspMsg& rspMsg)
{
	if (rspMsg.m_eErrCode == ERROR_CODE_TYPE::E_CODE_SUCCEED)
	{
		{
			auto item = m_SendWaitMsgMap.find(rspMsg.m_strFileHash);
			if (item != m_SendWaitMsgMap.end()) {
				pClientSess->SendMsg(&(item->second));
				m_SendWaitMsgMap.erase(rspMsg.m_strFileHash);
			}
		}

		{
			auto item = m_groupSendWaitMsgMap.find(rspMsg.m_strFileHash);
			if (item != m_groupSendWaitMsgMap.end()) {
				pClientSess->SendMsg(&(item->second));
				m_groupSendWaitMsgMap.erase(rspMsg.m_strFileHash);
			}
		}
	}
}

/**
 * @brief 处理对于已经接收或者拒绝接收文件的通知消息
 * 
 * @param notifyMsg 文件接收结果通知消息
 */
void CMediumServer::HandleFriendNotifyFileMsgReq(const FriendNotifyFileMsgReqMsg& notifyMsg)
{
	LOG_ERR(ms_loger,"{}",notifyMsg.ToString());
	//离线
	if(notifyMsg.m_transMode == FILE_TRANS_TYPE::TCP_OFFLINE_MODE)
	{
		//同意
		if (notifyMsg.m_eOption == E_FRIEND_OPTION::E_AGREE_ADD)
		{
			bool bResult = m_fileUtil.OpenReadFile(notifyMsg.m_nFileId, notifyMsg.m_strFileName);
			int nFileSize = 0;
			m_fileUtil.GetFileSize(nFileSize, notifyMsg.m_strFileName);
			auto pSess = GetUdpSess(notifyMsg.m_strUserId);
			if (pSess)
			{
				FileDataSendReqMsg reqMsg;
				reqMsg.m_nDataTotalCount = nFileSize / 1024 + (nFileSize % 1024 == 0 ? 0 : 1);
				int nIndex = 1;
				//while (nIndex <= reqMsg.m_nDataTotalCount)
				//{
				reqMsg.m_nDataIndex = nIndex;
				reqMsg.m_nFileId = notifyMsg.m_nFileId;
				reqMsg.m_strFriendId = notifyMsg.m_strFriendId;
				reqMsg.m_strUserId = notifyMsg.m_strUserId;
				m_fileUtil.OnReadData(reqMsg.m_nFileId, reqMsg.m_szData, reqMsg.m_nDataLength, 1024);
				pSess->sendToServer(&reqMsg);
				//	nIndex++;
				//}
				//pSess->DoSend();
			}
			else
			{
				LOG_ERR(ms_loger, "{} No Udp Sess", notifyMsg.m_strUserId);
			}
		}
		//拒绝
		else
		{

		}
	}
	else 
	{
		if(notifyMsg.m_eOption == E_FRIEND_OPTION::E_AGREE_ADD)
		{
			int nFileId = static_cast<int>(time(nullptr));
			std::string strFileName = notifyMsg.m_strFileName;
			int nFileSize = 0;
			m_fileUtil.GetFileSize(nFileSize, strFileName);
			if (m_fileUtil.OpenReadFile(notifyMsg.m_nFileId, strFileName)) {
				FileDataSendReqMsg sendReqMsg;
				sendReqMsg.m_strMsgId = m_httpServer->GenerateMsgId();
				sendReqMsg.m_strUserId = notifyMsg.m_strUserId;
				sendReqMsg.m_strFriendId = notifyMsg.m_strFriendId;
				sendReqMsg.m_nFileId = notifyMsg.m_nFileId;

				sendReqMsg.m_nDataTotalCount = nFileSize / 1024 + (nFileSize % 1024 == 0 ? 0 : 1);
				sendReqMsg.m_nDataIndex = 1;
				sendReqMsg.m_nDataLength = 0;
				m_fileUtil.OnReadData(sendReqMsg.m_nFileId, sendReqMsg.m_szData, sendReqMsg.m_nDataLength, 1024);

				{
					auto pUdpSess = GetUdpSess(sendReqMsg.m_strUserId);
					//if(notifyMsg.m_trans)
					if (pUdpSess)
					{
						auto udpItem = m_userIdUdpAddrMap.find(sendReqMsg.m_strUserId);
						if (notifyMsg.m_transMode == FILE_TRANS_TYPE::UDP_ONLINE_P2P_MODE &&  udpItem != m_userIdUdpAddrMap.end())
						{
							pUdpSess->send_msg(udpItem->second.m_strServerIp, udpItem->second.m_nPort, &sendReqMsg);
							pUdpSess->DoSend();
						}
						else
						{
							pUdpSess->sendToServer(&sendReqMsg);
						}
					}
					else
					{
						LOG_ERR(ms_loger, "UDP Sess Failed:{}", sendReqMsg.m_strFriendId);
					}
				}
			}
		}
	}
}


/**
 * @brief TCP消息处理,处理文件数据接收请求消息
 * 
 * @param pClientSess 用户会话
 * @param reqMsg 文件数据接收请求消息
 */
void CMediumServer::Handle_TcpMsg(const std::shared_ptr<CClientSess>& pClientSess, const FileDataRecvReqMsg& reqMsg)
{
	if (reqMsg.m_nDataIndex <= reqMsg.m_nDataTotalCount)
	{
		m_fileUtil.OnWriteData(reqMsg.m_nFileId + 1, reqMsg.m_szData, reqMsg.m_nDataLength);
		LOG_INFO(ms_loger, "WriteData {} [{} {}]", reqMsg.m_nFileId,__FILENAME__,__LINE__);
		FileDataRecvRspMsg rspMsg;
		{
			rspMsg.m_strMsgId = reqMsg.m_strMsgId;
			rspMsg.m_nFileId = reqMsg.m_nFileId;
			rspMsg.m_strUserId = reqMsg.m_strUserId;
			rspMsg.m_strFriendId = reqMsg.m_strFriendId;
			rspMsg.m_nDataTotalCount = reqMsg.m_nDataTotalCount;
			rspMsg.m_nDataIndex = reqMsg.m_nDataIndex;
		}
		pClientSess->SendMsg(&rspMsg);
	}

	if (reqMsg.m_nDataIndex == reqMsg.m_nDataTotalCount)
	{
		m_fileUtil.OnCloseFile(reqMsg.m_nFileId + 1);
	}
}
/**
 * @brief 处理从UDP收到文件数据的接收请求消息
 * 
 * @param endPt 消息发送者的地址
 * @param reqMsg 文件数据接收消息
 */
void CMediumServer::Handle_UdpMsg(const asio::ip::udp::endpoint endPt, const FileDataRecvReqMsg& reqMsg)
{
	if (reqMsg.m_nDataIndex <= reqMsg.m_nDataTotalCount)
	{
		m_fileUtil.OnWriteData(reqMsg.m_nFileId + 1, reqMsg.m_szData, reqMsg.m_nDataLength);
		LOG_INFO(ms_loger, "WriteData:File ID {} [{} {}]", reqMsg.m_nFileId, __FILENAME__,__LINE__);
		FileDataRecvRspMsg rspMsg;
		rspMsg.m_strMsgId = reqMsg.m_strMsgId;
		rspMsg.m_nFileId = reqMsg.m_nFileId;
		rspMsg.m_strUserId = reqMsg.m_strUserId;
		rspMsg.m_strFriendId = reqMsg.m_strFriendId;
		rspMsg.m_nDataTotalCount = reqMsg.m_nDataTotalCount;
		rspMsg.m_nDataIndex = reqMsg.m_nDataIndex;
		auto pUdpSess = GetUdpSess(reqMsg.m_strUserId);
		if (pUdpSess)
		{
			pUdpSess->send_msg(endPt,&rspMsg);
			pUdpSess->DoSend();
		}
		else
		{
			//LOG_ERR(ms_loger, "UDP Sess Failed:{}", reqMsg.m_strFromId);
		}
	}
	
	if(reqMsg.m_nDataIndex == reqMsg.m_nDataTotalCount)
	{
		m_fileUtil.OnCloseFile(reqMsg.m_nFileId + 1);
	}
	{
		{
			FileTransProgressNotifyReqMsg notifyMsg;
			notifyMsg.m_strMsgId = m_httpServer->GenerateMsgId();
			notifyMsg.m_strUserId = reqMsg.m_strUserId;
			notifyMsg.m_strFileName = m_fileUtil.GetFileName(reqMsg.m_nFileId);
			notifyMsg.m_strOtherId = reqMsg.m_strFriendId;
			notifyMsg.m_eDirection = FILE_TRANS_DIRECTION::E_RECV_FILE;
			if (reqMsg.m_nDataTotalCount != 0)
			{
				notifyMsg.m_nTransPercent = 100 * reqMsg.m_nDataIndex / reqMsg.m_nDataTotalCount;
			}
			auto pGuiSess = Get_GUI_Sess(notifyMsg.m_strUserId);
			if (pGuiSess)
			{
				pGuiSess->SendMsg(&notifyMsg);
			}
		}
	}
}


/**
 * @brief 将TCP的回复消息变为HTTP消息
 * 
 * @param pMsg TCP的回复消息
 */
void CMediumServer::OnHttpRsp(const std::shared_ptr<CClientSess>& pClientSess,std::shared_ptr<TransBaseMsg_t> pMsg)
{
	if (pMsg)
	{
		switch (pMsg->GetType())
		{
		case E_MsgType::UserRegisterRsp_Type:
		{
			UserRegisterRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string()))
			{
				if (m_httpServer)
				{
					m_httpServer->On_UserRegisterRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::UserUnRegisterRsp_Type:
		{
			UserUnRegisterRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				{
					std::string strUserId = GetUserId(rspMsg.m_strUserName);
					if (!strUserId.empty())
					{
						auto pUtil = GetMsgPersisUtil(strUserId);
						if (pUtil)
						{
							m_UserId_MsgPersistentUtilMap.erase(strUserId);
							pUtil->CloseDataBase();
						}
						CFileUtil::RemoveFile(strUserId + ".db");
						CFileUtil::RemoveFolder(strUserId);
					}
				}
				if (m_httpServer) {
					m_httpServer->On_UserUnRegisterRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::UserLoginRsp_Type:
		{
			UserLoginRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				HSB_UserLoginRsp(pClientSess, rspMsg);
				if (m_httpServer) {
					m_httpServer->On_UserLoginRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::UserLogoutRsp_Type:
		{
			UserLogoutRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				HSB_UserLogoutRsp(pClientSess, rspMsg);
				if (m_httpServer) {
					m_httpServer->On_UserLogoutRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::FindFriendRsp_Type:
		{
			FindFriendRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				if (m_httpServer) {
					m_httpServer->On_FindFriendRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::AddTeamRsp_Type:
		{
			AddTeamRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				if (m_httpServer) {
					m_httpServer->On_AddFriendTeamRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::RemoveTeamRsp_Type:
		{
			RemoveTeamRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				if (m_httpServer) {
					m_httpServer->On_RemoveFriendTeamRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::MoveFriendToTeamRsp_Type:
		{
			MoveFriendToTeamRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				if (m_httpServer) {
					m_httpServer->On_MoveFriendToTeamRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::AddToGroupRsp_Type:
		{
			AddToGroupRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				if (m_httpServer) {
					m_httpServer->On_AddToGroupRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::CreateGroupRsp_Type:
		{
			CreateGroupRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				if (m_httpServer) {
					m_httpServer->On_CreateGroupRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::GetGroupListRsp_Type:
		{
			GetGroupListRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				if (m_httpServer) {
					m_httpServer->On_GetGroupListRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::DestroyGroupRsp_Type:
		{
			DestroyGroupRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				if (m_httpServer) {
					m_httpServer->On_DestroyGroupRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::FindGroupRsp_Type:
		{
			FindGroupRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())){
				if (m_httpServer) {
					m_httpServer->On_FindGroupRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::SendGroupTextMsgRsp_Type:
		{
			SendGroupTextMsgRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				if (m_httpServer) {
					m_httpServer->On_SendGroupTextMsgRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::RecvGroupTextMsgReq_Type:
		{
			RecvGroupTextMsgReqMsg reqMsg;
			if (reqMsg.FromString(pMsg->to_string())) {
				//if (m_msgPersisUtil) {
				//	m_msgPersisUtil->Save_RecvGroupTextMsgReqMsg(reqMsg);
				//}
			}
		}break;
		case E_MsgType::FriendSendFileMsgRsp_Type:
		{
			FriendSendFileMsgRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				if (m_httpServer) {
					m_httpServer->On_SendFriendFileOnlineRspMsg(rspMsg);
				}
			}
		}break;
		case E_MsgType::FriendRecvFileMsgReq_Type:
		{
			FriendRecvFileMsgReqMsg reqMsg;
			if (reqMsg.FromString(pMsg->to_string())) {
				//if (m_msgPersisUtil) {
				//	m_msgPersisUtil->Save_FriendRecvFileMsgReqMsg(reqMsg);
				//}
			}
		}break;
		case E_MsgType::FriendNotifyFileMsgReq_Type:
		{
			FriendNotifyFileMsgReqMsg reqMsg;
			if (reqMsg.FromString(pMsg->to_string())) {
				//if (m_msgPersisUtil) {
				//	m_msgPersisUtil->Save_FriendNotifyFileMsgReqMsg(reqMsg);
				//}
				HandleFriendNotifyFileMsgReq(reqMsg);
			}
		}break;
		case E_MsgType::AddFriendSendRsp_Type:
		{
			AddFriendSendRspMsg reqMsg;
			if (reqMsg.FromString(pMsg->to_string())) {
				if (m_httpServer) {
					m_httpServer->On_AddFriendSendRspMsg(reqMsg);
				}
			}
		}break;
		case E_MsgType::RemoveFriendRsp_Type:
		{
			RemoveFriendRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				if (m_httpServer) {
					m_httpServer->On_RemoveFriendRspMsg(rspMsg);
				}
			}
		}break;
		case E_MsgType::AddFriendRecvReq_Type:
		{
			AddFriendRecvReqMsg reqMsg;
			if (reqMsg.FromString(pMsg->to_string())) {
				auto pUtil = GetMsgPersisUtil(reqMsg.m_strUserId);
				if (pUtil)
				{
					pUtil->Save_AddFriendRecvReqMsg(reqMsg);
				}
			}
		}break;
		case E_MsgType::FriendChatSendTxtMsgRsp_Type:
		{
			FriendChatSendTxtRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				if (m_httpServer) {
					m_httpServer->On_FriendChatSendTxtRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::AddFriendNotifyReq_Type:
		{
			AddFriendNotifyReqMsg reqMsg;
			if (reqMsg.FromString(pMsg->to_string())) {
				auto pUtil = GetMsgPersisUtil(reqMsg.m_strUserId);
				if (pUtil)
				{
					pUtil->Save_AddFriendNotifyReqMsg(reqMsg);
				}
				{
					AddFriendNotifyRspMsg rspMsg;
					rspMsg.m_strMsgId = reqMsg.m_strMsgId;
					pClientSess->SendMsg(&rspMsg);
				}
			}
		}break;
		case E_MsgType::FriendChatReceiveTxtMsgReq_Type:
		{
			FriendChatRecvTxtReqMsg reqMsg;
			if (reqMsg.FromString(pMsg->to_string())) {
				auto pUtil = GetMsgPersisUtil(reqMsg.m_chatMsg.m_strReceiverId);
				if (pUtil)
				{
					pUtil->Save_FriendChatRecvTxtReqMsg(reqMsg.m_chatMsg);
				}
			}

		}break;
		case E_MsgType::FriendUnReadMsgNotifyReq_Type:
		{
			//FriendUnReadNotifyReqMsg reqMsg;
			//if (reqMsg.FromString(pMsg->to_string())) {
			//	FriendUnReadNotifyRspMsg rspMsg;
			//	rspMsg.m_strMsgId = reqMsg.m_strMsgId;
			//	rspMsg.m_strUserId = reqMsg.m_strUserId;
			//	auto pSess = GetClientSess(rspMsg.m_strUserId);
			//	if (pSess != nullptr)
			//	{
			//		auto pSend = std::make_shared<TransBaseMsg_t>(rspMsg.GetMsgType(), rspMsg.ToString());
			//		pSess->SendMsg(pSend);
			//	}
			//}

		}break;
		case E_MsgType::FileVerifyReq_Type:
		{
			FileVerifyReqMsg reqMsg;
			if (reqMsg.FromString(pMsg->to_string())) {
				HandleFileVerifyReq(reqMsg);
			}
		}break;
		case E_MsgType::GetFriendListRsp_Type:
		{
			GetFriendListRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				if (m_httpServer) {
					m_httpServer->On_GetFriendListRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::GetRandomUserRsp_Type:
		{
			GetRandomUserRspMsg rspMsg;
			if (rspMsg.FromString(pMsg->to_string())) {
				if (m_httpServer) {
					m_httpServer->On_RandomUserRsp(rspMsg);
				}
			}
		}break;
		case E_MsgType::KeepAliveRsp_Type:
		{

		}break;
		case E_MsgType::NetRecoverReport_Type:
		{

		}break;
		case E_MsgType::UpdateFriendListNotifyReq_Type:
		{
			UpdateFriendListNotifyReqMsg reqMsg;
			if (reqMsg.FromString(pMsg->to_string())) {
				UpdateFriendListNotifyRspMsg rspMsg;
				rspMsg.m_strMsgId = reqMsg.m_strMsgId;
				rspMsg.m_strUserId = reqMsg.m_strUserId;
				pClientSess->SendMsg(&rspMsg);
			}
		}break;
		default:
		{
			LOG_ERR(ms_loger, "UnHandle MsgType:{} MsgContent:{} [{} {}]",MsgType(pMsg->GetType()),pMsg->to_string(), __FILENAME__, __LINE__);
		}break;
		}
	}
}

/**
 * @brief 处理接收在线文件请求回复消息
 * 
 * @param rspMsg 好友文件请求回复消息
 */
void CMediumServer::Handle_RecvFileOnlineRsp(const FriendRecvFileMsgRspMsg& rspMsg)
{
	if (rspMsg.m_eOption == E_FRIEND_OPTION::E_AGREE_ADD)
	{
		m_fileUtil.OpenWriteFile(rspMsg.m_nFileId+1, rspMsg.m_strUserId+"_"+std::to_string(rand())+"_"+m_fileUtil.GetFileNameFromPath(rspMsg.m_strFileName));
	}
}

/**
 * @brief 处理GUI客户端发过来的请求好友聊天记录的消息
 * 
 * @param pServerSess GUI的socket会话
 * @param reqMsg 获取好友聊天记录的请求消息
 */
void CMediumServer::HSF_GetFriendChatHistoryReq(const std::shared_ptr<CServerSess>& pServerSess, const GetFriendChatHistoryReq& reqMsg)
{
	GetFriendChatHistoryRsp rspMsg;
	rspMsg.m_strMsgId = reqMsg.m_strMsgId;
	rspMsg.m_strUserId = reqMsg.m_strUserId;
	rspMsg.m_strFriendId = reqMsg.m_strFriendId;
	rspMsg.m_strChatMsgId = reqMsg.m_strMsgId;
	auto pMsgUtil = GetMsgPersisUtil(reqMsg.m_strUserId);
	if (pMsgUtil)
	{
		//pMsgUtil->Save_FriendChatSendTxtRspMsg(reqMsg.m_chatMsg);
		rspMsg.m_msgHistory = pMsgUtil->Get_FriendChatHistory(reqMsg);

	}
	else
	{
		LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", reqMsg.m_strUserId, __FILENAME__, __LINE__);
	}
	//rspMsg.m_msgHistory = m_msgPersisUtil->Get_FriendChatHistory(reqMsg);
	pServerSess->SendMsg(&rspMsg);
}


/**
 * @brief 处理GUI客户端发送的获取群组聊天记录的请求消息
 * 
 * @param pServerSess GUI的socket会话
 * @param reqMsg 获取群组聊天记录的请求消息
 */
void CMediumServer::HSF_GetGroupChatHistoryReq(const std::shared_ptr<CServerSess>& pServerSess, const GetGroupChatHistoryReq& reqMsg)
{
	GetGroupChatHistoryRsp rspMsg;
	rspMsg.m_strMsgId = reqMsg.m_strMsgId;
	rspMsg.m_strUserId = reqMsg.m_strUserId;
	rspMsg.m_strGroupId = reqMsg.m_strGroupId;
	rspMsg.m_strChatMsgId = reqMsg.m_strMsgId;
	{
		auto pMsgUtil = GetMsgPersisUtil(reqMsg.m_strUserId);
		if (pMsgUtil)
		{
			rspMsg.m_msgHistory = pMsgUtil->Get_GroupChatHistory(reqMsg);

		}
		else
		{
			LOG_ERR(ms_loger, "UserId:{} No Msg Util[ {} {} ]", reqMsg.m_strUserId, __FILENAME__, __LINE__);
		}
	}
	pServerSess->SendMsg(&rspMsg);
}


/**
 * @brief 处理发送文件数据开始消息
 * 
 * @param pServerSess GUI客户端会话
 * @param reqMsg 发送文件数据开始消息
 */
void CMediumServer::HSF_FileSendDataBeginReq(const std::shared_ptr<CServerSess>& pServerSess,FileSendDataBeginReq& reqMsg)
{
	reqMsg.m_strFileHash = m_fileUtil.CalcHash(reqMsg.m_strFileName);
	{
		
	}
	std::string strFileName;// = m_msgPersisUtil->Get_FileByHash(reqMsg.m_strFileHash);
	auto pMsgUtil = GetMsgPersisUtil(reqMsg.m_strUserId);
	if (pMsgUtil)
	{
		pMsgUtil->Get_FileByHash(reqMsg.m_strFileHash);
	}
	else
	{
		LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", reqMsg.m_strUserId, __FILENAME__, __LINE__);
	}
	if (!strFileName.empty())
	{
		reqMsg.m_strFileName = strFileName;
	}
	else
	{
		std::string strOldFileName = reqMsg.m_strFileName;
		reqMsg.m_strFileName = m_fileUtil.GetFileNameFromPath(reqMsg.m_strFileName);
		std::string strNewFileName = GetUserFileDir(reqMsg.m_strUserId)+reqMsg.m_strFileName;
		if (m_fileUtil.UtilCopy(strOldFileName, strNewFileName))
		{
			LOG_INFO(ms_loger, "CopyFile Succeed {} {} [{} {}]", strOldFileName, strNewFileName,__FILENAME__,__LINE__);
		}
		else
		{
			LOG_ERR(ms_loger, "CopyFile Failed {} {} [{} {}]", strOldFileName, strNewFileName, __FILENAME__, __LINE__);
		}
	}
	//对于原始消息，原封不动的转发
	{
		auto pClientSess = GetClientSess(reqMsg.m_strUserId);
		if (pClientSess)
		{
			auto pMsg = std::make_shared<TransBaseMsg_t>(reqMsg.GetMsgType(), reqMsg.ToString());
			pClientSess->SendMsg(pMsg);
		}
	}
	//auto item = m_ForwardSessMap.find(pServerSess);
	//if (item != m_ForwardSessMap.end())
	//{
	//	auto pMsg = std::make_shared<TransBaseMsg_t>(reqMsg.GetMsgType(), reqMsg.ToString());
	//	item->second->SendMsg(pMsg);
	//}
}

/**
 * @brief 处理发往服务器的好友聊天消息
 * 
 * @param reqMsg 好友聊天请求消息
 * @return true 成功
 * @return false 失败
 */
bool CMediumServer::DoSF_FriendChatSendTxtReq(FriendChatSendTxtReqMsg& reqMsg)
{
	ChatMsgElemVec msgVec = MsgElemVec(reqMsg.m_strContext);
	ChatMsgElemVec newVec;
	std::string strFileHash;
	bool bHaveImage = false;
	for (const auto item : msgVec)
	{
		if (item.m_eType == CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE)
		{
			bHaveImage = true;
			FileSendDataBeginReq beginReqMsg;
			{
				beginReqMsg.m_nFileId = rand();
				beginReqMsg.m_strMsgId = m_httpServer->GenerateMsgId();
				beginReqMsg.m_strFileName = m_httpServer->GenerateMsgId() + m_fileUtil.GetFileNameExtension(item.m_strImageName);
				beginReqMsg.m_strUserId = reqMsg.m_strSenderId;
				beginReqMsg.m_strFriendId = reqMsg.m_strReceiverId;
				beginReqMsg.m_strFileHash = m_fileUtil.CalcHash(item.m_strImageName);
				strFileHash = beginReqMsg.m_strFileHash;
				std::string strNewFileName = GetUserImageDir(reqMsg.m_strSenderId)+beginReqMsg.m_strFileName;
				if (m_fileUtil.UtilCopy(item.m_strImageName, strNewFileName))
				{

				}
				else
				{
					LOG_ERR(ms_loger, "CopyFile Failed {} {}", item.m_strImageName, strNewFileName);
				}
				auto item = m_userId_ClientSessMap.find(reqMsg.m_strSenderId);
				if (item != m_userId_ClientSessMap.end())
				{
					auto pMsg = std::make_shared<TransBaseMsg_t>(beginReqMsg.GetMsgType(), beginReqMsg.ToString());
					item->second->SendMsg(pMsg);
				}
			}
			ChatMsgElem elem;
			elem.m_eType = CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE;
			elem.m_strImageName = beginReqMsg.m_strFileName;
			newVec.push_back(elem);
		}
		else
		{
			newVec.push_back(item);
		}
	}



	if (bHaveImage)
	{
		reqMsg.m_strContext = MsgElemVec(newVec);
		m_SendWaitMsgMap.insert({ strFileHash,reqMsg });
	}
	else
	{
		auto item = m_userId_ClientSessMap.find(reqMsg.m_strSenderId);
		if (item != m_userId_ClientSessMap.end())
		{
			auto pMsg = std::make_shared<TransBaseMsg_t>(reqMsg.GetMsgType(), reqMsg.ToString());
			item->second->SendMsg(pMsg);
		}
	}

	return true;
}

/**
 * @brief 处理发送好友聊天消息 在ClientCore中对消息进行解析,发送其中的图片消息
 * 
 * 
 * @param pServerSess GUI客户端的Sess
 * @param reqMsg 好友聊天请求消息
 */
void CMediumServer::HSF_FriendChatSendTxtReqMsg(const std::shared_ptr<CServerSess>& pServerSess,FriendChatSendTxtReqMsg& reqMsg)
{
	if(pServerSess)
	{
		DoSF_FriendChatSendTxtReq(reqMsg);
	}
}

void CMediumServer::HSF_SendGroupTextMsgReqMsg(const std::shared_ptr<CServerSess>& pServerSess, SendGroupTextMsgReqMsg& reqMsg)
{
	reqMsg.m_strMsgId = m_httpServer->GenerateMsgId();
	ChatMsgElemVec msgVec = MsgElemVec(reqMsg.m_chatMsg.m_strContext);
	ChatMsgElemVec newVec;
	std::string strFileHash;
	bool bHaveImage = false;
	for (const auto item : msgVec)
	{
		if (item.m_eType == CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE)
		{
			bHaveImage = true;
			FileSendDataBeginReq beginReqMsg;
			{
				beginReqMsg.m_nFileId = rand();
				beginReqMsg.m_strMsgId = m_httpServer->GenerateMsgId();
				beginReqMsg.m_strFileName = m_httpServer->GenerateMsgId()+m_fileUtil.GetFileNameExtension(item.m_strImageName);
				beginReqMsg.m_strUserId = reqMsg.m_chatMsg.m_strSenderId;
				beginReqMsg.m_strFriendId = reqMsg.m_chatMsg.m_strGroupId;
				beginReqMsg.m_strFileHash = m_fileUtil.CalcHash(item.m_strImageName);
				strFileHash = beginReqMsg.m_strFileHash;
				std::string strNewFileName = GetUserImageDir(reqMsg.m_chatMsg.m_strSenderId) + beginReqMsg.m_strFileName;
				if (m_fileUtil.UtilCopy(item.m_strImageName, strNewFileName))
				{

				}
				else
				{
					LOG_ERR(ms_loger, "CopyFile Failed {} {}", item.m_strImageName, strNewFileName);
				}
				auto item = m_userId_ClientSessMap.find(reqMsg.m_chatMsg.m_strSenderId);
				if (item != m_userId_ClientSessMap.end())
				{
					auto pMsg = std::make_shared<TransBaseMsg_t>(beginReqMsg.GetMsgType(), beginReqMsg.ToString());
					item->second->SendMsg(pMsg);
				}
			}
			ChatMsgElem elem;
			elem.m_eType = CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE;
			elem.m_strImageName = beginReqMsg.m_strFileName;
			newVec.push_back(elem);
		}
		else
		{
			newVec.push_back(item);
		}
	}



	if (bHaveImage)
	{
		reqMsg.m_chatMsg.m_strContext = MsgElemVec(newVec);
		m_groupSendWaitMsgMap.insert({ strFileHash,reqMsg });
	}
	else
	{
		auto item = m_userId_ClientSessMap.find(reqMsg.m_chatMsg.m_strSenderId);
		if (item != m_userId_ClientSessMap.end())
		{
			auto pMsg = std::make_shared<TransBaseMsg_t>(reqMsg.GetMsgType(), reqMsg.ToString());
			item->second->SendMsg(pMsg);
		}
	}
}

void CMediumServer::HSF_UserLoginReq(const std::shared_ptr<CServerSess>& pServerSess, UserLoginReqMsg& reqMsg)
{
	{
		auto item = m_ForwardSessMap.find(pServerSess);
		if (item != m_ForwardSessMap.end())
		{
			auto pSess = item->second;
			if (pSess->IsConnect())
			{
				pSess->SendMsg(&reqMsg);
			}
			else
			{
				pSess->StartConnect();
			}
		}
	}

	{
		auto item = m_userLoginMsgMap.find(reqMsg.m_strUserName);
		if (item != m_userLoginMsgMap.end())
		{
			m_userLoginMsgMap.erase(reqMsg.m_strUserName);
		}
		m_userLoginMsgMap.insert({ reqMsg.m_strUserName,reqMsg });
	}
}
/**
 * @brief 来自GUI客户端的部分消息,不需要发送到远端的服务器,在此函数进行处理
 * 
 * @param pServerSess GUI客户端的连接
 * @param msg 
 * @return true 成功处理,不需要发送到远端
 * @return false 没有处理,需要发送到远端处理
 */
bool CMediumServer::HandleSendForward(const std::shared_ptr<CServerSess>& pServerSess, const TransBaseMsg_t& msg)
{
	switch(msg.GetType())
	{
		case E_MsgType::GetFriendChatHistroyReq_Type:
		{
			GetFriendChatHistoryReq reqMsg;
			if (reqMsg.FromString(msg.to_string())) {
				HSF_GetFriendChatHistoryReq(pServerSess, reqMsg);
			}
		}break;
		case E_MsgType::GetGroupChatHistoryReq_Type:
		{
			GetGroupChatHistoryReq reqMsg;
			if (reqMsg.FromString(msg.to_string())) {
				HSF_GetGroupChatHistoryReq(pServerSess, reqMsg);
			}
			return true;
		}break;
		case E_MsgType::FileSendDataBeginReq_Type:
		{
			FileSendDataBeginReq reqMsg;
			if (reqMsg.FromString(msg.to_string())) {
				HSF_FileSendDataBeginReq(pServerSess, reqMsg);
			}
		}break;
		case E_MsgType::FriendChatSendTxtMsgReq_Type:
		{
			FriendChatSendTxtReqMsg reqMsg;
			if (reqMsg.FromString(msg.to_string())) {
				HSF_FriendChatSendTxtReqMsg(pServerSess, reqMsg);
			}
		}break;
		case E_MsgType::UserLoginReq_Type:
		{
			UserLoginReqMsg reqMsg;
			if (reqMsg.FromString(msg.to_string()))
			{
				HSF_UserLoginReq(pServerSess, reqMsg);
			}
		}break;
		case E_MsgType::NetFailedReport_Type:
		{
			HSF_NetFailedReq(pServerSess);
		}break;
		case E_MsgType::SendGroupTextMsgReq_Type:
		{
			SendGroupTextMsgReqMsg reqMsg;
			if (reqMsg.FromString(msg.to_string())) 
			{
				HSF_SendGroupTextMsgReqMsg(pServerSess, reqMsg);
			}
		}break;
		default:
		{
			//对于原始消息，原封不动的转发
			auto item = m_ForwardSessMap.find(pServerSess);
			if (item != m_ForwardSessMap.end())
			{
				auto pMsg = std::make_shared<TransBaseMsg_t>(msg.GetType(), msg.to_string());
				item->second->SendMsg(pMsg);
			}
			else
			{
				auto pClientSess = GetClientSess(pServerSess->UserId());
				if (pClientSess)
				{
					auto pMsg = std::make_shared<TransBaseMsg_t>(msg.GetType(), msg.to_string());
					pClientSess->SendMsg(pMsg);
				}
				else
				{
					return false;
				}
			}
		}break;
	}
	return true;
}


void CMediumServer::HSF_NetFailedReq(const std::shared_ptr<CServerSess>& pServerSess)
{
	LOG_WARN(ms_loger, "GUI User:{} Is OffLine [{} {}]",pServerSess->UserId(), __FILENAME__, __LINE__);
	auto item = m_userLoginMsgMap.find(pServerSess->UserName());
	if (item != m_userLoginMsgMap.end())
	{
		UserLogoutReqMsg reqMsg;
		reqMsg.m_eOsType = item->second.m_eOsType;
		reqMsg.m_strMsgId = m_httpServer->GenerateMsgId();
		reqMsg.m_strPassword = item->second.m_strPassword;
		reqMsg.m_strUserName = item->second.m_strUserName;
		auto pClientSess = GetClientSess(pServerSess->UserId());
		if (pClientSess)
		{
			pClientSess->SendMsg(&reqMsg);
		}
		else
		{
			LOG_ERR(ms_loger, "UserId:{} No Client Sess [{} {}]", pServerSess->UserId(), __FILENAME__, __LINE__);
		}
	}
	else
	{
		LOG_ERR(ms_loger, "UserName:{} No Login Req [{} {}]", pServerSess->UserId(), __FILENAME__, __LINE__);
	}
}

/**
 * @brief 根据用户ID获取到GUI客户端的连接
 * 
 * @param strUserId 用户ID
 * @return CServerSess_SHARED_PTR 
 */
CServerSess_SHARED_PTR CMediumServer::Get_GUI_Sess(const std::string strUserId)
{
	auto item = m_userId_ServerSessMap.find(strUserId);
	if (item != m_userId_ServerSessMap.end())
	{
		return item->second;
	}
	else
	{
		return nullptr;
	}
	/*auto pSess = GetClientSess(strUserId);
	if (pSess)
	{
		auto item = m_BackSessMap.find(pSess);
		if (item != m_BackSessMap.end())
		{
			return item->second;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}*/
}


/**
 * @brief 处理收到的好友聊天消息
 * 
 * @param pClientSess 和服务器的连接
 * @param reqMsg 收到的好友聊天消息
 */
void CMediumServer::HSB_FriendChatRecvTxtReq(const std::shared_ptr<CClientSess>& pClientSess, const FriendChatRecvTxtReqMsg reqMsg)
{
	{
		bool bWaitImage = false;
		bool bWaitFile = false;
		ChatMsgElemVec elemVec = MsgElemVec(reqMsg.m_chatMsg.m_strContext);
		for (const auto item : elemVec)
		{
			FileDownLoadReqMsg downMsg;
			downMsg.m_strUserId = reqMsg.m_chatMsg.m_strReceiverId;
			downMsg.m_strFriendId = reqMsg.m_chatMsg.m_strSenderId;
			downMsg.m_strMsgId = m_httpServer->GenerateMsgId();
			downMsg.m_strRelateMsgId = reqMsg.m_chatMsg.m_strChatMsgId;
			if (item.m_eType == CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE) 
			{
				downMsg.m_strFileName = item.m_strImageName;
				downMsg.m_eFileType = FILE_TYPE::FILE_TYPE_IMAGE;
				LOG_INFO(ms_loger, "Download File {} Begin [{} {}]", item.m_strImageName, __FILENAME__, __LINE__);
				pClientSess->SendMsg(&downMsg);
				bWaitImage = true;
			}
			else if (item.m_eType == CHAT_MSG_TYPE::E_CHAT_FILE_TYPE)
			{
				auto pGuiSess = Get_GUI_Sess(reqMsg.m_chatMsg.m_strReceiverId);
				if (pGuiSess)
				{
					FriendRecvFileMsgReqMsg fileReqMsg;
					{
						fileReqMsg.m_strUserId = reqMsg.m_chatMsg.m_strReceiverId;
						fileReqMsg.m_strFriendId = reqMsg.m_chatMsg.m_strSenderId;
						fileReqMsg.m_transMode = FILE_TRANS_TYPE::UDP_OFFLINE_MODE;
						fileReqMsg.m_strFileName = item.m_strImageName;
						fileReqMsg.m_strMsgId = reqMsg.m_strMsgId;
					}
					pGuiSess->SendMsg(&fileReqMsg);
				}
				bWaitFile = true;
			}
		}

		if (bWaitImage)
		{
			m_waitImageMsgMap.insert({ reqMsg.m_chatMsg.m_strChatMsgId,reqMsg });
			//FriendChatRecvTxtRspMsg rspMsg;
			//rspMsg.m_strUserId = reqMsg.m_chatMsg.m_strReceiverId;
			//rspMsg.m_strFriendId = reqMsg.m_chatMsg.m_strSenderId;
			//rspMsg.m_strMsgId = reqMsg.m_strMsgId;
			//rspMsg.m_strChatMsgId = reqMsg.m_chatMsg.m_strChatMsgId;
			//pClientSess->SendMsg(&rspMsg);
		}
		else if (bWaitFile)
		{
			auto pGuiSess = Get_GUI_Sess(reqMsg.m_chatMsg.m_strReceiverId);
			if (pGuiSess)
			{
				pGuiSess->SendMsg(&reqMsg);
				auto pMsgUtil = GetMsgPersisUtil(reqMsg.m_chatMsg.m_strReceiverId);
				if (pMsgUtil)
				{
					pMsgUtil->Save_FriendChatSendTxtRspMsg(reqMsg.m_chatMsg);
				}
				else
				{
					LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", reqMsg.m_chatMsg.m_strReceiverId, __FILENAME__, __LINE__);
				}
			}
			else
			{
				LOG_ERR(ms_loger, "User Id {} No Sess [{} {}]", reqMsg.m_chatMsg.m_strReceiverId, __FILENAME__, __LINE__);
			}
		}
		else	
		{
			auto pSess = Get_GUI_Sess(reqMsg.m_chatMsg.m_strReceiverId);
			if (pSess)
			{
				pSess->SendMsg(&reqMsg);
				auto pMsgUtil = GetMsgPersisUtil(reqMsg.m_chatMsg.m_strReceiverId);
				if(pMsgUtil)
				{
					pMsgUtil->Save_FriendChatSendTxtRspMsg(reqMsg.m_chatMsg);
				}
				else
				{
					LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", reqMsg.m_chatMsg.m_strReceiverId,__FILENAME__, __LINE__);
				}
			}
			else
			{
				LOG_ERR(ms_loger, "User Id {} No Sess [{} {}]", reqMsg.m_chatMsg.m_strReceiverId, __FILENAME__, __LINE__);
			}
			FriendChatRecvTxtRspMsg rspMsg;
			rspMsg.m_strUserId = reqMsg.m_chatMsg.m_strReceiverId;
			rspMsg.m_strFriendId = reqMsg.m_chatMsg.m_strSenderId;
			rspMsg.m_strMsgId = reqMsg.m_strMsgId;
			rspMsg.m_strChatMsgId = reqMsg.m_chatMsg.m_strChatMsgId;
			pClientSess->SendMsg(&rspMsg);
		}

	}

	//m_msgPersisUtil->Save_FriendChatSendTxtRspMsg(reqMsg.m_chatMsg);
}

void CMediumServer::HSB_NotifyGroupMsgReqMsg(const std::shared_ptr<CClientSess>& pClientSess, const NotifyGroupMsgReqMsg& reqMsg)
{
	NotifyGroupMsgRspMsg rspMsg;
	rspMsg.m_strGroupId = reqMsg.m_strGroupId;
	rspMsg.m_strUserId = reqMsg.m_strUserId;
	rspMsg.m_strMsgId = reqMsg.m_strMsgId;
	pClientSess->SendMsg(&rspMsg);
}

void CMediumServer::HSB_SendGroupTextMsgRspMsg(const std::shared_ptr<CClientSess>& pClientSess, const SendGroupTextMsgRspMsg& rspMsg)
{
	//SendGroupTextMsgRspMsg newRspMsg;
	////Create NewRspMsg
	//{
	//	ChatMsgElemVec oldVec = MsgElemVec(rspMsg.m_chatMsg.m_strContext);
	//	ChatMsgElemVec newVec;
	//	for (const auto item : oldVec)
	//	{
	//		if (item.m_eType == CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE)
	//		{
	//			std::string strImageName = m_fileUtil.GetCurDir() + rspMsg.m_chatMsg.m_strSenderId + "\\" + item.m_strImageName;
	//			ChatMsgElem elem;
	//			elem.m_eType = item.m_eType;
	//			elem.m_strImageName = strImageName;
	//			newVec.push_back(elem);
	//		}
	//		else
	//		{
	//			newVec.push_back(item);
	//		}
	//	}
	//	newRspMsg = rspMsg;
	//	newRspMsg.m_chatMsg.m_strContext = MsgElemVec(newVec);
	//}

	////Send To GUI
	//{
	//	auto pGuiSess = Get_GUI_Sess(pClientSess->UserId());
	//	if (pGuiSess)
	//	{
	//		pGuiSess->SendMsg(&newRspMsg);
	//	}
	//}

	////Save To DataBase
	//{
	//	try {
	//		auto pMsgUtil = GetMsgPersisUtil(newRspMsg.m_chatMsg.m_strSenderId);
	//		if (pMsgUtil)
	//		{
	//			pMsgUtil->Save_RecvGroupTextMsgReqMsg(newRspMsg);
	//		}
	//		else
	//		{
	//			LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", newRspMsg.m_chatMsg.m_strSenderId, __FILENAME__, __LINE__);
	//		}
	//	}
	//	catch (std::exception ex)
	//	{
	//		LOG_ERR(ms_loger, "Ex:{} [{} {}]", ex.what(), __FILENAME__, __LINE__);
	//	}
	//}
}


/**
 * @brief 处理发送的好友聊天消息的回复
 * 
 * @param pClientSess 和服务器的连接
 * @param rspMsg 好友聊天消息的回复
 */
void CMediumServer::HSB_FriendChatSendTxtRsp(const std::shared_ptr<CClientSess>& pClientSess, const FriendChatSendTxtRspMsg rspMsg)
{
	FriendChatSendTxtRspMsg newRspMsg;
	//Create NewRspMsg
	{
		ChatMsgElemVec oldVec = MsgElemVec(rspMsg.m_chatMsg.m_strContext);
		ChatMsgElemVec newVec;
		for (const auto item : oldVec)
		{
			if (item.m_eType == CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE)
			{
				std::string strImageName = GetUserImageDir(rspMsg.m_chatMsg.m_strSenderId) + item.m_strImageName;
				ChatMsgElem elem;
				elem.m_eType = item.m_eType;
				elem.m_strImageName = strImageName;
				newVec.push_back(elem);
			}
			else
			{
				newVec.push_back(item);
			}
		}
		newRspMsg = rspMsg;
		newRspMsg.m_chatMsg.m_strContext = MsgElemVec(newVec);
	}

	//Send To GUI
	{
		auto pGuiSess = Get_GUI_Sess(pClientSess->UserId());
		if (pGuiSess)
		{
			pGuiSess->SendMsg(&newRspMsg);
		}
	}

	//Save To DataBase
	{
		try {
			auto pMsgUtil = GetMsgPersisUtil(newRspMsg.m_chatMsg.m_strSenderId);
			if (pMsgUtil)
			{
				pMsgUtil->Save_FriendChatSendTxtRspMsg(newRspMsg.m_chatMsg);
			}
			else
			{
				LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", newRspMsg.m_chatMsg.m_strSenderId, __FILENAME__, __LINE__);
			}
		}
		catch (std::exception ex)
		{
			LOG_ERR(ms_loger, "Ex:{} [{} {}]", ex.what(), __FILENAME__, __LINE__);
		}
	}
}

void CMediumServer::HSB_RecvGroupTextMsgReqMsg(const std::shared_ptr<CClientSess>& pClientSess, const RecvGroupTextMsgReqMsg& reqMsg)
{
		bool bWaitImage = false;
		bool bWaitFile = false;
		ChatMsgElemVec elemVec = MsgElemVec(reqMsg.m_chatMsg.m_strContext);
		for (const auto item : elemVec)
		{
			FileDownLoadReqMsg downMsg;
			downMsg.m_strUserId = reqMsg.m_strUserId;
			downMsg.m_strFriendId = reqMsg.m_chatMsg.m_strGroupId;
			downMsg.m_strMsgId = m_httpServer->GenerateMsgId();
			downMsg.m_strRelateMsgId = reqMsg.m_strMsgId;
			if (item.m_eType == CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE)
			{
				downMsg.m_strFileName = item.m_strImageName;
				downMsg.m_eFileType = FILE_TYPE::FILE_TYPE_IMAGE;
				LOG_INFO(ms_loger, "Download File {} Begin [{} {}]", item.m_strImageName, __FILENAME__, __LINE__);
				pClientSess->SendMsg(&downMsg);
				bWaitImage = true;
			}
			else if (item.m_eType == CHAT_MSG_TYPE::E_CHAT_FILE_TYPE)
			{
				auto pGuiSess = Get_GUI_Sess(reqMsg.m_strUserId);
				if (pGuiSess)
				{
					FriendRecvFileMsgReqMsg fileReqMsg;
					{
						fileReqMsg.m_strUserId = reqMsg.m_strUserId;
						fileReqMsg.m_strFriendId = reqMsg.m_chatMsg.m_strGroupId;
						fileReqMsg.m_transMode = FILE_TRANS_TYPE::UDP_ONLINE_MEDIUM_MODE;
						fileReqMsg.m_strFileName = item.m_strImageName;
						fileReqMsg.m_strMsgId = reqMsg.m_strMsgId;
					}
					pGuiSess->SendMsg(&fileReqMsg);
				}
				bWaitFile = true;
			}
		}

		if (bWaitImage)
		{
			m_groupWaitImageMsgMap.insert({ reqMsg.m_strMsgId,reqMsg });
			//FriendChatRecvTxtRspMsg rspMsg;
			//rspMsg.m_strUserId = reqMsg.m_chatMsg.m_strReceiverId;
			//rspMsg.m_strFriendId = reqMsg.m_chatMsg.m_strSenderId;
			//rspMsg.m_strMsgId = reqMsg.m_strMsgId;
			//rspMsg.m_strChatMsgId = reqMsg.m_chatMsg.m_strChatMsgId;
			//pClientSess->SendMsg(&rspMsg);
		}
		else if (bWaitFile)
		{
			/*auto pGuiSess = Get_GUI_Sess(reqMsg.m_chatMsg.m_strReceiverId);
			if (pGuiSess)
			{
				pGuiSess->SendMsg(&reqMsg);
				auto pMsgUtil = GetMsgPersisUtil(reqMsg.m_chatMsg.m_strReceiverId);
				if (pMsgUtil)
				{
					pMsgUtil->Save_FriendChatSendTxtRspMsg(reqMsg.m_chatMsg);
				}
				else
				{
					LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", reqMsg.m_chatMsg.m_strReceiverId, __FILENAME__, __LINE__);
				}
			}
			else
			{
				LOG_ERR(ms_loger, "User Id {} No Sess [{} {}]", reqMsg.m_chatMsg.m_strReceiverId, __FILENAME__, __LINE__);
			}*/
		}
		else
		{
			auto pSess = Get_GUI_Sess(reqMsg.m_strUserId);
			if (pSess)
			{
				pSess->SendMsg(&reqMsg);
				auto pMsgUtil = GetMsgPersisUtil(reqMsg.m_strUserId);
				if (pMsgUtil)
				{
					pMsgUtil->Save_RecvGroupTextMsgReqMsg(reqMsg);
					//pMsgUtil->Save_FriendChatSendTxtRspMsg(reqMsg.m_chatMsg);
				}
				else
				{
					LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", reqMsg.m_strUserId, __FILENAME__, __LINE__);
				}
			}
			else
			{
				LOG_ERR(ms_loger, "User Id {} No Sess [{} {}]", reqMsg.m_strUserId, __FILENAME__, __LINE__);
			}
			RecvGroupTextMsgRspMsg rspMsg;
			rspMsg.m_strUserId = reqMsg.m_strUserId;
			rspMsg.m_strGroupId = reqMsg.m_chatMsg.m_strGroupId;
			rspMsg.m_strMsgId = reqMsg.m_strMsgId;
			rspMsg.m_strChatMsgId = reqMsg.m_chatMsg.m_strChatMsgId;
			pClientSess->SendMsg(&rspMsg);
		}
}

/**
 * @brief 处理服务器发送过来的文件数据开始请求
 * 
 * @param pClientSess 和服务器的连接
 * @param reqMsg 发送文件数据开始消息
 */
void CMediumServer::HSB_FileSendDataBeginReq(const std::shared_ptr<CClientSess>& pClientSess, const FileSendDataBeginReq reqMsg)
{
	FileSendDataBeginRsp rspMsg;
	std::string strFileName;
	auto pMsgUtil = GetMsgPersisUtil(reqMsg.m_strUserId);
	if (pMsgUtil)
	{
		strFileName = pMsgUtil->Get_FileByHash(reqMsg.m_strFileHash);
	}
	else
	{
		LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", reqMsg.m_strUserId, __FILENAME__, __LINE__);
	}



	//auto item = m_fileHashMsgIdMap.find(reqMsg.m_strFileHash);
	//auto hashItem = std::find(m_fileHashTransVec.begin(), m_fileHashTransVec.end(), reqMsg.m_strFileHash);
	if(strFileName.empty())
	{
		m_fileHashTransVec.push_back(reqMsg.m_strFileHash);
		rspMsg.m_strMsgId = reqMsg.m_strMsgId;
		rspMsg.m_errCode = ERROR_CODE_TYPE::E_CODE_SUCCEED;
		rspMsg.m_strFileName = reqMsg.m_strFileName;
		rspMsg.m_strUserId = reqMsg.m_strUserId;
		rspMsg.m_strFriendId = reqMsg.m_strFriendId;
		rspMsg.m_nFileId = reqMsg.m_nFileId;
		rspMsg.m_eFileType = reqMsg.m_eFileType;
		pClientSess->SendMsg(&rspMsg);
		std::string strFileName = m_fileUtil.GetCurDir() + pClientSess->UserId() + "\\";
		m_fileUtil.CreateFolder(strFileName);
		strFileName += m_fileUtil.GetFileNameFromPath(reqMsg.m_strFileName);
		if (m_fileUtil.OpenWriteFile(reqMsg.m_nFileId + 1, strFileName))
		{
			LOG_INFO(ms_loger, "{} Open File Succeed:{} [{} {}]", reqMsg.m_strUserId, strFileName, __FILENAME__, __LINE__);
		}
		else
		{
			LOG_ERR(ms_loger, "{} Open File Failed:{} [{} {}]", reqMsg.m_strUserId, strFileName, __FILENAME__, __LINE__);
		}
	}
	else
	{

		rspMsg.m_strMsgId = reqMsg.m_strMsgId;
		rspMsg.m_errCode = ERROR_CODE_TYPE::E_CODE_FILE_TRANSING;
		rspMsg.m_strFileName = reqMsg.m_strFileName;
		rspMsg.m_strUserId = reqMsg.m_strUserId;
		rspMsg.m_strFriendId = reqMsg.m_strFriendId;
		rspMsg.m_nFileId = reqMsg.m_nFileId;
		rspMsg.m_eFileType = reqMsg.m_eFileType;
		pClientSess->SendMsg(&rspMsg);

		auto hashItem = m_fileHashMsgIdMap.find(reqMsg.m_strFileHash);
		if (hashItem != m_fileHashMsgIdMap.end())
		{
			{
				auto waitItem = m_waitImageMsgMap.find(hashItem->second);
				if (waitItem != m_waitImageMsgMap.end())
				{
					ChatMsgElemVec oldMsgVec = MsgElemVec(waitItem->second.m_chatMsg.m_strContext);
					ChatMsgElemVec newVec;
					for (const auto item : oldMsgVec)
					{
						if (item.m_eType == CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE) {
							ChatMsgElem elem;
							elem.m_eType = item.m_eType;
							elem.m_strImageName = strFileName;
							newVec.push_back(elem);
						}
						else {
							newVec.push_back(item);
						}
					}
					waitItem->second.m_chatMsg.m_strContext = MsgElemVec(newVec);
					auto pMsg = std::make_shared<TransBaseMsg_t>(waitItem->second.GetMsgType(), waitItem->second.ToString());

					{
						auto pGuiSess = Get_GUI_Sess(pClientSess->UserId());
						if (pGuiSess)
						{
							pGuiSess->SendMsg(pMsg);
						}
					}
					{
						FriendChatRecvTxtRspMsg rspMsg;
						rspMsg.m_strMsgId = waitItem->second.m_strMsgId;
						rspMsg.m_strFriendId = waitItem->second.m_chatMsg.m_strSenderId;
						rspMsg.m_strUserId = waitItem->second.m_chatMsg.m_strReceiverId;
						rspMsg.m_strChatMsgId = waitItem->second.m_chatMsg.m_strChatMsgId;
						pClientSess->SendMsg(&rspMsg);
					}
					m_waitImageMsgMap.erase(hashItem->second);
				}
			}
				{
					{
						auto waitItem = m_groupWaitImageMsgMap.find(hashItem->second);
						if (waitItem != m_groupWaitImageMsgMap.end())
						{
							ChatMsgElemVec oldMsgVec = MsgElemVec(waitItem->second.m_chatMsg.m_strContext);
							ChatMsgElemVec newVec;
							for (const auto item : oldMsgVec)
							{
								if (item.m_eType == CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE) {
									ChatMsgElem elem;
									elem.m_eType = item.m_eType;
									elem.m_strImageName = strFileName;
									newVec.push_back(elem);
								}
								else {
									newVec.push_back(item);
								}
							}
							waitItem->second.m_chatMsg.m_strContext = MsgElemVec(newVec);
							auto pMsg = std::make_shared<TransBaseMsg_t>(waitItem->second.GetMsgType(), waitItem->second.ToString());

							{
								auto pGuiSess = Get_GUI_Sess(pClientSess->UserId());
								if (pGuiSess)
								{
									pGuiSess->SendMsg(pMsg);
								}
							}
							{
								RecvGroupTextMsgRspMsg rspMsg;
								rspMsg.m_strMsgId = waitItem->second.m_strMsgId;
								rspMsg.m_strGroupId = waitItem->second.m_chatMsg.m_strGroupId;
								rspMsg.m_strUserId = waitItem->second.m_strUserId;
								rspMsg.m_strChatMsgId = waitItem->second.m_chatMsg.m_strChatMsgId;
								pClientSess->SendMsg(&rspMsg);
							}
							m_groupWaitImageMsgMap.erase(hashItem->second);
						}
					}
				}
		}
	}
}

/**
 * @brief 处理服务器发送过来的文件下载回复消息
 * 
 * @param pClientSess 客户端连接
 * @param rspMsg 文件下载回复消息
 */
void CMediumServer::HSB_FileDownLoadRsp(const std::shared_ptr<CClientSess>& pClientSess, const FileDownLoadRspMsg rspMsg)
{
	if(rspMsg.m_errCode == ERROR_CODE_TYPE::E_CODE_SUCCEED)
	{
		{
			auto msgItem = m_waitImageMsgMap.find(rspMsg.m_strRelateMsgId);
			if (msgItem != m_waitImageMsgMap.end())
			{
				std::string strFileName;// = m_msgPersisUtil->Get_FileByHash(rspMsg.m_strFileHash);

				{
					auto pMsgUtil = GetMsgPersisUtil(rspMsg.m_strUserId);
					if (pMsgUtil)
					{
						strFileName = pMsgUtil->Get_FileByHash(rspMsg.m_strFileHash);
					}
					else
					{
						LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", rspMsg.m_strUserId, __FILENAME__, __LINE__);
					}
				}
				bool bFileExit = m_fileUtil.IsFileExist(strFileName);
				if (!bFileExit)
				{
					//m_msgPersisUtil->Delete_FileByHash(rspMsg.m_strFileHash);
					auto pMsgUtil = GetMsgPersisUtil(rspMsg.m_strUserId);
					if (pMsgUtil)
					{
						pMsgUtil->Delete_FileByHash(rspMsg.m_strFileHash);
					}
					else
					{
						LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", rspMsg.m_strUserId, __FILENAME__, __LINE__);
					}
				}
				if (strFileName.empty() || !bFileExit)
				{
					m_fileHashMsgIdMap.insert({ rspMsg.m_strFileHash,rspMsg.m_strRelateMsgId });
				}
				else
				{
					ChatMsgElemVec oldMsgVec = MsgElemVec(msgItem->second.m_chatMsg.m_strContext);
					ChatMsgElemVec newVec;
					for (const auto item : oldMsgVec)
					{
						if (item.m_eType == CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE) {
							ChatMsgElem elem;
							elem.m_eType = item.m_eType;
							elem.m_strImageName = strFileName;
							newVec.push_back(elem);
						}
						else {
							newVec.push_back(item);
						}
					}
					msgItem->second.m_chatMsg.m_strContext = MsgElemVec(newVec);
					//SendBack
					{

						auto pGuiSess = Get_GUI_Sess(pClientSess->UserId());
						if (pGuiSess)
						{
							pGuiSess->SendMsg(&(msgItem->second));
						}
					}
					//SendForward
					{
						FriendChatRecvTxtRspMsg rspMsg;
						rspMsg.m_strMsgId = msgItem->second.m_strMsgId;
						rspMsg.m_strFriendId = msgItem->second.m_chatMsg.m_strSenderId;
						rspMsg.m_strUserId = msgItem->second.m_chatMsg.m_strReceiverId;
						rspMsg.m_strChatMsgId = msgItem->second.m_chatMsg.m_strChatMsgId;
						pClientSess->SendMsg(&rspMsg);
					}
					m_waitImageMsgMap.erase(rspMsg.m_strRelateMsgId);
				}
			}
		}

		{
			auto msgItem = m_groupWaitImageMsgMap.find(rspMsg.m_strRelateMsgId);
			if (msgItem != m_groupWaitImageMsgMap.end())
			{
				std::string strFileName;// = m_msgPersisUtil->Get_FileByHash(rspMsg.m_strFileHash);

				{
					auto pMsgUtil = GetMsgPersisUtil(rspMsg.m_strUserId);
					if (pMsgUtil)
					{
						strFileName = pMsgUtil->Get_FileByHash(rspMsg.m_strFileHash);
					}
					else
					{
						LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", rspMsg.m_strUserId, __FILENAME__, __LINE__);
					}
				}
				bool bFileExit = m_fileUtil.IsFileExist(strFileName);
				if (!bFileExit)
				{
					//m_msgPersisUtil->Delete_FileByHash(rspMsg.m_strFileHash);
					auto pMsgUtil = GetMsgPersisUtil(rspMsg.m_strUserId);
					if (pMsgUtil)
					{
						pMsgUtil->Delete_FileByHash(rspMsg.m_strFileHash);
					}
					else
					{
						LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", rspMsg.m_strUserId, __FILENAME__, __LINE__);
					}
				}
				if (strFileName.empty() || !bFileExit)
				{
					m_fileHashMsgIdMap.insert({ rspMsg.m_strFileHash,rspMsg.m_strRelateMsgId });
				}
				else
				{
					ChatMsgElemVec oldMsgVec = MsgElemVec(msgItem->second.m_chatMsg.m_strContext);
					ChatMsgElemVec newVec;
					for (const auto item : oldMsgVec)
					{
						if (item.m_eType == CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE) {
							ChatMsgElem elem;
							elem.m_eType = item.m_eType;
							elem.m_strImageName = strFileName;
							newVec.push_back(elem);
						}
						else {
							newVec.push_back(item);
						}
					}
					msgItem->second.m_chatMsg.m_strContext = MsgElemVec(newVec);
					//SendBack
					{

						auto pGuiSess = Get_GUI_Sess(pClientSess->UserId());
						if (pGuiSess)
						{
							pGuiSess->SendMsg(&(msgItem->second));
						}
					}
					//SendForward
					{
						RecvGroupTextMsgRspMsg rspMsg;
						rspMsg.m_strMsgId = msgItem->second.m_strMsgId;
						rspMsg.m_strUserId = msgItem->second.m_strUserId;
						rspMsg.m_strGroupId = msgItem->second.m_chatMsg.m_strGroupId;
						rspMsg.m_strChatMsgId = msgItem->second.m_chatMsg.m_strChatMsgId;
						pClientSess->SendMsg(&rspMsg);
					}
					m_groupWaitImageMsgMap.erase(rspMsg.m_strRelateMsgId);
				}
			}
		}
	}
	else
	{
		{
			auto msgItem = m_waitImageMsgMap.find(rspMsg.m_strRelateMsgId);
			if (msgItem != m_waitImageMsgMap.end())
			{
				FriendChatRecvTxtRspMsg sendRspMsg;
				sendRspMsg.m_strMsgId = msgItem->second.m_strMsgId;
				sendRspMsg.m_strFriendId = msgItem->second.m_chatMsg.m_strSenderId;
				sendRspMsg.m_strUserId = msgItem->second.m_chatMsg.m_strReceiverId;
				sendRspMsg.m_strChatMsgId = msgItem->second.m_chatMsg.m_strChatMsgId;
				pClientSess->SendMsg(&sendRspMsg);
				m_waitImageMsgMap.erase(rspMsg.m_strRelateMsgId);
			}
		}
		{
			auto msgItem = m_groupWaitImageMsgMap.find(rspMsg.m_strRelateMsgId);
			if (msgItem != m_groupWaitImageMsgMap.end())
			{
				RecvGroupTextMsgRspMsg sendRspMsg;
				sendRspMsg.m_strMsgId = msgItem->second.m_strMsgId;
				sendRspMsg.m_strChatMsgId = msgItem->second.m_chatMsg.m_strChatMsgId;
				sendRspMsg.m_strUserId = msgItem->second.m_strUserId;
				sendRspMsg.m_strGroupId = msgItem->second.m_chatMsg.m_strGroupId;
				pClientSess->SendMsg(&sendRspMsg);
				m_waitImageMsgMap.erase(rspMsg.m_strRelateMsgId);
			}
		}
	}
}

/**
 * @brief 处理发送数据开始的回复消息,在文件上传的时候使用
 * 
 * @param pClientSess 和服务器的连接会话
 * @param rspMsg 发送文件数据开始回复消息
 */
void CMediumServer::HSB_FileSendDataBeginRsp(const std::shared_ptr<CClientSess>& pClientSess, const FileSendDataBeginRsp rspMsg)
{
	if (rspMsg.m_errCode == ERROR_CODE_TYPE::E_CODE_SUCCEED)
	{
		//int nFileId = static_cast<int>(time(nullptr));
		int nFileSize = 0;
		std::string strImageName = m_fileUtil.GetCurDir() + rspMsg.m_strUserId + "\\" + rspMsg.m_strFileName;
		m_fileUtil.GetFileSize(nFileSize, strImageName);
		if (m_fileUtil.OpenReadFile(rspMsg.m_nFileId, strImageName)) {
			FileDataSendReqMsg sendReqMsg;
			sendReqMsg.m_strMsgId = m_httpServer->GenerateMsgId();
			sendReqMsg.m_strFriendId = rspMsg.m_strFriendId;
			sendReqMsg.m_strUserId = rspMsg.m_strUserId;
			sendReqMsg.m_nFileId = rspMsg.m_nFileId;

			sendReqMsg.m_nDataTotalCount = nFileSize / 1024 + (nFileSize % 1024 == 0 ? 0 : 1);
			sendReqMsg.m_nDataIndex = 1;
			sendReqMsg.m_nDataLength = 0;
			m_fileUtil.OnReadData(sendReqMsg.m_nFileId, sendReqMsg.m_szData, sendReqMsg.m_nDataLength, 1024);
			pClientSess->SendMsg(&sendReqMsg);
			//auto pUdpSess = GetUdpSess(sendReqMsg.m_strUserId);
			//if (pUdpSess)
			//{
			//	pUdpSess->sendToServer(&sendReqMsg);
			//}
			//else
			//{
			//	//LOG_ERR(ms_loger, "UDP Sess Failed:{}", sendReqMsg.m_strFromId);
			//}
		}
	}
	else {
		std::string strImageName = m_fileUtil.GetCurDir() + rspMsg.m_strUserId + "\\" + rspMsg.m_strFileName;
		std::string strFileHash = m_fileUtil.CalcHash(strImageName);
		{
			auto item = m_SendWaitMsgMap.find(strFileHash);
			if (item != m_SendWaitMsgMap.end()) {
				pClientSess->SendMsg(&(item->second));
				m_SendWaitMsgMap.erase(strFileHash);
			}
		}
		{
			auto item = m_groupSendWaitMsgMap.find(strFileHash);
			if (item != m_groupSendWaitMsgMap.end())
			{
				pClientSess->SendMsg(&(item->second));
				m_groupSendWaitMsgMap.erase(strFileHash);
			}
		}
	}
}

/**
 * @brief 处理用户登录回复消息
 * 
 * @param pClientSess 和服务器的连接会话
 * @param rspMsg 登录回复消息
 */
void CMediumServer::HSB_UserLoginRsp(const std::shared_ptr<CClientSess>& pClientSess, const UserLoginRspMsg rspMsg) {
	if (rspMsg.m_eErrCode == ERROR_CODE_TYPE::E_CODE_SUCCEED)
	{
		m_userStateMap.erase(rspMsg.m_strUserId);
		m_userStateMap.insert({ rspMsg.m_strUserId,CLIENT_SESS_STATE::SESS_LOGIN_FINISHED });
		//ForwardMap 和 BackMap的对应关系删除,移动到UserId的Map
		{
			
			auto item = m_BackSessMap.find(pClientSess);
			if (item != m_BackSessMap.end())
			{
				//对Sess设置UserId和UserName
				{
					item->second->SetUserId(rspMsg.m_strUserId);
					item->second->SetUserName(rspMsg.m_strUserName);
				}

				//设置UserId和Sess的对应关系
				{
					m_userId_ServerSessMap.erase(rspMsg.m_strUserId);
					m_userId_ServerSessMap.insert({ rspMsg.m_strUserId,item->second });
				}

				//ForwardMap 和 BackMap的对应关系删除
				{
					m_ForwardSessMap.erase(item->second);
					m_BackSessMap.erase(pClientSess);
				}
			}
			pClientSess->SetUserId(rspMsg.m_strUserId);
			pClientSess->SetUserName(rspMsg.m_strUserName);
			m_userId_ClientSessMap.erase(rspMsg.m_strUserId);
			m_userId_ClientSessMap.insert({ rspMsg.m_strUserId,pClientSess });
		}
		//UserId和UserName的对应关系
		{
			m_userName_UserIdMap.erase(rspMsg.m_strUserName);
			m_userName_UserIdMap.insert({ rspMsg.m_strUserName, rspMsg.m_strUserId });

			m_userId_UserNameMap.erase(rspMsg.m_strUserId);
			m_userId_UserNameMap.insert({ rspMsg.m_strUserId,rspMsg.m_strUserName });
		}
		//添加UDPSess
		{
			auto pUdpSess = GetUdpSess(rspMsg.m_strUserId);
			if (pUdpSess == nullptr)
			{
				auto pNewUdp = CreateUdpSess();
				pNewUdp->SetUserId(rspMsg.m_strUserId);
				m_userUdpSessMap.insert({ rspMsg.m_strUserId,pNewUdp });
			}
		}

		//添加目录
		{
			std::string strMainFolder = GetUserMainFolder(rspMsg.m_strUserId);
			if (!m_fileUtil.IsFolder(strMainFolder))
			{
				m_fileUtil.CreateFolder(strMainFolder);
			}

			{
				std::string strImageFolder = GetUserImageDir(rspMsg.m_strUserId);
				if (!m_fileUtil.IsFolder(strImageFolder))
				{
					m_fileUtil.CreateFolder(strImageFolder);
				}
			}
			{
				std::string strFileFolder = GetUserFileDir(rspMsg.m_strUserId);
				if (!m_fileUtil.IsFolder(strFileFolder))
				{
					m_fileUtil.CreateFolder(strFileFolder);
				}
			}
		}
		{
			auto pOldUtil = GetMsgPersisUtil(rspMsg.m_strUserId);
			if (pOldUtil == nullptr)
			{
				auto pMsgUtil = std::make_shared<CMsgPersistentUtil>();
				std::string strDbFileName = GetUserDataBaseFileName(rspMsg.m_strUserId);
				if (pMsgUtil->InitDataBase(strDbFileName))
				{
					m_UserId_MsgPersistentUtilMap.insert({ rspMsg.m_strUserId,pMsgUtil });
				}
				else
				{

				}
			}
		}

		//获取好友请求
		{
			GetFriendListReqMsg reqMsg;
			reqMsg.m_strMsgId = m_httpServer->GenerateMsgId();
			reqMsg.m_strUserId = rspMsg.m_strUserId;
			pClientSess->SendMsg(&reqMsg);
		}

		{
			auto pGuiSess = Get_GUI_Sess(rspMsg.m_strUserId);
			if (pGuiSess)
			{
				pGuiSess->SendMsg(&rspMsg);
			}
		}
		
		//移除重连情况
		{
			auto item = m_reConnectSessMap.find(pClientSess);
			if (item != m_reConnectSessMap.end())
			{
				LOG_WARN(ms_loger, "UserName:{} UserId:{} ReConnect Use: {} seconds[{} {}]", pClientSess->UserName(), pClientSess->UserId(), time(nullptr) - item->second, __FILENAME__, __LINE__);
			}
			m_reConnectSessMap.erase(pClientSess);
		}
	}

}

/**
 * @brief 处理用户退出登录回复消息
 * 
 * @param pClientSess 和服务器的连接
 * @param rspMsg 用户退出登录回复消息
 */
void CMediumServer::HSB_UserLogoutRsp(const std::shared_ptr<CClientSess>& pClientSess, const UserLogoutRspMsg rspMsg) {
	if (rspMsg.m_eErrCode == ERROR_CODE_TYPE::E_CODE_SUCCEED) {
		LOG_ERR(ms_loger, "User: {} Handle User Logout Err [{} {}]", rspMsg.m_strUserName, __FILENAME__, __LINE__);

		m_userLoginMsgMap.erase(rspMsg.m_strUserName);
		std::string strUserId = GetUserId(rspMsg.m_strUserName);
		m_userId_ClientSessMap.erase(strUserId);
		m_userStateMap.erase(strUserId);
		m_userStateMap.insert({ strUserId,CLIENT_SESS_STATE::SESS_LOGOUT });
		m_userUdpSessMap.erase(strUserId);

		{
			auto item = m_BackSessMap.find(pClientSess);
			if (item != m_BackSessMap.end())
			{
				m_ForwardSessMap.erase(item->second);
				m_BackSessMap.erase(pClientSess);
				item->second->StopConnect();
			}
		}

		pClientSess->StopConnect();
	}
	else
	{
		LOG_ERR(ms_loger, "User: {} Handle User Logout Err [{} {}]", rspMsg.m_strUserName, __FILENAME__, __LINE__);
	}
}


/**
 * @brief 响应GUI客户端断开连接
 * 
 * @param pServerSess 客户端连接会话
 */
void CMediumServer::ServerSessClose(const CServerSess_SHARED_PTR pServerSess)
{
	auto itemForwad = m_ForwardSessMap.find(pServerSess);
	if (itemForwad != m_ForwardSessMap.end()) {
		m_BackSessMap.erase(itemForwad->second);
		m_ForwardSessMap.erase(pServerSess);
	}
	else
	{
		LOG_ERR(ms_loger, "Could Not Close Server Sess [{} {}]",__FILENAME__,__LINE__);
	}

	//Clear UserID
	{
		std::string strUserId = pServerSess->UserId();
		m_userId_UserNameMap.erase(strUserId);
		m_userIdUdpAddrMap.erase(strUserId);
		m_userUdpSessMap.erase(strUserId);
		m_userId_ClientSessMap.erase(strUserId);
		m_userKeepAliveMap.erase(strUserId);
	}
	//Clear UserName
	{
		std::string strUserName = pServerSess->UserName();
		m_userName_UserIdMap.erase(strUserName);
	}
}

/**
 * @brief 响应发送消息到服务器失败
 * 
 * @param pClientSess 和服务器的连接
 */
void CMediumServer::HSB_NetFailed(const std::shared_ptr<CClientSess>& pClientSess)
{
	auto item = m_userStateMap.find(pClientSess->UserId());
	if (item != m_userStateMap.end())
	{
		if (item->second == CLIENT_SESS_STATE::SESS_LOGIN_FINISHED)
		{
			m_userStateMap.erase(pClientSess->UserId());
			m_userStateMap.insert({ pClientSess->UserId(),CLIENT_SESS_STATE::SESS_LOGIN_SEND });
			m_reConnectSessMap.insert({ pClientSess, time(nullptr) });
			LOG_ERR(ms_loger, "[{} {}]", __FILENAME__, __LINE__);

		}
		else if(item->second == CLIENT_SESS_STATE::SESS_LOGOUT)
		{
			m_reConnectSessMap.erase(pClientSess);
			pClientSess->StopConnect();
			LOG_ERR(ms_loger, "[{} {}]", __FILENAME__, __LINE__);
		}
	}
	else
	{
		LOG_ERR(ms_loger, "[{} {}]", __FILENAME__, __LINE__);
	}
}

void CMediumServer::HSB_FriendRecvFileMsgReq(const std::shared_ptr<CClientSess>& pClientSess, const FriendRecvFileMsgReqMsg reqMsg)
{
	/*FriendRecvFileMsgRspMsg rspMsg;
	rspMsg.m_strMsgId = reqMsg.m_strMsgId;
	rspMsg.m_strFromId = reqMsg.m_strToId;
	rspMsg.m_strToId = reqMsg.m_strFromId;
	rspMsg.m_strFileName = reqMsg.m_strFileName;
	rspMsg.m_eOnlineType = reqMsg.m_eOnlineType;
	rspMsg.m_eOption = E_FRIEND_OPTION::E_AGREE_ADD;
	rspMsg.m_transMode = reqMsg.m_transMode;
	pClientSess->SendMsg(&rspMsg);*/

}
/**
 * @brief 处理查询UDP地址的回复消息
 * 
 * @param pClientSess 和服务器的连接
 * @param rspMsg UDP地址回复消息
 */
void CMediumServer::HSB_QueryUserUdpAddrRsp(const std::shared_ptr<CClientSess>& pClientSess, const QueryUserUdpAddrRspMsg rspMsg)
{
	if(pClientSess)
	{

	}
	if (ERROR_CODE_TYPE::E_CODE_SUCCEED == rspMsg.m_errCode)
	{
		m_userIdUdpAddrMap.erase(rspMsg.m_strUdpUserId);
		m_userIdUdpAddrMap.insert({ rspMsg.m_strUdpUserId,rspMsg.m_udpEndPt });
	}
	else
	{
		//TODO再次获取时间
		auto pMsg = std::make_shared<QueryUserUdpAddrReqMsg>();
		pMsg->m_strUserId = rspMsg.m_strUserId;
		pMsg->m_strUdpUserId = rspMsg.m_strUdpUserId;
		auto item = m_RecvWaitMsgMap.find(pMsg->m_strUserId);
		if (item != m_RecvWaitMsgMap.end())
		{
			m_RecvWaitMsgMap[rspMsg.m_strUserId].push_back(pMsg);
		}
		else
		{
			std::vector<std::shared_ptr<BaseMsg>> mapVec;
			mapVec.push_back(pMsg);
			m_RecvWaitMsgMap.insert({ rspMsg.m_strUserId,mapVec });
		}
	}
}

/**
 * @brief 部分消息不需要返回给GUI客户端，在此函数进行处理
 * 
 * @param pClientSess TCP链接会话
 * @param msg 收到的消息
 * @return true 处理成功
 * @return false 处理失败
 */
bool CMediumServer::HandleSendBack(const std::shared_ptr<CClientSess>& pClientSess, const TransBaseMsg_t& msg)
{
	switch (msg.GetType())
	{
	case E_MsgType::FriendChatReceiveTxtMsgReq_Type:
	{
		FriendChatRecvTxtReqMsg reqMsg;
		if (reqMsg.FromString(msg.to_string())) {
			HSB_FriendChatRecvTxtReq(pClientSess, reqMsg);
		}
	}break;
	case E_MsgType::FriendChatSendTxtMsgRsp_Type:
	{
		FriendChatSendTxtRspMsg rspMsg;
		if (rspMsg.FromString(msg.to_string())) {
			HSB_FriendChatSendTxtRsp(pClientSess, rspMsg);
		}
	}break;
	case E_MsgType::SendGroupTextMsgRsp_Type:
	{
		SendGroupTextMsgRspMsg rspMsg;
		if (rspMsg.FromString(msg.to_string())) {
			HSB_SendGroupTextMsgRspMsg(pClientSess, rspMsg);
		}
	}break;
	case E_MsgType::RecvGroupTextMsgReq_Type:
	{
		RecvGroupTextMsgReqMsg reqMsg;
		if (reqMsg.FromString(msg.to_string())) {
			HSB_RecvGroupTextMsgReqMsg(pClientSess,reqMsg);
		}
	}break;
	case E_MsgType::FileSendDataBeginReq_Type:
	{
		FileSendDataBeginReq reqMsg;
		if (reqMsg.FromString(msg.to_string())) {
			HSB_FileSendDataBeginReq(pClientSess, reqMsg);
		}
	}break;
	case E_MsgType::FileSendDataBeginRsp_Type:
	{
		FileSendDataBeginRsp rspMsg;
		if (rspMsg.FromString(msg.to_string())) {
			HSB_FileSendDataBeginRsp(pClientSess, rspMsg);
		}
	}break;
	case E_MsgType::FileVerifyReq_Type:
	{
		FileVerifyReqMsg reqMsg;
		if (reqMsg.FromString(msg.to_string())) {
			HandleFileVerifyReq(reqMsg);
		}
	}break;
	case E_MsgType::UserLoginRsp_Type:
	{
		UserLoginRspMsg rspMsg;
		if (rspMsg.FromString(msg.to_string())) {
			HSB_UserLoginRsp(pClientSess, rspMsg);
		}
	}break;
	case E_MsgType::UserLogoutRsp_Type:
	{
		UserLogoutRspMsg rspMsg;
		if (rspMsg.FromString(msg.to_string())) {
			HSB_UserLogoutRsp(pClientSess, rspMsg);
		}
	}break;
	case E_MsgType::NetFailedReport_Type:
	{
		HSB_NetFailed(pClientSess);
	}break;
	case E_MsgType::FileDownLoadRsp_Type:
	{
		FileDownLoadRspMsg rspMsg;
		if (rspMsg.FromString(msg.to_string())) {
			HSB_FileDownLoadRsp(pClientSess,rspMsg);
		}
	}break;
	case E_MsgType::FriendRecvFileMsgReq_Type:
	{
		FriendRecvFileMsgReqMsg reqMsg;
		if (reqMsg.FromString(msg.to_string())) {
			HSB_FriendRecvFileMsgReq(pClientSess, reqMsg);
		}
	}break;
	case E_MsgType::NetRecoverReport_Type: {
		auto stateItem = m_userStateMap.find(pClientSess->UserId());
		if(stateItem != m_userStateMap.end()){
			if (stateItem->second == CLIENT_SESS_STATE::SESS_LOGIN_SEND) {
				auto item = m_userLoginMsgMap.find(pClientSess->UserName());
				if (item != m_userLoginMsgMap.end())
				{
					pClientSess->SendMsg(&(item->second));
				}
			}
		}
	}break;
	case E_MsgType::KeepAliveRsp_Type:
	{
		KeepAliveRspMsg rspMsg;
		if (rspMsg.FromString(msg.to_string()))
		{
			KeepAliveReqMsg reqMsg;
			reqMsg.m_strClientId = pClientSess->UserId();
			{
				auto pUdpSess = GetUdpSess(pClientSess->UserId());
				if (nullptr != pUdpSess)
				{
					pUdpSess->sendToServer(&reqMsg);
				}
			}
		}
	}break;
	case E_MsgType::KeepAliveReq_Type:
	{
		KeepAliveReqMsg reqMsg;
		if (reqMsg.FromString(msg.to_string()))
		{
			KeepAliveReqMsg reqMsg2;
			reqMsg2.m_strClientId = pClientSess->UserId();
			{
				auto pUdpSess = GetUdpSess(pClientSess->UserId());
				if (nullptr != pUdpSess)
				{
					pUdpSess->sendToServer(&reqMsg2);
				}
			}
		}
	}break;
	case E_MsgType::QueryUserUdpAddrRsp_Type:
	{
		QueryUserUdpAddrRspMsg rspMsg;
		if (rspMsg.FromString(msg.to_string())) {
			HSB_QueryUserUdpAddrRsp(pClientSess, rspMsg);
		}
	}break;
	case E_MsgType::FriendUnReadMsgNotifyReq_Type:
	{
		FriendUnReadNotifyReqMsg reqMsg;
		if (reqMsg.FromString(msg.to_string())) {
			FriendUnReadNotifyRspMsg rspMsg;
			rspMsg.m_strMsgId = reqMsg.m_strMsgId;
			rspMsg.m_strUserId = reqMsg.m_strUserId;
			auto pSess = GetClientSess(rspMsg.m_strUserId);
			if (pSess != nullptr)
			{
				auto pSend = std::make_shared<TransBaseMsg_t>(rspMsg.GetMsgType(), rspMsg.ToString());
				pSess->SendMsg(pSend);
			}
		}
	}break;
	case E_MsgType::FileSendDataRsp_Type:
	{
		FileDataSendRspMsg rspMsg;
		if (rspMsg.FromString(msg.to_string())) {
			HSB_FileDataSendRsp(pClientSess,rspMsg);
		}
	}break;
	case E_MsgType::FileRecvDataReq_Type:
	{
		FileDataRecvReqMsg reqMsg;
		if (reqMsg.FromString(msg.to_string())) {
			Handle_TcpMsg(pClientSess, reqMsg);
		}
		return true;
	}break;
	case E_MsgType::FileVerifyRsp_Type:
	{
		FileVerifyRspMsg rspMsg;
		if (rspMsg.FromString(msg.to_string())) {
			HSB_FileVerifyRsp(pClientSess, rspMsg);
		}
	}break;
	case E_MsgType::GetFriendListRsp_Type:
	{
		GetFriendListRspMsg rspMsg;
		if (rspMsg.FromString(msg.to_string())) {
			HSB_GetFriendListRsp(pClientSess, rspMsg);
		}
	}break;
	case E_MsgType::FriendNotifyFileMsgReq_Type:
	{
		FriendNotifyFileMsgReqMsg reqMsg;
		if (reqMsg.FromString(msg.to_string())) {
			HandleFriendNotifyFileMsgReq(reqMsg);
		}
	}break;
	case E_MsgType::UserRegisterRsp_Type:
	{
		UserRegisterRspMsg rspMsg;
		if (rspMsg.FromString(msg.to_string())) {
		}
	}break;
	case E_MsgType::NotifyGroupMsgReq_Type:
	{
		NotifyGroupMsgReqMsg reqMsg;
		if (reqMsg.FromString(msg.to_string())) {
			HSB_NotifyGroupMsgReqMsg(pClientSess,reqMsg);
		}
	}break;
	default:
	{
		LOG_WARN(ms_loger, "UnHandle MsgType:{} Content:{} [{} {}]", MsgType(msg.GetType()), msg.to_string(), __FILENAME__, __LINE__);
		return false;
	}break;
	}
	return true;
}

void CMediumServer::SetUserIdUserName(const std::string strUserId, const std::string strUserName)
{
	m_userId_UserNameMap.erase(strUserId);
	m_userId_UserNameMap.insert({ strUserId,strUserName });
}
std::string CMediumServer::GetUserNameById(const std::string strUserId)
{
	auto item = m_userId_UserNameMap.find(strUserId);
	if (item != m_userId_UserNameMap.end())
	{
		return item->second;
	}
	else
	{
		return "";
	}
}

std::string CMediumServer::GetUserMainFolder(const std::string strUserId)
{
	std::string strUserName = GetUserNameById(strUserId);
	if (!strUserName.empty())
	{
		return m_fileUtil.GetCurDir() + strUserName + "\\";
	}
	else
	{
		return "";
	}
}

std::string CMediumServer::GetUserImageDir(const std::string strUserId)
{
	std::string strUserName = GetUserNameById(strUserId);
	if (strUserName.empty())
	{
		return "";
	}
	else
	{
		return m_fileUtil.GetCurDir() + strUserName + "\\Image\\";
	}
}

std::string CMediumServer::GetUserFileDir(const std::string strUserId)
{

	std::string strUserName = GetUserNameById(strUserId);
	if (strUserName.empty())
	{
		return "";
	}
	else
	{
		return m_fileUtil.GetCurDir() + strUserName + "\\FileRecv\\";
	}
}

std::string CMediumServer::GetUserDataBaseFileName(const std::string strUserId)
{
	std::string strUserName = GetUserNameById(strUserId);
	if (strUserName.empty())
	{
		return "";
	}
	else
	{
		return m_fileUtil.GetCurDir() + strUserName + "\\"+strUserName+".db";
	}
}

/**
 * @brief 处理获取好友聊天记录的请求 
 * 
 * @param reqMsg 获取好友聊天记录的请求
 * @return GetFriendChatHistoryRsp 获取好友聊天记录的回复
 */
GetFriendChatHistoryRsp CMediumServer::DoFriendChatHistoryReq(const GetFriendChatHistoryReq& reqMsg)
{
	GetFriendChatHistoryRsp result;


	auto pMsgUtil = GetMsgPersisUtil(reqMsg.m_strUserId);
	if (pMsgUtil)
	{
		auto rspMsgVec = pMsgUtil->Get_FriendChatHistory(reqMsg);
		result.m_msgHistory = rspMsgVec;
	}
	else
	{
		LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", reqMsg.m_strUserId, __FILENAME__, __LINE__);
	}
	//if (m_msgPersisUtil)
	//{
	//	auto rspMsgVec = m_msgPersisUtil->Get_FriendChatHistory(reqMsg);
	//	result.m_msgHistory = rspMsgVec;
	//}
	result.m_strMsgId = reqMsg.m_strMsgId;
	result.m_strUserId = reqMsg.m_strUserId;
	result.m_strFriendId = reqMsg.m_strFriendId;
	result.m_strChatMsgId = reqMsg.m_strChatMsgId;
	return result;
}

/**
 * @brief 处理获取群组聊天记录的请求
 * 
 * @param reqMsg 获取群组聊天记录的请求消息
 * @return GetGroupChatHistoryRsp 群组聊天记录的回复消息
 */
GetGroupChatHistoryRsp CMediumServer::DoFriendChatHistoryReq(const GetGroupChatHistoryReq& reqMsg)
{
	GetGroupChatHistoryRsp result;
	auto pMsgUtil = GetMsgPersisUtil(reqMsg.m_strUserId);
	if (pMsgUtil)
	{
		auto rspMsgVec = pMsgUtil->Get_GroupChatHistory(reqMsg);
		result.m_msgHistory = rspMsgVec;

	}
	else
	{
		LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", reqMsg.m_strUserId, __FILENAME__, __LINE__);
	}

	result.m_strMsgId = reqMsg.m_strMsgId;
	result.m_strUserId = reqMsg.m_strUserId;
	result.m_strGroupId = reqMsg.m_strGroupId;
	result.m_strChatMsgId = reqMsg.m_strChatMsgId;
	return result;
}

/**
 * @brief 处理聊天记录查找请求
 * 
 * @param reqMsg 聊天记录查找请求消息
 * @return SearchChatHistoryRsp 聊天记录查找回复消息
 */
SearchChatHistoryRsp CMediumServer::DoSearchChatHistoryReq(const SearchChatHistoryReq& reqMsg)
{
	SearchChatHistoryRsp result;
	auto pMsgUtil = GetMsgPersisUtil(reqMsg.m_strUserId);
	if (pMsgUtil)
	{
		auto friendMsgVec = pMsgUtil->Get_SearchFriendChatMsg(reqMsg);
		auto groupMsgVec = pMsgUtil->Get_SearchGroupChatMsg(reqMsg);
		result.m_friendChatMsgVec = friendMsgVec;
		result.m_groupChatMsgVec = groupMsgVec;
	}
	else
	{
		LOG_ERR(ms_loger, "UserId {} No Msg Util [{} {}] ", reqMsg.m_strUserId, __FILENAME__, __LINE__);
	}

	result.m_strMsgId = reqMsg.m_strMsgId;
	result.m_strUserId = reqMsg.m_strUserId;
	return result;
}

/**
 * @brief 创建UDP链接
 * 
 * @return CUdpClient_PTR 
 */
CUdpClient_PTR CMediumServer::CreateUdpSess()
{
	std::weak_ptr<CMediumServer> pSelf = shared_from_this();
	auto pSess = std::make_shared<CUdpClient>(m_ioService, m_udpCfg.m_strServerIp,m_udpCfg.m_nPort, [this, pSelf](const asio::ip::udp::endpoint endPt, TransBaseMsg_t* pMsg) {
		if (pSelf.lock())
		{
			DispatchUdpMsg(endPt, pMsg);
		}
	});
	pSess->StartConnect();
	return pSess;
}

/**
 * @brief 根据用户ID获取UDP链接
 * 
 * @param strUserId 用户ID
 * @return CUdpClient_PTR UDP链接
 */
CUdpClient_PTR CMediumServer::GetUdpSess(const std::string strUserId) {
	auto item = m_userUdpSessMap.find(strUserId);
	if (item != m_userUdpSessMap.end()) {
		return item->second;
	}
	else
	{
		return nullptr;
	}
}
}
