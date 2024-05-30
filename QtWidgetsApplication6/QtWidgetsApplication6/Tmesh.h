#ifndef TMESH_H
#define TMESH_H

#include<vector>
#include<list>
#include <cassert>
#include<iostream>
#include <algorithm>
//#include "AFX.h"
using namespace std;

#define _AFXDLL

#define SAFEDELETE(p)    if(p) { delete p;    p=NULL;}
#define SAFEDELETES(p)   if(p) { delete []p;  p=NULL;}//释放连续的多个空间
#define TOLERANCE 1.0e-13

enum DrawType {points, lines, faces};

class CHalfEdge;

class CVertex{                     
public:    //attributes
	//命名结构：成员变量（member variable）_数据类型+意义
	int        	m_nVerid;            //m_integer vertex identifier

    int        	m_nvLevel;
	double     	m_dVerx, m_dVery;    //vx,vy
    CHalfEdge*  pHalfEdge;          //hep
	bool       	m_bDealWith;			//是否被处理/初始化
	char       	m_cVerType;
	vector<int> m_nRelatedBasis;       
	
	
	CVertex()//没有任何参数
	{
		m_nVerid = -1;
		m_nvLevel = 0;
		pHalfEdge = NULL;
		m_bDealWith = false;//没有初始化
		m_cVerType = 'C';
	}
	CVertex(int id, double vx, double vy, CHalfEdge *hep)
	{
		m_nVerid = id;
		m_dVerx = vx;
		m_dVery = vy;
		pHalfEdge = hep;
		m_bDealWith = false;
	
	}
	CVertex(const CVertex &v)
	{
		if(this != &v) //地址
		{
			m_nVerid = v.m_nVerid;
			m_dVerx = v.m_dVerx;
			m_dVery = v.m_dVery;
			pHalfEdge = v.pHalfEdge;
			m_bDealWith = v.m_bDealWith;
			m_cVerType = v.m_cVerType;
			m_nvLevel = v.m_nvLevel;
			m_nRelatedBasis = v.m_nRelatedBasis;
		}

	}
	CVertex & operator=(const CVertex &v)//不检查是否为自己，直接赋值
	{
		m_nVerid = v.m_nVerid;
		m_dVerx = v.m_dVerx;
		m_dVery = v.m_dVery;
		pHalfEdge = v.pHalfEdge;
		m_bDealWith = v.m_bDealWith;
		m_cVerType = v.m_cVerType;
		m_nvLevel = v.m_nvLevel;
		m_nRelatedBasis = v.m_nRelatedBasis;
        return *this;
     } 
	
	~CVertex();
	

public:   //operation
     bool IsBoundaryVertex(double x1,double x2,double y1,double y2); //lie on or not the boundary of rectangle of x1,x2,y1,y2
     void VertexInHalfEdge(vector<CHalfEdge*> &lst_hes);	//??????????????????
	 char VertexType();

	 int LRHalfEdge(vector<CHalfEdge*> lst_hes,char type);//由点得边
	 int UDHalfEdge(vector<CHalfEdge*> lst_hes,char type);
	 int LRUDHalfEdge(vector<CHalfEdge*> lst_hes,char type);
	 
};

struct BsCoord 
{
	int   blevel;
	int   bid;
	int   bsup;
};

class CFace{
public:
	int             m_nFaceid;
	int             m_nFaceLevel;
	int             m_nFourIndex[4];
	double     		m_dErr;
	bool          	m_bMark;             //sign of refined or not
	bool          	IsSubdivid;//是否进行了细分
    
    bool            IsSub_H;//水平
    bool            IsSub_V;//垂直
    int             flag_H;
    int             flag_V;
    
	CHalfEdge  		*pHalfedge;
	
	vector<BsCoord> m_CoeffOnFacet;

	int m_nSiblings[4]; //xhw,  0-- ld, 1-- lu, 2-- rd, 3-- ru;兄弟
	
