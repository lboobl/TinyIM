/**
 * @file CommonDef.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 
 * @version 0.1
 * @date 2019-11-25
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "CommonDef.h"

/**
 * @brief 添加好友选择的枚举类型转字符串
 * 
 * @param option 添加好友选择枚举类型
 * @return std::string 枚举类型的字符串表示
 */
std::string FriendOption(const E_FRIEND_OPTION& option) 
{
	switch (option)
	{
	case E_FRIEND_OPTION::E_UN_KNOWN:
	{
		return "UN_KNOWN";
	}break;
	case E_FRIEND_OPTION::E_AGREE_ADD:
	{
		return "AGREE_ADD";
	}break;
	case E_FRIEND_OPTION::E_REFUSE_ADD:
	{
		return "REFUSE_ADD";
	}break;
	default:
	{
		return "UN_KNOWN";
	}break;
	}
}

/**
 * @brief 字符串转枚举类型
 * 
 * @param strOption 添加好友选择的字符串 
 * @return E_FRIEND_OPTION 添加好友选择的枚举类型
 */
E_FRIEND_OPTION FriendOption(const std::string strOption)
{
	if ("UN_KNOWN" == strOption)
	{
		return E_FRIEND_OPTION::E_UN_KNOWN;
	}

	if ("REFUSE_ADD" == strOption)
	{
		return E_FRIEND_OPTION::E_REFUSE_ADD;
	}

	if ("AGREE_ADD" == strOption)
	{
		return E_FRIEND_OPTION::E_AGREE_ADD;
	}

	return E_FRIEND_OPTION::E_UN_KNOWN;
}


/**
 * @brief 好友聊天消息状态枚举类型转字符串
 * 
 * @param relation 
 * @return std::string 
 */
std::string FriendStatus(const E_ADD_FRIEND_STATUS relation)
{
	switch (relation)
	{
	case E_ADD_FRIEND_STATUS::E_UN_KNOWN:
	{
		return "UN_KNOWN";
	}break;
	case E_ADD_FRIEND_STATUS::E_UN_READ:
	{
		return "UN_READ";
	}break;
	case E_ADD_FRIEND_STATUS::E_READ_UN_NOTIFY:
	{
		return "READ_UN_NOTIFY";
	}break;
	case E_ADD_FRIEND_STATUS::E_NOTIFY:
	{
		return "NOTIFY";
	}break;
	default:
	{
		return "UN_KNOWN";
	}break;
	}
}

/**
 * @brief 好友消息的状态
 * 
 * @param strStatus 
 * @return E_ADD_FRIEND_STATUS 
 */
E_ADD_FRIEND_STATUS FriendStatus(const std::string strStatus)
{

	if ("UN_KNOWN" == strStatus)
	{
		return E_ADD_FRIEND_STATUS::E_UN_KNOWN;
	}
	
	if ("UN_READ" == strStatus)
	{
		return E_ADD_FRIEND_STATUS::E_UN_READ;
	}

	if ("READ_UN_NOTIFY" == strStatus)
	{
		return E_ADD_FRIEND_STATUS::E_READ_UN_NOTIFY;
	}

	if ("NOTIFY" == strStatus)
	{
		return E_ADD_FRIEND_STATUS::E_NOTIFY;
	}

	return E_ADD_FRIEND_STATUS::E_UN_KNOWN;
}

/**
 * @brief 字符串转为群组角色
 * 
 * @param strRole 
 * @return E_GROUP_MEMBER_ROLE 
 */
E_GROUP_MEMBER_ROLE MemberRole(const std::string strRole)
{
	if ("CREATER" == strRole)
	{
		return E_GROUP_MEMBER_ROLE::E_ROLE_CREATER;
	}

	if ("MANAGER" == strRole)
	{
		return E_GROUP_MEMBER_ROLE::E_ROLE_MANAGER;
	}

	if ("MEMBER" == strRole)
	{
		return E_GROUP_MEMBER_ROLE::E_ROLE_MEMBER;
	}

	return E_GROUP_MEMBER_ROLE::E_ROLE_UNKNOWN;

}

/**
 * @brief 群组角色转为字符串
 * 
 * @param role 群组角色
 * @return std::string 角色的字符串
 */
