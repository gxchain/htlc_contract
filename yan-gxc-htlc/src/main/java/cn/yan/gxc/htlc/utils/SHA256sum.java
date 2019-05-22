package cn.yan.gxc.htlc.utils;

import java.io.*;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.logging.Level;
import java.util.logging.Logger;
/*MD256sum provides several methods for md256 standards
 *Copyright (C) 2015  Adam Outler
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see https://www.gnu.org/licenses/ .
 */

/**
 * provides several methods for md256 standards
 *
 * @author Adam Outler adamoutler@gmail.com
 */
/**
 * attempts to replicates linux's sha256sum.. there appears to be a problem with
 * *Linux* when tested against test vectors from this page:
 * http://www.nsrl.nist.gov/testdata/ I will need to review all data and figure
 * out how to implement this later
 *
 */

/**
* Created by gentlemen_yan on 2019/3/30.
*/
public class SHA256sum {


   /**
    * spacer used to separate SHA256 and filename in standard sha256sum
    */
   final protected static String LINUXSPACER = "  ";

   /**
    * returns SHA256 sum in standard linux command line format
    *
    * @param file to use for filename
    * @return linux sha256sum output
    */
   public static String getLinuxSum(File file) {
       String name = file.getName();
       String sum;

       try {
           sum = new SHA256sum(file).getSha256();
           String linuxSHA256;
           linuxSHA256 = formatLinuxOutputSHA256Sum(sum, name);
           return linuxSHA256;
       } catch (FileNotFoundException ex) {
           return "";
       } catch (NoSuchAlgorithmException ex) {
           return "";
       } catch (IOException ex) {
           return "";
       }
   }

   /**
    * gets the filename from a commandline sha256sum output
    *
    * @param sha256sum linux sha256sum to extract name from
    * @return name of file mentioned in sha256sum
    */
   public static String getName(String sha256sum) {
       if (sha256sum.contains(LINUXSPACER)) {
           String[] split = sha256sum.split(LINUXSPACER);
           return split[1];
       }
       return "";
   }

   /**
    * gets the sha256sum portion of a commandline sha256 output
    *
    * @param sha256sum linux sha256sum to extract sum from
    * @return sum portion of command line sha256 output
    */
   public static String getSum(String sha256sum) {
       if (sha256sum.contains(LINUXSPACER)) {
           String[] split = sha256sum.split(LINUXSPACER);
           return split[0];
       }
       return "";
   }

   /**
    * converts a byte array to hexadecimal output
    *
    * @param bytes to be turned into hex
    * @return hex string from bytes
    */
   public static String bytesToHex(byte[] bytes) {
       StringBuilder sb = new StringBuilder();
       for (byte b : bytes) {
           sb.append(String.format("%02x", b));
       }
       return sb.toString();
   }

   /**
    * formats a sha256sum from a sum and a filename
    *
    * @param sum the sha256 sum
    * @param name the file name
    * @return equal to command line output from linux sha256sum command
    */
   public static String formatLinuxOutputSHA256Sum(String sum, String name) {
       String linuxSHA256;
       linuxSHA256 = sum + LINUXSPACER + name;
       return linuxSHA256;
   }
   final ByteArrayInputStream toBeSHA256;

   /**
    * constructor to make an SHA256 from a string
    *
    * @param s string to sha256
    * @throws IOException {@inheritDoc}
    */
   public SHA256sum(String s) throws IOException {
       ByteArrayInputStream bas = new ByteArrayInputStream(s.getBytes());
       toBeSHA256 = bas;
       toBeSHA256.mark(0);
   }

   /**
    * constructor to make an SHA256 from an InputStream
    *
    * @param is inputstream to sha256
    * @throws IOException {@inheritDoc}
    */
   public SHA256sum(InputStream is) throws IOException {

       byte[] buff = new byte[8120];
       int bytesRead;
       ByteArrayOutputStream bao = new ByteArrayOutputStream();
       while ((bytesRead = is.read(buff)) != -1) {
           bao.write(buff, 0, bytesRead);
       }
       ByteArrayInputStream bin = new ByteArrayInputStream(bao.toByteArray());
       toBeSHA256 = bin;
       toBeSHA256.mark(0);
   }

   /**
    * constructor to sha256 a file
    *
    * @param f file to digest
    * @throws FileNotFoundException {@inheritDoc}
    * @throws IOException {@inheritDoc}
    */
   public SHA256sum(File f) throws FileNotFoundException, IOException {

       RandomAccessFile ra;
       ra = new RandomAccessFile(f, "rw");
       byte[] b = new byte[(int) f.length()];
       ra.read(b);
       ByteArrayInputStream bas = new ByteArrayInputStream(b);
       toBeSHA256 = bas;
       toBeSHA256.mark(0);
   }

   /**
    * returns SHA256 sum in standard linux command line format
    *
    * @param filename to use for filename
    * @return linux sha256sum output
    */
   public String getLinuxSum(String filename) {
       if (filename.isEmpty()) {
           filename = "-";
       }
       try {
           String sha = getSha256();
           return sha + LINUXSPACER + filename;
       } catch (IOException ex) {
           return null;
       } catch (NoSuchAlgorithmException ex) {
           return null;
       }

   }

   /**
    * does the SHA256
    *
    * @return hex string representation of the input
    * @throws IOException {@inheritDoc}
    * @throws NoSuchAlgorithmException {@inheritDoc}
    */
   public String getSha256() throws IOException, NoSuchAlgorithmException {
       toBeSHA256.reset();

       MessageDigest digest = MessageDigest.getInstance("SHA-256");

       byte[] buffer = new byte[8192];
       int read;
       while ((read = toBeSHA256.read(buffer)) > 0) {
           digest.update(buffer, 0, read);
       }
       byte[] md5sum = digest.digest();
       BigInteger bigInt = new BigInteger(1, md5sum);
       String output = bigInt.toString(16);
       while (output.length() != 64) {
           output = "0" + output;
       }
       return output;

   }

   @Override
   public String toString() {
       String sum = "INVALID000000000000000000000000000000000000000000000000000000000";
       try {
           sum = getSha256();
       } catch (IOException ex) {
           Logger.getLogger(SHA256sum.class.getName()).log(Level.SEVERE, null, ex);
       } catch (NoSuchAlgorithmException ex) {
           Logger.getLogger(SHA256sum.class.getName()).log(Level.SEVERE, null, ex);
       }
       return sum;
   }

    public SHA256sum(byte[] binStr) throws IOException {
        ByteArrayInputStream bas = new ByteArrayInputStream(binStr);
        toBeSHA256 = bas;
        toBeSHA256.mark(0);
    }

    /**
     * [root@izt4ne1v58t0rxvzy142w3z ~]# echo -n '9c490308664645d104eaef0a80c24ec253f8f656ef7d0832929ce46a1655b35c' | xxd -r -p | sha256sum -b | awk '{print $1}'
     * d4faff0ab6d1005ed0f91271ff00fbab090392bb5e4cc0ed1d50ec7654326cfc
     */
    public static void main(String[] args) throws IOException {
        // 这个有问题
        byte[] binStr = YanStrUtils.hexStringToByte("9c490308664645d104eaef0a80c24ec253f8f656ef7d0832929ce46a1655b35c");
        SHA256sum sha256sum = new SHA256sum(binStr);
        System.out.println(sha256sum);
    }
}