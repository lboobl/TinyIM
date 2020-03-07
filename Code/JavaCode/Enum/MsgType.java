/**
 * 定义消息的枚举类型
 */
package Enum;
public enum MsgType{
    Base_Type,
	//心跳请求消息
	KeepAliveReq_Type,
	//心跳回复消息
	KeepAliveRsp_Type,


	//登录请求
	UserLoginReq_Type,
	//登录回复
	UserLoginRsp_Type,

	//登出请求
	UserLogoutReq_Type,
	//登出回复
	UserLogoutRsp_Type,

	//注册请求
	UserRegisterReq_Type,
	//注册回复
	UserRegisterRsp_Type,

	//注销请求
UserUnRegisterReq_Type,

//注销回复
UserUnRegisterRsp_Type,


//好友聊天之文本消息接收请求 Server--->Receiver
FriendChatReceiveTxtMsgReq_Type,

//Receiver--->Server
FriendChatReceiveTxtMsgRsp_Type,

//好友聊天之文本消息请求 Sender---->Server
FriendChatSendTxtMsgReq_Type,

//好友聊天之文本消息回复 Server----->Sender
FriendChatSendTxtMsgRsp_Type,

//表情消息请求   Sender--->Server
FriendChatSendEmojiMsgReq_Type,

//表情消息回复   Server--->Server
FriendChatSendEmojiMsgRsp_Type,

//图片消息请求
FriendChatSendImageMsgReq_Type,

//图片消息回复
FriendChatSendImageMsgRsp_Type,


//发送文件请求
FriendSendFileMsgReq_Type,

//发送文件回复
FriendSendFileMsgRsp_Type,

//接收文件请求
FriendRecvFileMsgReq_Type,

//接收文件请求
FriendRecvFileMsgRsp_Type,

//接收文件通知请求
FriendNotifyFileMsgReq_Type,

//接收文件通知回复
FriendNotifyFileMsgRsp_Type,

//好友发送在线文件数据回复
FriendChatSendFileDataOnLineRsp_Type,

//好友发送离线文件数据请求
FriendChatSendFileDataOffLineReq_Type,

//好友发送离线文件回复
FriendChatSendFileDataOffLineRsp_Type,

//查找好友请求
FindFriendReq_Type,

//查找好友回复
FindFriendRsp_Type,

//添加好友请求[请求者--->服务器]
AddFriendSendReq_Type,

//添加好友回复[服务器--->请求者]
AddFriendSendRsp_Type,

//接收添加好友请求[服务器-->接收者]
AddFriendRecvReq_Type,

//发送添加好友回复[接收者-->服务器]
AddFriendRecvRsp_Type,

//添加好友回复通知[服务器-->发送者]
AddFriendNotifyReq_Type,

//添加好友回复通知[发送者-->服务器]
AddFriendNotifyRsp_Type,

//删除好友请求
RemoveFriendReq_Type,

//删除好友回复
RemoveFriendRsp_Type,

//获取好友列表请求消息
GetFriendListReq_Type,

//获取好友列表回复消息
GetFriendListRsp_Type,

//获取群组列表请求消息
GetGroupListReq_Type,

//获取群组列表回复消息
GetGroupListRsp_Type,

//网络失去连接
NetFailedReport_Type,

//网络连接恢复正常
NetRecoverReport_Type,

//增加分组请求消息
AddTeamReq_Type,

//增加分组回复消息
AddTeamRsp_Type,

//删除分组请求消息
RemoveTeamReq_Type,

//删除分组回复消息
RemoveTeamRsp_Type,

//添加好友到分组的请求
AddFriendToTeamReq_Type,

//添加好友到分组的回复
AddFriendToTeamRsp_Type,

//移动好友到分组请求
MoveFriendToTeamReq_Type,

//移动好友到分组回复
MoveFriendToTeamRsp_Type,

//修改分组名称请求
ModifyTeamNameReq_Type,

//修改分组名称回复
ModifyTeamNameRsp_Type,

//创建群聊请求
CreateGroupReq_Type,

//创建群聊回复
CreateGroupRsp_Type,

//解散群聊请求
DestroyGroupReq_Type,

//解散群聊回复
DestroyGroupRsp_Type,

//查找群组请求
FindGroupReq_Type,

//查找群组回复
FindGroupRsp_Type,

//加入群组请求[请求者--->服务器]
AddToGroupReq_Type,

//加入群组回复[服务器--->请求者]
AddToGroupRsp_Type,

//加入群组通过请求[服务器-->管理员]
AddToGroupRecvReq_Type,

//加入群组通过回复[管理员-->服务器]
AddToGroupRecvRsp_Type,

//加入群组结果通知请求[服务器--->请求方]
AddToGroupNotifyReq_Type,

//加入群组结果通知回复[服务器--->请求方]
AddToGroupNotifyRsp_Type,


//邀请加入群组请求[发起请求方--->服务器]
InviteFriendToGroupReq_Type,

//邀请加入群组回复[服务器--->发起请求方]
InviteFriendToGroupRsp_Type,


//邀请请求送达对方[服务器--->被请求方]
InviteToGroupRecvReq_Type,

//邀请请求送达对方后，对方的回复[被请求方---->服务器]
InviteToGroupRecvRsp_Type,

//邀请结果通知[服务器-->发起方]
InviteResultNotifyReq_Type,

//邀请结果通知回复[发起方---->服务器]
InviteResultNotifyRsp_Type,

//群组文本聊天消息请求
SendGroupTextMsgReq_Type,

//群组文本聊天消息回复
SendGroupTextMsgRsp_Type,

//群组文本聊天消息请求
RecvGroupTextMsgReq_Type,

//群组文本聊天消息回复
RecvGroupTextMsgRsp_Type,

//退出群请求
QuitGroupReq_Type,

//退出群回复
QuitGroupRsp_Type,

//文件发送数据请求 [发送方]---->[服务器]
FileSendDataReq_Type,

//文件发送数据回复 [服务器]----->[接收方]
FileSendDataRsp_Type,

//文件接收数据请求 [服务器]----->[接收方]
FileRecvDataReq_Type,

//文件接收数据回复 [接收方]----->[服务器]
FileRecvDataRsp_Type,

//文件验证请求 [文件发送方]----->[文件接收方]
FileVerifyReq_Type,

//文件验证回复 [文件接收方]----->[文件发送方]
FileVerifyRsp_Type,

//用户被踢出消息请求(用户的账号在其他地点登录) [服务器]---->[接收方]
UserKickOffReq_Type,

//用户被踢出消息回复(用户的账号在其他地点登录) [接收方]---->[服务器]
UserKickOffRsp_Type,

//用户有好友未读消息通知[服务器]---->[接收方]
FriendUnReadMsgNotifyReq_Type,

//用户有好友未读消息通知回复[接收方]---->[服务器]
FriendUnReadMsgNotifyRsp_Type,

//群组未读消息通知
GroupUnReadMsgNotifyReq_Type,

//群组未读消息通知回复
GroupUnReadMsgNotifyRsp_Type,

//更新好友列表通知
UpdateFriendListNotifyReq_Type,

//更新好友通知回复
UpdateFriendListNotifyRsp_Type,

//群组更新通知
UpdateGroupListNotifyReq_Type,

//群组更新回复
UpdateGroupListNotifyRsp_Type,


//UdpP2P开始请求
UdpP2PStartReq_Type,

//UdpP2P开始回复
UdpP2PStartRsp_Type,

//查询用户的UDP地址
QueryUserUdpAddrReq_Type,

//查询用户的UDP地址的回复
QueryUserUdpAddrRsp_Type,

//获取好友聊天的历史记录的请求
GetFriendChatHistroyReq_Type,

//获取好友聊天的历史记录的回复
GetFriendChatHistoryRsp_Type,

//获取群组聊天的历史记录的请求
GetGroupChatHistoryReq_Type,

//获取群组聊天的历史记录的回复
GetGroupChatHistoryRsp_Type,

//查找聊天记录的请求
SearchChatMsgReq_Type,

//查找聊天记录的回复
SearchChatMsgRsp_Type,

//好友聊天消息同步请求
AsyncFriendChatMsgReq_Type,

//好友聊天消息同步回复
AsyncFriendChatMsgRsp_Type,

//群组聊天消息同步请求
AsyncGroupChatMsgReq_Type,

//群组聊天消息同步回复
AsyncGroupChatMsgRsp_Type,

//开始发送文件数据请求
FileSendDataBeginReq_Type,

//开始发送文件数据回复
FileSendDataBeginRsp_Type,

//文件传输进度的通知消息
FileTransProgressNotifyReq_Type,

//文件下载开始请求
FileDownLoadReq_Type,

//文件下载开始回复
FileDownLoadRsp_Type,

//获取一个随机用户的请求
GetRandomUserReq_Type,

//获取随机一个用户的回复
GetRandomUserRsp_Type,

NetSpeedDataReq_Type,

NetSpeedTestReq_Type,

NetSpeedTestRsp_Type,

UdpMultiCastReq_Type,
UdpMultiCastRsp_Type,

NotifyGroupMsgReq_Type,

NotifyGroupMsgRsp_Type,

FriendStateChangeNotifyReq_Type,//好友状态改变通知请求
FriendStateChangeNotifyRsp_Type,//好友状态改变通知回复

GroupMemberStateChangeNotifyReq_Type,//群组成员状态改变通知请求
GroupMemberStateChangeNotifyRsp_Type,//群组成
}


public enum ClientOS_Type{
    
}