std::string MemberRole(const E_GROUP_MEMBER_ROLE role)
{
	switch (role)
	{
	case E_GROUP_MEMBER_ROLE::E_ROLE_CREATER:
	{
		return "CREATER";
	}break;
	case E_GROUP_MEMBER_ROLE::E_ROLE_MANAGER:
	{
		return "MANAGER";
	}break;
	case E_GROUP_MEMBER_ROLE::E_ROLE_MEMBER:
	{
		return "MEMBER";
	}
	default:
	{
		return "UNKNOWN";
	}break;
	}
}

#define ENUM_TO_STRING(ENUM_TYPE) case ENUM_TYPE:{return #ENUM_TYPE ;}break;

/**
 * @brief 消息类型转为字符串
 * 
 * @param msgType 消息类型
 * @return std::string 字符串表示
 */
std::string MsgType(const E_MsgType& msgType)
{
	switch (msgType)
	{
		ENUM_TO_STRING(E_MsgType::Base_Type)
		ENUM_TO_STRING(E_MsgType::KeepAliveReq_Type)
		ENUM_TO_STRING(E_MsgType::KeepAliveRsp_Type)
		ENUM_TO_STRING(E_MsgType::UserLoginReq_Type)
		ENUM_TO_STRING(E_MsgType::UserLoginRsp_Type)
		ENUM_TO_STRING(E_MsgType::UserLogoutReq_Type)
		ENUM_TO_STRING(E_MsgType::UserLogoutRsp_Type)
		ENUM_TO_STRING(E_MsgType::UserRegisterReq_Type)
		ENUM_TO_STRING(E_MsgType::UserRegisterRsp_Type)
		ENUM_TO_STRING(E_MsgType::UserUnRegisterReq_Type)
		ENUM_TO_STRING(E_MsgType::UserUnRegisterRsp_Type)
		ENUM_TO_STRING(E_MsgType::FriendChatReceiveTxtMsgReq_Type)
		ENUM_TO_STRING(E_MsgType::FriendChatReceiveTxtMsgRsp_Type)
		ENUM_TO_STRING(E_MsgType::FriendChatSendTxtMsgReq_Type)
		ENUM_TO_STRING(E_MsgType::FriendChatSendTxtMsgRsp_Type)
		ENUM_TO_STRING(E_MsgType::FriendChatSendEmojiMsgReq_Type)
		ENUM_TO_STRING(E_MsgType::FriendChatSendEmojiMsgRsp_Type)
		ENUM_TO_STRING(E_MsgType::FriendChatSendImageMsgReq_Type)

		ENUM_TO_STRING(E_MsgType::FriendChatSendImageMsgRsp_Type)
		ENUM_TO_STRING(E_MsgType::FriendSendFileMsgReq_Type)
		ENUM_TO_STRING(E_MsgType::FriendSendFileMsgRsp_Type)
		ENUM_TO_STRING(E_MsgType::FriendRecvFileMsgReq_Type)
		ENUM_TO_STRING(E_MsgType::FriendRecvFileMsgRsp_Type)
		ENUM_TO_STRING(E_MsgType::FriendNotifyFileMsgReq_Type)
		ENUM_TO_STRING(E_MsgType::FriendNotifyFileMsgRsp_Type)

		ENUM_TO_STRING(E_MsgType::FileSendDataReq_Type)
		ENUM_TO_STRING(E_MsgType::FileSendDataRsp_Type)
		ENUM_TO_STRING(E_MsgType::FileRecvDataReq_Type)
		ENUM_TO_STRING(E_MsgType::FileRecvDataRsp_Type)
		ENUM_TO_STRING(E_MsgType::FriendUnReadMsgNotifyReq_Type)
		ENUM_TO_STRING(E_MsgType::FriendUnReadMsgNotifyRsp_Type)
		ENUM_TO_STRING(E_MsgType::GroupUnReadMsgNotifyReq_Type)
		ENUM_TO_STRING(E_MsgType::GroupUnReadMsgNotifyRsp_Type)
		ENUM_TO_STRING(E_MsgType::FriendChatSendFileDataOnLineRsp_Type)
		ENUM_TO_STRING(E_MsgType::FriendChatSendFileDataOffLineReq_Type)
		ENUM_TO_STRING(E_MsgType::FriendChatSendFileDataOffLineRsp_Type)
		ENUM_TO_STRING(E_MsgType::FindFriendReq_Type)
		ENUM_TO_STRING(E_MsgType::FindFriendRsp_Type)
		ENUM_TO_STRING(E_MsgType::AddFriendSendReq_Type)
		ENUM_TO_STRING(E_MsgType::AddFriendSendRsp_Type)
		ENUM_TO_STRING(E_MsgType::AddFriendRecvReq_Type)
		ENUM_TO_STRING(E_MsgType::AddFriendRecvRsp_Type)
		ENUM_TO_STRING(E_MsgType::AddFriendNotifyReq_Type)
		ENUM_TO_STRING(E_MsgType::AddFriendNotifyRsp_Type)
		ENUM_TO_STRING(E_MsgType::RemoveFriendReq_Type)
		ENUM_TO_STRING(E_MsgType::RemoveFriendRsp_Type)
		ENUM_TO_STRING(E_MsgType::GetFriendListReq_Type)
		ENUM_TO_STRING(E_MsgType::GetFriendListRsp_Type)
		ENUM_TO_STRING(E_MsgType::GetGroupListReq_Type)
		ENUM_TO_STRING(E_MsgType::GetGroupListRsp_Type)
		ENUM_TO_STRING(E_MsgType::NetFailedReport_Type)
		ENUM_TO_STRING(E_MsgType::NetRecoverReport_Type)
		ENUM_TO_STRING(E_MsgType::AddTeamReq_Type)
		ENUM_TO_STRING(E_MsgType::AddTeamRsp_Type)
		ENUM_TO_STRING(E_MsgType::RemoveTeamReq_Type)
		ENUM_TO_STRING(E_MsgType::RemoveTeamRsp_Type)
		ENUM_TO_STRING(E_MsgType::AddFriendToTeamReq_Type)
		ENUM_TO_STRING(E_MsgType::AddFriendToTeamRsp_Type)
		ENUM_TO_STRING(E_MsgType::MoveFriendToTeamReq_Type)
		ENUM_TO_STRING(E_MsgType::MoveFriendToTeamRsp_Type)
		ENUM_TO_STRING(E_MsgType::ModifyTeamNameReq_Type)
		ENUM_TO_STRING(E_MsgType::ModifyTeamNameRsp_Type)
		ENUM_TO_STRING(E_MsgType::CreateGroupReq_Type)
		ENUM_TO_STRING(E_MsgType::CreateGroupRsp_Type)
		ENUM_TO_STRING(E_MsgType::DestroyGroupReq_Type)
		ENUM_TO_STRING(E_MsgType::DestroyGroupRsp_Type)
		ENUM_TO_STRING(E_MsgType::FindGroupReq_Type)
		ENUM_TO_STRING(E_MsgType::FindGroupRsp_Type)
		ENUM_TO_STRING(E_MsgType::AddToGroupReq_Type)
		ENUM_TO_STRING(E_MsgType::AddToGroupRsp_Type)
		ENUM_TO_STRING(E_MsgType::AddToGroupRecvReq_Type)
		ENUM_TO_STRING(E_MsgType::AddToGroupRecvRsp_Type)
		ENUM_TO_STRING(E_MsgType::AddToGroupNotifyReq_Type)
		ENUM_TO_STRING(E_MsgType::AddToGroupNotifyRsp_Type)
		ENUM_TO_STRING(E_MsgType::InviteFriendToGroupReq_Type)
		ENUM_TO_STRING(E_MsgType::InviteFriendToGroupRsp_Type)
		ENUM_TO_STRING(E_MsgType::InviteToGroupRecvReq_Type)
		ENUM_TO_STRING(E_MsgType::InviteToGroupRecvRsp_Type)
		ENUM_TO_STRING(E_MsgType::InviteResultNotifyReq_Type)
		ENUM_TO_STRING(E_MsgType::InviteResultNotifyRsp_Type)
		ENUM_TO_STRING(E_MsgType::SendGroupTextMsgReq_Type)
		ENUM_TO_STRING(E_MsgType::SendGroupTextMsgRsp_Type)
		ENUM_TO_STRING(E_MsgType::RecvGroupTextMsgReq_Type)
		ENUM_TO_STRING(E_MsgType::RecvGroupTextMsgRsp_Type)
		ENUM_TO_STRING(E_MsgType::QuitGroupReq_Type)
		ENUM_TO_STRING(E_MsgType::QuitGroupRsp_Type)
		ENUM_TO_STRING(E_MsgType::UpdateGroupListNotifyRsp_Type)
		ENUM_TO_STRING(E_MsgType::FileVerifyReq_Type)
		ENUM_TO_STRING(E_MsgType::FileVerifyRsp_Type)
		ENUM_TO_STRING(E_MsgType::UserKickOffReq_Type)
		ENUM_TO_STRING(E_MsgType::UserKickOffRsp_Type)
		ENUM_TO_STRING(E_MsgType::UpdateFriendListNotifyReq_Type)
		ENUM_TO_STRING(E_MsgType::UpdateFriendListNotifyRsp_Type)
		ENUM_TO_STRING(E_MsgType::UpdateGroupListNotifyReq_Type)
		ENUM_TO_STRING(E_MsgType::UdpP2PStartReq_Type)
		ENUM_TO_STRING(E_MsgType::UdpP2PStartRsp_Type)
		ENUM_TO_STRING(E_MsgType::QueryUserUdpAddrReq_Type)
		ENUM_TO_STRING(E_MsgType::QueryUserUdpAddrRsp_Type)
		ENUM_TO_STRING(E_MsgType::GetFriendChatHistroyReq_Type)
		ENUM_TO_STRING(E_MsgType::GetFriendChatHistoryRsp_Type)
		ENUM_TO_STRING(E_MsgType::GetGroupChatHistoryReq_Type)
		ENUM_TO_STRING(E_MsgType::GetGroupChatHistoryRsp_Type)
		ENUM_TO_STRING(E_MsgType::SearchChatMsgReq_Type)
		ENUM_TO_STRING(E_MsgType::SearchChatMsgRsp_Type)
		ENUM_TO_STRING(E_MsgType::AsyncFriendChatMsgReq_Type)
		ENUM_TO_STRING(E_MsgType::AsyncFriendChatMsgRsp_Type)
		ENUM_TO_STRING(E_MsgType::AsyncGroupChatMsgReq_Type)
		ENUM_TO_STRING(E_MsgType::AsyncGroupChatMsgRsp_Type)
		ENUM_TO_STRING(E_MsgType::FileSendDataBeginReq_Type)
		ENUM_TO_STRING(E_MsgType::FileSendDataBeginRsp_Type)
		ENUM_TO_STRING(E_MsgType::FileTransProgressNotifyReq_Type)
		ENUM_TO_STRING(E_MsgType::FileDownLoadReq_Type)
		ENUM_TO_STRING(E_MsgType::FileDownLoadRsp_Type)
		ENUM_TO_STRING(E_MsgType::UdpMultiCastReq_Type)
		ENUM_TO_STRING(E_MsgType::UdpMultiCastRsp_Type)
	default:
		{
			return "UnKnownMsgType: "+std::to_string(static_cast<int>(msgType));
		}break;
	}
}


