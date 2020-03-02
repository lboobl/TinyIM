# 一个支持聊天机器人的开源IM

[TinyIM](https://www.github.com/DennisThink/TinyIM)

经过6个月的开发工作,TinyIM的第一个版本终于完成了。IM这么多,它有哪些不一样的地方呢？

## 1. 支持聊天机器人

目前的主流IM客户端的设计上,收发消息都需要通过界面完成。我们知道GUI的代码做自动化测试比较困难,TinyIM对客户端采用了将GUI和Core分离的架构设计,并在Core部分同时支持TCP协议和HTTP协议。

这样做的好处有两点:
1、可以通过同步的HTTP协议编写ImRobot;
2、可以通过同步的HTTP协议编写业务逻辑的测试代码。

光说不练假把式,还是有图有真相的好。
![ImRobot](https://www.dennisthink.com/wp-content/uploads/2020/01/2019-12-31_183552.jpg)


## 2. 主模块跨平台

多样化的社会也是丰富多彩的社会,那么跨平台就是多样化的一个体现。TinyIM一共有4个主要的程序组成,除ClientGUI以外,其他的模块都实现了跨平台。


|组件|Windows|Linux|MacOS|
|---|----|----|----|
|ClientCore|支持|支持|理论支持,暂未测试|
|ClientGUI|支持|可以通过Electron等技术支持|可以通过Electron等技术支持|
|ChatServer|支持|支持|理论支持,暂未测试|
|ImRobot|支持|支持|理论支持,暂未测试|

**实际测试**:
在Windows平台和Ubunbu上我已经进行了测试,程序的编译和运行都没有问题。ClientGUI的部分,可以使用QT等跨平台技术开发,也可以使用Electron等技术。

## 3. 图示流程,注释丰富

对于每个主要的业务流程,TinyIM都力图使用流程图描述。从简单的逻辑到复杂的流程,都可以让您做到----“胸有成图,码到成功”。


发送图片流程图:

![发送图片](https://www.dennisthink.com/wp-content/uploads/2020/01/用户发送图片消息-scaled.jpg)

看代码最郁闷的就是没有注释了,TinyIM没有这个问题。TinyIM为几乎每一个函数都添加了Doxygen风格的注释,让您轻轻松松读代码,明明白白改bug。

![CodeComment](https://www.dennisthink.com/wp-content/uploads/2020/01/代码注释.gif)

## 4. 联系作者

QQ群: 106350930
邮箱: dennismi1024@gmail.com
blog: [浮生笔记](https://www.dennisthink.com/)