#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include "CommonMsg.h"
#include "CMsgPersistentUtil.h"
#include "SnowFlake.h"
static CMsgPersistentUtil util;
static SnowFlake g_snowFlake(4, 4);
static std::string MsgId() {
	return std::to_string(g_snowFlake.nextId());
}

std::string ChatMsgContext() {
	{
		ChatMsgElemVec elemVec;
		{
			{
				ChatMsgElem elem;
				elem.m_eType = CHAT_MSG_TYPE::E_CHAT_EMOJI_TYPE;
				elem.m_nFaceId = 1;

				elemVec.push_back(elem);
			}

			{
				ChatMsgElem elem;
				elem.m_eType = CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE;
				elem.m_strImageName = "C:\\Windows\\Program File\\Test.jpg";

				elemVec.push_back(elem);
			}

			{
				ChatMsgElem elem;
				elem.m_eType = CHAT_MSG_TYPE::E_CHAT_TEXT_TYPE;
				elem.m_strContext = "I'am a \"Famous| boy,I am Neo";

				elemVec.push_back(elem);
			}
		}
		return MsgElemVec(elemVec);
	}
}
static std::string UserId(){
	std::string strMsgId = MsgId();
	if (strMsgId.length() > 8) {
		return strMsgId.substr(strMsgId.length() - 8, 8);
	}
	else {
		return "12345678";
	}
}

TEST_CASE("AddFriendMsg") {
	AddFriendRecvReqMsg  recvMsg;
	recvMsg.m_strMsgId = MsgId();
	recvMsg.m_strFriendId = UserId();
	recvMsg.m_strUserId = UserId();
	CFileUtil fileUtil;
	std::string strCurFolder = fileUtil.GetCurDir();
	CHECK(util.InitDataBase(strCurFolder));
	CHECK(util.Save_AddFriendRecvReqMsg(recvMsg));
	AddFriendRecvReqMsg recvFriendMsg;
	CHECK(util.Get_AddFriendRecvReqMsg(recvMsg));
	CHECK(util.Remove_AddFriendRecvReqMsg(recvMsg));
}

TEST_CASE("AddFriendNotifyMsg") {
	AddFriendNotifyReqMsg reqMsg;
	reqMsg.m_strMsgId = MsgId();
	reqMsg.m_strUserId = UserId();
	reqMsg.m_strFriendId = UserId();
	reqMsg.m_option = E_FRIEND_OPTION::E_AGREE_ADD;
	CFileUtil fileUtil;
	std::string strCurFolder = fileUtil.GetCurDir();
	CHECK(util.InitDataBase(strCurFolder));
	CHECK(util.Save_AddFriendNotifyReqMsg(reqMsg));
	{
		AddFriendNotifyReqMsg reqGetMsg;
		CHECK(util.Get_AddFriendNotifyReqMsg(reqGetMsg));
		CHECK(util.Remove_AddFriendNotifyReqMsg(reqGetMsg));
	}
}

TEST_CASE("FriendChatRecvTextMsgInsert") {
	CFileUtil fileUtil;
	std::string strCurFolder = fileUtil.GetCurDir();
	CHECK(util.InitDataBase(strCurFolder));
	FriendChatRecvTxtReqMsg reqMsg;
	reqMsg.m_strMsgId = MsgId();
	reqMsg.m_chatMsg.m_strChatMsgId = MsgId();
	reqMsg.m_chatMsg.m_strSenderId = UserId();
	reqMsg.m_chatMsg.m_strReceiverId = UserId();
	reqMsg.m_chatMsg.m_strMsgTime = "2019-01-01 10:10:10";
	{
		ChatMsgElemVec elemVec;
		{
			{
				ChatMsgElem elem;
				elem.m_eType = CHAT_MSG_TYPE::E_CHAT_EMOJI_TYPE;
				elem.m_nFaceId = 1;

				elemVec.push_back(elem);
			}

			{
				ChatMsgElem elem;
				elem.m_eType = CHAT_MSG_TYPE::E_CHAT_IMAGE_TYPE;
				elem.m_strImageName = "C:\\Windows\\Program File\\Test.jpg";

				elemVec.push_back(elem);
			}

			{
				ChatMsgElem elem;
				elem.m_eType = CHAT_MSG_TYPE::E_CHAT_TEXT_TYPE;
				elem.m_strContext = "I'am a \"Famous| boy,I am Neo";

				elemVec.push_back(elem);
			}
		}
		reqMsg.m_chatMsg.m_strContext = MsgElemVec(elemVec);
	}
	
	CHECK(util.Save_FriendChatSendTxtRspMsg(reqMsg.m_chatMsg));
	{
		FriendChatMsg_s recvMsg;
		CHECK(util.Get_FriendChatRecvTxtReqMsg(recvMsg));
		CHECK(util.Update_FriendChatRecvTxtReqMsg(recvMsg));
	}
}


TEST_CASE("FriendRecvFileMsgReqMsg") {

}

TEST_CASE("FriendNotifyFileMsgReqMsg") {

}

TEST_CASE("GetFriendChatHistoryReq") {

}

TEST_CASE("GetGroupChatHistoryReq") {

}
TEST_CASE("GroupChatRecvTextMsgInsert") {
	CFileUtil fileUtil;
	std::string strCurFolder = fileUtil.GetCurDir();
	CHECK(util.InitDataBase(strCurFolder));
	RecvGroupTextMsgReqMsg reqMsg;
	reqMsg.m_strMsgId = MsgId();
	reqMsg.m_chatMsg.m_strGroupId = UserId();
	reqMsg.m_chatMsg.m_strSenderId = UserId();
	reqMsg.m_chatMsg.m_strContext = ChatMsgContext();
	CHECK(util.Save_RecvGroupTextMsgReqMsg(reqMsg));
	RecvGroupTextMsgReqMsg recvMsg;
	CHECK(util.Get_RecvGroupTextMsgReqMsg(recvMsg));
	std::cout << recvMsg.ToString() << std::endl;
	CHECK(util.Update_RecvGroupTextMsgReqMsg(recvMsg));
}

TEST_CASE("FileHash") {

}