	/*int        m_nParent;
	int        m_nChild[8];*/

public:
	bool 			m_bAniso; 
	bool 			m_bAnisoU; 
	bool 			m_bAnisoV; 

public:
	CFace()
	{
	    m_nFaceid = -1;
	    for(int i=0;i<4;i++)
		   m_nFourIndex[i] = 0;
	    m_dErr = 0;
		m_bMark = false;
		pHalfedge = NULL;

		m_nFaceLevel = 0;
		IsSubdivid = 0;
        IsSub_H = 0;
        IsSub_V = 0;
        flag_H = 0;
        flag_V = 0;

        for(int i=0; i<4; i++)//xhw
        	m_nSiblings[i] = -1;
		/*m_nParent = -1;
		for(int i=0;i < 8;++i)
		   m_nChild[i] = -1;*/
		

	}

	CFace & operator=(const CFace &cell1)
	{
		m_nFaceid = cell1.m_nFaceid;
		for(int i=0;i<4;i++)
			m_nFourIndex[i] = cell1.m_nFourIndex[i];
		m_dErr    = cell1.m_dErr;
		m_bMark   = cell1.m_bMark;
		pHalfedge = cell1.pHalfedge;

		m_nFaceLevel = cell1.m_nFaceLevel;
		/*m_nParent = cell1.m_nParent;
		for(int i=0;i < 8;++i)
			m_nChild[i] = cell1.m_nChild[i];*/
		for (int i=0;i < (int)cell1.m_CoeffOnFacet.size();++i)
			m_CoeffOnFacet.push_back(cell1.m_CoeffOnFacet[i]);
		IsSubdivid = cell1.IsSubdivid;


		for(int i=0; i<4; i++)//xhw
        	m_nSiblings[i] = cell1.m_nSiblings[i];
      
		 return *this;
     }
	~CFace();
	
public://operation
	void face_size(double &x0, double &x1, double &y0, double &y1); //鈥溌柯ヅ掆垶鈥斺�溌垙藛cell碌茠鈥ε撆撀棅脹鈥濃�溍�茠鈭徦喡碘�炩�欌�溾墺藛驴楼拢篓鈮ぢ⑩垜鈮埪幻庘棅炉卤脥
	bool IsBoundaryCell();  //鈮堚�撯垈艙聽芦鈭懨捙掆劉卤铿偽┟乧ell
	void DrawFace(void);
	void DrawFace(int ch);
	int  BoundaryCellType();
	char IsBoundaryCell_LRUD();
};

class CEdge{
public:
	int          m_nTwoVer[2];//两个点
	double       m_dLength;
    CHalfEdge    *pLeftHalfEdge;//左
	CHalfEdge    *pRightHalfEdge;//右
public:     
	 CEdge()
	{
       m_nTwoVer[0] = -1;
	   m_nTwoVer[1] = -1;
	   m_dLength = 0;
	   pLeftHalfEdge = NULL;
	   pRightHalfEdge = NULL;

	}
    
	CEdge & operator=(const CEdge &e1)
	{
		m_nTwoVer[0] = e1.m_nTwoVer[0];
		m_nTwoVer[1] = e1.m_nTwoVer[1];
		m_dLength    = e1.m_dLength;
		pLeftHalfEdge  = e1.pLeftHalfEdge;
		pRightHalfEdge = e1.pRightHalfEdge;
		return *this;
     }
	
	~CEdge();
	
public:  //operation
	char edge_type();  //鈮堚�撯垈艙鈥溌兡甭憋瑐聽芦聽藱梅卤卤铿偮�聽芦脌脝鈭單┞憋瑐
	bool IsBoundaryEdge(double x1,double x2,double y1,double y2);
	char EdgeTypeLRUD(CFace lyonface);  //鈮堚�撯垈艙鈥溌兡甭憋瑐鈥樷亜脌藰鈭徛灯掆垰脢鈥ε撀灯掆棅脹鈥濃�溾�ε撆撀憋瑐碌每驴鈥♀�撁?
};

