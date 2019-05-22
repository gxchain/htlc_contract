package cn.yan.gxc.htlc.utils;

import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 * Created by gentlemen_yan on 2019/5/22.
 */
public class SHA256Util {

    /**
 　　* 利用java原生的摘要实现SHA256加密
 　　* @param str 加密后的报文
 　　* @return
 　　*/
    public static String getSHA256StrJava(String str){
        MessageDigest messageDigest;
        String encodeStr = "";
        try {
            messageDigest = MessageDigest.getInstance("SHA-256");
            messageDigest.update(str.getBytes("UTF-8"));
            encodeStr = byte2Hex(messageDigest.digest());
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        return encodeStr;
    }

    /**
 　　* 将byte转为16进制
 　　* @param bytes
 　　* @return
 　　*/
    private static String byte2Hex(byte[] bytes){
        StringBuffer stringBuffer = new StringBuffer();
        String temp = null;
        for (int i=0;i<bytes.length;i++){
            temp = Integer.toHexString(bytes[i] & 0xFF);
            if (temp.length()==1){
                //1得到一位的进行补0操作
                stringBuffer.append("0");
            }
            stringBuffer.append(temp);
        }
        return stringBuffer.toString();
    }

    public static void main(String[] args) {
        String source = "9c490308664645d104eaef0a80c24ec253f8f656ef7d0832929ce46a1655b35c";
        String hash = getSHA256StrJava(source);
        System.out.println(hash);
    }

}