/**
 * @brief 错误代码转字符串
 * 
 * @param errCode 错误代码
 * @return std::string 错误代码字符串
 */
std::string ErrMsg(const ERROR_CODE_TYPE& errCode) {
	switch (errCode)
	{
		ENUM_TO_STRING(ERROR_CODE_TYPE::E_CODE_SUCCEED)

		//ͨ�ô���
		ENUM_TO_STRING(ERROR_CODE_TYPE::E_CODE_CONNECT_FAILED)

		//�û�ע�������
		ENUM_TO_STRING(ERROR_CODE_TYPE::E_CODE_USER_HAS_EXIST)
		//
		ENUM_TO_STRING(ERROR_CODE_TYPE::E_CODE_LOGIN_FAILED)
		ENUM_TO_STRING(ERROR_CODE_TYPE::E_CODE_NO_SUCH_USER)
		ENUM_TO_STRING(ERROR_CODE_TYPE::E_CODE_USER_NAME_PASS_WORD_NOT_MATCH)
		ENUM_TO_STRING(ERROR_CODE_TYPE::E_CODE_VERIFY_CODE_NOT_MATCH)
		default:
		{
			return "UNKNOWN_ERROR";
		}break;
	}
}

/**
 * @brief 消息类型转字符串
 * 
 * @param chatType 
 * @return std::string 
 */
