#include <math.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <sstream>
#include <spatialindex/SpatialIndex.h>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Cartesian.h>
#include <CGAL/centroid.h>
#include <CGAL/squared_distance_2.h>
#include <CGAL/bounding_box.h>
#include <CGAL/number_utils.h>

#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <IndexParam.h>

using namespace SpatialIndex;
using namespace std;

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_2                                   Point_2;
typedef CGAL::Polygon_2<Kernel>                           Polygon_2;
typedef CGAL::Polygon_with_holes_2<Kernel>                Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>                   Pwh_list_2;

const string tab = "\t";
const char comma = ',';
const char spatiu = ' ';

vector<string> joinresults;
vector<double> values(4,0.0);


vector <string> v_pairs_l; // for ysmartspatial
vector <string> v_pairs_r; // for ysmartspatial
int n_v_pairs=0;

const int DEBUG=0;
string bar= "|" ;

    template<class Kernel, class Container>
string print_polygon (const CGAL::Polygon_2<Kernel, Container>& P)
{
    stringstream polyoss;
    bool flag =true; //indicates start ;
    typename CGAL::Polygon_2<Kernel, Container>::Vertex_const_iterator  vit;

    for (vit = P.vertices_begin(); vit != P.vertices_end(); ++vit)
    {
	if (flag)
	{
	    polyoss<< *vit;
	    flag =false;
	}
	else 
	    polyoss << comma <<*vit ;
    }
    return polyoss.str();

}

    template<class Kernel, class Container>
void print_polygon_with_holes(const CGAL::Polygon_with_holes_2<Kernel, Container> & pwh)
{
    if (! pwh.is_unbounded()) {
	print_polygon (pwh.outer_boundary());
    }
    else
	std::cerr << "{ unbounded polygon }" << std::endl;

    typename CGAL::Polygon_with_holes_2<Kernel,Container>::Hole_const_iterator hit;
    unsigned int k = 1;

    std::cout << "  " << pwh.number_of_holes() << " holes:" << std::endl;
    for (hit = pwh.holes_begin(); hit != pwh.holes_end(); ++hit, ++k) {
	std::cout << "    Hole #" << k << " = ";
	print_polygon (*hit);
    }
    std::cout << " }" << std::endl;
}


// this function is useful for ysmartspatial, introduced on August 13th
void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
	start_pos += to.length(); 

        
    }
}


// this function is useful for ysmartspatial, introduced on August 13th
string extract_util_wkt(string p1)
{
  string auxstr;
  string utilstr="";
  string lastpoint="";
  size_t pos,pos2;
  
  pos = p1.find_first_of(spatiu);
  auxstr = p1.substr(0,pos-0);
  
  pos2 = p1.find_first_of(spatiu, pos+1);
  auxstr=p1.substr(pos+1,pos2-pos-1);
  
  
  pos = p1.find_first_of(spatiu, pos2+1);
  auxstr=p1.substr(pos2+1,pos-pos2-1);
  
  pos2 = p1.find_first_of(spatiu, pos+1);
  auxstr=p1.substr(pos+1,pos2-pos-1);
  
  
  //now is the second point of the polygon
  while(pos2!=-1)
  {
    lastpoint="";
    pos = p1.find_first_of(spatiu, pos2+1);
    auxstr=p1.substr(pos2+1,pos-pos2-1);
    
    utilstr=utilstr+auxstr;
    lastpoint=lastpoint+auxstr;
    
    pos2 = p1.find_first_of(spatiu, pos+1);
    auxstr=p1.substr(pos+1,pos2-pos-1);
    
    utilstr=utilstr+" "+auxstr+",";
    
    lastpoint=lastpoint+" "+auxstr+",";
        
  }
  
  utilstr=lastpoint+utilstr; //add last point at the beginning
  utilstr=utilstr.substr(0,utilstr.length()-1);
  
  
  
  return utilstr;
  
}


// this function is useful for ysmartspatial, introduced on August 13th
void store_pairs(string p1,string p2)
{

  string utilstr1="";
  string utilstr2="";
 
  
  
  
  
  utilstr1="POLYGON(("+extract_util_wkt(p1)+"))";
  utilstr2="POLYGON(("+extract_util_wkt(p2)+"))";
  
  
  //std::cout <<"______________________A PAIR OF MATCHING GEOMETRIES_______________________"<< std::endl;
  //std::cout <<"utilstr1="<<utilstr1<< std::endl;
  //std::cout <<"utilstr2="<<utilstr2<< std::endl;
      
  v_pairs_l.push_back(utilstr1);
  v_pairs_r.push_back(utilstr2); //synchronized ith v_pairs_l
  n_v_pairs++;
  
  
}


