import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.InetAddress;
import java.net.Socket;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import javax.crypto.spec.SecretKeySpec;
import java.security.NoSuchAlgorithmException;
import java.io.FileInputStream;
import java.io.UnsupportedEncodingException;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.io.*;
import java.io.File;
import java.util.Arrays;
import javax.crypto.spec.SecretKeySpec;
import javax.crypto.spec.IvParameterSpec;

import javax.crypto.Cipher;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.NoSuchAlgorithmException;

import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import javax.crypto.spec.SecretKeySpec;
import java.io.*;
import java.io.File;
import java.util.Arrays;
public class Client
{
  //For converting byte array to Hex string
  public static String byteArrayToHex(byte[] a) {
   StringBuilder sb = new StringBuilder(a.length * 2);
   for(byte b: a)
      sb.append(String.format("%02x", b));
   return sb.toString();
}
//For Generating Key!
	private static byte[] generateEncryptionSecret() {
    try{
      KeyGenerator keyGen = KeyGenerator.getInstance("DES");

      // generate a key
      SecretKey key = keyGen.generateKey();

      // get the raw key bytes
      byte[] keyBytes = key.getEncoded();
      return keyBytes;}
      catch (NoSuchAlgorithmException e) {
  			System.out.println("No Such Algorithm:" + e.getMessage());
  			return null;
  		}
	}
  // For writing String in File
  private static void WriteStrInFile(String text, String path){

       try {
           Files.write(Paths.get(path), text.getBytes());
       } catch (IOException e) {
           e.printStackTrace();
       }
  }
  // For Encryption
    private static String IV = "AAAAAAAAAAAAAAAA";
    private static String encryptionKey;
    //preparing the string for encryption
    public static String prepareString(String plaintext){
      if(plaintext.length()%16!=0){
        int l=plaintext.length()%16;
        for(int i=l;i<16;i++){
          plaintext+="\0";
        }
      }
      return plaintext;
    }
    // encrypt.
    public static byte[] encrypt(String plainText, String encryptionKey) throws Exception {

      Cipher cipher = Cipher.getInstance("AES/CBC/NoPadding", "SunJCE");
      SecretKeySpec key = new SecretKeySpec(encryptionKey.getBytes("UTF-8"), "AES");
      cipher.init(Cipher.ENCRYPT_MODE, key,new IvParameterSpec(IV.getBytes("UTF-8")));
      return cipher.doFinal(plainText.getBytes("UTF-8"));
    }

    private static Socket socket;
    private static int Client_ID;
    // Calculating MD5
    public static String getMD5(String input) {
        try {
            MessageDigest md = MessageDigest.getInstance("MD5");
            byte[] messageDigest = md.digest(input.getBytes());
            BigInteger number = new BigInteger(1, messageDigest);
            String hashtext = number.toString(16);
            // Now we need to zero pad it if you actually want the full 32 chars.
            while (hashtext.length() < 32) {
                hashtext = "0" + hashtext;
            }
            return hashtext;
        }
        catch (NoSuchAlgorithmException e) {
            throw new RuntimeException(e);
        }
    }
    // This is main function. The program start from here!
    public static void main(String args[])
    {
      // For checking the Input.
      String MyInput="";
      if(args.length==0){
        System.out.println("You should send an Input as an argument");
        return;
      }
      else
      {
        int inputLength=0;
      for(int i=0;i<args.length;i++)
      {
        if(i==args.length)
        MyInput+=args[i];
        else
        MyInput+=args[i]+" ";
        inputLength+=args[i].length()+1;
      }
      // Check size of Input..
      if(inputLength>50)
      {
        System.out.println("Size Exceed!");
        return;
      }
    }
        try
        {
            String host = "127.0.0.1";
            int port = 6722;
            InetAddress address = InetAddress.getByName(host);
            socket = new Socket(address, port);
            //Recieve the ID
            InputStream iss = socket.getInputStream();
            InputStreamReader isrs = new InputStreamReader(iss);
            BufferedReader brs = new BufferedReader(isrs);
            String messages = brs.readLine();
          // server assigned an Integer number to client.
            System.out.println("Client Number: " +messages);
            Client_ID=Integer.parseInt(messages);
            //Create File consist of key!
            byte[] keyBytes= generateEncryptionSecret();
          //Write String in File
          encryptionKey=byteArrayToHex(keyBytes);
          WriteStrInFile(encryptionKey,messages+".txt");
            //Send the message to the server
            OutputStream os = socket.getOutputStream();
            OutputStreamWriter osw = new OutputStreamWriter(os);
            BufferedWriter bw = new BufferedWriter(osw);

            for(int i=0;i<2;i++){
// Send two messages to Server.
            String number = MyInput;
            if(i==1){
              number=getMD5(number);

              // Apply Secret key to hashed text
              String plaintext=prepareString(number);
                byte[] cipher = encrypt(plaintext, encryptionKey);
                cipher=Arrays.copyOfRange(cipher,0, number.length()/2);
                number="";
                for (int ii=0; ii<cipher.length; ii++)

                number+=Integer.toString(new Integer(cipher[ii]));
            }
            String sendMessage = number + "\n";
            bw.write(sendMessage);
            bw.flush();

            System.out.println("Message sent to the server : "+sendMessage );


            //Get the return message from the server
            InputStream is = socket.getInputStream();
            InputStreamReader isr = new InputStreamReader(is);
            BufferedReader br = new BufferedReader(isr);
            String message = br.readLine();
            System.out.println("Message received from the server : " +message);
          }
        }
        catch (Exception exception)
        {
            exception.printStackTrace();
        }
        finally
        {
            //Closing the socket
            try
            {
                socket.close();
            }
            catch(Exception e)
            {
                e.printStackTrace();
            }
        }
    }
}