std::string   ChatType(const CHAT_MSG_TYPE& chatType)
{
	switch (chatType)
	{
	case CHAT_MSG_TYPE::E_CHAT_TEXT_TYPE:
	{
		return "E_CHAT_TEXT_TYPE";
	}break;
	case CHAT_MSG_TYPE::E_CHAT_EMOJI_TYPE:
	{
		return "E_CHAT_EMOJI_TYPE";
	}break;
	case CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE:
	{
		return "E_CHAT_IMAGE_TYPE";
	}break;
	case CHAT_MSG_TYPE::E_CHAT_FILE_TYPE:
	{
		return "E_CHAT_FILE_TYPE";
	}break;
	default:
	{
		return "E_CHAT_UNKNOWN_TYPE";
	}break;

	}
}

/**
 * @brief 字符串转消息类型
 * TODO: 以后可能用不到了
 * @param strType 
 * @return CHAT_MSG_TYPE 
 */
CHAT_MSG_TYPE ChatType(const std::string strType)
{
	if ("E_CHAT_TEXT_TYPE" == strType)
	{
		return CHAT_MSG_TYPE::E_CHAT_TEXT_TYPE;
	}
	else if ("E_CHAT_IMAGE_TYPE" == strType) {
		return CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE;
	}
	else if ("E_CHAT_EMOJI_TYPE" == strType) {
		return CHAT_MSG_TYPE::E_CHAT_EMOJI_TYPE;
	}
	else if ("E_CHAT_FILE_TYPE" == strType) {
		return CHAT_MSG_TYPE::E_CHAT_FILE_TYPE;
	}
	else {
		return CHAT_MSG_TYPE::E_CHAT_UNKNOWN_TYPE;
	}
}