class CHalfEdge{
public:
	CHalfEdge   *pPrevHalfedge;//前
	CHalfEdge   *pNextHalfedge;//后
	CEdge       *pEdge;
	CFace       *pFace;
	CVertex     *pVertex;
public:
	CHalfEdge()
	{
		pPrevHalfedge = NULL;
		pNextHalfedge = NULL;
		pEdge = NULL;
		pFace = NULL;
		pVertex = NULL;

	}
	~CHalfEdge()
	{
		/*SAFEDELETE(pPrevHalfedge);
		SAFEDELETE(pNextHalfedge);
		SAFEDELETE(pEdge);
		SAFEDELETE(pFace);
		SAFEDELETE(pVertex);*/
	}

public:   //operation
	inline CVertex *source() const{	return this->pPrevHalfedge->pVertex;}//返回当前半边的前一条半边的点指针
	inline CVertex *target() const{	return this->pVertex;}
	CHalfEdge *dual() const//求对偶（duality)
	{
			CEdge *pe=this->pEdge;
			assert(pe!=NULL);//断言函数
			CHalfEdge *ph;
			if(pe->pLeftHalfEdge != this)
		    	ph = pe->pLeftHalfEdge;
			else
				ph = pe->pRightHalfEdge;
			return ph;
	}
   CHalfEdge& operator=(const CHalfEdge &hedge)
	{
	   //所有成员变量
		pPrevHalfedge=hedge.pPrevHalfedge;
		pNextHalfedge=hedge.pNextHalfedge;
		pEdge=hedge.pEdge;
		pFace=hedge.pFace;
		pVertex=hedge.pVertex;
      return *this;
	}

};

class Gvertex{
public:
	int    m_nGvid;  //id number
	double m_dGVx,  m_dGVy; //x, y position
public:
	Gvertex() 
	{
		m_nGvid = -1;
	}
	Gvertex(int id, double x, double y)
	{
		m_nGvid = id;
		m_dGVx = x;
		m_dGVy =y;
	}
	Gvertex(const Gvertex &gv)
	{
		m_nGvid = gv.m_nGvid;
		m_dGVx = gv.m_dGVx;
		m_dGVy = gv.m_dGVy;
	}
	Gvertex& operator=(const Gvertex &gv)
	{
		m_nGvid = gv.m_nGvid;
		m_dGVx = gv.m_dGVx;
		m_dGVy = gv.m_dGVy;
		return *this;
	}
	~Gvertex() {};

};

class Gface{
public:
	int m_nGfid;//Geometry Face ID
	int m_nGver[4];   //可能是Gvertex的id？
public:
	Gface()
	{
		m_nGfid = -1;
		for ( int i=0;i<4;i++)
			m_nGver[i] = -1;
	}
	Gface(int fid, int ver0, int ver1,int ver2,int ver3)
	{
		m_nGfid = fid;
		m_nGver[0] = ver0;
		m_nGver[1] = ver1;
		m_nGver[2] = ver2;
		m_nGver[3] = ver3;

	}
	Gface(int fid, int ver[4])
	{
		m_nGfid = fid;
		for ( int i=0;i<4;i++)
			m_nGver[i] = ver[i];

	}
	Gface(const Gface &gf)
	{
		m_nGfid = gf.m_nGfid;
		for ( int i=0;i<4;i++)
			m_nGver[i] = gf.m_nGver[i];
	}
	Gface& operator=(const Gface &gf)
	{
		m_nGfid = gf.m_nGfid;
		for ( int i=0;i<4;i++)
			m_nGver[i] = gf.m_nGver[i];
		return *this;
	}
	~Gface(){};
};

class CMesh{
public:
	int              m_nMeshLevel;//level
	int              m_nBoundVer, m_nBoundFace;//边界顶点数、面数
	vector<CVertex>  m_Vertices;//包含的点
	vector<CFace>    m_Faces;//包含的面
	vector<int>      nVerLevelNum, nFaceLevelNum;//每个层级的顶点数、面数
	double           m_dX0, m_dX1, m_dY0, m_dY1;//网格空间范围
	double           interval;//网格间隔

public:  // operation
	CMesh(){}
	~CMesh(){};
    
	void Vertex_anticlocksort(vector<CVertex> &splitvertex, char type);//对顶点进行逆时针排序
	void split_edge_connect(CHalfEdge *split_he, vector<CVertex> splitvertex);//分割边
	void GeneratTMesh(vector<Gvertex> &gvertices, vector<Gface> &gfaces);//生成TMesh
	
