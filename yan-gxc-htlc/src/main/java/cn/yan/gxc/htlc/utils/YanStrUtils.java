//package cn.yan.gxc.htlc.utils;
//
//import java.util.Random;
//
///**
// * Created
// * User  wankunYan
// * Date  2018/6/25
// * Time  19:28
// */
//public class YanStrUtils {
//
//    public static boolean isEmpty(String str) {
//        if(str != null && str.length() != 0) {
//            for(int i = 0; i < str.length(); ++i) {
//                char c = str.charAt(i);
//                if(c != 32 && c != 9 && c != 13 && c != 10) {
//                    return false;
//                }
//            }
//
//            return true;
//        } else {
//            return true;
//        }
//    }
//
//    public static byte[] hexStringToByte(String hex) {
//        int len = (hex.length() / 2);
//        byte[] result = new byte[len];
//        char[] achar = hex.toCharArray();
//        for (int i = 0; i < len; i++) {
//            int pos = i * 2;
//            result[i] = (byte) (toByte(achar[pos]) << 4 | toByte(achar[pos + 1]));
//        }
//        return result;
//    }
//
//    /**
//     * 获取16进制随机数
//     * @param len
//     * @return
//     */
//    public static String randomHexString(int len)  {
//        Random random = new Random();
//        try {
//            StringBuffer result = new StringBuffer();
//            for(int i=0;i<len;i++) {
//                result.append(Integer.toHexString(random.nextInt(16)));
//            }
//            return result.toString().toUpperCase();
//
//        } catch (Exception e) {
//            // TODO: handle exception
//            e.printStackTrace();
//
//        }
//        return null;
//
//    }
//
//
//    private static byte toByte(char c) {
//        byte b = (byte) "0123456789abcdef".indexOf(c);
//        return b;
//    }
//
//
//    public static String hexString2binaryString(String hexString) {
//        if (hexString == null || hexString.length() % 2 != 0)
//            return null;
//        String bString = "", tmp;
//        for (int i = 0; i < hexString.length(); i++) {
//            tmp = "0000" + Integer.toBinaryString(Integer.parseInt(hexString.substring(i, i + 1), 16));
//            bString += tmp.substring(tmp.length() - 4);
//        }
//        return bString;
//    }
//
//
//    public static String binaryString2hexString(String bString) {
//        if (bString == null || bString.equals("") || bString.length() % 8 != 0)
//            return null;
//        StringBuffer tmp=new StringBuffer();
//        int iTmp = 0;
//        for (int i = 0; i < bString.length(); i += 4) {
//            iTmp = 0;
//            for (int j = 0; j < 4; j++) {
//                iTmp += Integer.parseInt(bString.substring(i + j, i + j + 1)) << (4 - j - 1);
//            }
//            tmp.append(Integer.toHexString(iTmp));
//        }
//        return tmp.toString();
//    }
//
//    // todo
//    public static String getRandomCode(int length) {
//        return "yan";
//    }
//}
