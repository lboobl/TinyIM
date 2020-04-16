
/**
 * @brief 定义需要使用的Windows消息
 * 
 */

#ifndef _TINY_IM_UI_WND_MSG_DEF_H_
#define _TINY_IM_UI_WND_MSG_DEF_H_

#define		WM_UPDATE_FONTINFO				WM_USER + 2             // 字体更新信息
#define     FMG_MSG_NET_ERROR               WM_USER + 50            // 网络错误

#define		FMG_MSG_LOGIN_RESULT			WM_USER + 100		    // 登录返回消息
#define		FMG_MSG_LOGOUT_RESULT			WM_USER + 101			// 注销返回消息
#define		FMG_MSG_UPDATE_BUDDY_LIST		WM_USER + 103			// 更新好友列表消息
#define		FMG_MSG_UPDATE_GROUP_LIST		WM_USER + 104			// 更新群列表消息
#define		FMG_MSG_UPDATE_RECENT_LIST		WM_USER + 105		    // 更新最近联系人列表消息
#define		FMG_MSG_BUDDY_MSG				WM_USER + 106			// 好友消息
#define		FMG_MSG_GROUP_MSG				WM_USER + 107			// 群消息
#define		FMG_MSG_SESS_MSG				WM_USER + 108		    // 临时会话消息
#define		FMG_MSG_STATUS_CHANGE_MSG		WM_USER + 109			// 好友状态改变消息
#define		FMG_MSG_KICK_MSG				WM_USER + 110		    // 被踢下线消息
#define		FMG_MSG_SYS_GROUP_MSG			WM_USER + 111			// 群系统消息
#define		FMG_MSG_UPDATE_BUDDY_NUMBER		WM_USER + 112		    // 更新好友号码
#define		FMG_MSG_UPDATE_GMEMBER_NUMBER	WM_USER + 113			// 更新群成员号码
#define		FMG_MSG_UPDATE_GROUP_NUMBER		WM_USER + 114		    // 更新群号码
#define		FMG_MSG_UPDATE_BUDDY_SIGN		WM_USER + 115			// 更新好友个性签名
#define		FMG_MSG_UPDATE_GMEMBER_SIGN		WM_USER + 116		    // 更新群成员个性签名
#define		FMG_MSG_UPDATE_BUDDY_INFO		WM_USER + 117			// 更新好友信息
#define		FMG_MSG_UPDATE_GMEMBER_INFO		WM_USER + 118		    // 更新群成员信息
#define		FMG_MSG_UPDATE_GROUP_INFO		WM_USER + 119			// 更新群信息
#define		FMG_MSG_UPDATE_C2CMSGSIG		WM_USER + 120		    // 更新临时会话信令
#define		FMG_MSG_UPDATE_GROUPFACESIG		WM_USER + 121		    // 更新群表情信令
#define		FMG_MSG_UPDATE_BUDDY_HEADPIC	WM_USER + 122		    // 更新好友头像
#define		FMG_MSG_UPDATE_GMEMBER_HEADPIC	WM_USER + 123		    // 更新群成员头像
#define		FMG_MSG_UPDATE_GROUP_HEADPIC	WM_USER + 124		    // 更新群头像
#define		FMG_MSG_CHANGE_STATUS_RESULT	WM_USER + 125		    // 改变在线状态返回消息

#define		FMG_MSG_INTERNAL_GETBUDDYDATA	WM_USER + 400
#define		FMG_MSG_INTERNAL_GETGROUPDATA	WM_USER + 401
#define		FMG_MSG_INTERNAL_GETGMEMBERDATA	WM_USER + 402
#define		FMG_MSG_INTERNAL_GROUPID2CODE	WM_USER + 403

#define		FMG_MSG_REGISTER				WM_USER + 404			//注册新用户
#define		FMG_MSG_FINDFREIND			    WM_USER + 405
#define     FMG_MSG_ADDFREIND				WM_USER + 406			//主动发送加好友请求
#define		FMG_MSG_RECVADDFRIENDREQUSET	WM_USER + 407			//收到加好友请求
#define		FMG_MSG_DELETEFRIEND			WM_USER + 408
#define		FMG_MSG_SENDCHATMSG_RESULT	    WM_USER + 409			//给好友发消息的反馈结果
//#define		FMG_MSG_UPLOADFILE_RESULT		WM_USER + 410		//上传文件反馈结果
#define		FMG_MSG_RECVFILE_REQUEST		WM_USER + 411			//接受文件请求
#define		FMG_MSG_RECVFILE_RESULT		    WM_USER + 412			//下载文件反馈结果
#define     FMG_MSG_RECVADDFRIENDANS		WM_USER + 413			//收到请求添加好友的对方反馈(拒绝或者同意)
#define		FMG_MSG_HEARTBEAT				WM_USER + 414			//心跳消息
#define		FMG_MSG_GOOFFLINE				WM_USER + 415			//下线或者掉线
#define		FMG_MSG_UPDATE_USER_BASIC_INFO  WM_USER + 416			//更新某个用户基本信息
#define		FMG_MSG_UPDATE_GROUP_BASIC_INFO WM_USER + 417		    //更新某个群组基本信息