// this function is useful for ysmartspatial, introduced on August 13th

Polygon_2 build_polyg(string s)
{
  size_t pos,pos2;
  size_t pos3;
  Polygon_2 Pol;
  string auxstr_x,auxstr_y,lastpoint;
  int x,y;
  
  //process first point of the polygon
  pos = s.find_first_of(comma);
  pos2=pos;
  
  
  //process points from 1 to n-1 of the polygon (except  last)
  while(pos2!=-1)
  {
    lastpoint="";
    pos3 = s.find_first_of(spatiu, pos2+1);
    auxstr_x=s.substr(pos2+1,pos3-pos2-1);
    
    
    pos=pos2;
    
    pos2 = s.find_first_of(comma, pos+1);
    auxstr_y=s.substr(pos3+1,pos2-pos3-1);
    

    stringstream ss_x(auxstr_x);
    stringstream ss_y(auxstr_y);
    
    ss_x >> x;
    ss_y >> y;
    
    Pol.push_back (Point_2 (x,y));
    
  }
  
  
  
  return Pol;
  
}



void update_join(double overlap_area, double union_area, double squared_distance) {
    values[0] += overlap_area/union_area;
    values[1] += squared_distance;
    values[2] += overlap_area;
    values[3] += union_area;
}

void reset () {
    values[0] =0.0;
    values[1] =0.0;
    values[2] =0.0;
    values[3] =0.0;
}

string summarize() {
    stringstream oss;
    oss << values[0] << bar<< values[1] << bar << values[2] << bar << values[3];
    reset();
    return oss.str();
}

std::string getDataString(const IData* d) {
    byte* pData = 0;
    uint32_t cLen = 0;
    d->getData(cLen, &pData);
    string s = reinterpret_cast<char*>(pData);
    delete[] pData;
    return s;
}

void parsePolygon(std::string str,Polygon_2 &P,string &nameP) {
    std::istringstream iss2(str);
    int n;
    double x,y,lastx,lasty;
    iss2>>nameP;
    iss2>>n;
    for (int i=0; i<n; i++) {
	iss2>>x>>y;
	if (COMPRESS)
	    if (i>0) {
		x+=lastx;
		y+=lasty;
	    }
	lastx=x;
	lasty=y;
	P.push_back (Point_2 (x, y));
    }
    //std::cout<<"Polygon: "<<P<<std::endl;
}


template <class Kernel, class Container>
    typename CGAL::Polygon_2<Kernel, Container>::FT
area( const CGAL::Polygon_with_holes_2<Kernel, Container>& pwh)
{
    typedef typename CGAL::Polygon_2<Kernel, Container>::FT                                         FT;
    FT result = pwh.outer_boundary().area();
    for (Polygon_with_holes_2::Hole_const_iterator jt = pwh.holes_begin();
	    jt != pwh.holes_end(); ++jt)
    {
	result += jt->area();
    }
    return result;
}

template <class Kernel, class Container>
    typename CGAL::Polygon_2<Kernel, Container>::FT
overlap_area(const CGAL::Polygon_2<Kernel, Container> &P,
	const CGAL::Polygon_2<Kernel, Container> &Q)
{
    CGAL_precondition(P.is_simple());
    CGAL_precondition(Q.is_simple());

    typedef typename CGAL::Polygon_2<Kernel, Container>::FT FT;
    Pwh_list_2 overlap;

    //intersectionTimer.start(timerCount);
    //cerr<<"before calling CGAL::intersection: \nP="<<P<<"\nQ="<<Q<<endl;
    CGAL::intersection(P, Q, std::back_inserter(overlap));
    //CGAL::join(P, Q, std::back_inserter(overlap));
    //cerr<<"after calling CGAL::intersection"<<endl;
    //exit(1);
    //intersectionTimer.end(timerCount);

    if (overlap.empty())
	return 0;
    // summing the areas and reducing the area of holes.
    FT result = 0;
    for (Pwh_list_2::iterator it = overlap.begin(); it != overlap.end(); ++it)
    {
	result += area(*it);
    }
    return result;
}


template <class Kernel, class Container>
    typename CGAL::Polygon_2<Kernel, Container>::FT
union_area(const CGAL::Polygon_2<Kernel, Container> &P,
	const CGAL::Polygon_2<Kernel, Container> &Q)
{
    CGAL_precondition(P.is_simple());
    CGAL_precondition(Q.is_simple());

    typedef typename CGAL::Polygon_2<Kernel, Container>::FT                                         FT;

    // Compute the union of P and Q.
    Polygon_with_holes_2 unionR;

    //unionTimer.start(timerCount);
    bool join = CGAL::join (P, Q, unionR);
    //unionTimer.end(timerCount);

    if (join)
	return area(unionR);
    else
	return P.area()+Q.area();
}


