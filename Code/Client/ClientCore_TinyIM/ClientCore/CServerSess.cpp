/**
 * @file CServerSess.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 接受ClientGUI的TCP连接的类
 * @version 0.1
 * @date 2019-12-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "CServerSess.h"
#include "CMediumServer.h"
namespace ClientCore
{
std::shared_ptr<spdlog::logger> CServerSess::ms_loger;

/**
 * @brief 从socket的读取数据的函数
 * 
 */
void CServerSess::do_read()
{
	auto self = shared_from_this();
	m_socket.async_read_some(
		asio::buffer(m_recvbuf + m_recvpos, max_length - m_recvpos),
		[this, self](std::error_code ec, std::size_t length) {
		TransBaseMsg_t msg(m_recvbuf);
		auto curlen = m_recvpos + length;
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


/**
 * @brief 停止连接,响应GUI断开与Core的连接
 * 
 */
void CServerSess::StopConnect()
{
#if 0
	if (m_server)
	{
		m_server->ServerSessClose(shared_from_this());
	}
#endif
	m_socket.close();
	m_bConnect = false;
}
/**
 * @brief 当do_read函数接收到一个完整消息的时候，调用此函数，在此函数中完成消息类型的判断和消息分发
 * 
 * @param hdr 
 */
void CServerSess::handle_message(const TransBaseMsg_t& hdr)
{
	LOG_INFO(ms_loger, "Recv From GUI: MsgType:{}  Msg:{}  [{} {}]",MsgType(hdr.GetType()),hdr.to_string(),__FILENAME__, __LINE__);
	if (m_server)
	{
		m_server->SendFoward(shared_from_this(),hdr);
	}
}

/**
 * @brief 处理心跳请求消息
 * 
 * @param reqMsg 心跳请求消息
 */
void CServerSess::handleKeepAliveReq(const KeepAliveReqMsg &reqMsg)
{
	LOG_INFO(ms_loger, "KeepAliveReq:{}  [{} {}]", reqMsg.ToString(), __FILENAME__, __LINE__);
	KeepAliveRspMsg rspMsg("MediumServer");
	//auto pMsg =
	//	std::make_shared<TransBaseMsg_t>(rspMsg.GetMsgType(), rspMsg.ToString());
	
	//auto pMsg = std::make_shared<CTransMessage>((const char *)(reqMsg))
	//SendMsg(pMsg);
}

/**
 * @brief 处理心跳回复消息
 * 
 * @param rspMsg 心跳回复消息
 */
void CServerSess::handleKeepAliveRsp(const KeepAliveRspMsg &rspMsg)
{
	LOG_INFO(ms_loger, "KeepAliveRsp:{} [{} {}]", rspMsg.ToString(), __FILENAME__, __LINE__);
}
} // namespace MediumServer