	void DrawMesh(DrawType type);//绘制网格
    void WriteTmsfile(string str);//写入文件
	//void ExpandMesh();
	void UniformExpandMesh(vector<Gvertex> &gvertices, vector<Gface> &gfaces);//均匀扩展
	int  FindInWhichCell(double x, double y);//找定点坐标在哪个网格单元
};


//鈥欌�溌滴┾垙炉鈭偮甌鈭傗�⒙碘�灻�藰鈥樷亜碌茠卤铿偮得库垶脦卤铿偽┞废�蟺
CHalfEdge *find_TEdgeFace_of_Tvertex(CVertex v);

CFace *insert_edge_Lface_con(int index,CFace insert_face,CVertex *v[2],CHalfEdge *phd,CHalfEdge *phu,int newnumd,int newnumu);
CFace *insert_edge_Rface_con(int index,CFace insert_face,CVertex *v[2],CHalfEdge *phd,CHalfEdge *phu,int newnumd,int newnumu);
CFace *insert_edge_Dface_con(int index,CFace insert_face,CVertex *v[2],CHalfEdge *phl,CHalfEdge *phr,int newnuml,int newnumr);
CFace *insert_edge_Uface_con(int index,CFace insert_face,CVertex *v[2],CHalfEdge *phl,CHalfEdge *phr,int newnuml,int newnumr);
CFace *split_edge_of_insert_vertex(CFace *face,CVertex *v,char type);


void Insert_Mesh_H(CMesh &M,CFace insert_face,CVertex *v[2]);
void Insert_Mesh_V(CMesh &M,CFace insert_face,CVertex *v[2]);
void Mesh_InsertHalfHV(CMesh &M,CFace insert_face,char insert_type);
void Mesh_Insert_Edge(CMesh &M,CFace insert_face,char insert_type,double val);
void CrossInsertMesh(CFace m_Insertface,vector<Gvertex> &gves,vector<Gface> &gfaces,CMesh &M);
void InsertMeshHA(CFace m_Insertface,vector<Gvertex> &gves,vector<Gface> &gfaces,CMesh &M);
void InsertMeshVA(CFace m_Insertface,vector<Gvertex> &gves,vector<Gface> &gfaces,CMesh &M);
char Mesh_InsertTPoint(CMesh &M, CFace insert_face, CVertex T_vertex);


int    Tver_FaceExtension(CVertex findv, CHalfEdge *directionhe);
void   find_vertices_in_subcell(CMesh &M,CHalfEdge *ph,vector<int> &vid);
void   Find_Adjacent_Cell(CFace f,vector<int> &fid);


void   SubdividCell(CMesh &M,int fid);
void   CrossRefineHierarMesh(CMesh &M, int fid);

//my new function
void myRefine_QQ(CMesh &M, vector<int> &vid);// input vector of cells, determine which one should be subdivide
void myRefine_QQ_Cross(CMesh &M, vector<int> &vid);// input vector of cells, determine which one should be subdivide
char Is_Neighbor(CFace &f1, CFace &f2);//判断两个面是否邻接？
void Unidirectional_Refine(CMesh &M, int fid, char insert_type );
void Unidirectional_Refine_new(CMesh &M, int fid, char insert_type );
void myMark_QQ(CMesh &M, vector<int> &vid, vector<int> &cvid, vector<int> &hvid, vector<int> &vvid);
void myRefine_QQ(CMesh &M, vector<int> &vid, vector<int> &exo_vid );
char Is_Neighbor_new(CMesh &M, CFace &f1, CFace &f2);
void adjustHV(CMesh &M,  vector<int> &vid);
void addbasisHV(CMesh &M,  vector<int> &vid);
void myRefine_QQ_1(CMesh &M, vector<int> &vid);
void classifycells(CMesh &M, vector<int> &vid, vector<vector<int>> &temp2);
void myRefine_QQ_local(CMesh &M, vector<int> &cid);

//初始化mesh，水平点个数，竖直点个数，坐标值
void InitialMesh(CMesh& cmesh, int un, int vn, double x0, double x1, double y0, double y1);
//判断now和remain里有哪些连通，连通分量保存在connected，已划分的从remain删除
void Connected(CMesh& M, vector<int>& remain, vector<vector<int>>& connected, vector<int>& nextStack);

#endif