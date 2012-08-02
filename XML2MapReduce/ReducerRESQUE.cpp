/*refactored from reducer.cpp of the RESQUE system*/

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <list>
#include <fstream>
#include <sstream>

#include <jni.h>
#include "edu_osu_cse_ysmart_testquery_ReducerRESQUE.h"

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

typedef map<string,map<int,vector<polygon*> > > polymap;
typedef map<string,map<int,vector<box*> > > boxmap;

polymap markup;
boxmap outline;

const char offset = '1';
const string tab = "\t";
const char comma = ',';




JNIEXPORT jobjectArray JNICALL Java_edu_osu_cse_ysmart_testquery_ReducerRESQUE_intersects (JNIEnv *env, jobject obj, jstring s1, jstring s2)
{
  //read input as 2 strings
  jboolean blnIsCopy1;
  jboolean blnIsCopy2;
  const char* strCIn1 = (env)->GetStringUTFChars(s1 , &blnIsCopy1);//codeop
  const char* strCIn2 = (env)->GetStringUTFChars(s2 , &blnIsCopy2);//input file name

  jobjectArray rez;
  list<string> rez_partial;
  int l_rez_partial=0;
  int i=0;
  
  //belong to resque
  string input_line;

  polygon * shape= NULL;
  box * mbb = NULL;

  int index =0;
  string key ;
  string value;
  boxmap::iterator iter;
 
  int size = -1;
  string leftright;
  string strindex;
  int index2 =0;
  
  size_t pos2=0;
  
  //read from input file, then standard RESQUE processing, with matching pairs put in list rez_partial
  
  ifstream myfile (strCIn2);
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {
      getline (myfile,input_line);
      cout << input_line << endl;
    
      size_t pos = input_line.find_first_of(tab);
      key = input_line.substr(0,pos-0);
      //cerr << "de la 0 la "<<pos;
      
      pos2 = input_line.find_first_of(tab, pos+1);
      leftright=input_line.substr(pos+1,pos2-pos-1);
      //cerr << "de la "<<pos+1<<" la "<<pos2;
      
      //index = input_line[pos+1] - offset;   // the very first character denotes join index
      pos = input_line.find_first_of(tab, pos2+1);
      strindex=input_line.substr(pos2+1,pos-pos2-1);
      //cerr << "de la "<<pos2+1<<" la "<<pos;
      //index = atoi( strindex.c_str() );//convert string to int
      stringstream ss(strindex);
      ss >> index2;
      
      
      pos2 = input_line.find_first_of(tab, pos+1);
      value = input_line.substr(pos+1,pos2-pos-1); // already in wkt format
      //cerr << "de la "<<pos+1<<" la "<<pos2;
      
      cerr << "key: "   <<  key << endl;
      cerr << "leftright:"<< leftright <<endl;
      cerr << "index2: " << index2 << endl;
      cerr << "value: " << value << endl;
      
      shape = new polygon();
      mbb   = new box();
      boost::geometry::read_wkt(value,*shape);
      boost::geometry::correct(*shape);
      boost::geometry::envelope(*shape, *mbb);
      
      if(leftright.compare("L")==0)
	index=0;
      else if(leftright.compare("R")==0)
	index=1;
      else
	cerr << "ERROR: unexpected value, neither L nor R";
      
      outline[key][index].push_back(mbb);
      markup[key][index].push_back(shape);
    }
    
    myfile.close();
  }
  else cout << "Unable to open file "<<strCIn2; 
  
  
  for (iter= outline.begin(); iter != outline.end(); iter++)
	{
	    key  = iter->first;
	    map<int ,vector<box*> > &mbbs = outline[key];
	    map<int ,vector<polygon*> > &shapes = markup[key];

	    size = mbbs.size(); // join cardinality 
	    vector<polygon> output;

	    int outerloop_size = (mbbs[0]).size(); //src tile polygon size
	    for (int i =1; i< size ;i++)
	    {
		int innerloop_size = (mbbs[i]).size(); // tar tile polygon size 
		for (int j =0; j< outerloop_size; j++)
		{
		    for (int k=0; k< innerloop_size; k++)
		    {
		        //check opcode --> boost::overlaps,etc
			if (boost::geometry::intersects( *(mbbs[0][j]), *(mbbs[i][k])) )
			    //&& boost::geometry::intersects(*((*shapes)[0][j]), *((*shapes)[i][k]))) // if the mbb && shapes intersects 
			{
			  
			  //put in list rez_partial
			  cerr<< "they do intersect";
			  string ss2="";
			  stringstream ss,sss;
			  ss << j;
			  
			  ss2+=ss.str();
			  ss2+=string("@");
			  
			  sss << k;
			  ss2+=sss.str();
			  
			  cerr << "ss2="<<ss2<<endl;
			  rez_partial.push_back(ss2);
			  
			  
			}
		    }
		}
	    }
	}
  
  
  
  
  //put the list elements into the output jObjectArray
  for (list<string>::iterator it = rez_partial.begin(); it != rez_partial.end(); it++)
    l_rez_partial++;
  
  rez= (jobjectArray)env->NewObjectArray(l_rez_partial, env->FindClass("java/lang/String"),env->NewStringUTF(""));  
  
  
  for (list<string>::iterator it2 = rez_partial.begin(); it2 != rez_partial.end(); it2++){
     
            env->SetObjectArrayElement(rez,i,env->NewStringUTF((*it2).c_str()));  
	    i++;
	    cerr << (*it2).c_str() << endl;
        } 
  
  return (rez);
  
  
  
}


