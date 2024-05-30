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
#define SAFEDELETES(p)   if(p) { delete []p;  p=NULL;}//�ͷ������Ķ���ռ�
#define TOLERANCE 1.0e-13

enum DrawType {points, lines, faces};

class CHalfEdge;

class CVertex{                     
public:    //attributes
	//�����ṹ����Ա������member variable��_��������+����
	int        	m_nVerid;            //m_integer vertex identifier

    int        	m_nvLevel;
	double     	m_dVerx, m_dVery;    //vx,vy
    CHalfEdge*  pHalfEdge;          //hep
	bool       	m_bDealWith;			//�Ƿ񱻴���/��ʼ��
	char       	m_cVerType;
	vector<int> m_nRelatedBasis;       
	
	
	CVertex()//û���κβ���
	{
		m_nVerid = -1;
		m_nvLevel = 0;
		pHalfEdge = NULL;
		m_bDealWith = false;//û�г�ʼ��
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
		if(this != &v) //��ַ
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
	CVertex & operator=(const CVertex &v)//������Ƿ�Ϊ�Լ���ֱ�Ӹ�ֵ
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

	 int LRHalfEdge(vector<CHalfEdge*> lst_hes,char type);//�ɵ�ñ�
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
	bool          	IsSubdivid;//�Ƿ������ϸ��
    
    bool            IsSub_H;//ˮƽ
    bool            IsSub_V;//��ֱ
    int             flag_H;
    int             flag_V;
    
	CHalfEdge  		*pHalfedge;
	
	vector<BsCoord> m_CoeffOnFacet;

	int m_nSiblings[4]; //xhw,  0-- ld, 1-- lu, 2-- rd, 3-- ru;�ֵ�
	
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
	void face_size(double &x0, double &x1, double &y0, double &y1); //“¿¥Œ∞—“ª∏ˆcellµƒ…œœ¬◊Û”“Àƒ∏ˆµ„’“≥ˆ¿¥£¨≤¢∑≈»Î◊¯±Í
	bool IsBoundaryCell();  //≈–∂œ «∑ÒŒ™±ﬂΩÁcell
	void DrawFace(void);
	void DrawFace(int ch);
	int  BoundaryCellType();
	char IsBoundaryCell_LRUD();
};

class CEdge{
public:
	int          m_nTwoVer[2];//������
	double       m_dLength;
    CHalfEdge    *pLeftHalfEdge;//��
	CHalfEdge    *pRightHalfEdge;//��
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
	char edge_type();  //≈–∂œ“ªÃı±ﬂ « ˙÷±±ﬂªπ «ÀÆ∆Ω±ﬂ
	bool IsBoundaryEdge(double x1,double x2,double y1,double y2);
	char EdgeTypeLRUD(CFace lyonface);  //≈–∂œ“ªÃı±ﬂ‘⁄À˘∏¯µƒ√Ê…œµƒ◊Û”“…œœ¬±ﬂµÿ¿‡–�?
};

class CHalfEdge{
public:
	CHalfEdge   *pPrevHalfedge;//ǰ
	CHalfEdge   *pNextHalfedge;//��
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
	inline CVertex *source() const{	return this->pPrevHalfedge->pVertex;}//���ص�ǰ��ߵ�ǰһ����ߵĵ�ָ��
	inline CVertex *target() const{	return this->pVertex;}
	CHalfEdge *dual() const//���ż��duality)
	{
			CEdge *pe=this->pEdge;
			assert(pe!=NULL);//���Ժ���
			CHalfEdge *ph;
			if(pe->pLeftHalfEdge != this)
		    	ph = pe->pLeftHalfEdge;
			else
				ph = pe->pRightHalfEdge;
			return ph;
	}
   CHalfEdge& operator=(const CHalfEdge &hedge)
	{
	   //���г�Ա����
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
	int m_nGver[4];   //������Gvertex��id��
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
	int              m_nBoundVer, m_nBoundFace;//�߽綥����������
	vector<CVertex>  m_Vertices;//�����ĵ�
	vector<CFace>    m_Faces;//��������
	vector<int>      nVerLevelNum, nFaceLevelNum;//ÿ���㼶�Ķ�����������
	double           m_dX0, m_dX1, m_dY0, m_dY1;//����ռ䷶Χ
	double           interval;//������

public:  // operation
	CMesh(){}
	~CMesh(){};
    
	void Vertex_anticlocksort(vector<CVertex> &splitvertex, char type);//�Զ��������ʱ������
	void split_edge_connect(CHalfEdge *split_he, vector<CVertex> splitvertex);//�ָ��
	void GeneratTMesh(vector<Gvertex> &gvertices, vector<Gface> &gfaces);//����TMesh
	
	void DrawMesh(DrawType type);//��������
    void WriteTmsfile(string str);//д���ļ�
	//void ExpandMesh();
	void UniformExpandMesh(vector<Gvertex> &gvertices, vector<Gface> &gfaces);//������չ
	int  FindInWhichCell(double x, double y);//�Ҷ����������ĸ�����Ԫ
};


//’“µΩ∏¯∂®T∂•µ„À˘‘⁄µƒ±ﬂµÿ∞Î±ﬂΩ·ππ
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
char Is_Neighbor(CFace &f1, CFace &f2);//�ж��������Ƿ��ڽӣ�
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

//��ʼ��mesh��ˮƽ���������ֱ�����������ֵ
void InitialMesh(CMesh& cmesh, int un, int vn, double x0, double x1, double y0, double y1);
//�ж�now��remain������Щ��ͨ����ͨ����������connected���ѻ��ֵĴ�remainɾ��
void Connected(CMesh& M, vector<int>& remain, vector<vector<int>>& connected, vector<int>& nextStack);

#endif