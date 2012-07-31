public class ReducerRESQUE{

  public static void main(String[] args) {

  System.out.println("rez="+(new ReducerRESQUE().intersects("POLYGON((10 20, 20 30, 40 50, 10,20))","POLYGON((10 20, 20 30, 30 40, 10 20))")));
  }

  private native int intersects(String a,String b);
  //here to be added declarations for other native methods like overlaps,etc

  static 
  {
    System.loadLibrary("ReducerRESQUE");
  }
}