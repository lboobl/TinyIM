/**
 * @file CUdpClient.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief UDP收发数据的类头文件
 * @version 0.1
 * @date 2019-12-27
 *
 * @copyright Copyright (c) 2019
 *
 */
#ifndef _DENNIS_THINK_C_UDP_MULTI_CAST_RECIVER_H_
#define _DENNIS_THINK_C_UDP_MULTI_CAST_RECIVER_H_
#include "CommonMsg.h"
#include "asio_common.h"
#include "Log.h"
#include <memory>
#include <queue>

using RECIVER_CALL_BACK = std::function<void(const asio::ip::udp::endpoint endPt, TransBaseMsg_t* pMsg)>;
namespace ClientCore {
	class CUdpMultiCastReciver : public std::enable_shared_from_this<CUdpMultiCastReciver>
	{
	public:
		CUdpMultiCastReciver(asio::io_context& ioService, const std::string strIp, const int port, RECIVER_CALL_BACK&& callBack);
		void StartConnect();
		void SendKeepAlive();
		void SendMultiCastMsg(const UdpMultiCastReqMsg& reqMsg);
		void send_msg(const asio::ip::udp::endpoint endPt, TransBaseMsg_S_PTR pMsg);
		void send_msg(const asio::ip::udp::endpoint endPt, const BaseMsg* pMsg);
		void send_msg(const std::string strIp, const int port, const BaseMsg* pMsg);
		void DoSend();
		void sendToServer(const BaseMsg* pMsg);
		asio::ip::udp::endpoint GetServerEndPt() {
			return m_udpServerPt;
		}
		void SetUserId(const std::string strUserId) {
			m_strUserId = strUserId;
		}

		std::string UserId()const {
			return m_strUserId;
		}
	public:
		static std::shared_ptr<spdlog::logger> ms_loger;
	private:
		std::string m_udpServerIp;
		int m_udpServerPort;
		asio::ip::udp::endpoint m_udpServerPt;
		asio::ip::udp::endpoint m_clientPt;
		asio::ip::udp::endpoint m_recvFromPt;
		std::shared_ptr<asio::ip::udp::socket> m_udpSocket;
		void do_read();
		std::string EndPoint(const asio::ip::udp::endpoint endPt);
		void handle_msg(const asio::ip::udp::endpoint endPt, TransBaseMsg_t* pMsg);

		static const int max_msg_length_udp = 8092;
		char m_sendbuf[max_msg_length_udp];
		char m_recvbuf[max_msg_length_udp];

		RECIVER_CALL_BACK m_callBack;
		std::string m_strUserId;
		std::atomic_bool m_bDoSend{ false };
		void do_SendMsg();
	};
	using CUdpMultiCastReciver_PTR = std::shared_ptr< CUdpMultiCastReciver>;
}
#endif