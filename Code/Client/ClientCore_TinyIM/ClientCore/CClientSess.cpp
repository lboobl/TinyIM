
/**
 * @file CClientSess.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 连接服务器的Sess
 * @version 0.1
 * @date 2019-09-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "CClientSess.h"
#include "CMediumServer.h"
#include "asio.hpp"
#include "json11.hpp"
#include <iostream>
namespace ClientCore
{
std::shared_ptr<spdlog::logger> CClientSess::ms_loger;

CClientSess::CClientSess(asio::io_service &ioService, std::string &strIp,
						 int port, CMediumServer *queue)
	: m_ioService(ioService), m_serverIp(strIp), m_serverPort(port),
	  m_queue(queue), m_socket(ioService), m_bConnect(ST_NOT_CONNECT)
{
	m_connectInfo = m_serverIp + ":" + std::to_string(m_serverPort);
	//StartConnect();
}

/**
 * @brief 连接socket
 *
 * @return int 返回0
 */
int CClientSess::StartConnect()
{
	//LOG_INFO(ms_loger, "Try To Connected To :{}", GetConnectInfo());
	if (ST_NOT_CONNECT == m_bConnect)
	{
		m_bConnect = ST_CONNING;
		LOG_INFO(ms_loger, "Start Connect To  IP:{} Port:{} [{} {}]", m_serverIp, m_serverPort,__FILENAME__,__LINE__);
		tcp::resolver reso(m_ioService);
		std::string strPort = std::to_string(m_serverPort);
		auto endpoint_iterator = reso.resolve({m_serverIp, strPort});
		auto self = shared_from_this();
		asio::async_connect(
			m_socket, endpoint_iterator,
			[this, self](std::error_code ec, asio::ip::tcp::endpoint /*endPt*/) {
				if (!ec)
				{
					m_bConnect = ST_CONN_FINISHED;
					{
						//m_connectInfo.clear();
						//m_connectInfo = m_socket.local_endpoint().address().to_v4().to_string() + ":" + std::to_string(m_socket.local_endpoint().port());
						//m_connectInfo += "--->";
						//m_connectInfo = m_connectInfo + m_socket.remote_endpoint().address().to_v4().to_string() + ":" + std::to_string(m_socket.remote_endpoint().port());
					}
					{
						if (m_queue)
						{
							NetRecoverReportMsg reqMsg;
							TransBaseMsg_t transMsg(reqMsg.GetMsgType(), reqMsg.ToString());
							m_queue->SendBack(shared_from_this(), transMsg);
						}
					}
					do_read();
					LOG_INFO(ms_loger, "Connect Success {} [{} {}]",GetConnectInfo(),__FILENAME__,__LINE__);
				}
				else
				{
					m_bConnect = ST_NOT_CONNECT;
					LOG_WARN(ms_loger, "Connect Failed To {} Code:{} Msg:{} [{} {}]",
						 this->GetConnectInfo(), ec.value(), ec.message(),__FILENAME__,__LINE__);
				}
			});
	}
	
	if(IsConnect())
	{
		if (m_queue)
		{
			NetRecoverReportMsg reqMsg;
			TransBaseMsg_t transMsg(reqMsg.GetMsgType(), reqMsg.ToString());
			m_queue->SendBack(shared_from_this(), transMsg);
		}
	}
	return 0;
}

/**
 * @brief 关闭socket
 *
 * @return int 返回0
 */
int CClientSess::StopConnect()
{
	LOG_INFO(ms_loger, "Connect Closed IP:{},port:{} [{} {}]", m_serverIp, m_serverPort,__FILENAME__,__LINE__);
	m_bConnect = ST_NOT_CONNECT;
	m_socket.close();
	if (m_queue)
	{
		NetFailedReportMsg reqMsg;
		TransBaseMsg_t transMsg(reqMsg.GetMsgType(), reqMsg.ToString());
		m_queue->SendBack(shared_from_this(), transMsg);
	}
	return 0;
}

/**
 * @brief
 * 处理消息的总入口，在此处根据消息类型，完成消息的分发,需要增加新消息的，在此处增加分发代码
 *
 * @param hdr TransBaseMsg_t消息体指针
 */
