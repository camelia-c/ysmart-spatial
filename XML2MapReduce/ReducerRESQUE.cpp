/*refactored from reducer.cpp of the RESQUE system*/

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>

#include <jni.h>
#include "ReducerRESQUE.h"

#include <boost/foreach.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/domains/gis/io/wkt/wkt.hpp>
#include <boost/geometry/domains/gis/io/wkt/wkt.hpp>
//#include <boost/geometry/algorithms/centroid.hpp>
//#include <boost/geometry/geometries/adapted/boost_tuple.hpp>

//BOOST_GEOMETRY_REGISTER_BOOST_TUPLE_CS(cs::cartesian)

using namespace std;

typedef boost::geometry::model::d2::point_xy<int> point;
typedef boost::geometry::model::polygon<point> polygon;
typedef boost::geometry::model::box<point> box;



JNIEXPORT jint JNICALL Java_ReducerRESQUE_intersects (JNIEnv *env, jobject obj, jstring s1, jstring s2)
{
  //read and build mbr for the first geometry, read and build mbr for second geometry
  jboolean blnIsCopy1;
  jboolean blnIsCopy2;
  const char* strCIn1 = (env)->GetStringUTFChars(s1 , &blnIsCopy1);
  const char* strCIn2 = (env)->GetStringUTFChars(s2 , &blnIsCopy2);

  
  polygon * shape1= NULL;
  box * mbb1 = NULL;
  shape1 = new polygon();
  mbb1   = new box();
  
  polygon * shape2= NULL;
  box * mbb2 = NULL;
  shape2 = new polygon();
  mbb2   = new box();
  
  
  boost::geometry::read_wkt(strCIn1,*shape1);
  boost::geometry::correct(*shape1);
  boost::geometry::envelope(*shape1, *mbb1);

  boost::geometry::read_wkt(strCIn2,*shape2);
  boost::geometry::correct(*shape2);
  boost::geometry::envelope(*shape2, *mbb2);
  
  if (boost::geometry::intersects( *mbb1, *mbb2) )
  {
    cout << "they intersect s1= "<<strCIn1<<" and s2= "<<strCIn2;
    return 1;
  }
  
  return 0;
  
}


