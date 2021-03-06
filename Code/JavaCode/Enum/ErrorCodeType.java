package Enum;
public enum ErrorCodeType {
    E_CODE_SUCCEED = 0,//成功

	//通用错误
	E_CODE_CONNECT_FAILED,//网络已经断开

	//用户注册错误码
	E_CODE_USER_HAS_EXIST, //用户已存在
	//
	E_CODE_LOGIN_FAILED,//登录失败
	E_CODE_NO_SUCH_USER,//没有此用户
	E_CODE_USER_NAME_PASS_WORD_NOT_MATCH, //用户名密码不匹配
	E_CODE_VERIFY_CODE_NOT_MATCH,         //验证码不匹配
	E_CODE_USER_OFF_LINE,//用户不在线
	E_CODE_EMPTY_GROUP_NAME,//群组名称为空

	E_CODE_ADD_SELF_AS_FRIEND,//添加自己为好友
	E_CODE_EMPTY_USER_ID,//用户ID为空
	E_CODE_EMPTY_GROUP_ID,//群组ID为空
	E_CODE_EMPTY_USER_NAME,//用户名为空
	E_CODE_EMPTY_PASS_WORD,//用户密码为空
	E_CODE_NOT_GROUP_MEMBER,//不是群组成员
	E_CODE_NOT_FRIEND_RELATION,//不是好友关系
	E_CODE_ERROR_PARAM,//参数错误
	E_CODE_BAD_REQUEST_FORMAT,//请求格式错误
	E_CODE_USER_NOT_LOGIN,//用户未登录
	E_CODE_NO_SUCH_FILE,//没有此文件
	E_CODE_FILE_TRANSING,//文件正在传输
	E_CODE_FILE_HAS_EXIST,//文件已存在
	E_CODE_FILE_SEND_FAILED,//文件传输失败
}