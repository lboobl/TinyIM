import Enum.MsgType;
public class EnumTest{
    EnumTest(){

    }
    public void PrintMsgTypeValue(){
        System.out.println("TinyIM Enum Type");
        for(MsgType type:MsgType.values())
        {
            System.out.println("Type:"+type+"  Value:"+type.ordinal());
        }
    }
}