// example of a Visitor pattern.
// findes the index and leaf IO for answering the query and prints
// the resulting data IDs to stdout.
class MyVisitor : public IVisitor
{
    public:
	size_t m_indexIO;
	size_t m_leafIO;

    private: 
	int m_count;

    public:
	MyVisitor() : m_indexIO(0), m_leafIO(0) {m_count=0;}

	void visitNode(const INode& n)
	{   
	    if (n.isLeaf()) m_leafIO++;
	    else m_indexIO++;
	}   
	void visitData(std::vector<uint32_t>& v) {}
	void visitData(std::string& s) {}

	void visitData(const IData& d)
	{   
	    IShape* pS; 
	    d.getShape(&pS);
	    // do something.
	    delete pS; 

	    // data should be an array of characters representing a Region as a string.
	    byte* pData = 0;
	    uint32_t cLen = 0;
	    d.getData(cLen, &pData);
	    // do something.
	    //string s = reinterpret_cast<char*>(pData);
	    //cout << s << endl;
	    delete[] pData;

	    cout << "answer: "<<d.getIdentifier() << endl;
	    // the ID of this data entry is an answer to the query. I will just print it to stdout.
	}

	void visitData(std::vector<const IData*>& v)
	{   
	    //std::cerr << "We visited Data at least. !!!" << std::endl;
	    //visitDataTimer.start(timerCount);

	    //parsePolygonTimer.start(timerCount);
	    //cerr<<m_count<<'|'<<v[0]->getIdentifier() << "|" << v[1]->getIdentifier()<<endl;
	    string sP = getDataString(v[0]);
	    string sQ = getDataString(v[1]);
	    Polygon_2 P,Q;
	    string nameP,nameQ;
	    
	    parsePolygon(sP,P,nameP);
	    parsePolygon(sQ,Q,nameQ);
	    //parsePolygonTimer.end(timerCount);

	    //overlapAreaTimer.start(timerCount);
	    //double overlap_a = CGAL::to_double(overlap_area(P,Q)); //ysmartspatial
	    //overlapAreaTimer.end(timerCount);	

	    //std::cerr<<  "before overlap test: " <<overlap_a << std::endl;
	    
	    //std::cout<<"Polygon P : "<<P<<std::endl;
	    //std::cout<<"Polygon Q : "<<Q<<std::endl;

	    //if (overlap_a==0) return; //ysmartspatial
	    
	    //added for ysmartspatial
	    string wkt1=extract_util_wkt(sP);
	    string wkt2=extract_util_wkt(sQ);
	    
	    Polygon_2 PP=build_polyg(wkt1);
	    Polygon_2 QQ=build_polyg(wkt2);
	    
	    std::cerr<< sP << " | " << sQ <<std::endl;
	    
	    CGAL_precondition(PP.is_simple());
	    CGAL_precondition(QQ.is_simple());
	    
	    if (PP.orientation()<0)
	      PP.reverse_orientation();
	    if (QQ.orientation()<0)
	      QQ.reverse_orientation();
	    
	   
	    
	    if (!do_intersect(PP,QQ)) 	      return;//ysmartspatial

	    //unionAreaTimer.start(timerCount);
	    //double union_a1 = CGAL::to_double(union_area(P,Q));
	    //double union_a = CGAL::to_double(P.area()+Q.area())-overlap_a;//ysmartspatial
	    //cout<<union_a1<<' '<<union_a<<endl;
	    //exit(1);
	    //unionAreaTimer.end(timerCount);

	    //centroidDistanceTimer.start(timerCount);
	    //Point_2 c1;
	    //Point_2 c2;
	    //Point_2 c1 = CGAL::centroid(P.vertices_begin(), P.vertices_end()); //ysmartspatial
	    //Point_2 c2 = CGAL::centroid(Q.vertices_begin(), Q.vertices_end()); //ysmartspatial
	    //double squared_dist = CGAL::to_double(CGAL::squared_distance(c1,c2)); //ysmartspatial
	    //centroidDistanceTimer.end(timerCount);
	    /*
	    cout << "answer "<<m_count<<": "<<v[0]->getIdentifier() << " " << v[1]->getIdentifier() << endl;
	      std::cout<<"overlap area: "<<overlap_area(P,Q)<<std::endl;
	      std::cout<<"union area: "<<union_area(P,Q)<<std::endl;
	      std::cout<<"centroid: "<<c1<<" | "<<c2<<std::endl;
	      std::cout<<"area ratio: "<<overlap_area(P,Q)*1.0/union_area(P,Q)<<std::endl;
	      std::cout<< "distance: "<<sqrt(squared_dist)<<std::endl;
	      std::cout<<std::endl;
	      */
	    m_count++;
	    //stringstream oss;
	    //oss <<m_count<<'|'<<nameP<<'|'<<nameQ<<'|'<<v[0]->getIdentifier() << "|" << v[1]->getIdentifier()<< "|"<<overlap_a/union_a << "|"<<sqrt(squared_dist) << "|"<<overlap_a << "|"<< union_a <<"|"<<c1 <<"|"<<c2 ;
	    //std::cout<<m_count<<'|'<<nameP<<'|'<<nameQ<<'|'<<v[0]->getIdentifier() << "|" << v[1]->getIdentifier()<< "|"<<overlap_a/union_a << "|"<<sqrt(squared_dist) << "|"<<overlap_a << "|"<< union_a <<"|"<<c1 <<"|"<<c2<<std::endl;
	    if (DEBUG) std::cout<<"P1="<<P<<"\nP2="<<Q<<std::endl;
	    //outputstrings.push_back(oss.str());
	    //visitDataTimer.end(timerCount);

	    /*ablimit aji 2012 Febuary 07, added to simplify data aggregation process */
	    //update_join(overlap_a,union_a, squared_dist); //ysmartspatial
	    
	    /* camelia ciolac 2012 August 13, added for ysmartspatial integration*/
	     store_pairs(sP,sQ);
	     
	}
};




