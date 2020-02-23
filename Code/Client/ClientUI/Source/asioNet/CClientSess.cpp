
//
// Copyright(c) 2018 DennisMi.
// dennismi1024@gmail.com
// https://www.dennisthink.com/
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "CClientSess.h"
#include "json11.hpp"
#include "CSessManager.h"
#include <iostream>
#include "../IULog.h"

std::string StringToHex(const char * data, const std::size_t length)
{
	const std::string hex = "0123456789ABCDEF";
	std::stringstream ss;

	for (std::string::size_type i = 0; i < length; ++i)
	{
		ss << hex[(unsigned char)data[i] >> 4] << hex[(unsigned char)data[i] & 0xf];
	}
	return ss.str();
}


namespace SourceServer
{
std::shared_ptr<spdlog::logger> CClientSess::ms_loger;

CClientSess::CClientSess(asio::io_service &ioService, std::string &strIp, int port, CSessManager *manager) : m_ioService(ioService),
																											 m_manager(manager),
																											 m_serverIp(strIp),
																											 m_serverPort(port),
																											 m_socket(ioService),
																											 m_ConnectState(STATE_E::ST_NOT_CONNECT)
{
	m_connectInfo = m_serverIp + ":" + std::to_string(m_serverPort);
	//m_bReciveKeepAlive =true;
	//StartConnect();
	ms_loger = CreateLogger();
}

/**
 * @brief 连接到服务器
 * 
 */
void CClientSess::StartConnect()
{
	DBG(ms_loger, "Try To Connected To :{}", GetConnectInfo());
	if (!IsConnect())
	{
		memset(m_recvbuf, 0, sizeof(m_recvbuf));

		m_ConnectState = STATE_E::ST_CONNING;
		WARN(ms_loger, "Start Connect To  IP:{} Port:{}", m_serverIp, m_serverPort);
		tcp::resolver reso(m_ioService);
		std::string strPort = std::to_string(m_serverPort);
		auto endpoint_iterator = reso.resolve({m_serverIp, strPort});
		std::shared_ptr<CClientSess> pSelf = shared_from_this();
		asio::async_connect(m_socket, endpoint_iterator, [this, pSelf](std::error_code ec, tcp::endpoint endPt) {
			if (!ec)
			{
				m_ConnectState = STATE_E::ST_CONN_FINISHED;
				m_recvpos = 0;
				OnConnectRecover();
				do_read();
				INFO(ms_loger, "Connect Success IP:{} Port:{}", this->m_serverIp, this->m_serverPort);
				INFO(ms_loger, "ConnectID :{}:{}", endPt.address().to_string(), endPt.port());
			}
			else
			{
				StopConnect();
				WARN(ms_loger, "Connect Failed To {} Code:{} Msg:{}", this->GetConnectInfo(), ec.value(), ec.message());
			}
		});
	}
	else
	{
		INFO(ms_loger, "Connect Has Established IP:{} Port:{}", this->m_serverIp, this->m_serverPort);
	}
}

/**
 * @brief 断开Socket的连接
 * 
 */
void CClientSess::StopConnect()
{
	/**/
	INFO(ms_loger, "Connect Closed IP:{},port:{}", m_serverIp, m_serverPort);
	m_ConnectState = STATE_E::ST_NOT_CONNECT;
	m_socket.close();
}

/*响应连接失败*/
void CClientSess::OnConnectFailed()
{
	NetFailedReportMsg msg;
	TransBaseMsg_t trans(msg.GetMsgType(), msg.ToString());
	if (m_manager)
	{
		m_manager->handleMessage(&trans);
	}
}

void CClientSess::OnConnectRecover()
{
	NetRecoverReportMsg reportMsg;
	TransBaseMsg_t trans(reportMsg.GetMsgType(), reportMsg.ToString());
	if (m_manager)
	{
		m_manager->handleMessage(&trans);
	}
}
/**
 * @brief 获取套接字的连接状态
 * 
 * @return true 
 * @return false 
 */
bool CClientSess::IsConnect()
{
	return (STATE_E::ST_CONN_FINISHED == m_ConnectState);
}

void CClientSess::handleMessage(const TransBaseMsg_t *msg)
{
	if (msg == nullptr)
	{
		return;
	}
	switch (msg->GetType())
	{
	case E_MsgType::KeepAliveReq_Type:
	{
		KeepAliveReqMsg reqMsg;
		if (reqMsg.FromString(msg->to_string()))
		{
			handleKeepAliveReq(reqMsg);
		}
		else
		{
			ERR(ms_loger, "Message Not Match:{} {}", msg->to_string(), reqMsg.ToString());
		}
	}
	break;
	case E_MsgType::KeepAliveRsp_Type:
	{
		KeepAliveRspMsg rspMsg;
		if (rspMsg.FromString(msg->to_string()))
		{
			handleKeepAliveRsp(rspMsg);
		}
		else
		{
			ERR(ms_loger, "Message Not Match:{} {}", msg->to_string(), rspMsg.ToString());
		}
		if (m_manager)
		{
			m_manager->handleMessage(msg);
		}
	}
	break;
	default:
	{
		if (m_manager)
		{
			m_manager->handleMessage(msg);
		}
	}
	}
}

/**
 * @brief 从socket读取数据
 * 
 * @return int 
 */
int CClientSess::do_read()
{
	if (IsConnect())
	{
		auto self = shared_from_this();
		m_socket.async_read_some(
			asio::buffer(m_msgbuf, msg_max_len), [this, self](std::error_code ec, std::size_t length) {
			if (!ec)
			{
				memcpy(m_recvbuf + m_recvpos, m_msgbuf, length);
				m_recvpos += length;
				Header * pHeader = (Header*)(m_recvbuf);
				while(m_recvpos > 8 && m_recvpos >= pHeader->m_length)
				{
					int nMoveLen = pHeader->m_length;
					m_recvpos = m_recvpos - pHeader->m_length;
					TransBaseMsg_t msg(m_recvbuf);
					INFO(ms_loger, "MsgType:{} MsgLen:{} Msg:{} ", MsgType(static_cast<E_MsgType>(pHeader->m_type)), pHeader->m_length, msg.to_string());
					handleMessage(&msg);
					memmove(m_recvbuf, m_recvbuf + nMoveLen, m_recvpos);
				}
				if (m_recvpos < max_length && !ec)
				{
					do_read();
				}
			}
			else
			{
				INFO(ms_loger, "EC:{} [{} {}]", ec.message(),__FILENAME__,__LINE__);
			}
		});
	}
	return 0;
}

/**
 * @brief 发送心跳请求消息
 * 
 * @return true 发送成功
 * @return false 发送失败
 */
bool CClientSess::SendKeepAlive()
{
	DBG(ms_loger, "Send Keep Alive");
	if (IsConnect())
	{
		KeepAliveReqMsg msg("SourceServer");
		auto pMsg = std::make_shared<TransBaseMsg_t>(msg.GetMsgType(), msg.ToString());
		send_msg(pMsg);
		return true;
	}
	else
	{
		WARN(ms_loger, "{} Not Connect", GetConnectInfo());
		return false;
	}
}

/**
 * @brief 发送消息到远端服务器
 * 
 * @param msg 待发送的消息
 */
void CClientSess::send_msg(std::shared_ptr<TransBaseMsg_t> msg)
{
	auto self = shared_from_this();
	asio::async_write(m_socket, asio::buffer(msg->GetData(), msg->GetSize()), [this, self, msg](std::error_code ec, std::size_t length) {
		if (ec)
		{
			OnConnectFailed();
			StopConnect();
		}
		else
		{
			INFO(ms_loger, "Send Succeed:{}  {} [{} {}]", MsgType(msg->GetType()), msg->to_string(),__FILENAME__,__LINE__);
		}
	});
}

/**
 * @brief 处理接收到的心跳请求消息
 * 
 * @param reqMsg 
 */
void CClientSess::handleKeepAliveReq(const KeepAliveReqMsg &reqMsg)
{
	INFO(ms_loger, "Recv KeepAliveReq:{}", reqMsg.ToString());
	KeepAliveRspMsg msg("SourceServer");
	auto pMsg = std::make_shared<TransBaseMsg_t>(msg.GetMsgType(), msg.ToString());
	send_msg(pMsg);
}

/**
 * @brief 处理心跳回复消息
 * 
 * @param rspMsg 心跳回复消息
 */
void CClientSess::handleKeepAliveRsp(const KeepAliveRspMsg &rspMsg)
{
	INFO(ms_loger, "Recv KeepAliveRsp:{}", rspMsg.ToString());
	/*if(1)
	{
		NetRecoverReportMsg reportMsg;
		TransBaseMsg_t trans(reportMsg.GetMsgType(), reportMsg.ToString());
		if (m_manager) {
			m_manager->handleMessage(&trans);
		}
	}*/
}
} // namespace SourceServer