/**
 * @brief 原生字符串转16进制
 * 
 * @param input 原始字符串
 * @return std::string 转换后的16进制字符串
 */
std::string StringToHex(const std::string& input)
{
	static const char* const lut = "0123456789ABCDEF";
	size_t len = input.length();

	std::string output;
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i)
	{
		const unsigned char c = input[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}
	return output;
}

#include <algorithm>
#include <stdexcept>

/**
 * @brief 16进制转为原始字符串
 * 
 * @param input 16进制的字符串
 * @return std::string 原始字符串
 */
std::string HexToString(const std::string& input)
{
	static const char* const lut = "0123456789ABCDEF";
	size_t len = input.length();
	if (len & 1) throw std::invalid_argument("odd length");

	std::string output;
	output.reserve(len / 2);
	for (size_t i = 0; i < len; i += 2)
	{
		char a = input[i];
		const char* p = std::lower_bound(lut, lut + 16, a);
		if (*p != a) throw std::invalid_argument("not a hex digit");

		char b = input[i + 1];
		const char* q = std::lower_bound(lut, lut + 16, b);
		if (*q != b) throw std::invalid_argument("not a hex digit");

		output.push_back(static_cast<char>(((p - lut) << 4) | (q - lut)));
	}
	return output;
}

/**
 * @brief 字符串转为在线离线状态
 * 
 * @param strType 字符串
 * @return CLIENT_ONLINE_TYPE 在线离线状态 
 */
CLIENT_ONLINE_TYPE OnLineType(const std::string strType)
{
	if (strType == "ON_LINE")
	{
		return CLIENT_ONLINE_TYPE::C_ONLINE_TYPE_ONLINE;
	}
	else
	{
		return CLIENT_ONLINE_TYPE::C_ONLINE_TYPE_OFFLINE;
	}
}

/**
 * @brief 在线离线状态转为字符串
 * 
 * @param onLineType 在线离线状态
 * @return std::string 在线状态的字符串表示
 */
