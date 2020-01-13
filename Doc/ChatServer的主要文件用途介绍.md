
## MediumServer路径下

1、ClientSess.h ClientSess.cpp

用于连接到远端的服务器,为以后对ChatServer做集群化的功能开发留下接口。

2. CMakeLists.txt 

组织文件

3. CMediumServer.h CMediumServer.cpp

ChatServer的核心业务逻辑的处理类,

4. CMySqlConnect.h CMySqlConnect.cpp 

和数据库进行交互的类,完成数据库的增删查改操作，目前只支持MySql,这个部分以后抽象出接口的部分，考虑支持Oracle，SqlServer。

5. CServerSess.h CServerSess.cpp 

处理客户端连接的会话类。

6. CTimeUtil.h CTimeUtil.cpp 

时间处理的类。

7. CUdpServer.h CUdpServer.cpp 

用来收发UDP消息的类。

8. EncodingUtil.h EncodingUtil.cpp 

编码转换的实现类，实现ASCII到UTF8,UTF8到UNICODE的字符转换。

9. MediumServer.cpp 

程序的入口,main函数。