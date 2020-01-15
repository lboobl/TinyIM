## ClientUI的主要文件介绍

### Source目录下主要是源代码文件

1. asioNet

asioNet目录下主要是用来跟ClientCore进行通信的类

2. Proto/Proto.h Proto/Proto.cpp

用来收发实际消息的协议处理类.

3. RemoteDesktop

这个目录下主要是用来实现远程桌面的一些代码。

4. res

这个目录下主要是一些图标文件

5. SkinLib

这个目录下主要是一些UI界面的控件代码。

6. SQLite3.7.17 

此目录可以去除。

7. wtl9.0

UI有关的代码。

8. zlib1.2.11

zlib有关的代码，可以完成压缩和解压缩(可以去掉)

9. CAboutDlg.h CAboutDlg.cpp 

关于对话框的头文件和实现文件 

10. AddFriendConfirmDlg.cpp  AddFriendConfirmDlg.h

添加好友确认对话框。

11. BuddyChatDlg.cpp  BuddyChatDlg.h

好友聊天对话框。

12. BuddyInfoDlg.cpp BuddyInfoDlg.h
好友信息对话框

13. BuddyListCtrl.cpp BuddyListCtrl.h

好友列表控件

14. BuddyListCtrlEx.cpp BuddyListCtrlEx.h

好友列表扩展控件

15. CascadeWinManager.cpp  CascadeWinManager.h

TODO: 暂不确定

16. ChangePicHead.h ChangePicHead.cpp

修改头像功能。

17. ChangePicHeadDlg.cpp ChangePicHeadDlg.h

修改头像功能的对话框

18. ChatDlgCommon.h ChatDlgCommon.cpp

窗口抖动功能的实现。

ChatTabMgr.cpp
ChatTabMgr.h

20. ClosePromptDlg.h ClosePromptDlg.cpp

退出提示对话框。

21. Crash.cpp

处理程序崩溃的相关问题。

22. CreateNewGroupDlg.h CreateNewGroupDlg.cpp

创建新群组的对话框。

23. CRegisterSkinHyperLink.h CRegisterSkinHyperLink.cpp

超链接相关操作的对话框

24. CustomMsgDef.h

WM一些消息的定义,可以考虑和其他的文件合并。

25. DstVersion.h

程序的目标版本

26. EncodingUtil.h EncodingUtil.cpp

字符转码的相关函数

27. FaceCtrl.h FaceCtrl.cpp

头像控件。

28. FaceList.h FaceList.cpp

头像列表控件。

29. FaceSelDlg.h  FaceSelDlg.cpp

头像选择对话框。

30. FileTool.h FileTool.cpp

完成文件的读取和写入。

31. FileTransferCtrl.h FileTransferCtrl.cpp

文件传输控件。

32. FindFriendDlg.h FindFriendDlg.cpp

查找好友对话框

33. flamingo.rc

资源文件。

33. FontSelDlg.h FontSelDlg.cpp

字体选择对话框。

34. GDIFactory.h GDIFactory.cpp

字体等信息的操作。

35. GifImage.h GifImage.cpp

支持GIF图片的显示。

36. GroupChatDlg.h GroupChatDlg.cpp

群组聊天对话框

37. GroupInfoDlg.h GroupInfoDlg.cpp

群组信息对话框。

38. GroupListCtrl.h GroupListCtrl.cpp

群组列表控件。

39. IImageOle.h

具体用途不明。

40. ImageEx.h ImageEx.cpp

用来显示图片的扩展类

41. IniFile.h IniFile.cpp

Ini文件操作的类

42. IULog.h IULog.cpp

打印日志的类

43. LoginAccountList.h LoginAccountList.cpp

登录账号列表类。

44. LoginDlg.h LoginDlg.cpp

登录对话框类。

45. LoginSettingsDlg.h LoginSettingsDlg.cpp

登录设置对话框类

46. LogonUserInfoDlg.h LogonUserInfoDlg.cpp

登录用户信息对话框类

47. MainDlg.h MainDlg.cpp

主界面对话框。

48. MessageBoxDlg.h MessageBoxDlg.cpp

弹出的消息提示框。

49. MessageLogger.h MessageLogger.cpp

可以考虑去掉。

50. MiniBuffer.h MinuBuffer.cpp

可以考虑去掉。


51. ModifyMarkNameDlg.h ModifyMarkNameDlg.cpp

修改备注名对话框。

52. ModifyPasswordDlg.h ModifyPasswordDlg.cpp

修改密码对话框。

53. MsgTipDlg.cpp MsgTipDlg.h

状态栏消息提示对话框。

net/

54. Path.h Path.cpp

路径相关的一些操作

55. PerformanceCounter.h PerformanceCounter.cpp

性能测量的类，可以去掉。

56. PicBarDlg.h PicBarDlg.cpp

用来显示图片的对话框，图片的放大，缩小

Proto/
ProtoTestMain.cpp

57. RecentListCtrl.h RecentListCtrl.cpp

最近聊天列表的控件类。

58. RegisterDialog.h RegisterDialog.cpp

注册对话框类。

59. RichEditUtil.h RichEditUtil.cpp

用来显示聊天消息的窗口类，包括发送消息、接收消息、历史消息。

60. SplitterCtrl.h SplitterCtrl.cpp

分割控件类

61. SrcVersion.h

客户端版本信息头文件

62. Startup.h Startup.cpp

程序入口的头文件和实现文件

63. stdafx.h stdafx.cpp

windows平台公用头文件和实现文件。

64. SystemSettingDlg.h SystemSettingDlg.cpp

系统设置对话框。

65. TeamDlg.h TeamDlg.cpp

群组对话框.

66. Thread.h Thread.cpp

线程的封装(可以考虑去掉）

67. ThreadPool.h ThreadPool.cpp

线程池的封装。

68. TrayIcon.h TrayIcon.cpp

托盘区图标的封装。

69. UI_Core_Convert.h UI_Core_Convert.cpp

GUI消息、数据结构和Core消息以及数据结构的相互转换。

70. UI_USER_INFO.h UI_USER_INFO.cpp

GUI用户信息的定义和实现

71. UICommonDef.h

GUI需要使用的一些公用的定义。

72. UICommonStruct.h UICommonStruct.cpp

GUI 公用的结构体定义。

73. UICommonType.h

GUI的公用类型定义。

74. UIDefaultValue.h

GUI的默认值定义。

75. UIText.h

GUI文本定义。

76. UIWndMsgDef.h

GUI的Wnd消息的定义

77. UpdateDlg.h UpdateDlg.cpp

程序更新对话框。

78. Updater.h Updater.cpp

程序更新的实现类。

79. UserConfig.h UserConfig.cpp

用户配置的实现类。

80. UserMgr.h UserMgr.cpp

用户信息管理的实现类

81. UserSnapInfoDlg.h UserSnapInfoDlg.cpp

用户的Snap信息对话框。

82. Utils.h Utils.cpp 

常用功能的实现类

83. UtilTime.h UtilTime.cpp

时间操作的工具类。


84. XmlDocument.h XmlDocument.cpp

Xml文档操作的类。