std::string OnLineType(const CLIENT_ONLINE_TYPE onLineType)
{
	switch (onLineType)
	{
	case CLIENT_ONLINE_TYPE::C_ONLINE_TYPE_ONLINE:
	{
		return "ON_LINE";
	}break;
	default:
	{
		return "OFF_LINE";
	}break;
	}
}

/**
 * @brief 好友关系转为字符串
 * 
 * @param relation 好友关系
 * @return std::string 字符串
 */
std::string FriendRelation(const E_FRIEND_RELATION& relation)
{
	switch (relation)
	{
	case E_FRIEND_RELATION::E_FRIEND_TYPE:
	{
		return "FRIEND";
	}break;
	case E_FRIEND_RELATION::E_BLACK_TYPE:
	{
		return "BLACK";
	}break;
	case E_FRIEND_RELATION::E_STRANGER_TYPE:
	{
		return "STRANGER";
	}break;
	default:
	{
		return "UNKNOWN";
	}break;
	}
}

/**
 * @brief 字符串转为好友关系
 * 
 * @param strRelation 字符串
 * @return E_FRIEND_RELATION 好友关系
 */
E_FRIEND_RELATION FriendRelation(const std::string& strRelation)
{
	if (strRelation == "FRIEND")
	{
		return E_FRIEND_RELATION::E_FRIEND_TYPE;
	}
	else if (strRelation == "BLACK")
	{
		return E_FRIEND_RELATION::E_BLACK_TYPE;
	}
	else if (strRelation == "STRANGER")
	{
		return E_FRIEND_RELATION::E_STRANGER_TYPE;
	}
	return E_FRIEND_RELATION::E_STRANGER_TYPE;
}


/**
 * @brief 错误码转错误消息
 * 
 * @param errType 错误码
 * @return std::string 错误消息
 */
std::string ErrMsg(const ERROR_CODE_TYPE errType)
{
	switch (errType)
	{
	case ERROR_CODE_TYPE::E_CODE_SUCCEED:
	{
		return "成功";
	}break;

	case ERROR_CODE_TYPE::E_CODE_CONNECT_FAILED:
	{
		return "连接到服务器失败";
	}break;

	case ERROR_CODE_TYPE::E_CODE_USER_HAS_EXIST:
	{
		return "用户已存在";
	}break;

	case ERROR_CODE_TYPE::E_CODE_LOGIN_FAILED:
	{
		return "登录失败";
	}break;
	case ERROR_CODE_TYPE::E_CODE_NO_SUCH_USER:
	{
		return "没有此用户";
	}break;
	case ERROR_CODE_TYPE::E_CODE_USER_NAME_PASS_WORD_NOT_MATCH:
	{
		return "用户名密码不匹配";
	}break;
	case ERROR_CODE_TYPE::E_CODE_USER_OFF_LINE:
	{
		return "用户不在线";
	}break;
	case ERROR_CODE_TYPE::E_CODE_VERIFY_CODE_NOT_MATCH:
	{
		return "验证码不匹配";
	}break;
	case ERROR_CODE_TYPE::E_CODE_EMPTY_GROUP_NAME:
	{
		return "群组名为空";
	}break; 
	case ERROR_CODE_TYPE::E_CODE_ADD_SELF_AS_FRIEND:
	{
		return "不能添加自己为好友";
	}break;
	case ERROR_CODE_TYPE::E_CODE_EMPTY_USER_ID:
	{
		return "用户ID为空";
	}break; 
	case ERROR_CODE_TYPE::E_CODE_EMPTY_GROUP_ID:
	{
		return "群组ID为空";
	}break;
	case ERROR_CODE_TYPE::E_CODE_EMPTY_USER_NAME:
	{
		return "用户名为空";
	}break;
	case ERROR_CODE_TYPE::E_CODE_EMPTY_PASS_WORD:
	{
		return "登录失败";
	}break;
	case ERROR_CODE_TYPE::E_CODE_NOT_FRIEND_RELATION:
	{
		return "非好友关系";
	}break;
	case ERROR_CODE_TYPE::E_CODE_ERROR_PARAM:
	{
		return "参数错误";
	}break;
	default:
	{
		return "未知错误";
	}break;
	}
}