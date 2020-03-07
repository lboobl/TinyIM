package CommonMsg;
import Enum.*;
public final class  TransMsg
{
    TransMsg(BaseMsg baseMsg){

    }

    TransMsg(byte[] msgData){
        if(msgData.length >= TRNAS_MSG_BASE_LENGTH){
            m_msgType = getMsgType(msgData);
        }
    }
    public static MsgType getMsgType(byte[] msgData){
        final int TRNAS_MSG_BASE_LENGTH = 8;
        if(msgData.length < TRNAS_MSG_BASE_LENGTH)
        {
            return MsgType.Base_Type;
        }
        else
        {
            int int1=msgData[0]&0xff;
            int int2=(msgData[1]&0xff)<<8;
            int int3=(msgData[2]&0xff)<<16;
            int int4=(msgData[3]&0xff)<<24;
            int nIndex = int1|int2|int3|int4;
            if(nIndex >=0 && nIndex < MsgType.values().length)
            {
                return MsgType.values()[nIndex];
            }
            else
            {
                return MsgType.Base_Type;
            }
        }
    }

    public static int GetMsgSize(byte[] msgData){
        final int TRNAS_MSG_BASE_LENGTH = 8;
        if(msgData.length < TRNAS_MSG_BASE_LENGTH)
        {
            return 0;
        }
        else
        {
            int int1=msgData[4]&0xff;
            int int2=(msgData[5]&0xff)<<8;
            int int3=(msgData[6]&0xff)<<16;
            int int4=(msgData[7]&0xff)<<24;
            int nIndex = int1|int2|int3|int4;
            if(nIndex >=0)
            {
                return nIndex;
            }
            else
            {
                return 0;
            }
        }
    }
    public MsgType getMsgType(){
        return m_msgType;
    }

    public int getMsgSize(){
        return m_msgStringLength;
    }

    public String GetMsgString(){
        return m_msgString;
    }

    public byte[] GetMsgData(){
        return m_dataBytes;
    }
    //形式1
    private byte[] m_dataBytes;

    //形式2 
    private MsgType m_msgType;
    private int m_msgStringLength;
    private String m_msgString;
    private final int TRNAS_MSG_BASE_LENGTH = 8;
}