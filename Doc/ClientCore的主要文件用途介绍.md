## ClientCore的文件用途介绍

1. ClientSess.h ClientSess.cpp 

用于TCP连接到服务器的类,收发TCP消息。

2. CHttpServer.h CHttpServer.cpp 

HTTP服务的接口的实现类。

3. CMakeLists.txt

CMake的编译配置文件。

4. CMediumServer.h CMediumServer.cpp 

ClientCore的主类,业务逻辑代码的实现。

5. CMsgPersistentUtil.h CMsgPersistentUtil.cpp 

保存消息等数据到SQLite的类,从SQLite读取数据的类。

6. CServerSess.h CServerSess.cpp 

ClientGUI连接到ClientCore的TCP连接类，收发GUI的TCP消息。

7. CUdpClient.h CUdpClient.cpp

ClientCore进行UDP消息的收发类。

8. MainFunc.h MainFunc.cpp 

Window和Linux平台下相同的启动函数。

9. MediumServer.cpp 

ClientCore的main函数文件。