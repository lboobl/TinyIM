package Enum;
public enum FileTransType{
	TCP_OFFLINE_MODE,//TCP服务器保存模式
	TCP_ONLINE_MODE,//TCP的服务器中继模式
	UDP_ONLINE_MEDIUM_MODE,//UDP的中继传递模式,UDP的P2P失败的时候使用
	UDP_ONLINE_P2P_MODE,//UDP的P2P模式
	UDP_OFFLINE_MODE,//UDP的服务器保存模式，
}