#ifndef _DENNIS_THINK_C_UDP_SERVER_H_
#define _DENNIS_THINK_C_UDP_SERVER_H_
#include "json11.hpp"
#include "CommonMsg.h"
#include "asio_common.h"
#include "Log.h"
#include <atomic>
using UDP_CALL_BACK=std::function<void(const asio::ip::udp::endpoint sendPt, const TransBaseMsg_t* pMsg)>;
namespace ChatServer
{
	using asio::ip::tcp;
	/**
	 * @brief 用于连接到远端服务器的Sess
	 *
	 */
	class CUdpServer : public std::enable_shared_from_this<CUdpServer> {
	protected:
		asio::io_service& m_ioService;

		std::string m_serverIp;
		int 		m_serverPort;



		//连接信息,需要多次获取,因此保存到变量中
		std::string m_connectInfo;

		enum { max_length = 16384, msg_max_len = 8192 };

		//接收buf
		char m_recvbuf[max_length];

		//接收buf
		char m_sendbuf[max_length];

		//接收位置
		uint32_t m_recvpos = 0;
	public:
		static	std::shared_ptr<spdlog::logger> ms_loger;
	public:
		//检查是否为同一个连接
		bool is_same_connect(const std::string& strIp, const int nport)
		{
			return ((m_serverIp.compare(strIp) == 0) &&
				(nport == m_serverPort));
		}
		//获取连接的ip地址和端口号,格式为 xx.xx.xx.xx:port
		std::string GetConnectInfo() { return m_connectInfo; }

		bool SendKeepAlive();



		CUdpServer(asio::io_service& ioService, std::string strIp, int port, UDP_CALL_BACK&& callBack);
		int StartConnect();
		int StopConnect();
		bool SendMessage(std::shared_ptr<TransBaseMsg_t> pMsg);
		void HandleRecvMsg(const asio::ip::udp::endpoint senderPt, const TransBaseMsg_t* msg);

		std::string EndPoint(const asio::ip::udp::endpoint senderPt);
		void sendMsg(const std::string strIp, const int port, const BaseMsg* pMsg);
		void sendMsg(const asio::ip::udp::endpoint senderPt, const BaseMsg* pMsg);
	private:
		asio::ip::udp::endpoint m_senderPt;
		int do_read();
		void handleKeepAliveReq(const KeepAliveReqMsg& reqMsg);

		void handleKeepAliveRsp(const KeepAliveRspMsg& rspMsg);
		UDP_CALL_BACK m_udpCallBack;
		//tcp连接的socket
		std::shared_ptr<asio::ip::udp::socket>   m_socket;
		asio::ip::udp::endpoint m_serverPt;
	};
}
#endif