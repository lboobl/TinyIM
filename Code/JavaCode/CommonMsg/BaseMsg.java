package CommonMsg;
import Enum.*;
public class BaseMsg{
    /**
     * 默认构造函数
     */
    BaseMsg(){
        m_msgType = MsgType.Base_Type;
    }
    /**
     * 返回消息类型
     * @return
     */
    public MsgType getMsgType(){
        return m_msgType;
    }

    /**
     * 序列化为json字符串的方法
     * @return 返回json字符串
     */
    public String ToString() {
        return "";
    }

    /**
     * 消息反序列化的方法,需要子类继承
     * @param strJson 
     * @return 
     */
    public boolean FromString(String strJson){
        return false;
    }

    /**
     * 校验消息是否正确
     * @return
     */
    public boolean Valid(){
        return false;
    }
    
    protected MsgType m_msgType;
    
}
