/*refactored from reducer2.cpp  of the RESQUE system*/

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <list>
#include <fstream>
#include <sstream>

#include "Reducer.h"

#include <jni.h>
#include "edu_osu_cse_ysmart_testquery_ReducerRESQUE.h"


using namespace SpatialIndex;
using namespace std;

#define INSERT 1
#define DELETE 0
#define QUERY 2

map<string,map<int,vector<string> > > data;

map<string,vector<int> > leftgeoms;
map<string,vector<int> > rightgeoms;



const char offset = '1';




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


  // ----------------- start of code from RESQUE
  string input_line;
  int index =0;
  string key ;
  string value;
  
  
  int size = -1;
  string leftright;
  string strindex;
  int index2 =0;
  
  size_t pos2=0;
  
  if(!rez_partial.empty())
  {
    rez_partial.clear();
    //cout << "list initialized, size=" <<(int)rez_partial.size()<< endl;
  }
  if(!data.empty())
  {
    data.clear();;
    //cout << "map initialized, size=" <<(int)data.size()<< endl;
  }
  if(!v_pairs_l.empty())
  {
    v_pairs_l.clear();;
    //cout << "v_pairs_l initialized, size=" <<(int)v_pairs_l.size()<< endl;
  }
  if(!v_pairs_r.empty())
  {
    v_pairs_r.clear();;
    //cout << "v_pairs_r initialized, size=" <<(int)v_pairs_r.size()<< endl;
  }
  n_v_pairs=0;
  
  //read from input file, then standard RESQUE processing, with matching pairs put in list rez_partial
  
  
  ifstream myfile (strCIn2);
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {
      //cerr << "--------------------------------------- READ FROM RESQUE_TEXT.TXT----------------------------------------" << endl;
      getline (myfile,input_line);
      //cout << input_line << endl;
    
      size_t pos = input_line.find_first_of(tab);
      key = input_line.substr(0,pos-0);
      
      
      pos2 = input_line.find_first_of(tab, pos+1);
      leftright=input_line.substr(pos+1,pos2-pos-1);
      
      
      
      pos = input_line.find_first_of(tab, pos2+1);
      strindex=input_line.substr(pos2+1,pos-pos2-1);
      
      
      stringstream ss(strindex);
      ss >> index2;
      
      
      pos2 = input_line.find_first_of(tab, pos+1);
      value = input_line.substr(pos+1,pos2-pos-1); // already in wkt format
      
      
      //cerr << "key: "   <<  key << endl;
      //cerr << "leftright:"<< leftright <<endl;
      //cerr << "index2: " << index2 << endl;
      //cerr << "value: " << value << endl;
      
      replaceAll(value, ", ", ",");
      replaceAll(value, " ,", ",");
      //cerr << "value after blanks elimination: " << value << endl;
      
      
      if(leftright.compare("L")==0)
      {
	index=0;
	leftgeoms[value].push_back(index2);
	//cerr << "leftgeoms["<<value<<"]="<<leftgeoms[value]<<endl;
      }
      else if(leftright.compare("R")==0)
      {
	index=1;
	rightgeoms[value].push_back(index2);
	//cerr << "rightgeoms["<<value<<"]="<<rightgeoms[value]<<endl;
      }
      else
	cerr << "ERROR: unexpected value, neither L nor R";
      
      
      data[key][index].push_back(value);
      
      
     
    }
    myfile.close();
    //cerr << "................................................. END READ FROM RESQUE_TEXT.TXT................................................ " << endl;
    //cout << "map size=" <<(int)data.size()<< endl;
    
  }
  else cout << "Unable to open file "<<strCIn2; 
  
  //-------------------- what was previously in main
  IStorageManager ** storages = NULL ;//new IStorageManager * [data.size()];
  ISpatialIndex ** forest = NULL ;    //new ISpatialIndex * [data.size()];
  id_type * indexIdentifier = NULL;
  map<string,map<int,vector<string> > >::iterator iter;

  bool initilize = true;
  
  size = -1;
   try
    {
        //std::cerr << "ham-ham" << std::endl;
	time_t begin, end;
	
	
	// for each key in the input stream 
	    for (iter= data.begin(); iter !=data.end(); iter++)
	    {
		key  = iter->first;
		map<int ,vector<string> > * images = &(iter->second);
		size = (*images).size();
		
		//std::cerr <<"size="<<size;
		std::cerr << "NEXT, THE INITIALIZE PART.........."<<std::endl;
		if (initilize)
		{
		    //we assume all the multiwya joins are the same.
		    storages = new IStorageManager * [size];
		    forest = new ISpatialIndex * [size];
		    indexIdentifier = new id_type [size];
		    initilize =false;
		}

		// for each spatial data in a set of images 
		std::cerr << "NEXT, THE FOREST PART.........."<<std::endl;
		for (int i =0 ;i <size; i++)
		{
		    storages[i]= StorageManager::createNewMemoryStorageManager();
		    MyDataStream stream((*images)[i], i+1);

		    // Create and bulk load a new RTree with dimensionality 2, using memory as the StorageManager and the RSTAR splitting policy.
		    forest[i]= RTree::createAndBulkLoadNewRTree( 
			    RTree::BLM_STR, stream, *storages[i], FillFactor, IndexCapacity, LeafCapacity, 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier[i]);

		    std::cerr << "FOREST["<<i<<"]= "<<*forest[i];
		    //std::cerr << "Buffer hits: " << file->getHits() << std::endl;
		    std::cerr << "Index ID: " << indexIdentifier[i] << std::endl;

		    bool ret = forest[i]->isIndexValid();
		    if (ret == false) std::cerr << "ERROR: Structure is invalid!" << std::endl;
		    else std::cerr << "The stucture seems O.K." << std::endl;
		}

		/*Spatial Join 
		 * Query Type: Star Query --> One standard baseline is compared against multiple allgorithms.
		 * Then the intersection result is reported individually */
		std::cerr << "NEXT, THE JOIN PART.........."<<std::endl;
		for (int i =1 ; i < size; i++)
		{
		    MyVisitor vis;
		    forest[0]->joinQuery(vis, forest[i]);
		    //cerr<< "done with one " << i<< endl; 
		    
		    //cerr<< "size is "<<outputstrings.size()<< endl; 
		}
		//reduce output 
		std::cerr << "NEXT, THE OUTPUT PART.........."<<std::endl;
		//we get the pairs of geometries that match and find their index in the map
		for(int i=0;i< n_v_pairs;i++)
		{
		    int ind_left=0;
		    int ind_right=0;
		    string geom_left=v_pairs_l[i];
		    string geom_right=v_pairs_r[i];
		    
		    //std::cerr<<"geom_left="<<geom_left<<std::endl;
		    //std::cerr<<"geom_right="<<geom_right<<std::endl;
		    
		    //to search in vector
		    for(int jj=0;jj<leftgeoms[geom_left].size();jj++)
		    {
		      ind_left=leftgeoms[geom_left][jj];
		      for(int kk=0;kk<rightgeoms[geom_right].size();kk++)
		      {
			
			ind_right=rightgeoms[geom_right][kk];
			
			string ss2="";
			stringstream ss,sss;
			ss << ind_left;
			
			ss2+=ss.str();
			ss2+=string("@");
			
			sss << ind_right;
			ss2+=sss.str();
			
			//cerr << "ss2="<<ss2<<endl;
			
			//put it only if it diesn't already exist
			int existsalready=0;
			for (list<string>::iterator it3 = rez_partial.begin(); it3 != rez_partial.end(); it3++)
			{
			  string anelem=(*it3).c_str();
			  //cerr << "a string in list="<< anelem;
			  if(ss2.compare(anelem)==0)
			    existsalready=1;
			}
			if (existsalready==0)
			  rez_partial.push_back(ss2);
		      }
		    }
		}
		
		//cout << key << tab << summarize()<< endl;

		// clean up the allocated space
		for (int i =0 ; i < size; i++)
		{
		    delete forest[i];
		    delete storages[i];
		}
	    }
	    delete [] forest;
	    delete [] storages;
	    delete [] indexIdentifier;
	
	
	
	
         
      }
    catch (Tools::Exception& e)
    {
	std::cerr << "******ERROR******" << std::endl;
	std::string s = e.what();
	std::cerr << s << std::endl;
	rez= (jobjectArray)env->NewObjectArray(l_rez_partial, env->FindClass("java/lang/String"),env->NewStringUTF(""));  
	return (rez);
    }

  




  // -------------------end of refactored & adapted code from RESQUE

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