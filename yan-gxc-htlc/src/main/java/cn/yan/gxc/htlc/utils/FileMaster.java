package cn.yan.gxc.htlc.utils;

import java.io.*;
import java.util.HashMap;

/**
 * Created by gentlemen_yan on 2019/5/22.
 */
public class FileMaster {

    public static String loadTextFromFile(String filepath) throws IOException {
        File f = new File(filepath);
        BufferedReader fin = new BufferedReader(new FileReader(f));
        StringBuffer text = new StringBuffer();
        String line;
        while ((line = fin.readLine()) != null) {
            text.append(line);
        }
        fin.close();
        return text.toString();
    }

    public static void saveTextToFile(String filepath, String text) throws IOException {
        File f = new File(filepath);
        BufferedWriter fout = new BufferedWriter(new FileWriter(f));
        fout.write(text);
        fout.flush();
        fout.close();
        System.out.println("save - " + text.length());
    }

    public static byte[] loadBytesFromFile(String filepath) throws IOException {
        File f = new File(filepath);
        BufferedReader fin = new BufferedReader(new FileReader(f));
        StringBuffer text = new StringBuffer();
        String line;
        // fin.rea
        while ((line = fin.readLine()) != null) {
            text.append(line);
        }
        fin.close();
        return text.toString().getBytes();
    }

}