void CClientSess::handle_message(const TransBaseMsg_t& hdr)
{
	if (hdr.GetType() != E_MsgType::FileRecvDataReq_Type && hdr.GetType() != E_MsgType::FileSendDataReq_Type)
	{
		LOG_INFO(ms_loger, "[{}] TCP Recv: {} Msg:{} {} [{} {}]",UserId(), GetConnectInfo(), MsgType(hdr.GetType()), hdr.to_string(), __FILENAME__, __LINE__);
	}
	if (m_queue)
	{
		m_queue->SendBack(shared_from_this(),hdr);
	}
}

/**
 * @brief 从socket读取数据
 *
 * @return int 返回0
 */
int CClientSess::do_read()
{
	if (IsConnect())
	{
		auto self = shared_from_this();
		m_socket.async_read_some(
			asio::buffer(m_recvbuf + m_recvpos, max_length - m_recvpos),
			[this, self](std::error_code ec, std::size_t length) {
			TransBaseMsg_t msg(m_recvbuf);
			auto curlen = m_recvpos + length;
			//此处必须为 >=，否则需要等到下一条消息到来的时候，前一条消息才能处理
			while (curlen >= sizeof(Header) && curlen >= msg.GetSize())
			{
				handle_message(msg);
				curlen -= msg.GetSize();
				memmove(m_recvbuf, m_recvbuf + msg.GetSize(), curlen);
			}
			m_recvpos = (uint32_t)curlen;
			if (m_recvpos < max_length && !ec)
			{
				do_read();
			}
			else
			{
				StopConnect();
			}
		});
	}
	else
	{
		LOG_ERR(ms_loger, "DisConnectFrom {} ",GetConnectInfo());
	}
	return 0;
}
bool CClientSess::SendMsg(const BaseMsg* pMsg)
{
	auto pSend = std::make_shared<TransBaseMsg_t>(pMsg->GetMsgType(), pMsg->ToString());
	return SendMsg(pSend);
}
/**
 * @brief 发送消息到对端
 *
 * @param pMsg 待发送的消息
 * @return true 发送成功
 * @return false 发送失败
 */
bool CClientSess::SendMsg(std::shared_ptr<TransBaseMsg_t> pMsg)
{
	if (IsConnect())
	{
		auto self = shared_from_this();
		asio::async_write(
			m_socket, asio::buffer(pMsg->GetData(), pMsg->GetSize()),
			[this, self, pMsg](std::error_code ec, std::size_t /*length*/) mutable {
				if (ec )
				{
					LOG_ERR(ms_loger, "[{}] TCP Send: {} Msg:{}{} [{} {}]", UserId(), GetConnectInfo(), MsgType(pMsg->GetType()), pMsg->to_string(), __FILENAME__, __LINE__);
					StopConnect();
				}
				else
				{
					if (pMsg->GetType() != E_MsgType::FileRecvDataReq_Type && pMsg->GetType() != E_MsgType::FileSendDataReq_Type)
					{
						LOG_INFO(ms_loger, "[{}] TCP Send: {} Msg:{} {} [{} {}]",UserId(),GetConnectInfo(), MsgType(pMsg->GetType()), pMsg->to_string(), __FILENAME__, __LINE__);
					}
				}
			});
	}
	else
	{
		LOG_WARN(ms_loger, "{} Not TCP Connect {}", GetConnectInfo(), pMsg->to_string());
		StartConnect();
	}
	return true;
}


/**
 * @brief 发送心跳请求消息
 * 
 * @return true 发送成功
 * @return false 
 */
bool CClientSess::SendKeepAlive()
{
	KeepAliveReqMsg reqMsg(UserId());
	auto pSendMsg = std::make_shared<TransBaseMsg_t>(reqMsg.GetMsgType(), reqMsg.ToString());
	SendMsg(pSendMsg);
	return true;
}

/**
 * @brief 处理心跳请求消息
 * 
 * @param reqMsg 心跳请求消息
 */
void CClientSess::handleKeepAliveReq(const KeepAliveReqMsg &reqMsg)
{
	LOG_INFO(ms_loger, "KeepAliveReq: {}", reqMsg.ToString());
	KeepAliveRspMsg rspMsg(m_strUserId);
	auto pSendMsg = std::make_shared<TransBaseMsg_t>(rspMsg.GetMsgType(), rspMsg.ToString());
	SendMsg(pSendMsg);
}

/**
 * @brief 处理心跳回复消息
 * 
 * @param rspMsg 心跳回复消息
 */
void CClientSess::handleKeepAliveRsp(const KeepAliveRspMsg &rspMsg)
{
	LOG_INFO(ms_loger, "KeepAliveRsp:{}", rspMsg.ToString());
}

} // namespace MediumServer