#define		FMG_MSG_UPDATE_FRIEND_IDS		WM_USER + 418			//更新所有好友ID
#define		FMG_MSG_MODIFY_USER_INFO		WM_USER + 421			//修改用户信息成功
#define		FMG_MSG_RECV_USER_STATUS_CHANGE_DATA	WM_USER + 422	//收到用户在线状态改变数据
#define		FMG_MSG_USER_STATUS_CHANGE	    WM_USER + 423			//用户在线状态改变
#define		FMG_MSG_SELF_STATUS_CHANGE	    WM_USER + 424			//自己在另外的终端上下线通知
#define     FMG_MSG_UPDATE_USER_CHAT_MSG_ID	WM_USER + 425		    //更新用户的聊天消息ID
#define		FMG_MSG_RECV_CHAT_MSG			WM_USER + 426			//收到聊天消息
#define		FMG_MSG_UPDATE_CHATDLG_USERINFO WM_USER + 427			//更新聊天窗口中用户状态
#define		FMG_MSG_SEND_FILE_PROGRESS	    WM_USER + 428			//文件发送进度消息
#define		FMG_MSG_RECV_FILE_PROGRESS	    WM_USER + 429			//接收文件进度消息
#define		FMG_MSG_SEND_FILE_RESULT		WM_USER + 430			//发送文件结果
#define		FMG_MSG_RECV_FILE_RESULT		WM_USER + 431			//接收文件结果
#define		FMG_MSG_NETWORK_STATUS_CHANGE   WM_USER + 432			//本机网络状态发送变化
#define		FMG_MSG_CUSTOMFACE_AVAILABLE	WM_USER + 433			//自定义头像变为可用
#define		FMG_MSG_UPLOAD_USER_THUMB		WM_USER + 434			//自定义头像消息
#define		FMG_MSG_TARGET_INFO_CHANGE	    WM_USER + 435			//服务器告诉客户端用户信息发生变化，需要用户去取新的用户信息
#define		FMG_MSG_MODIFY_PASSWORD_RESULT  WM_USER + 436			//修改密码结果
#define		FMG_MSG_CREATE_NEW_GROUP_RESULT WM_USER + 437			//创建群组结果
#define     FMG_MSG_SCREENSHOT              WM_USER + 438			//屏幕截图消息
#define     FMG_MSG_ADD_FRIEND_NOTIFY_REQ   WM_USER + 439           //添加好友的结果
//Dennis Think
#define     FMG_MSG_RECV_FRIEND_TEXT_MSG    WM_USER + 440           //好友的文本消息

#define     FMG_MSG_USER_LOGIN_RSP			WM_USER + 441           //用户登录结果

#define     FMG_MSG_NET_FAILED				WM_USER + 442           //网络失去连接
#define     FMG_MSG_NET_RECOVER				WM_USER + 443           //网络重新连接
#define     FMG_MSG_RECV_GROUP_MSG		    WM_USER + 444           //接收到群聊消息

#define     FMG_MSG_FRIEND_FILE_SEND_RSP    WM_USER + 450           //

#define     FMG_MSG_FRIEND_FILE_RECV_REQ    WM_USER + 451 

#define		FMG_MSG_FRIEND_FILE_NOTIFY_REQ  WM_USER + 452  //好友接收文件通知
#define		FMT_MSG_FRIEND_CHAT_HISTORY     WM_USER + 453  //好友历史消息、
#define     FMT_MSG_GROUP_CHAT_HISTORY      WM_USER + 454
#define		FMT_MSG_FRIEND_FILE_TRANS_RESULT WM_USER + 455 //好友传递文件结果消息
//Dennis Think


#define		FMG_MSG_FIRST					WM_USER + 001
#define		FMG_MSG_LAST					WM_USER + 800

#endif