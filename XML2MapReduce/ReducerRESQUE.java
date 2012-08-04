package edu.osu.cse.ysmart.testquery;

import org.apache.hadoop.conf.*;

public class ReducerRESQUE extends Configured{

  public static void main (String[] args) throws Exception{

  String[] res=new ReducerRESQUE().intersects("INTER","resque_text.txt");
  System.out.println("");
  System.out.println("nrelem="+res.length);
  for(int j=0;j<res.length;j++)
    System.out.println("******** "+res[j]+" **********");

  }

  public native String[] intersects(String codeop,String filename);
  

  static 
  {
    System.loadLibrary("ReducerRESQUE");
  }
}