RTree::Data* parseInputLine(string line, id_type &m_id, int tag) {
    double low[2], high[2];
    string token;
    istringstream iss(line);
    ostringstream os;
    int lastx, lasty;
    int x,y;
    //parsePolygonTimer.start(timerCount);
    int size = std::count(line.begin(), line.end(), ',');
    os<<tag<<'_'<<m_id<<' '<<size;
    //os<<m_id<<' '<<size;

    for (int i=0 ; getline(iss, token, comma); i++) {
	std::istringstream iss2(token);
	iss2>>x>>y;
	if (i==0) {
	    low[0] = high[1] = x;
	    low[1] = high[1] = y;
	} else {
	    if (low[0]>x) low[0] = x;
	    if (high[0]<x) high[0] = x;
	    if (low[1]>y) low[1] = y;
	    if (high[1]<y) high[1] = y;
	}

	// the polygon is stored using delta x and delta y in the rtree leaf node.
	if (COMPRESS) 
	    if (i==0) os<<' '<<x<<' '<<y;
	    else os<<' '<<x-lastx<<' '<<y-lasty;
	else 
	    os<<' '<<x<<' '<<y;
	lastx=x;
	lasty=y;
    }
    //parsePolygonTimer.end(timerCount);

    Region r(low, high, 2);
    string leafdata = os.str();

    return new RTree::Data(leafdata.size()+1, reinterpret_cast<const byte*>(leafdata.c_str()), r, m_id++);
    // Associate a bogus data array with every entry for testing purposes.
    // Once the data array is given to RTRee:Data a local copy will be created.
    // Hence, the input data array can be deleted after this operation if not
    // needed anymore.
}


class MyDataStream : public IDataStream
{
    public:
	MyDataStream(vector<string> & invec, int tag ) : m_pNext(0), index(0), len(0)
    {


	if ( invec.empty())
	    throw Tools::IllegalArgumentException("Input size is ZERO.");
	len = invec.size();
	vec = invec;
	readNextEntry();
	tagg= tag;
	m_id =0 ;
    }

	virtual ~MyDataStream()
	{
	    if (m_pNext != 0) delete m_pNext;
	}

	virtual IData* getNext()
	{
	    if (m_pNext == 0) return 0;

	    RTree::Data* ret = m_pNext;
	    m_pNext = 0;
	    readNextEntry();
	    return ret;
	}

	virtual bool hasNext()
	{
	    return (m_pNext != 0);
	}

	virtual uint32_t size()
	{
	    return vec.size();
	    //throw Tools::NotSupportedException("Operation not supported.");
	}

	virtual void rewind()
	{
	    if (m_pNext != 0)
	    {
		delete m_pNext;
		m_pNext = 0;
	    }

	    index =0 ;
	    readNextEntry();
	}

	void readNextEntry()
	{
	    if (index < len)
	    {
		m_pNext = parseInputLine(vec[index], m_id, tagg);
		index++;
	    }
	}

	RTree::Data* m_pNext;
	vector<string> vec ; 
	int len;
	int index ;
	id_type m_id;
	int tagg ;
};


