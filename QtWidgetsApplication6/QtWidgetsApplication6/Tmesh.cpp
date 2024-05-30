//#include"stdafx.h"
#include"Tmesh.h"
#include"math.h"
#include<fstream>
//#include "AFX.h"
#include <GL/glut.h>
//#include <GLUT/GLUT.h>

CVertex::~CVertex()
{
	/*SAFEDELETE(pHalfEdge);*/
}

bool CVertex::IsBoundaryVertex(double x1, double x2, double y1, double y2)//�����ֵ�Ǳ߽�
{
	if(    fabs( m_dVerx - x1) < TOLERANCE || fabs( m_dVerx - x2) < TOLERANCE  //����������ֵ
		|| fabs( m_dVery - y1) < TOLERANCE || fabs( m_dVery - y2) < TOLERANCE )//�û��ж��Ƿ��ھ��α߽�
		return true;
	else
		return false;

}

void CVertex::VertexInHalfEdge( vector<CHalfEdge*> &lst_hes )//������ָ��õ�İ��
{
	CHalfEdge *phead = pHalfEdge;//��Ա����ָ��İ��

	lst_hes.push_back(phead);
	CHalfEdge *ph=phead->dual();//�ڴ���һ��ָ�룬���ż���

	if(ph!=NULL)//��ż����
	{
		ph=ph->pPrevHalfedge; int k=0;//ָ���ż���ǰһ��
		while(ph!=phead )//&&k<5)
		{
			lst_hes.push_back(ph);
			ph=ph->dual();
			if(ph!=NULL)
				ph=ph->pPrevHalfedge;
			else
				break;
			k++;
		}
	}
	//��ż�����ڣ���һ����ߵĶ�ż���ڡ����������ֻ��һ��ָ��õ�İ�ߣ�ʱ���ڽ��ϣ�
	else if ( phead->pNextHalfedge->dual() != NULL) 
	{
		lst_hes.push_back(phead->pNextHalfedge->dual());
	}
}

int CVertex::LRHalfEdge( vector<CHalfEdge*> lst_hes, char type)//����lst_hes������
{
	int n = lst_hes.size();//�м���ָ��õ�İ�ߣ�����һ��
	int i=0;
	if ( 'L' == type)//�˵���ߵĵ�/���
	{
		while (i < n)
		{
			//source()���ص�ǰ��ߵ�ǰһ����ߵĵ�ָ��
			if (lst_hes[i]->source()->m_dVerx < m_dVerx) //left or right
				return i;
			++i;
		}
	}
	else//�˵��ұߵĵ�
	{
		while (i < n)
		{
			if (lst_hes[i]->source()->m_dVerx > m_dVerx) //left or right
				return i;
			++i;
		}
	}
	
	return -1;//û�ҵ�


}

int CVertex::UDHalfEdge( vector<CHalfEdge*> lst_hes, char type )//����
{
	int n = lst_hes.size();
	int i=0;

	if ( 'U' == type)
	{
		while (i < n)
		{
			
			if (lst_hes[i]->source()->m_dVery > m_dVery) //left or right
				return i;
			++i;
		}
	}
	else
	{
		while (i < n)
		{
			if (lst_hes[i]->source()->m_dVery < m_dVery) //left or right
				return i;
			++i;
		}
	}
	return -1;

}


int CVertex::LRUDHalfEdge(vector<CHalfEdge*> lst_hes, char type)
{
	int  n=0;
	
	if('L' == type)
		n = 1;
	else if('R' == type)
		n = 3;
	else if('U' == type)
		n = 0;
	else if('D' == type)
		n = 2;

	for (int i=0;i < (int)lst_hes.size();++i)
	{
		if (lst_hes[i]->pVertex->m_nVerid == lst_hes[i]->pFace->m_nFourIndex[n])
		   return i;
	}
    return  -1;

}


char CVertex::VertexType()
{
      char type = 'B';

      vector<CHalfEdge*> lst_hes;
	  VertexInHalfEdge(lst_hes);   
	  
	  if(lst_hes.size()==3)
		  type='T';
	  else if(lst_hes.size()==4)
		  type='C';
	  else
		  type='B';

	  return type;
 
}


//////////////////////////////////////////////////////////////////////////
//Edge operation
//////////////////////////////////////////////////////////////////////////
CEdge::~CEdge()
{
	/*SAFEDELETE(pLeftHalfEdge);
	SAFEDELETE(pRightHalfEdge);*/
}

char CEdge::edge_type()
{
	char type;
	CVertex *v1 = NULL;
	CVertex *v2 = NULL;
   
	if (  pLeftHalfEdge )
	{
		v1 = pLeftHalfEdge->source();
		v2 = pLeftHalfEdge->target();
	}
	else if (pRightHalfEdge)
	{
		v1 = pRightHalfEdge->source();
		v2 = pRightHalfEdge->target(); 
	}
	
	if( fabs( v1->m_dVerx - v2->m_dVerx ) < TOLERANCE )
		type='V';
	else if( fabs( v1->m_dVery - v2->m_dVery ) < TOLERANCE)
		type='H';
	else
	{  
		type='N';cout<<"It is not a edge!!";
	}

	return type;

}

bool CEdge::IsBoundaryEdge(double x1,double x2,double y1,double y2) 
{
	  char type = edge_type();

	  assert(pLeftHalfEdge);
	  CVertex *v1 = pLeftHalfEdge->pVertex;

	  if( type == 'H')
	 {
	   if(fabs(v1->m_dVery - y1) < TOLERANCE || fabs( v1->m_dVery - y2) < TOLERANCE)
		    return true;
	   else 
		   return false;
	 }
	 else
	 {
        if(fabs(v1->m_dVerx - x1) < TOLERANCE || fabs(v1->m_dVerx - x2) < TOLERANCE)
			return true;
		else
			return false;
	 }
    
}

char CEdge::EdgeTypeLRUD(CFace lyonface)
{
	char type;
	int ver1 = m_nTwoVer[0];
	int ver2 = m_nTwoVer[1];

	if( ( ver1 == lyonface.m_nFourIndex[0] && ver2 == lyonface.m_nFourIndex[3]) ||
		( ver2 == lyonface.m_nFourIndex[0] && ver1 == lyonface.m_nFourIndex[3]) )
		type='L';

	else if( ( ver1 == lyonface.m_nFourIndex[2] && ver2 == lyonface.m_nFourIndex[1]) ||
		     ( ver2 == lyonface.m_nFourIndex[2] && ver1 == lyonface.m_nFourIndex[1]) )
		type='R';

	else if( (ver1 == lyonface.m_nFourIndex[2] && ver2 == lyonface.m_nFourIndex[3]) ||
		     (ver2 == lyonface.m_nFourIndex[2] && ver1 == lyonface.m_nFourIndex[3]))
		type='U';
	else
		type='D';

	return type;

}


//////////////////////////////////////////////////////////////////////////
//Face
//////////////////////////////////////////////////////////////////////////
CFace::~CFace()
{
	/*SAFEDELETE(pHalfedge);*/
}


void CFace::face_size(double &x0, double &x1, double &y0, double &y1)
{
	CHalfEdge *phead = pHalfedge;
	CHalfEdge *ph = phead;
	
	do{//����һ���ҵ�0��2�Ĵ���Ȼ���ʼ������x0��y0��x1��y1
		if(ph->pVertex->m_nVerid == m_nFourIndex[0])
		{
			x0=ph->pVertex->m_dVerx;
			y0=ph->pVertex->m_dVery; 
		}
		if( ph->pVertex->m_nVerid == m_nFourIndex[2])
		{
			x1=ph->pVertex->m_dVerx;
			y1=ph->pVertex->m_dVery; 
		}
		ph=ph->pNextHalfedge;

	}while(ph!=phead);	

}

//�Ƿ��Ǳ߽�face��ֻҪ��halfedgeû��dual��Ϊ�߽�face
bool CFace::IsBoundaryCell() //“ª∏ˆcell£¨“¿¥Œ≈–±∆‰À˘”–halfedge «∑ÒŒﬁdual
{

    CHalfEdge  *ph,*phead;

	phead = pHalfedge;
	ph = phead;
	do 
	{
		if(ph->dual() == NULL)
			return true;
		ph = ph->pNextHalfedge;
	} while (ph != phead);

	return false;
}

//qq///new
int CFace::BoundaryCellType()// LR(0) or UD(1) 
{
	CHalfEdge  *ph,*phead;

	phead = pHalfedge;
	ph = phead;
	do 
	{
		if(ph->dual() == NULL)
		{
			if(abs(ph->pVertex->m_dVerx - ph->pPrevHalfedge->pVertex->m_dVerx)<=10e-16)
			return 0;
			else if(abs(ph->pVertex->m_dVery - ph->pPrevHalfedge->pVertex->m_dVery)<=10e-16)
			return 1;
		}	
		ph = ph->pNextHalfedge;
	} while (ph != phead);

	return -1;
}

void CFace::DrawFace(void)
{
	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	//otherwise, black color
	
	

	CHalfEdge *phead,*ph;
	

	glColor3f(0.2f,0.4f,0.5f);  
	glBegin(GL_POLYGON);
	phead = pHalfedge;
	ph = phead;
	do{
		glVertex3d(ph->pVertex->m_dVerx,ph->pVertex->m_dVery,0.0);
		ph=ph->pNextHalfedge;
	}while(ph!=phead);
	glEnd();

	
	//glLineWidth(1.0);
	glEnable(GL_LIGHTING);
}



void CFace::DrawFace(int colori)
{
	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	//otherwise, black color
	
	

	CHalfEdge *phead,*ph;

	// switch(colori)
	// {
	// 	for(int i=0; i<15; i++)
	// 	{
	// 		case i: glColor3f((0.1+i*0.05)f,(0.9-i*0.05)f,(i%2)f);  break;
	// 	}
	// }
	float tr, tg, tb;
	tr = float(colori%20)*0.05; 
	tg = 1.0 - float(colori%20)*0.1;
	tb = float(colori%20)*0.05;

	glColor3f(tr, tg, tb);
	
	
	
	glBegin(GL_POLYGON);
	phead = pHalfedge;
	ph = phead;
	do{
		glVertex3d(ph->pVertex->m_dVerx,ph->pVertex->m_dVery,0.0);
		ph=ph->pNextHalfedge;
	}while(ph!=phead);
	glEnd();

	
	//glLineWidth(1.0);
	glEnable(GL_LIGHTING);
}
//////////////////////////////////////////////////////////////////////////
//other relative operation
//////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------*/
//Ω´∏¯∂®µƒ±ﬂ…œµƒ“ª–©∂•µ„∞¥’’¥Œ√Êƒ⁄µƒƒÊ ±’Î∑ΩœÚ≈≈¡–£�?
//∑Ω±„”¶”√split_Edge_severel’‚∏ˆ∫Ø ˝£¨’‚∂º «≥ı ºππΩ®Õ¯∏Ò–Ë“™µƒ∫Ø �?
/*--------------------------------------------------------------*/
void CMesh::WriteTmsfile(string str)
{
	ofstream rm;	

	rm.open(str);
	rm<<m_Vertices.size()<<" "<<m_Faces.size()<<endl;

	for(int i=0;i < (int)m_Vertices.size();++i)
		rm<<m_Vertices[i].m_dVerx<<" "<<m_Vertices[i].m_dVery<<endl;

	for (int i=0;i < (int)m_Faces.size();++i)
	{
		rm<<m_Faces[i].m_nFourIndex[0]<<" "<<m_Faces[i].m_nFourIndex[1]<<" "
		  <<m_Faces[i].m_nFourIndex[2]<<" "<<m_Faces[i].m_nFourIndex[3]<<endl;
	}
	rm.close();
}


void CMesh::Vertex_anticlocksort(vector<CVertex> &splitvertex, char type)//������ʱ������
{
	int      j=0;
	int      n=splitvertex.size();
	CVertex  tempver;

	if(type=='V')
	{
		if(splitvertex[0].m_dVery>splitvertex[1].m_dVery)//left edge
		{
			for(int i=1;i<n-1;i++)
			{
				j=i;
				for (int k=i+1;k<n-1;k++)
				{
					if(splitvertex[k].m_dVery>splitvertex[j].m_dVery)
						j = k;
				}
				if (i != j)
				{
					tempver=splitvertex[j];
					splitvertex[j]=splitvertex[i];
					splitvertex[i]=tempver;
				}
			}

		}
		else
		{
			for(int i=1;i<n-1;i++)
			{
				j=i;
				for (int k=i+1;k<n-1;k++)
				{
					if(splitvertex[k].m_dVery<splitvertex[j].m_dVery)
						j = k;
				}
				if (i != j)
				{
					tempver=splitvertex[j];
					splitvertex[j]=splitvertex[i];
					splitvertex[i]=tempver;
				}
			}

		}
	}
	else
	{
		if(splitvertex[0].m_dVerx>splitvertex[1].m_dVerx)     //up edge
		{
			for(int i=1;i<n-1;i++)
			{
				j=i;
				for (int k=i+1;k<n-1;k++)
				{
					if(splitvertex[k].m_dVerx>splitvertex[j].m_dVerx)
						j = k;
				}
				if (i != j)
				{
					tempver=splitvertex[j];
					splitvertex[j]=splitvertex[i];
					splitvertex[i]=tempver;
				}
			}

		}
		else
		{
			for(int i=1;i<n-1;i++)
			{
				j=i;
				for (int k=i+1;k<n-1;k++)
				{
					if(splitvertex[k].m_dVerx<splitvertex[j].m_dVerx)
						j = k;
				}
				if (i != j)
				{
					tempver=splitvertex[j];
					splitvertex[j]=splitvertex[i];
					splitvertex[i]=tempver;
				}
			}

		}
	}

}

void CMesh::split_edge_connect(CHalfEdge *split_he, vector<CVertex> splitvertex)
{
	vector<CHalfEdge*> 	he;
	vector<CVertex*> 	lst_ver;
	
	int          m = (int)splitvertex.size();
	int          ver_num;
	CVertex*     v = NULL;
	CHalfEdge    *phead = NULL; 
	CHalfEdge    *ph = NULL;
    


	phead = split_he->pFace->pHalfedge; 
	ph = phead;                         //’‚—˘◊ˆµƒƒøµƒ «Œ™¡ÀÀ≥¥Œ∞—∂•µ„∑≈»�?
	 do{
			v = ph->pVertex;
			if( v->m_nVerid == splitvertex[0].m_nVerid )//“ÚŒ™æ˘ «ƒÊ ±’ÎÀ˘“‘“ªµ©’“µΩsplitvertex÷–µƒ◊Ó∫Û“ª∏ˆæÕø…“‘ø™ ºsplitµƒ∏≥÷µ
			{
				  lst_ver.push_back(v);
				  
				  for(int i=1;i<m-1;i++)
				  {
					 CVertex* sv = new CVertex;
					 *sv = splitvertex[i];
					 lst_ver.push_back(sv);
				  }
			 }
			 else
				 lst_ver.push_back(ph->pVertex);    
			 
			 ph=ph->pNextHalfedge;
		 }while(ph!=phead);

	  ver_num=(int)lst_ver.size();
/*---------------------------------------------------------------------------------------------------------------------*/
        CFace *face=new CFace;

		face->m_nFaceid = split_he->pFace->m_nFaceid;
		for(int i=0;i<4;i++)
			face->m_nFourIndex[i]=split_he->pFace->m_nFourIndex[i];

		 he.resize(ver_num);
		 for(int i=0;i<ver_num;i++)
		 {
			 he[i] = new CHalfEdge;
			 he[i]->pVertex = lst_ver[i];
			 he[i]->pFace = face;
			 if(lst_ver[i]->pHalfEdge == split_he)
				 lst_ver[i]->pHalfEdge=he[i];
			/* cout<<lst_ver[i]->m_nVerid<<" ";*/
		 }

		 for(int i=0;i<ver_num;i++)
		 {
			 he[i]->pNextHalfedge=he[(i+1)%ver_num];
			 he[i]->pPrevHalfedge=he[(i+ver_num-1)%ver_num];

		 }
			 
		 for(int i=0;i<ver_num;i++)
		 {
			 if(he[i]->pVertex->m_nVerid == face->m_nFourIndex[0])
			 {
				 face->pHalfedge=he[i];
				 break;
			 }
		 }

		m_Faces[split_he->pFace->m_nFaceid].pHalfedge = face->pHalfedge;
			
}

//void CMesh::ExpandMesh()//�԰�߽��г�ʼ��
//{
//
//	int         k = (int)m_Vertices.size();//�������
//	double      dknot;
//	CHalfEdge   *phead,*ph;
//	CVertex     v;
//	phead = m_Faces[0].pHalfedge;
//	ph = phead;
//	do 
//	{
//		if(ph->pVertex->m_nVerid == m_Faces[0].m_nFourIndex[0])
//			break;
//       ph = ph->pNextHalfedge;
//	} while (ph != phead);
//
//	phead = ph;
//	do 
//	{
//		ph = phead->pNextHalfedge;
//        v.m_nVerid = k++;
//		v.m_dVerx = ph->pVertex->m_dVerx;
//        v.m_dVery = ph->pVertex->m_dVery - dknot;
//
//		phead = phead->pNextHalfedge->dual();
//	
//	} while (phead != NULL);
//}

void CMesh::UniformExpandMesh(vector<Gvertex> &gvertices,vector<Gface> &gfaces)
{
	int     N;
	int     nV,nF;
    double  interval;
	Gvertex  gv;
	Gface    gf;

	m_dX0 = gvertices[0].m_dGVx;
	m_dY0 = gvertices[0].m_dGVy;
	m_dX1 = gvertices[1].m_dGVx;
	m_dY1 = gvertices[1].m_dGVy;
	for(int i=0;i<(int)gvertices.size();i++)
	{
		if (gvertices[i].m_dGVx > m_dX1)
			m_dX1 = gvertices[i].m_dGVx;
		if (gvertices[i].m_dGVy > m_dY1)
			m_dY1 = gvertices[i].m_dGVy;

	}
	interval = gvertices[gfaces[0].m_nGver[1]].m_dGVx - gvertices[gfaces[0].m_nGver[0]].m_dGVx;
    N = (int)((m_dX1 - m_dX0)/interval)+1;

	nV = (int)gvertices.size();
	nF = (int)gfaces.size();

	gv.m_nGvid = nV;
	gv.m_dGVx = m_dX0 - interval;
	gv.m_dGVy = m_dY0 - interval;
	gvertices.push_back(gv);
	for (int i=0;i < N;++i)
	{
		gv.m_nGvid = nV+1+i;
		gv.m_dGVx = m_dX0 + i*interval ;
		gv.m_dGVy = m_dY0 - interval;
		gvertices.push_back(gv);
	}
	gv.m_nGvid = nV+N+1;
	gv.m_dGVx = m_dX1 + interval;
	gv.m_dGVy = m_dY0 - interval;
	gvertices.push_back(gv);

	for (int i=0;i < N;++i)
	{
		gv.m_nGvid = nV+N+2+2*i;
		gv.m_dGVx = m_dX0 - interval ;
		gv.m_dGVy = m_dY0 + i*interval;
		gvertices.push_back(gv);

		gv.m_nGvid = nV+N+3+2*i;
		gv.m_dGVx = m_dX1 + interval ;
		gv.m_dGVy = m_dY0 + i*interval;
		gvertices.push_back(gv);
	}

	gv.m_nGvid = (int)gvertices.size();
	gv.m_dGVx = m_dX0 - interval;
	gv.m_dGVy = m_dY1 + interval;
	gvertices.push_back(gv);
	for (int i=0;i < N;++i)
	{
		gv.m_nGvid = nV+3*N+3+i;
		gv.m_dGVx = m_dX0 + i*interval ;
		gv.m_dGVy = m_dY1 + interval;
		gvertices.push_back(gv);
	}
	gv.m_nGvid = (int)gvertices.size();
	gv.m_dGVx = m_dX1 + interval;
	gv.m_dGVy = m_dY1 + interval;
	gvertices.push_back(gv);          //total 4N+4
	
	gf.m_nGfid = nF;
	gf.m_nGver[0] = nV; gf.m_nGver[1] = nV+1; gf.m_nGver[2] = 0; gf.m_nGver[3] = nV+N+2;
	gfaces.push_back(gf);

	for (int i=0;i < N-1;++i)
	{
         gf.m_nGfid = nF+1+i;
		 gf.m_nGver[0] = nV+1+i;
		 gf.m_nGver[1] = nV+2+i;
		 gf.m_nGver[2] = i+1;
		 gf.m_nGver[3] = i;
		 gfaces.push_back(gf);
	}
	gf.m_nGfid = nF+N;
	gf.m_nGver[0] = nV+N; gf.m_nGver[1] = nV+N+1; gf.m_nGver[2] = nV+N+3; gf.m_nGver[3] = N-1;
	gfaces.push_back(gf);

	for (int i=0;i < N-1;++i)
	{
		gf.m_nGfid = nF+N+1+2*i;
		gf.m_nGver[0] = nV+N+(i+1)*2;
		gf.m_nGver[1] = i*N;
		gf.m_nGver[2] = (i+1)*N;
		gf.m_nGver[3] = nV+N+(i+2)*2;
		gfaces.push_back(gf);

		gf.m_nGfid = nF+N+2*i+2;
		gf.m_nGver[0] = (i+1)*N-1;
		gf.m_nGver[1] = nV+N+(i+1)*2+1;
		gf.m_nGver[2] = nV+N+(i+2)*2+1;
		gf.m_nGver[3] = (i+2)*N-1;
		gfaces.push_back(gf);
	}

	nF = (int)gfaces.size();
	gf.m_nGfid = nF;
	gf.m_nGver[0] = nV+3*N; gf.m_nGver[1] = (N-1)*N; gf.m_nGver[2] = nV+3*(N+1); gf.m_nGver[3] = nV+3*N+2;
	gfaces.push_back(gf);

	for (int i=0;i < N-1;++i)
	{
		gf.m_nGfid = nF+1+i;
		gf.m_nGver[0] = (N-1)*N+i;
		gf.m_nGver[1] = (N-1)*N+i+1;
		gf.m_nGver[2] = nV+3*N+4+i;
		gf.m_nGver[3] = nV+3*N+3+i;
		gfaces.push_back(gf);
	}
	gf.m_nGfid = nF+N;
	gf.m_nGver[0] = N*N-1; gf.m_nGver[1] = nV+3*N+1; gf.m_nGver[2] = nV+4*N+3; gf.m_nGver[3] = nV+4*N+2;
	gfaces.push_back(gf);
 

}

void CMesh::GeneratTMesh(vector<Gvertex> &gvertices,vector<Gface> &gfaces)//��������
{

		 int     id=0;
	     double  lengthedge;
		 CVertex temp_ver;
	     vector<vector<CHalfEdge*>>  point_to_ver;
		
		// UniformExpandMesh(gvertices,gfaces);
		 m_Faces.clear();
		 m_Vertices.clear();

		 m_dX0 = gvertices[0].m_dGVx;
		 m_dY0 = gvertices[0].m_dGVy;
		 m_dX1 = gvertices[1].m_dGVx;
		 m_dY1 = gvertices[1].m_dGVy;

		 for(int i=0;i<(int)gvertices.size();i++)//��ʼ��cmesh�ĵ�
		 {
			 temp_ver.m_nVerid =  gvertices[i].m_nGvid;//��id
			 temp_ver.m_dVerx  =  gvertices[i].m_dGVx;//��x
			 temp_ver.m_dVery  =  gvertices[i].m_dGVy;//��y
			 temp_ver.pHalfEdge = NULL;//����
	
              m_Vertices.push_back(temp_ver);//�������cmesh
			 if (gvertices[i].m_dGVx > m_dX1)
			     m_dX1 = gvertices[i].m_dGVx;//��չ
			 if (gvertices[i].m_dGVy > m_dY1)
			    m_dY1 = gvertices[i].m_dGVy;//��չ
			 
		 }

		point_to_ver.resize(m_Vertices.size()); //��Ĵ�С

		 for(int faceid=0;faceid<(int)gfaces.size();faceid++)//��ʼ��cmesh����
		{
				CFace *face=new CFace;
				face->m_nFaceid = faceid;
				face->m_dErr = 0;
				face->m_bMark = 0;
				for(int i=0;i<4;i++)//����һ����
					face->m_nFourIndex[i] = gfaces[faceid].m_nGver[i];
				
				CHalfEdge  *he[4];
				CVertex    *v[4];
				for(int i=0;i<4;i++)
				{
					  v[i]=new CVertex;
                     he[i]=new CHalfEdge;
				}
				for(int i=0;i<4;i++)
				{
					id = gfaces[faceid].m_nGver[i];
					v[i]->m_dVerx = gvertices[id].m_dGVx;
					v[i]->m_dVery=gvertices[id].m_dGVy;
					v[i]->m_nVerid=gvertices[id].m_nGvid;
					v[i]->pHalfEdge=he[i];
					 he[i]->pVertex=v[i];
					 he[i]->pFace=face;
				 }
			         
                  for(int i=0;i<4;i++)
				 {
					 he[i]->pNextHalfedge=he[(i+1)%4];
					 he[i]->pPrevHalfedge=he[(i+3)%4];
				 }
				 for(int i=0;i<4;i++)
				  {
					  id = gfaces[faceid].m_nGver[i];
					  point_to_ver[id].push_back(he[i]);
				  }
				
				 face->pHalfedge = he[0];
				
				 //Attach half-edge to edge,
				 bool flag=false;
				 for(int i=0;i<4;i++)
				 {
					 int id_1=he[i]->pVertex->m_nVerid;
					 int id_2=he[i]->pPrevHalfedge->pVertex->m_nVerid;
					 int j=0;
					 id=gfaces[faceid].m_nGver[i]; /* cout<<id_1<<" "<<id_2<<";";*/
                     flag=false;

					 for(j=0;j < (int)point_to_ver[id].size();j++)
					  {
						  if(point_to_ver[id][j]->pNextHalfedge->source()->m_nVerid==id_1 && 
							  point_to_ver[id][j]->pNextHalfedge->pVertex->m_nVerid==id_2 )
						  {
							  flag=true; break;
						  }
						  if(point_to_ver[id][j]->pNextHalfedge->source()->m_nVerid==id_2 && 
							  point_to_ver[id][j]->pNextHalfedge->pVertex->m_nVerid==id_1 )
						  {
							  flag=true; break;
						  }
					  }
				 	  if(flag == true)
					  { 
						 
						  CEdge *pe=new CEdge;
						  lengthedge=sqrt(pow(he[i]->source()->m_dVerx - he[i]->target()->m_dVerx,2)+
										  pow(he[i]->source()->m_dVery - he[i]->target()->m_dVery,2));
						  pe->m_dLength = lengthedge;
						  pe->m_nTwoVer[0]=id_1; 
						  pe->m_nTwoVer[1]=id_2;
						  pe->pLeftHalfEdge=point_to_ver[id][j]->pNextHalfedge;
						  pe->pRightHalfEdge=he[i];
						  point_to_ver[id][j]->pNextHalfedge->pEdge=pe;
						  he[i]->pEdge=pe;
						   
					  }
					  else
					  {
                         CEdge *pe=new CEdge;
						 lengthedge=sqrt(pow(he[i]->source()->m_dVerx - he[i]->target()->m_dVerx,2)+
										 pow(he[i]->source()->m_dVery - he[i]->target()->m_dVery,2));
						 pe->m_dLength=lengthedge;
						 pe->m_nTwoVer[0]=id_1; 
						 pe->m_nTwoVer[1]=id_2;
						 pe->pLeftHalfEdge=he[i];
						 pe->pRightHalfEdge=NULL;
						 he[i]->pEdge=pe;  
						
					  }

				   }
				 for(int ii=0; ii<4; ii++) //xhw
					face->m_nSiblings[ii] = -1; 

				 m_Faces.push_back(*face);

		}
	   
		for(int i=0;i< (int)m_Vertices.size();++i)
	    {
		   if(point_to_ver[i].size()!=0)
			   m_Vertices[i].pHalfEdge = point_to_ver[i][0];
	    }

		//////////////////////////////////////////////////////////////////////////
		//split edge
		//////////////////////////////////////////////////////////////////////////  
       int       vn;
       double    tempxy;
	   char      type,typej;
	   double    xi1,xi2,yi1,yi2;
	   double    xj1,xj2,yj1,yj2;
	   CVertex   adjv;
	   CVertex   veri1,veri2,verj1,verj2;
	   CHalfEdge *phead = NULL;
	   CHalfEdge *ph = NULL;
       vector<CHalfEdge*> right_null_edges;    //righthaldedge is null
       vector<CVertex> adjver;
	  
	  
	   //cout<<"Mesh size:"<<m_dX0<<" "<<m_dX1<<" "<< m_dY0<<" "<<m_dY1<<endl;
		for(int k=0; k < (int)m_Faces.size();++k)
		{
			phead = m_Faces[k].pHalfedge; 
			ph=phead;   
			do{
				if(ph->pEdge->pRightHalfEdge == NULL)
				{ 
					if(ph->pEdge->IsBoundaryEdge(m_dX0, m_dX1, m_dY0, m_dY1) == false)
					   right_null_edges.push_back(ph);
				}
				ph=ph->pNextHalfedge;
			}while(ph!=phead);
			
		}
	
	    for(int i=0; i < (int)right_null_edges.size();i++)
	   {
		        adjver.clear();
              
				veri1 = *(right_null_edges[i]->source());
		        veri2 = *(right_null_edges[i]->pVertex);   
				xi1=veri1.m_dVerx;yi1=veri1.m_dVery;
				xi2=veri2.m_dVerx;yi2=veri2.m_dVery; 
                if(yi1>yi2) 
				{  
					tempxy=yi1;yi1=yi2;yi2=tempxy;  
				}
				if(xi1>xi2) 
				{  
					tempxy=xi1;xi1=xi2;xi2=tempxy; 
				}
				type=right_null_edges[i]->pEdge->edge_type(); //cout<<i<<":"<<right_null_edges[i]->pEdge->m_nTwoVer[0]<<" "<<right_null_edges[i]->pEdge->m_nTwoVer[1]<<endl;
				adjver.push_back(veri1);  

		        for(int j=0; j<(int)right_null_edges.size();j++)
		       {   
				   if(j!=i)
				   {
		             typej= right_null_edges[j]->pEdge->edge_type();
				   if( typej==type )
				   {
					  
					   if(type=='V')
					   {
						   verj1=*(right_null_edges[j]->source());
						   verj2=*(right_null_edges[j]->pVertex);
						   xj1=verj1.m_dVerx;  
						   yj1=verj1.m_dVery;
						   xj2=verj2.m_dVerx;  
						   yj2=verj2.m_dVery; 
						   if(fabs(xi1-xj1)<10e-17)    //on a line
						   {
								if(yj1>yi1 &&yj1<yi2)
								{   
									 int j=0;
								    for(;j < (int)adjver.size();j++)
								   {
									   if(adjver[j].m_nVerid==verj1.m_nVerid)
										  break;
								   }
								   if(j==adjver.size())        //Àµ√˜¿Ô√Êªπ√ª”�?
									   adjver.push_back(verj1);
								   
							     }
								   if(yj2>yi1 &&yj2<yi2)
								   {
									   int j=0;
									   for(;j < (int)adjver.size();j++)
									   {
										   if(adjver[j].m_nVerid==verj2.m_nVerid)
											  break;
									   }
									   if(j==adjver.size())//Àµ√˜¿Ô√Êªπ√ª”�?
											adjver.push_back(verj2);
								    }
								  
					     }
				      }
				   else 
				   {
					       verj1=*(right_null_edges[j]->source());
						   verj2=*(right_null_edges[j]->pVertex);
						   xj1=verj1.m_dVerx;   yj1=verj1.m_dVery; xj2=verj2.m_dVerx;   yj2=verj2.m_dVery; 
						   if(fabs(yi1-yj1)<10e-17)    //on a line
						   {
							   if(xj1>xi1 &&xj1<xi2)
							   {
									 int j=0;
								   for(;j < (int)adjver.size();j++)
								  {
									  if(adjver[j].m_nVerid == verj1.m_nVerid)
										  break;
								   }
								   if(j==adjver.size())//Àµ√˜¿Ô√Êªπ√ª”�?
									  adjver.push_back(verj1);
							   }

								 if(xj2>xi1 &&xj2<xi2)
								 {
									 int j=0;
									   for(;j < (int)adjver.size();j++)
									  {
										  if(adjver[j].m_nVerid==verj2.m_nVerid)
											  break;
									   }
									   if(j==adjver.size())
									  adjver.push_back(verj2);
									   
								 }
						   }
					 }//type v,H
			   }//typej==type
				   }//j!=i
			}//for j=0
            
		     adjver.push_back(veri2);
			 if(adjver.size()>2)
			{	
				Vertex_anticlocksort(adjver,type);
				/*cout<<type<<" "<<"start:";
				for(int k=0;k<adjver.size();k++)
					cout<<adjver[k].m_nVerid<<" ";
				cout<<endl;*/
			   split_edge_connect(right_null_edges[i],adjver);
			}
           
		}//for i=0
		
		////////////////////////////////////////////////////////////////////////////
		////split edge end
		////////////////////////////////////////////////////////////////////////////

		point_to_ver.clear();
		point_to_ver.resize(m_Vertices.size());
		
		for(int k=0; k < (int)m_Faces.size();++k)
		{
		    CFace *face=new CFace;
			face->m_nFaceid = k;
			for(int i=0;i<4;i++)
				face->m_nFourIndex[i] = m_Faces[k].m_nFourIndex[i];
		
			
			vector<CVertex*>  v;
		    phead = m_Faces[k].pHalfedge; 
			ph=phead;
			do{
				  v.push_back(ph->pVertex);
			      ph=ph->pNextHalfedge;
		     }while(ph!=phead);
			
			vn = (int)v.size();
			vector<CHalfEdge*> he(vn);
			for(int i=0;i<vn;i++)
			{
				 he[i]=new CHalfEdge;
				 he[i]->pVertex=v[i];
				 he[i]->pFace=face;
				 v[i]->pHalfEdge=he[i];
			 }
			         
             for(int i=0;i<vn;i++)
			 {
				 he[i]->pNextHalfedge=he[(i+1)%vn];
				 he[i]->pPrevHalfedge=he[(i+vn-1)%vn];
			 }
			  for(int i=0;i<vn;i++)
			  {
				  id=v[i]->m_nVerid;
				  point_to_ver[id].push_back(he[i]);
			  }
			  for(int i=0;i<vn;i++)
			  {
				  if(he[i]->pVertex->m_nVerid == face->m_nFourIndex[0])
				  {
					  face->pHalfedge=he[0];break;
				  }
			  }
				
			 //Attach half-edge to edge,
			 bool flag=false;
			 for(int i=0;i<vn;i++)
			 {
				 int id_1=he[i]->pVertex->m_nVerid;
				 int id_2=he[i]->pPrevHalfedge->pVertex->m_nVerid;
				 int j=0;
				 id=v[i]->m_nVerid; /* cout<<id_1<<" "<<id_2<<";";*/
                 flag=false;

				 for(j=0;j<(int)point_to_ver[id].size();j++)
				  {
					  if(point_to_ver[id][j]->pNextHalfedge->source()->m_nVerid==id_1 && 
						  point_to_ver[id][j]->pNextHalfedge->pVertex->m_nVerid==id_2 )
					  {
						  flag=true; break;
					  }
					  if(point_to_ver[id][j]->pNextHalfedge->source()->m_nVerid==id_2 && 
						  point_to_ver[id][j]->pNextHalfedge->pVertex->m_nVerid==id_1 )
					  {
						  flag=true; break;
					  }
				  }
			 	  if(flag==true)
				  { 
					 
					  CEdge *pe=new CEdge;
					  lengthedge=sqrt(pow(he[i]->source()->m_dVerx - he[i]->target()->m_dVerx,2)+
									  pow(he[i]->source()->m_dVery - he[i]->target()->m_dVery,2));
					  pe->m_dLength = lengthedge;
					  pe->m_nTwoVer[0] = id_2; pe->m_nTwoVer[1]=id_1;
					  pe->pLeftHalfEdge=point_to_ver[id][j]->pNextHalfedge;
					  pe->pRightHalfEdge=he[i];
					  point_to_ver[id][j]->pNextHalfedge->pEdge=pe;
					  he[i]->pEdge=pe;
					   
				  }
				  else
				  {
                     CEdge *pe=new CEdge;
					 lengthedge=sqrt(pow(he[i]->source()->m_dVerx - he[i]->target()->m_dVerx,2)+
									 pow(he[i]->source()->m_dVery - he[i]->target()->m_dVery,2));
					 pe->m_dLength = lengthedge;
					 pe->m_nTwoVer[0]=id_2; pe->m_nTwoVer[1]=id_1;
					 pe->pLeftHalfEdge=he[i];
					 pe->pRightHalfEdge=NULL;
					 he[i]->pEdge=pe;  
					 
				  }
				  m_Faces[k].pHalfedge=face->pHalfedge;
			
	          }

		}

		for(int i=0;i < (int)m_Vertices.size();++i)
	    {
		   if(point_to_ver[i].size()!=0)
			   m_Vertices[i].pHalfEdge=point_to_ver[i][0];
		   /*if(itrv->pHalfEdge->dual()!=NULL)
		   cout<<itrv->m_nVerid<<" "<<itrv->pHalfEdge->pEdge->m_nTwoVer[0]<<" "<<itrv->pHalfEdge->pEdge->m_nTwoVer[1]<<endl;*/
	    }

/*--------------------------------------------------------------------------------------------*/	
	
       /* for(int i=0;i<gvertices.size();i++)
        {
			 cout<<i<<":"<<point_to_ver[i].size()<<endl;
           for(int k=0;k<point_to_ver[i].size();k++)
			   cout<<point_to_ver[i][k]->pEdge->m_nTwoVer[0]<<" ";
	       if(point_to_ver[i].size()!=0)
               cout<<endl;
         }
        cout<<endl;*/
		
  //        HalfEdge *phead,*ph;
  //       list<Face>::iterator itr=m_Faces.begin();
	 //for(;itr!=m_Faces.end();itr++)
	 //{
		//phead=itr->pHalfedge;
		//ph=phead;
		//do{
		//	//if(ph->dual()!=NULL)
		//	//{
		//	//	//cout<<ph->pEdge->m_nTwoVer[0]<<" "<<ph->pEdge->m_nTwoVer[1]<<";";
		//	//	cout<<ph->dual()->pFace->m_nFaceid<<" ";
		//	//}
		////	cout<<ph->pVertex->pHalfEdge->pEdge->m_nTwoVer[0]<<" "<<ph->pVertex->pHalfEdge->pEdge->m_nTwoVer[1]<<",";
		//	cout<<ph->pVertex->m_nVerid<<" ";
		//	ph=ph->pNextHalfedge;
		//   }while(ph!=phead);

		//cout<<endl;
		//
	 //    }
		
		
		/*itrv=m_Vertices.begin();
		for(;itrv!=m_Vertices.end();itrv++)
		{	
			if (itrv->m_nVerid == 35)
		  { 
			  cout<<itrv->m_nVerid<<":";
			  vector<HalfEdge*> lst_hes;
			  itrv->VertexInHalfEdge(lst_hes);
			  for(int i=0;i<lst_hes.size();i++)
				  cout<<lst_hes[i]->pEdge->m_nTwoVer[0]<<" "<<lst_hes[i]->pEdge->m_nTwoVer[1]<<";";
			  cout<<endl;
		   }
			
		}*/
	
}


void CMesh::DrawMesh(DrawType type)
{

	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
	glColor3f(0.0f, 0.0f, 1.0f);
	glLineWidth(0.5);

	CHalfEdge *phead,*ph;
	for(int k=0;k < (int)m_Faces.size();++k)
	{   
	/*	if(m_Faces[k].IsBoundaryCell())
			continue;*/

		switch(type){

			case points: glBegin(GL_POINTS); break;
			case lines:  glBegin(GL_LINE_LOOP); break;
			case faces:  glBegin(GL_POLYGON); break;
		}
		phead = m_Faces[k].pHalfedge;
		ph=phead;
		do{
			//glColor3f(0.0,0.0,1.0);
			glVertex3d(ph->pVertex->m_dVerx,ph->pVertex->m_dVery,0.0);
			ph=ph->pNextHalfedge;
		}while(ph!=phead);
		glEnd();

	}
	glLineWidth(0.5);
	glEnable(GL_LIGHTING);

}

int    CMesh::FindInWhichCell(double x,double y)
{

	double   s,t;
	for (int  fi=0;fi < (int)m_Faces.size();++fi)
	{
		s = (x - m_Vertices[m_Faces[fi].m_nFourIndex[0]].m_dVerx )/( m_Vertices[m_Faces[fi].m_nFourIndex[2]].m_dVerx - m_Vertices[m_Faces[fi].m_nFourIndex[0]].m_dVerx);
		t = (y - m_Vertices[m_Faces[fi].m_nFourIndex[0]].m_dVery)/( m_Vertices[m_Faces[fi].m_nFourIndex[2]].m_dVery- m_Vertices[m_Faces[fi].m_nFourIndex[0]].m_dVery);
		if ( s >= 0 && s <= 1 && t >= 0 && t<= 1 )
		      return  fi;
		//if( fabs(x- 0.1)<TOLERANCE && fabs(y- 0.6)<TOLERANCE)
	
	}
	return  -1;
}
//////////////////////////////////////////////////////////////////////////
//other operations
//////////////////////////////////////////////////////////////////////////
CHalfEdge *find_TEdgeFace_of_Tvertex(CVertex v)
{

        int i;
		char edge_type[3];
		double x1,y1;
        vector<CHalfEdge*> lst_hes;

 		v.VertexInHalfEdge(lst_hes);
		for(i=0;i<3;i++)
		{
			x1=lst_hes[i]->source()->m_dVerx;
			y1=lst_hes[i]->source()->m_dVery;
			if(fabs(x1-v.m_dVerx)>0)
			   edge_type[i]='H';
		    if(fabs(y1-v.m_dVery)>0)
			   edge_type[i]='V';
		}
		for( i=0;i<3;i++)
		{
			if(edge_type[(i+1)%3]==edge_type[(i+2)%3])
				break;
	    } 
		return lst_hes[i];	
			
}


void find_T_vertex_faceextension(CVertex findv,CHalfEdge *directionhe,vector<CVertex> THver,double &knot1,double &knot2)
{
                                                            
	   int n=0;
	   CHalfEdge *phead,*ph;
	   double knot[2];
	   bool flag = false;

	   if(THver[findv.m_nVerid].m_cVerType == 'U')
		   n=2; 
	   else if(THver[findv.m_nVerid].m_cVerType == 'D')
			n=0; 
	   else if(THver[findv.m_nVerid].m_cVerType == 'L')
			 n=3; 
		else
		   n=1;  

	   if(n==3 || n==1)
	   {
		   int k = 0;
		   phead = directionhe->pNextHalfedge->dual();
		   do 
		   {
			   ph=phead;
			   do{
				   if( ph->pVertex->m_nVerid == phead->pFace->m_nFourIndex[n] )
					   break;
				   ph = ph->pNextHalfedge;

			   }while(ph!=phead); 

			   if(THver[ph->pVertex->m_nVerid].m_bDealWith == false)
                  knot[k++] = ph->pVertex->m_dVerx;

			   ph = ph->pNextHalfedge;
			   if ( 1 == n)
			   {
				   do{
					   if( ph->pVertex->m_dVery > findv.m_dVery ) //”ˆµΩµ⁄“ª∏ˆ¥Û”⁄’‚∏ˆT-µ„µƒ£¨ƒ«√¥’‚∏ˆ√Ê≤≈ «œ¬“ª∏ˆ≤È’“µƒ√Ê
					   {
						   if( ph->dual() != NULL)   
							   phead = ph->dual();
						   else                              //»Áπ˚”ˆµΩµƒ «±ﬂΩÁ±�?≤ª–Ë“™ºÃ–¯’“œ¬“ª∏ˆface¡À
							   flag = true;
						   break;
					   }
					   
					   ph=ph->pNextHalfedge;

				   }while( ph->pVertex->m_nVerid != ph->pFace->m_nFourIndex[n+2] );
			   }

			   if( 3 == n )
			   {
				   do{
					   if( ph->pVertex->m_dVery < findv.m_dVery ) //”ˆµΩµ⁄“ª∏ˆxiao”⁄’‚∏ˆT-µ„µƒ£¨ƒ«√¥’‚∏ˆ√Ê≤≈ «œ¬“ª∏ˆ≤È’“µƒ√Ê
					   {
						   if( ph->dual() != NULL)   
							   phead = ph->dual();
						   else                              //»Áπ˚”ˆµΩµƒ «±ﬂΩÁ±�?≤ª–Ë“™ºÃ–¯’“œ¬“ª∏ˆface¡À
							   flag = true;
						   break;
					   }
					   
					   ph=ph->pNextHalfedge;

				    }while( ph->pVertex->m_nVerid != ph->pFace->m_nFourIndex[(n+2)%4] );

			   }

		   } while ( k < 2 && !flag );
   
	   }
	   else
	   {
		   int k = 0;
		   phead = directionhe->pNextHalfedge->dual();
		   do 
		   {
			   ph=phead;
			   do{
				   if( ph->pVertex->m_nVerid == phead->pFace->m_nFourIndex[n] )
					   break;
				   ph = ph->pNextHalfedge;

			   }while(ph!=phead);   

			   if(THver[findv.m_nVerid].m_bDealWith == false)
                    knot[k++] = ph->pVertex->m_dVery;

			   ph = ph->pNextHalfedge;
			   if ( 2 == n)
			   {
				   do{
					   if( ph->pVertex->m_dVerx < findv.m_dVerx ) 
					   {
						   if( ph->dual() != NULL)   
							   phead = ph->dual();
						   else                              //»Áπ˚”ˆµΩµƒ «±ﬂΩÁ±�?≤ª–Ë“™ºÃ–¯’“œ¬“ª∏ˆface¡À
							   flag = true;
						   break;
					   }

					   ph=ph->pNextHalfedge;

				   }while( ph->pVertex->m_nVerid != ph->pFace->m_nFourIndex[0] );
			   }

			   if( 0 == n )
			   {
				   do{
					    if( ph->pVertex->m_dVerx > findv.m_dVerx ) //”ˆµΩµ⁄“ª∏ˆxiao”⁄’‚∏ˆT-µ„µƒ£¨ƒ«√¥’‚∏ˆ√Ê≤≈ «œ¬“ª∏ˆ≤È’“µƒ√Ê
					    {
						   if( ph->dual() != NULL)   
							   phead = ph->dual();
						   else                              //»Áπ˚”ˆµΩµƒ «±ﬂΩÁ±�?≤ª–Ë“™ºÃ–¯’“œ¬“ª∏ˆface¡À
							   flag = true;
						   break;
					    }

					   ph=ph->pNextHalfedge;

				    }while( ph->pVertex->m_nVerid != ph->pFace->m_nFourIndex[(n+2)%4] );

			   }

		   } while ( k < 2 &&!flag );

           
	   }


}
void Insert_Mesh_H(CMesh &M,CFace insert_face,CVertex *v[2])
{
	       int newnuml,newnumr;
           CHalfEdge *ph=NULL;
		   CHalfEdge *phead=NULL;
		   CHalfEdge *phl=NULL;
		   CHalfEdge *phr=NULL;
/*--------------------------------find phd,phu,then find vertex-----------------------------------*/			   
		    phead=insert_face.pHalfedge; 
			ph=phead;   
			do               //’“µΩƒ«–©±ª≤Â»Îµƒ ˙÷±±ﬂ≈¸≥…¡Ω∞Îµƒ∞Î±ﬂΩ·ππphl,phr;
			{
				if(phr==NULL && ph->pVertex->m_dVery >= v[1]->m_dVery )
				{
					if(ph->pVertex->m_dVery > v[1]->m_dVery)
						newnumr=2;            //—œ∏Ò¥Û”⁄ ±¥À ±–Ëππ‘�?Ãı–¬±ﬂ
					else 
						newnumr=0;
					phr=ph;
				}
				if( phl==NULL && ph->source()->m_dVery >= v[0]->m_dVery 
					&& ph->pVertex->m_dVery < v[0]->m_dVery ) 
				{
					if(ph->source()->m_dVery > v[0]->m_dVery)
						newnuml=2;
					else
						newnuml=0;
					phl=ph;
				}
				ph=ph->pNextHalfedge; 

			}while(ph!=phead);
/*-------------------------------modify v[0],v[1] id------------------------------------*/	          
			if(newnuml==0)
			{
				v[1]->m_nVerid=v[0]->m_nVerid;
				v[0]=phl->source();
			}
			if(newnumr==0)
			{
				v[1]=phr->pVertex;
			}

/*----------------------------------down up face construct--------------------------------------------------------------------*/
			int face_index_d=insert_face.m_nFaceid;
			int face_index_u=M.m_Faces.size();
			CFace *fu=NULL; 
			CFace *fd=NULL;

			
			
			fd=insert_edge_Dface_con(face_index_d,M.m_Faces[face_index_d],v,phl,phr,newnuml, newnumr);//insert_face,v,phl,phr,newnuml, newnumr);
			
			/*fu=insert_edge_Uface_con(face_index_u,insert_face,v,phl,phr,newnuml, newnumr);*/
			fu=insert_edge_Uface_con(face_index_u,M.m_Faces[face_index_d],v,phl,phr,newnuml, newnumr);
             
			CHalfEdge *ph_face_u=NULL;
			CHalfEdge *ph_face_d=NULL;

			phead=fd->pHalfedge;
			ph_face_d=phead;

			do{
				if(ph_face_d->pVertex->m_nVerid==v[0]->m_nVerid)
					break;
				ph_face_d = ph_face_d->pNextHalfedge;
			}while(ph_face_d!=phead);

            phead=fu->pHalfedge;
			ph_face_u=phead;

			do{
				if(ph_face_u->pVertex->m_nVerid==v[1]->m_nVerid)
					break;
				ph_face_u=ph_face_u->pNextHalfedge;
			}while(ph_face_u!=phead);

			ph_face_d->pEdge->pRightHalfEdge=ph_face_u;

		     CEdge *v_edge=new CEdge;
			 v_edge->m_dLength=fabs(v[1]->m_dVery -v[0]->m_dVery);
			 v_edge->m_nTwoVer[0]=v[0]->m_nVerid;     
			 v_edge->m_nTwoVer[1]=v[1]->m_nVerid;
			 v_edge->pLeftHalfEdge=ph_face_d;
			 v_edge->pRightHalfEdge=ph_face_u;
			 ph_face_u->pEdge = v_edge;

/*----------------------------------related face is modified--------------------------------------------------------*/	
			                             
			 if(phl->dual()!=NULL )                 
			{
				if( newnuml==2)
				{
					 CFace *modify_face_left=NULL;
				
					 modify_face_left=split_edge_of_insert_vertex(&M.m_Faces[phl->dual()->pFace->m_nFaceid],v[0],'R');//phl->dual()->pFace,v[0],'R');	

					// modify_face_left->m_bMark = M.m_Faces[phl->dual()->pFace].m_bMark;
					
					 phead=modify_face_left->pHalfedge; ph=phead; 
					 do{
						 if(ph->pVertex->m_nVerid==v[0]->m_nVerid)
						 {
							 ph->pEdge->pRightHalfEdge=ph->pEdge->pLeftHalfEdge;
							 ph->pEdge->pLeftHalfEdge=ph_face_d->pNextHalfedge;
							 ph_face_d->pNextHalfedge->pEdge->pRightHalfEdge=ph;
						 }
						 if(ph->source()->m_nVerid==v[0]->m_nVerid)
						 {
							 ph->pEdge->pRightHalfEdge=ph->pEdge->pLeftHalfEdge;
							 ph->pEdge->pLeftHalfEdge=ph_face_u->pPrevHalfedge;
							 ph_face_u->pPrevHalfedge->pEdge->pRightHalfEdge=ph;
						 }
						 ph=ph->pNextHalfedge;
					 }while(ph!=phead);

					 M.m_Faces[phl->dual()->pFace->m_nFaceid] = *modify_face_left;
					/* list<Face>::iterator itr=M.m_Faces.begin();
					 for(;itr!=M.m_Faces.end();itr++)
					 {
						 if(itr->m_nFaceid==phl->dual()->pFace->m_nFaceid)
						 { *itr=*modify_face_left; break; }
					 }*/

				}
				else                 //À‰Àµ¥À ±√ª”–‘ˆº”–¬µƒ±ﬂ£¨µ´ «“ÚŒ™√ÊµƒΩ·ππ±‰¡À£¨ªπ «“™∏ƒ“ªœ�?
				{
					 phead=phl->dual()->pFace->pHalfedge; ph=phead;
					 do{
						 if(ph->pVertex->m_nVerid == v[0]->m_nVerid)
						 {
							 if(ph->pEdge->pLeftHalfEdge==phl)
							 {
								 ph->pEdge->pLeftHalfEdge=ph_face_d->pNextHalfedge;
								 break;
							 }
							 if(ph->pEdge->pRightHalfEdge==phl)
							 {
								 ph->pEdge->pRightHalfEdge=ph_face_d->pNextHalfedge;
								 break;
							 }
							
						 }
						 ph=ph->pNextHalfedge;
					 }while(ph!=phead);

					 phead=phl->pPrevHalfedge->dual()->pFace->pHalfedge; ph=phead;
					 do{
						 if(ph->source()->m_nVerid==v[0]->m_nVerid)
						 {
							 if(ph->pEdge->pLeftHalfEdge==phl->pPrevHalfedge)
							 {
								 ph->pEdge->pLeftHalfEdge=ph_face_u->pPrevHalfedge;
								 break;
							 }
							 if(ph->pEdge->pRightHalfEdge==phl->pPrevHalfedge)
							 {
								 ph->pEdge->pRightHalfEdge=ph_face_u->pPrevHalfedge;
								 break;
							 }
							
						 }
						 ph=ph->pNextHalfedge;
					 }while(ph!=phead);
					 
				}
				
			}
			
            if(phr->dual()!=NULL )
			{
				if(newnumr==2)
				{
						 CFace *modify_face_right=NULL;

						 modify_face_right=split_edge_of_insert_vertex(&M.m_Faces[phr->dual()->pFace->m_nFaceid],v[1],'L');//phr->dual()->pFace,v[1],'L');	
						
						 phead=modify_face_right->pHalfedge; ph=phead;
						 do{
							 if(ph->pVertex->m_nVerid==v[1]->m_nVerid)
							 {
								 ph->pEdge->pRightHalfEdge=ph->pEdge->pLeftHalfEdge;
								 ph->pEdge->pLeftHalfEdge=ph_face_u->pNextHalfedge;
								 ph_face_u->pNextHalfedge->pEdge->pRightHalfEdge=ph;
							 }
							 if(ph->source()->m_nVerid==v[1]->m_nVerid)
							 {
								 ph->pEdge->pRightHalfEdge=ph->pEdge->pLeftHalfEdge;
								 ph->pEdge->pLeftHalfEdge=ph_face_d->pPrevHalfedge;
								 ph_face_d->pPrevHalfedge->pEdge->pRightHalfEdge=ph;
							 }
							 ph=ph->pNextHalfedge;
						 }while(ph!=phead);

						 M.m_Faces[phr->dual()->pFace->m_nFaceid] = *modify_face_right;
						 /*list<Face>::iterator itr=M.m_Faces.begin();
						 for(;itr!=M.m_Faces.end();itr++)
						 {
							 if(itr->m_nFaceid==phr->dual()->pFace->m_nFaceid)
							 { *itr=*modify_face_right; break; }
						 }*/

				}
				 else       //À‰Àµ¥À ±√ª”–‘ˆº”–¬µƒ±ﬂ£¨µ´ «“ÚŒ™√ÊµƒΩ·ππ±‰¡À£¨ªπ «“™∏ƒ“ªœ�?
				{
					
					 phead=phr->dual()->pFace->pHalfedge; ph=phead;
					 do{
						 if(ph->source()->m_nVerid==v[1]->m_nVerid)
						 {
							 if(ph->pEdge->pLeftHalfEdge==phr)
							 {
								 ph->pEdge->pLeftHalfEdge=ph_face_d->pPrevHalfedge;
								 break;
							 }
							 if(ph->pEdge->pRightHalfEdge==phr)
							 {
								 ph->pEdge->pRightHalfEdge=ph_face_d->pPrevHalfedge;
								 break;
							 }
							
						 }
						 ph=ph->pNextHalfedge;
					 }while(ph!=phead);

					 phead=phr->pNextHalfedge->dual()->pFace->pHalfedge; ph=phead;
					 do{
						 if(ph->pVertex->m_nVerid==v[1]->m_nVerid)
						 {
							 if(ph->pEdge->pLeftHalfEdge==phr->pNextHalfedge)
							 {
								 ph->pEdge->pLeftHalfEdge=ph_face_u->pNextHalfedge;
								 break;
							 }
							 if(ph->pEdge->pRightHalfEdge==phr->pNextHalfedge)
							 {
								 ph->pEdge->pRightHalfEdge=ph_face_u->pNextHalfedge;
								 break;
							 }
							
						 }
						 ph=ph->pNextHalfedge;
					 }while(ph!=phead);
					 
				}
				
			}
			
/*----------------------------------all the face is construct--------------------------------------------------------*/
			//cout<<"vertex push in M:"<<endl;

			
			 CHalfEdge *pvhead=NULL;
			 CHalfEdge *pv=NULL;
			 vector<CVertex> mody_ver;

			 pvhead=insert_face.pHalfedge;pv=pvhead;
			 do{
				 mody_ver.push_back(*(pv->pVertex));
				 pv=pv->pNextHalfedge;
			 }while(pv!=pvhead);
			 if(phl->dual()!=NULL)
			 {
				 pvhead=phl->dual()->pFace->pHalfedge; pv=pvhead;
                  do{
					  int j=0;
					  for(;j<(int)mody_ver.size();j++)
					  {
						  if(pv->pVertex->m_nVerid==mody_ver[j].m_nVerid)
							  break;
					  }
					  if(j==(int)mody_ver.size())
				         mody_ver.push_back(*(pv->pVertex));
				      pv=pv->pNextHalfedge;
			       }while(pv!=pvhead);
			 }
			 if(phr->dual()!=NULL)
			 {
				 pvhead=phr->dual()->pFace->pHalfedge; pv=pvhead;
                  do{
					  int j=0;
					  for(;j<(int)mody_ver.size();j++)
					  {
						  if(pv->pVertex->m_nVerid==mody_ver[j].m_nVerid)
							  break;
					  }
					  if(j==(int)mody_ver.size())
				         mody_ver.push_back(*(pv->pVertex));
				      pv=pv->pNextHalfedge;
			       }while(pv!=pvhead);
			 }
			
			 for(int j=0;j< (int)mody_ver.size();j++)
				 M.m_Vertices[mody_ver[j].m_nVerid].pHalfEdge = mody_ver[j].pHalfEdge; 
				
			if(newnuml==2)
			{ 
				//Vertex push_ver;
				//push_ver=*(ph_face_d->pVertex);
				//M.m_Vertices.push_back(push_ver);
				M.m_Vertices.push_back(*(ph_face_d->pVertex));
			}
			if(newnumr==2)
			{
                /*Vertex push_ver;
				push_ver=*(ph_face_u->pVertex);
				M.m_Vertices.push_back(push_ver);*/
				M.m_Vertices.push_back(*(ph_face_u->pVertex));
			}
			
/*----------------------------------all the face is construct--------------------------------------------------------*/
		
			/*vector<CFace>::iterator itr = M.m_Faces.begin();
			for(;itr!=M.m_Faces.end();itr++)
			{
				if(itr->m_nFaceid == insert_face.m_nFaceid)
				{
					itr = M.m_Faces.erase(itr);
					if(itr!=M.m_Faces.end())
						M.m_Faces.insert(itr,*fd);
					else
						M.m_Faces.push_back(*fd);
					break;
				}
			}*/

			for(int ii=0; ii<4; ii++)//xhw initial siblings
			{
				fu->m_nSiblings[ii] = -1;
				fd->m_nSiblings[ii] = -1;
			}
			
			M.m_Faces[insert_face.m_nFaceid] = *fd;
			M.m_Faces.push_back(*fu);
			
/*----------------------------------test for the function-----------------------------------------------------------*/
		//	cout<<"test for Insert_Mesh_H for M:"<<endl;
			/*itr=M.m_Faces.begin();
			for(;itr!=M.m_Faces.end();itr++)
			{
				cout<<"H M:"<<itr->m_nFaceid<<itr->m_bMark<<endl;
			}*/
             /* itr=M.m_Faces.begin();
			for(;itr!=M.m_Faces.end();itr++)
			{
				phead=itr->pHalfedge;ph=phead;cout<<itr->m_nFaceid<<":"<<itr->m_nFourIndex[0]<<" "<<itr->m_nFourIndex[1]<<
					" "<<itr->m_nFourIndex[2]<<" "<<itr->m_nFourIndex[3]<<endl;
				do{
					cout<<ph->pVertex->m_nVerid<<","<<ph->pEdge->m_nTwoVer[0]<<" "<<ph->pEdge->m_nTwoVer[1]<<";";
					ph=ph->pNextHalfedge;
				}while(ph!=phead);
				cout<<endl;
			}*/

		//phead=fu->pHalfedge;
		//ph=phead;
		//cout<<"test for lface:"<<fu->m_nFaceid<<":";
		//do{
		//	//if(ph->pEdge->pRightHalfEdge!=NULL)
		//		cout<<ph->pVertex->m_nVerid<<" ";
		//		
		//	ph=ph->pNextHalfedge;
		//}while(ph!=phead);
		//cout<<endl;
		//list<Vertex>::iterator itrv;//=M.m_Vertices.begin();
		//itrv=M.m_Vertices.begin();
		//for(;itrv!=M.m_Vertices.end();itrv++)
		//	cout<<itrv->m_nVerid<<" "
			/*itrv=M.m_Vertices.begin();
		for(;itrv!=M.m_Vertices.end();itrv++)
		{		cout<<itrv->m_nVerid<<":"<<itrv->pHalfEdge->pEdge->m_nTwoVer[0]<<" "<<itrv->pHalfEdge->pEdge->m_nTwoVer[1]<<",,";
		         vector<HalfEdge*> lst_hes;
	             VertexInHalfEdge(*itrv,lst_hes);
	          for(int i=0;i<lst_hes.size();i++)
		            cout<<lst_hes[i]->pEdge->m_nTwoVer[0]<<" "<<lst_hes[i]->pEdge->m_nTwoVer[1]<<";";
	          cout<<endl;
		}*/

}



//’“µΩ∏¯∂®T∂•µ„À˘‘⁄µƒ±ﬂµÿ∞Î±ﬂΩ·ππ
void Insert_Mesh_V(CMesh &M,CFace insert_face,CVertex *v[2])
{
	       int       newnumu,newnumd;
           CHalfEdge *ph,*phead;
		   CHalfEdge *phu=NULL;
		   CHalfEdge *phd=NULL;
          
/*--------------------------------find phd,phu,then find vertex-----------------------------------*/	
		   
		    phead=insert_face.pHalfedge; 
			ph=phead;   
			do               //’“µΩƒ«–©±ª≤Â»Îµƒ ˙÷±±ﬂ≈¸≥…¡Ω∞Îµƒ∞Î±ﬂΩ·ππphU,phD;
			{
				if(phd==NULL && ph->pVertex->m_dVerx >=v[0]->m_dVerx )
				{
					if(ph->pVertex->m_dVerx > v[0]->m_dVerx)
						newnumd=2;            //—œ∏Ò¥Û”⁄ ±¥À ±–Ëππ‘�?Ãı–¬±ﬂ
					else 
						newnumd=0;
					phd=ph;
				}
				if(phd!=NULL && phu==NULL && ph->pVertex->m_dVerx < v[1]->m_dVerx ) //≤ªø…»°µ»∫≈«“◊‹µ√µ»œ¬∞Î±ﬂÕÍ≥…‘Ÿ◊ˆÀ¸
				{
					if(ph->source()->m_dVerx > v[1]->m_dVerx)
						newnumu=2;
					else
						newnumu=0;
					phu=ph;
				}
				ph=ph->pNextHalfedge;
			}while(ph!=phead);
			/*if(phu->dual()!=NULL)
			 cout<<"phu dual m_bMark:"<<phu->dual()->pFace->m_nFaceid<<" "<<phu->dual()->pFace->m_bMark<<endl;	*/
			//cout<<"newnum:"<<newnumu<<" "<<newnumd<<" "<<phd->pVertex->m_nVerid<<" "<<phu->pVertex->m_nVerid<<" ";
			
/*-------------------------------modify v[0],v[1] id------------------------------------*/	
			if(newnumd==0)
			{
				v[1]->m_nVerid=v[0]->m_nVerid;
				v[0]=phd->pVertex;
			}
			if(newnumu==0)
			{
				v[1]=phu->source();
			}
			//cout<<v[0]->m_nVerid<<" "<<v[1]->m_nVerid<<endl;
			
/*----------------------------------left,right face construct--------------------------------------------------------------------*/
			int facelindex = insert_face.m_nFaceid;
			int facerindex = M.m_Faces.size();
			CFace *fl=NULL; 
			CFace *fr=NULL;

			
			//cout<<"left face construction start:"<<endl;
			//fl=insert_edge_Lface_con(facelindex,insert_face,v,phd,phu,newnumd, newnumu);
			////cout<<"right face construction start:"<<endl;
			//fr=insert_edge_Rface_con(facerindex,insert_face,v,phd,phu,newnumd, newnumu);

			fl=insert_edge_Lface_con(facelindex,M.m_Faces[facelindex],v,phd,phu,newnumd, newnumu);
			fr=insert_edge_Rface_con(facerindex,M.m_Faces[facelindex],v,phd,phu,newnumd, newnumu);
			
			phead=fl->pHalfedge;
			ph=phead;
			do{
				if(ph->pVertex->m_nVerid==v[1]->m_nVerid)
					break;
				ph=ph->pNextHalfedge;
			}while(ph!=phead);
			ph->pEdge->pRightHalfEdge=fr->pHalfedge;

		     CEdge *v_edge=new CEdge;
			 v_edge->m_dLength=fabs(v[1]->m_dVery -v[0]->m_dVery);
			 v_edge->m_nTwoVer[0]=v[1]->m_nVerid;     v_edge->m_nTwoVer[1]=v[0]->m_nVerid;
			 v_edge->pLeftHalfEdge=ph;
			 v_edge->pRightHalfEdge=fr->pHalfedge;
			 fr->pHalfedge->pEdge=v_edge; 
			

			// HalfEdge *ph1,*ph2;
			// ph1=insert_face.pHalfedge;ph2=ph1;
			// do{
			//	 cout<<ph2->dual()->pFace->m_nFaceid<<" ";
			//	 ph2=ph2->pNextHalfedge;
			// }while(ph2!=ph1);
			// HalfEdge *pheadw,*phw;
			//cout<<"mesh insert T-point:"<<M.m_Faces.size()<<endl;
			// list<Face>::iterator itrf=M.m_Faces.begin();
			// for(;itrf!=M.m_Faces.end();itrf++)
			// {
			//	pheadw=itrf->pHalfedge;
			//	phw=pheadw;
			//	cout<<itrf->m_nFaceid<<":";
			//	do{
			//		if(phw->dual()!=NULL)
			//		{
			//			//cout<<ph->pEdge->m_nTwoVer[0]<<" "<<ph->pEdge->m_nTwoVer[1]<<";";
			//			cout<<phw->dual()->pFace->m_nFaceid<<" ";
			//		}
			//	//	cout<<ph->pVertex->pHalfEdge->pEdge->m_nTwoVer[0]<<" "<<ph->pVertex->pHalfEdge->pEdge->m_nTwoVer[1]<<",";
			//		phw=phw->pNextHalfedge;
			//	   }while(phw!=pheadw);
			//	cout<<endl;
			//	
			//  }
			// cout<<"mesh insert T-point end:"<<endl;
			/* cout<<"fl,fr:"<<fl->m_bMark<<" "<<fr->m_bMark<<" "<<insert_face.m_bMark<<endl;*/
/*----------------------------------related face is modified--------------------------------------------------------*/	
			
			// cout<<"related face is modified---"<<endl;
			 if(phd->dual()!=NULL )
			{
			    if(newnumd==2)
				{
						 CFace *modify_face_down=NULL;
						 modify_face_down=split_edge_of_insert_vertex(&M.m_Faces[phd->dual()->pFace->m_nFaceid],v[0],'U');//phd->dual()->pFace,v[0],'U');	
						 phead=modify_face_down->pHalfedge; ph=phead;
						 do{
							 if(ph->pVertex->m_nVerid==v[0]->m_nVerid)
							 {
								 ph->pEdge->pRightHalfEdge=ph->pEdge->pLeftHalfEdge;
								 ph->pEdge->pLeftHalfEdge=fr->pHalfedge->pNextHalfedge;
								 fr->pHalfedge->pNextHalfedge->pEdge->pRightHalfEdge=ph;
							 }
							 if(ph->source()->m_nVerid==v[0]->m_nVerid)
							 {
								 ph->pEdge->pRightHalfEdge=ph->pEdge->pLeftHalfEdge;
								 ph->pEdge->pLeftHalfEdge=fr->pHalfedge->dual()->pPrevHalfedge;
								fr->pHalfedge->dual()->pPrevHalfedge->pEdge->pRightHalfEdge=ph;
							 }
							 ph=ph->pNextHalfedge;
						 }while(ph!=phead);
						 M.m_Faces[phd->dual()->pFace->m_nFaceid]= *modify_face_down;
						/* list<Face>::iterator itr=M.m_Faces.begin();
						 for(;itr!=M.m_Faces.end();itr++)
						 {
							 if(itr->m_nFaceid==phd->dual()->pFace->m_nFaceid)
							 { *itr=*modify_face_down; break; }
						 }*/
				  }
				else
				{       // cout<<"phd dual face is modyfied:"<<phd->dual()->pFace->m_nFaceid<<" ";
					     phead=phd->dual()->pFace->pHalfedge; ph=phead;
						 do{
							 if(ph->source()->m_nVerid==v[0]->m_nVerid)
							 {
								 if( ph->pEdge->pLeftHalfEdge==phd)
									 ph->pEdge->pLeftHalfEdge=fr->pHalfedge->dual()->pPrevHalfedge;
								 if( ph->pEdge->pRightHalfEdge==phd)
									 ph->pEdge->pRightHalfEdge=fr->pHalfedge->dual()->pPrevHalfedge;
								// cout<<ph->pEdge->m_nTwoVer[0]<<" "<<ph->pEdge->m_nTwoVer[1]<<";";
								 break;
							 }
							 ph=ph->pNextHalfedge;
						 }while(ph!=phead);  
						// cout<<phd->pNextHalfedge->dual()->pFace->m_nFaceid<<":";
						 phead=phd->pNextHalfedge->dual()->pFace->pHalfedge; ph=phead;
						 do{
							 if(ph->pVertex->m_nVerid==v[0]->m_nVerid)
							 {
								 if( ph->pEdge->pLeftHalfEdge==phd->pNextHalfedge)
									 ph->pEdge->pLeftHalfEdge=fr->pHalfedge->pNextHalfedge;
								 if( ph->pEdge->pRightHalfEdge==phd->pNextHalfedge)
									 ph->pEdge->pRightHalfEdge=fr->pHalfedge->pNextHalfedge;
								// cout<<ph->pEdge->m_nTwoVer[0]<<" "<<ph->pEdge->m_nTwoVer[1]<<";";
								 break;
							 }
							 ph=ph->pNextHalfedge;
						 }while(ph!=phead);  
				}
			}
			
            if(phu->dual()!=NULL)
			{
				if(newnumu==2)
				{
						 CFace *modify_face_up=NULL;
						 
						// cout<<"phu->dual():"<<phu->dual()->pVertex->m_nVerid<<" "<<phu->dual()->pEdge->m_nTwoVer[0]<<" "<<phu->dual()->pEdge->m_nTwoVer[1]<<endl;
											
						 modify_face_up=split_edge_of_insert_vertex(&M.m_Faces[phu->dual()->pFace->m_nFaceid],v[1],'D');//phu->dual()->pFace,v[1],'D');	
						
						// cout<<"up face is modified:";
						 phead=modify_face_up->pHalfedge; ph=phead;
						 do{
							 if(ph->pVertex->m_nVerid==v[1]->m_nVerid)
							 {
								 ph->pEdge->pRightHalfEdge=ph->pEdge->pLeftHalfEdge;
								 ph->pEdge->pLeftHalfEdge=fr->pHalfedge->dual()->pNextHalfedge;
								 fr->pHalfedge->dual()->pNextHalfedge->pEdge->pRightHalfEdge=ph;
							 }
							 if(ph->source()->m_nVerid==v[1]->m_nVerid)
							 {
								 ph->pEdge->pRightHalfEdge=ph->pEdge->pLeftHalfEdge;
								 ph->pEdge->pLeftHalfEdge=fr->pHalfedge->pPrevHalfedge;
								fr->pHalfedge->pPrevHalfedge->pEdge->pRightHalfEdge=ph;
							 }
							 ph=ph->pNextHalfedge;
						 }while(ph!=phead);
						  /* int k=0;HalfEdge *phe,*phh;
							phe=modify_face_up->pHalfedge;phh=phe;
							cout<<modify_face_up->m_nFaceid<<":";
							do{
								cout<<phh->pEdge->m_nTwoVer[0]<<" "<<phh->pEdge->m_nTwoVer[1]<<";";
								phh=phh->pNextHalfedge;k++;
							}while(phh!=phe );*/
						/*  list<Face>::iterator fh=M.m_Faces.begin();
			            for(;fh!=M.m_Faces.end();fh++)
			            {
							cout<<"test:"<<fh->m_nFaceid<<" "<<fh->m_bMark<<endl;
			            }
						cout<<"phu->dual()->pFace->m_nFaceid:"<<phu->dual()->pFace->m_bMark<<" "<<modify_face_up->m_bMark<<endl;*/

						 M.m_Faces[phu->dual()->pFace->m_nFaceid] = *modify_face_up;
						 /*list<Face>::iterator itr=M.m_Faces.begin();
						 for(;itr!=M.m_Faces.end();itr++)
						 {
							 if(itr->m_nFaceid==phu->dual()->pFace->m_nFaceid)
							 { *itr=*modify_face_up; break; }
						 }*/
						 
				}
				else
				{
					//cout<<"phu dual face is modyfied:"<<phu->pEdge->m_nTwoVer[0]<<" "<<phu->pEdge->m_nTwoVer[1]<<"face id"<<phu->pFace->m_nFaceid<<" "<<phu->dual()->pFace->m_nFaceid<<" ";
					     phead=phu->dual()->pFace->pHalfedge; ph=phead;
						 do{
							 if(ph->pVertex->m_nVerid==v[1]->m_nVerid)
							 {
								 if( ph->pEdge->pLeftHalfEdge==phu)
									 ph->pEdge->pLeftHalfEdge=fr->pHalfedge->dual()->pNextHalfedge;
								 if( ph->pEdge->pRightHalfEdge==phu)
									 ph->pEdge->pRightHalfEdge=fr->pHalfedge->dual()->pNextHalfedge;
								// cout<<ph->pEdge->m_nTwoVer[0]<<" "<<ph->pEdge->m_nTwoVer[1]<<";";
								 break;
							 }
							 ph=ph->pNextHalfedge;
						 }while(ph!=phead);  
						// cout<<phu->pPrevHalfedge->dual()->pFace->m_nFaceid<<":";
						 phead=phu->pPrevHalfedge->dual()->pFace->pHalfedge; ph=phead;
						 do{
							 if(ph->source()->m_nVerid==v[1]->m_nVerid)
							 {
								 if( ph->pEdge->pLeftHalfEdge==phu->pPrevHalfedge)
									 ph->pEdge->pLeftHalfEdge=fr->pHalfedge->pPrevHalfedge;
								 if( ph->pEdge->pRightHalfEdge==phu->pPrevHalfedge)
									 ph->pEdge->pRightHalfEdge=fr->pHalfedge->pPrevHalfedge;
								//  cout<<ph->pEdge->m_nTwoVer[0]<<" "<<ph->pEdge->m_nTwoVer[1]<<";";
								 break;
							 }
							 ph=ph->pNextHalfedge;
						 }while(ph!=phead);  
				}
			}
			/*HalfEdge *ph1,*ph2;cout<<"my wangt:";
			 ph1=fr->pHalfedge;ph2=ph1;
			 do{
				 cout<<ph2->dual()->pEdge->m_nTwoVer[0]<<" "<<ph2->dual()->pEdge->m_nTwoVer[1]<<";";
				 ph2=ph2->pNextHalfedge;
			 }while(ph2!=ph1);*/
          
/*----------------------------------all the face is construct--------------------------------------------------------*/
			
			//cout<<"vertex push in M:"<<endl; // ¬«È‘∂√ª”–»Á¥ÀºÚµ•£¨±ª≤Â»Îµƒ√Ê“‘º∞ƒ«–©œ‡πÿµƒ√Êµƒ∂•µ„æ˘–Ë“™¥¶¿Ì£¨“ÚŒ™À¸√«‘⁄
			                                //M°£m_Vertices÷–≤¢√ª”––ﬁ∏�?
             
			 vector<CVertex> mody_ver_index;
			 CHalfEdge *pvhead=NULL;
			 CHalfEdge *pv=NULL;

			 pvhead=insert_face.pHalfedge;pv=pvhead;
			 do{
				 mody_ver_index.push_back(*(pv->pVertex));
				 pv=pv->pNextHalfedge;
			 }while(pv!=pvhead);
			 if(phd->dual()!=NULL)
			 {
				 pvhead=phd->dual()->pFace->pHalfedge; pv=pvhead;
                  do{
					  int j=0;
					  for(;j<(int)mody_ver_index.size();j++)
					  {
						  if(pv->pVertex->m_nVerid==mody_ver_index[j].m_nVerid)
							  break;
					  }
					  if(j==(int)mody_ver_index.size())
				         mody_ver_index.push_back(*(pv->pVertex));
				      pv=pv->pNextHalfedge;
			       }while(pv!=pvhead);
			 }
			 if(phu->dual()!=NULL)
			 {
				 pvhead=phu->dual()->pFace->pHalfedge; pv=pvhead;
                  do{
					  int j=0;
					  for(;j<(int)mody_ver_index.size();j++)
					  {
						  if(pv->pVertex->m_nVerid==mody_ver_index[j].m_nVerid)
							  break;
					  }
					  if(j==(int)mody_ver_index.size())
				         mody_ver_index.push_back(*(pv->pVertex));
				      pv=pv->pNextHalfedge;
			       }while(pv!=pvhead);
			 }
			 /* cout<<"mody ver index output start:"<<" ";
			 for(int i=0;i<mody_ver_index.size();i++)
				 cout<<mody_ver_index[i].m_nVerid<<" ";
			 cout<<"mody ver index output:"<<endl;*/
			
				 for(int j=0;j < (int)mody_ver_index.size();j++)
				     M.m_Vertices[mody_ver_index[j].m_nVerid].pHalfEdge = mody_ver_index[j].pHalfEdge;
					 
			if(newnumd==2)
			{ 
				CVertex push_ver;
				push_ver=*(fr->pHalfedge->pVertex);
				M.m_Vertices.push_back(push_ver);
			}
			if(newnumu==2)
			{
                CVertex push_ver;
				push_ver=*(fr->pHalfedge->source());
				M.m_Vertices.push_back(push_ver);
			}
/*----------------------------------all the face is construct--------------------------------------------------------*/

			//cout<<"m_Faces push in M:"<<endl;
			/*itr=M.m_Faces.begin();cout<<"insert V before m_bMark:"<<endl;
			for(;itr!=M.m_Faces.end();itr++)
			{
				cout<<itr->m_nFaceid<<" "<<itr->m_bMark<<endl;
			}*/
			
			/*vector<CFace>::iterator itr=M.m_Faces.begin();
			for(;itr!=M.m_Faces.end();itr++)
			{
				if(itr->m_nFaceid==insert_face.m_nFaceid)
				{
					M.m_Faces.erase(itr++);
					if(itr!=M.m_Faces.end())
						M.m_Faces.insert(itr,*fl);
					else
						M.m_Faces.push_back(*fl);
					break;
				}
			}*/
			
			for(int ii=0; ii<4; ii++)//xhw initial siblings
			{
				fl->m_nSiblings[ii] = -1;
				fr->m_nSiblings[ii] = -1;
			}

			M.m_Faces[insert_face.m_nFaceid] = *fl;
			M.m_Faces.push_back(*fr);
			
/*----------------------------------test for the function-----------------------------------------------------------*/
		//	cout<<"test for Mesh M:"<<endl;

             /* itr=M.m_Faces.begin();
			for(;itr!=M.m_Faces.end();itr++)
			{
				cout<<itr->m_nFaceid<<" "<<itr->m_bMark<<endl;*/
				/*phead=itr->pHalfedge;ph=phead;cout<<itr->m_nFaceid<<":"<<itr->m_nFourIndex[0]<<" "<<itr->m_nFourIndex[1]<<
					" "<<itr->m_nFourIndex[2]<<" "<<itr->m_nFourIndex[3]<<endl;
				do{
					cout<<ph->pEdge->m_nTwoVer[0]<<" "<<ph->pEdge->m_nTwoVer[1]<<";";
					ph=ph->pNextHalfedge;
				}while(ph!=phead);
				cout<<endl;*/
			/*}*/
            /*for(int j=0;j<6;j++)
				itr++;
			cout<<itr->m_nFaceid<<":";
              phead=itr->pHalfedge;ph=phead;cout<<itr->m_nFaceid<<":"<<itr->m_nFourIndex[0]<<" "<<itr->m_nFourIndex[1]<<
					" "<<itr->m_nFourIndex[2]<<" "<<itr->m_nFourIndex[3]<<endl;
				do{
					cout<<ph->dual()->pPrevHalfedge->pEdge->m_nTwoVer[0]<<" "<<ph->dual()->pPrevHalfedge->pEdge->m_nTwoVer[1]<<";";
					ph=ph->pNextHalfedge;
				}while(ph!=phead);
		 cout<<endl;*/
		//=M.m_Vertices.begin();
		/*itrv=M.m_Vertices.begin();
		for(;itrv!=M.m_Vertices.end();itrv++)
		{		cout<<itrv->m_nVerid<<":"<<itrv->pHalfEdge->pEdge->m_nTwoVer[0]<<" "<<itrv->pHalfEdge->pEdge->m_nTwoVer[1]<<",,";
		         vector<HalfEdge*> lst_hes;
	             VertexInHalfEdge(*itrv,lst_hes);
	          for(int i=0;i<lst_hes.size();i++)
		            cout<<lst_hes[i]->pEdge->m_nTwoVer[0]<<" "<<lst_hes[i]->pEdge->m_nTwoVer[1]<<";";
	          cout<<endl;
		}*/

}

CFace *insert_edge_Lface_con(int index,CFace insert_face,CVertex *v[2],CHalfEdge *phd,CHalfEdge *phu,int newnumd,int newnumu)
{
	       
		   int              ver_num;
		   double           lengthedge;
	       vector<CVertex*> lst_ver;
           CHalfEdge        *ph,*phead;
            
		    phead=insert_face.pHalfedge; 
			ph=phead;   
            lst_ver.push_back(v[1]);
			ph=phu;
			do{
				lst_ver.push_back(ph->pVertex);
				ph=ph->pNextHalfedge;
			}while(ph!=phd);
			lst_ver.push_back(v[0]);
			ver_num=lst_ver.size();
			//cout<<"ver_num:"<<ver_num<<endl;
/*-------------------------------------------------------------------*/	
			CFace *face=new CFace;
			face->m_nFaceid=index;                      //left is the insert_face
			face->m_dErr=insert_face.m_dErr;
			face->m_bMark=insert_face.m_bMark;
			face->m_nFourIndex[0]=insert_face.m_nFourIndex[0];
			face->m_nFourIndex[1]=v[0]->m_nVerid;
			face->m_nFourIndex[2]=v[1]->m_nVerid;
			face->m_nFourIndex[3]=insert_face.m_nFourIndex[3];
			
		    //Generate half-edge and connect them
			vector<CHalfEdge*> he;
			he.resize(ver_num);
			 for(int i=0;i<ver_num;i++)
			 {
				 he[i]=new CHalfEdge;
				 he[i]->pVertex=lst_ver[i];
				 he[i]->pFace=face;
				 lst_ver[i]->pHalfEdge=he[i];	 
			//	 cout<<lst_ver[i]->m_nVerid<<" ";
			 }

			 for(int i=0;i<ver_num;i++)
			 {
				 he[i]->pNextHalfedge=he[(i+1)%ver_num];
				 he[i]->pPrevHalfedge=he[(i+ver_num-1)%ver_num];

			 }
			 for(int i=0;i<ver_num;i++)
			 {
				 if(he[i]->pVertex->m_nVerid==face->m_nFourIndex[0])
				 {  face->pHalfedge=he[i];break;}
			 }
			 int id;
			 if(newnumu==2)
			 {
                 CEdge *pe=new CEdge;
				 lengthedge=fabs(v[1]->m_dVerx - phu->pVertex->m_dVerx);
				 pe->m_dLength=lengthedge;
				 pe->m_nTwoVer[0]=v[1]->m_nVerid; pe->m_nTwoVer[1]=phu->pVertex->m_nVerid;
				 pe->pLeftHalfEdge=he[1];
				 pe->pRightHalfEdge=NULL;
				 he[1]->pEdge=pe;
				 ph=phu->pNextHalfedge; id=2;
			 }
			 else
			 {   ph=phu;  id=1;}

			 do{
				     /*Edge *pe=ph->pEdge;
				     assert(pe->pLeftHalfEdge!=NULL);
					 if(pe->pLeftHalfEdge==ph)
					  pe->pLeftHalfEdge=he[id];
					 if(pe->pRightHalfEdge==ph)
						 pe->pRightHalfEdge=he[id];*/
					 CEdge *pe=new CEdge;
					 *pe=*(ph->pEdge);
					  assert(pe->pLeftHalfEdge!=NULL);
					 if(pe->pLeftHalfEdge==ph)
					 {
					    pe->pLeftHalfEdge=he[id];
					    if(ph->dual()!=NULL)
							ph->dual()->pEdge=pe;
					 }
					 if(pe->pRightHalfEdge==ph)
					 {
						 pe->pRightHalfEdge=he[id];ph->dual()->pEdge=pe;
					 }
					 he[id]->pEdge=pe;
					 id++; ph=ph->pNextHalfedge;

			 }while(ph!=phd);
			/* HalfEdge *ph1,*ph2;
			 ph1=insert_face.pHalfedge;ph2=ph1;
			 do{
				 if(ph2->dual()!=NULL)
				 cout<<ph2->dual()->pFace->m_nFaceid<<" ";
				 ph2=ph2->pNextHalfedge;
			 }while(ph2!=ph1);
			 cout<<"left insert face output edn";*/
			 if(newnumd==2)
			 {
                 CEdge *pe=new CEdge;
				 lengthedge=fabs(v[0]->m_dVerx - phd->source()->m_dVerx);
				 pe->m_dLength=lengthedge;
				 pe->m_nTwoVer[0]=v[0]->m_nVerid; pe->m_nTwoVer[1]=phd->source()->m_nVerid;
				 pe->pLeftHalfEdge=he[id];
				 pe->pRightHalfEdge=NULL;
				 he[id]->pEdge=pe;
				 
			 }
			 else
			 {      
				 CEdge *pe=new CEdge;
		         *pe=*(ph->pEdge);
			     assert(pe->pLeftHalfEdge!=NULL);
				 if(pe->pLeftHalfEdge==ph)
				 {
					 pe->pLeftHalfEdge=he[id];
					 if(ph->dual()!=NULL)
						ph->dual()->pEdge=pe;

				 }
				 if(pe->pRightHalfEdge==ph)
				 {
					 pe->pRightHalfEdge=he[id]; ph->dual()->pEdge=pe;
				 }
				 he[id]->pEdge=pe;
			 }
			 CEdge *v_edge=new CEdge;
			 v_edge->m_dLength=fabs(v[1]->m_dVery -v[0]->m_dVery);
			 v_edge->m_nTwoVer[0]=v[0]->m_nVerid;     v_edge->m_nTwoVer[1]=v[1]->m_nVerid;
			 v_edge->pLeftHalfEdge=he[0];
			 v_edge->pRightHalfEdge=NULL;
			 he[0]->pEdge=v_edge;
            
			 return face;

}


CFace *insert_edge_Rface_con(int index,CFace insert_face,CVertex *v[2],CHalfEdge *phd,CHalfEdge *phu,int newnumd,int newnumu)
{
	       
		   int    ver_num;
		   double lengthedge;
	       vector<CVertex*> lst_ver;
           CHalfEdge *ph=NULL; //*phead;
/*-------------------------------------------------------------------*/	 
		   if(newnumd==2)
			   ph=phd;
		   else
			   ph=phd->pNextHalfedge;

		    lst_ver.push_back(v[0]);
			do{
				lst_ver.push_back(ph->pVertex);
				ph=ph->pNextHalfedge;
			}while(ph!=phu);
			if(newnumu==2)
			   lst_ver.push_back(v[1]);
			ver_num=lst_ver.size();
			//cout<<"right face ver_num,phd,phu,lst_ver:"<<ver_num<<" "<<phd->pVertex->m_nVerid<<" "<<phu->pVertex->m_nVerid<<";";

/*-------------------------------------------------------------------*/	
			CFace *face=new CFace;
			face->m_nFaceid=index;                      //left is the insert_face
			face->m_dErr=0.0;face->m_bMark=0;
			face->m_nFourIndex[0]=v[0]->m_nVerid;
			face->m_nFourIndex[1]=insert_face.m_nFourIndex[1];
			face->m_nFourIndex[2]=insert_face.m_nFourIndex[2];
			face->m_nFourIndex[3]=v[1]->m_nVerid;
			
		    //Generate half-edge and connect them
			vector<CHalfEdge*> he;
			he.resize(ver_num);
			 for(int i=0;i<ver_num;i++)
			 {
				 he[i]=new CHalfEdge;
				 he[i]->pVertex=lst_ver[i];
				 he[i]->pFace=face;
				 lst_ver[i]->pHalfEdge=he[i];
				// cout<<lst_ver[i]->m_nVerid<<" ";
			 }

			 for(int i=0;i<ver_num;i++)
			 {
				 he[i]->pNextHalfedge=he[(i+1)%ver_num];
				 he[i]->pPrevHalfedge=he[(i+ver_num-1)%ver_num];

			 }
			 face->pHalfedge=he[0];
			 int id;
			 if(newnumd==2)
			 {
                 CEdge *pe=new CEdge;
				 lengthedge=fabs(v[0]->m_dVerx - phd->pVertex->m_dVerx);
				 pe->m_dLength=lengthedge;
				 pe->m_nTwoVer[0]=v[0]->m_nVerid; pe->m_nTwoVer[1]=phd->pVertex->m_nVerid;
				 pe->pLeftHalfEdge=he[1];
				 pe->pRightHalfEdge=NULL;
				 he[1]->pEdge=pe;
				id=2;
			 }
			 else
			 {   id=1;}

              ph=phd->pNextHalfedge; 
			 do{
				     CEdge *pe=new CEdge;
					  *pe=*(ph->pEdge);
				     assert(pe->pLeftHalfEdge!=NULL);
					 if(pe->pLeftHalfEdge==ph)
					 {
						 pe->pLeftHalfEdge=he[id];
						 if(ph->dual()!=NULL)
							 ph->dual()->pEdge=pe;
					 }
					 if(pe->pRightHalfEdge==ph)
					 {
						 pe->pRightHalfEdge=he[id]; ph->dual()->pEdge=pe;
					 }
					
					 he[id]->pEdge=pe;
					 id++; ph=ph->pNextHalfedge;

			 }while(ph!=phu);
			 
			 if(newnumu==2)
			 {
                 CEdge *pe=new CEdge;
				 lengthedge=fabs(v[1]->m_dVerx - phu->source()->m_dVerx);
				 pe->m_dLength=lengthedge;
				 pe->m_nTwoVer[1]=v[1]->m_nVerid; pe->m_nTwoVer[0]=phu->source()->m_nVerid;
				 pe->pLeftHalfEdge=he[id];
				 pe->pRightHalfEdge=NULL;
				 he[id]->pEdge=pe;
				 
			 }
			 
			 /*Edge *v_edge=new Edge;
			 v_edge->m_dLength=fabs(v[1]->m_dVery -v[0]->m_dVery);
			 v_edge->m_nTwoVer[0]=v[1]->m_nVerid;     v_edge->m_nTwoVer[1]=v[0]->m_nVerid;
			 if(v_edge->pLeftHalfEdge==NULL)
			    v_edge->pLeftHalfEdge=he[0];
			 v_edge->pRightHalfEdge=NULL;*/
			/* he[0]->pEdge=NULL;
            HalfEdge  *phead=face->pHalfedge;
		ph=phead;
		cout<<"test for RIGHT face:";
		do{
			if(ph->pEdge!=NULL)
			cout<<ph->pVertex->m_nVerid<<","<<ph->pEdge->m_nTwoVer[0]<<" "<<ph->pEdge->m_nTwoVer[1]<<";";
			ph=ph->pNextHalfedge;
		}while(ph!=phead);
		cout<<endl;*/
			 return face;

}

CFace *insert_edge_Uface_con(int index,CFace insert_face,CVertex *v[2],CHalfEdge *phl,CHalfEdge *phr,int newnuml,int newnumr)
{
           int         ver_num;
		   double      lengthedge;
	       vector<CVertex*> lst_ver;
           CHalfEdge *ph=NULL; 
/*-------------------------------------------------------------------*/	 
		   if(newnumr==2)
			   ph=phr;
		   else
			   ph=phr->pNextHalfedge;

		    lst_ver.push_back(v[1]);
			do{
				lst_ver.push_back(ph->pVertex);
				ph=ph->pNextHalfedge;
			}while(ph!=phl);
			if(newnuml==2)
			   lst_ver.push_back(v[0]);
			ver_num=lst_ver.size();
			//cout<<"up face ver_num,phl,phr,lst_Ver:"<<ver_num<<";"<<phl->pVertex->m_nVerid<<" "<<phr->pVertex->m_nVerid<<";";

/*-------------------------------------------------------------------*/	
			CFace *face=new CFace;
			face->m_nFaceid=index;                     
			face->m_dErr=0.0;face->m_bMark=0;
			face->m_nFourIndex[0]=v[0]->m_nVerid;
			face->m_nFourIndex[1]=v[1]->m_nVerid;
			face->m_nFourIndex[2]=insert_face.m_nFourIndex[2];
			face->m_nFourIndex[3]=insert_face.m_nFourIndex[3];
			
		    //Generate half-edge and connect them
			vector<CHalfEdge*> he;
			he.resize(ver_num);
			 for(int i=0;i<ver_num;i++)
			 {
				 he[i]=new CHalfEdge;
				 he[i]->pVertex=lst_ver[i];
				 he[i]->pFace=face;
				 lst_ver[i]->pHalfEdge=he[i];
				 //cout<<lst_ver[i]->m_nVerid<<" ";
			 }
                   
			 for(int i=0;i<ver_num;i++)
			 {
				 he[i]->pNextHalfedge=he[(i+1)%ver_num];
				 he[i]->pPrevHalfedge=he[(i+ver_num-1)%ver_num];

			 }
			 for(int i=0;i<ver_num;i++)
			 {
				 if(he[i]->pVertex->m_nVerid==face->m_nFourIndex[0])
				 {  face->pHalfedge=he[i];break;}
			 }
			 
			 int id;
			 if(newnumr==2)
			 {
                 CEdge *pe=new CEdge;
				 lengthedge=fabs(v[1]->m_dVery - phr->pVertex->m_dVery);
				 pe->m_dLength=lengthedge;
				 pe->m_nTwoVer[0]=v[1]->m_nVerid; pe->m_nTwoVer[1]=phr->pVertex->m_nVerid;
				 pe->pLeftHalfEdge=he[1];
				 pe->pRightHalfEdge=NULL;
				 he[1]->pEdge=pe;
				id=2;
			 }
			 else
			 {   id=1;}

              ph=phr->pNextHalfedge; 
			 do{
				     CEdge *pe=new CEdge;
					 *pe=*(ph->pEdge);
				     assert(pe->pLeftHalfEdge!=NULL);
					 if(pe->pLeftHalfEdge==ph)
					 {
						 pe->pLeftHalfEdge=he[id];
						 if(ph->dual()!=NULL)
							 ph->dual()->pEdge=pe;
					 }
					 if(pe->pRightHalfEdge==ph)
					 {
						 pe->pRightHalfEdge=he[id];  ph->dual()->pEdge=pe;
					 }
					 he[id]->pEdge=pe;
					 id++; ph=ph->pNextHalfedge;
                  
			 }while(ph!=phl);
			 
			 if(newnuml==2)
			 {
                 CEdge *pe=new CEdge;
				 lengthedge=fabs(v[0]->m_dVery - phl->source()->m_dVery);
				 pe->m_dLength=lengthedge;
				 pe->m_nTwoVer[1]=v[0]->m_nVerid; pe->m_nTwoVer[0]=phl->source()->m_nVerid;
				 pe->pLeftHalfEdge=he[id];
				 pe->pRightHalfEdge=NULL;
				 he[id]->pEdge=pe;
				 
			 }
			
			/* Edge *v_edge=new Edge;
			 v_edge->m_dLength=fabs(v[1]->m_dVery -v[0]->m_dVery);
			 v_edge->m_nTwoVer[0]=v[1]->m_nVerid;     v_edge->m_nTwoVer[1]=v[0]->m_nVerid;
			 if(v_edge->pLeftHalfEdge==NULL)
			    v_edge->pLeftHalfEdge=he[0];
			 v_edge->pRightHalfEdge=NULL;
			 he[0]->pEdge=v_edge;*/
            
			 return face;

}


CFace *insert_edge_Dface_con(int index,CFace insert_face,CVertex *v[2],CHalfEdge *phl,CHalfEdge *phr,int newnuml,int newnumr)
{
	       
		   int ver_num;
		   double lengthedge;
	       vector<CVertex*> lst_ver;
           CHalfEdge *ph=NULL;
		   CHalfEdge *phead=NULL;
            
			ph=phl;
			do{
				lst_ver.push_back(ph->pVertex);
				ph=ph->pNextHalfedge;
			}while(ph!=phr);
			lst_ver.push_back(v[1]);
			lst_ver.push_back(v[0]);
			ver_num=lst_ver.size();
		
/*-------------------------------------------------------------------*/	
			CFace *face=new CFace;
			face->m_nFaceid=index;                      //down face is the insert_face
			face->m_dErr=insert_face.m_dErr;
			face->m_bMark=insert_face.m_bMark;
			face->m_nFourIndex[0]=insert_face.m_nFourIndex[0];
			face->m_nFourIndex[1]=insert_face.m_nFourIndex[1];
			face->m_nFourIndex[2]=v[1]->m_nVerid;
			face->m_nFourIndex[3]=v[0]->m_nVerid;
			
		    //Generate half-edge and connect them
			vector<CHalfEdge*> he;
			he.resize(ver_num);
			 for(int i=0;i<ver_num;i++)
			 {
				 he[i]=new CHalfEdge;
				 he[i]->pVertex=lst_ver[i];
				 he[i]->pFace=face;
				 lst_ver[i]->pHalfEdge=he[i];	 
			 }

			 for(int i=0;i<ver_num;i++)
			 {
				 he[i]->pNextHalfedge=he[(i+1)%ver_num];
				 he[i]->pPrevHalfedge=he[(i+ver_num-1)%ver_num];

			 }
			 for(int i=0;i<ver_num;i++)
			 {
				 if(he[i]->pVertex->m_nVerid==face->m_nFourIndex[0])
				 {  face->pHalfedge=he[i];break;}
			 }
			 int id;
			 if(newnuml==2)
			 {
                 CEdge *pe=new CEdge;
				 lengthedge=fabs(v[0]->m_dVery - phl->pVertex->m_dVery);
				 pe->m_dLength=lengthedge;
				 pe->m_nTwoVer[0]=v[0]->m_nVerid; 
				 pe->m_nTwoVer[1]=phl->pVertex->m_nVerid;
				 pe->pLeftHalfEdge=he[0];
				 pe->pRightHalfEdge=NULL;
				 he[0]->pEdge=pe;
				 ph=phl->pNextHalfedge; id=1;
			 }
			 else
			 {   ph=phl;  id=0;}

			 do{
				     CEdge *pe=new CEdge;
					 *pe=*(ph->pEdge);  
				     assert(pe->pLeftHalfEdge!=NULL);
					 if(pe->pLeftHalfEdge==ph)
					 {
						 pe->pLeftHalfEdge=he[id];
						 if(ph->dual()!=NULL)
							 ph->dual()->pEdge=pe;
					 }
					 if(pe->pRightHalfEdge==ph)
					 {
						 pe->pRightHalfEdge=he[id]; 
						 ph->dual()->pEdge=pe;
					 }
					
					 he[id]->pEdge=pe;
					 id++; 
					 ph=ph->pNextHalfedge;

			 }while(ph!=phr);
			 
			 if(newnumr==2)
			 {
                 CEdge *pe=new CEdge;
				 lengthedge=fabs(v[1]->m_dVery - phr->source()->m_dVery);
				 pe->m_dLength=lengthedge;
				 pe->m_nTwoVer[1]=v[1]->m_nVerid; 
				 pe->m_nTwoVer[0]=phr->source()->m_nVerid;
				 pe->pLeftHalfEdge=he[id];
				 pe->pRightHalfEdge=NULL;
				 he[id]->pEdge=pe;
				 
			 }
			 else
			 {  
			     CEdge *pe=new CEdge;
		         *pe=*(phr->pEdge);
			     assert(pe->pLeftHalfEdge!=NULL);
				 if(pe->pLeftHalfEdge==phr)
					 pe->pLeftHalfEdge=he[id];
				 if(pe->pRightHalfEdge==phr)
					 pe->pRightHalfEdge=he[id];
				 he[id]->pEdge=pe;
			 }

			 CEdge *v_edge = new CEdge;
			 v_edge->m_dLength=fabs(v[1]->m_dVerx -v[0]->m_dVerx);
			 v_edge->m_nTwoVer[0]=v[1]->m_nVerid;     
			 v_edge->m_nTwoVer[1]=v[0]->m_nVerid;
			 v_edge->pLeftHalfEdge=he[ver_num-1];
			 v_edge->pRightHalfEdge=NULL;
			 he[ver_num-1]->pEdge=v_edge;
            
			 return face;

}


CFace *split_edge_of_insert_vertex(CFace *insert_face,CVertex *v,char type)
{
           int              ver_num;
		   double           lengthedge;
	       vector<CVertex*> lst_ver;
           CHalfEdge *ph,*phead;
            
/*-------------------------------------------------------------------*/	
		   if(type=='D')                //put vertex v on the down edge
		   {
			    int flag=1;
				phead=insert_face->pHalfedge; 
				ph=phead;   
				do{
					
					if(flag==1 && v->m_dVerx > ph->source()->m_dVerx && v->m_dVerx <ph->pVertex->m_dVerx)
					{
                         lst_ver.push_back(v);flag=0;
					}
					lst_ver.push_back(ph->pVertex);
					ph=ph->pNextHalfedge;
				}while(ph!=phead);
				
		   }
		   if(type=='U')                //put vertex v on the UP edge
		   {
			    int flag=0;
				phead=insert_face->pHalfedge; 
				ph=phead;   
				do{
					if(ph->source()->m_nVerid==insert_face->m_nFourIndex[2])
						flag=1;
					if(flag==1 && v->m_dVerx < ph->source()->m_dVerx && v->m_dVerx >ph->pVertex->m_dVerx)
					{
                         lst_ver.push_back(v);flag=0;
					}
					lst_ver.push_back(ph->pVertex);
					ph=ph->pNextHalfedge;
				}while(ph!=phead);
				
		   }
		   if(type=='L')                //put vertex v on the UP edge
		   {
			    int flag=0;
				phead=insert_face->pHalfedge; 
				ph=phead;   
				
				do{
					
					if(flag==0 && ph->source()->m_nVerid==insert_face->m_nFourIndex[3])
						flag=1;
					if(flag==1 && v->m_dVery < ph->source()->m_dVery && v->m_dVery >ph->pVertex->m_dVery)
					{
                         lst_ver.push_back(v);flag=0;
					}
					lst_ver.push_back(ph->pVertex);
					ph=ph->pNextHalfedge;
				}while(ph!=phead);
				if(flag==1 && v->m_dVery < ph->source()->m_dVery && v->m_dVery >ph->pVertex->m_dVery)
				{ 
					lst_ver.push_back(v);
				}
				
				
		   }
		   if(type=='R')                //put vertex v on the UP edge
		   {
			    int flag=0;
				phead=insert_face->pHalfedge; 
				ph=phead;   
				do{
					if(ph->source()->m_nVerid==insert_face->m_nFourIndex[1])
						flag=1;
					if(flag==1 && v->m_dVery > ph->source()->m_dVery && v->m_dVery <ph->pVertex->m_dVery)
					{
                         lst_ver.push_back(v);flag=0;
					}
					lst_ver.push_back(ph->pVertex);
					ph=ph->pNextHalfedge;
				}while(ph!=phead);
				
		   }
			ver_num=lst_ver.size();
			/*cout<<"modyfied face output lst_ver:";
				for(int i=0;i<ver_num;i++)
					cout<<lst_ver[i]->m_nVerid<<" ";
			cout<<"TYPE:"<<type<<" "<<"ver_num:"<<ver_num<<"lst_ver i:"<<" ";*/
/*-------------------------------------------------------------------*/	
			CFace *face = new CFace;
			face->m_nFaceid = insert_face->m_nFaceid; 
			face->m_nFaceLevel = insert_face->m_nFaceLevel;
			face->m_dErr = insert_face->m_dErr;
			face->m_bMark = insert_face->m_bMark;
			face->m_nFourIndex[0]=insert_face->m_nFourIndex[0];
			face->m_nFourIndex[1]=insert_face->m_nFourIndex[1];
			face->m_nFourIndex[2]=insert_face->m_nFourIndex[2];
			face->m_nFourIndex[3]=insert_face->m_nFourIndex[3];
			
			vector<CHalfEdge*> he;
			he.resize(ver_num);
			 for(int i=0;i<ver_num;i++)
			 {
				 he[i]=new CHalfEdge;
				 he[i]->pVertex=lst_ver[i];
				 he[i]->pFace=face;
				 lst_ver[i]->pHalfEdge=he[i];
				/* cout<<lst_ver[i]->m_nVerid<<" ";*/
				// lst_ver[i]->pHalfEdge=he[i];	 ’‚¿Ôµƒ∂•µ„æ˘ «“—æ≠¥Ê‘⁄µƒ£¨÷Æ«∞¥¶¿Ìµƒ ±∫Ú÷∏œÚÀ¸√«µƒ∞Î±ﬂ“—ππΩ®∫√
			 }
             
			 for(int i=0;i<ver_num;i++)
			 {
				 he[i]->pNextHalfedge=he[(i+1)%ver_num];
				 he[i]->pPrevHalfedge=he[(i+ver_num-1)%ver_num];

			 }
			 
			 for(int i=0;i<ver_num;i++)
			 {
				 if(he[i]->pVertex->m_nVerid==face->m_nFourIndex[0])
				 {
					 face->pHalfedge=he[i];
					 break;
				 }
			 }
			 phead=insert_face->pHalfedge; ph=phead; 
			 for(int i=0;i<ver_num;i++)
			 {
				 if(he[i]->pVertex->m_nVerid!=ph->pVertex->m_nVerid||he[i]->source()->m_nVerid!=ph->source()->m_nVerid)
				 {
                     CEdge *pe=new CEdge;
					 lengthedge=sqrt(pow((he[i]->pVertex->m_dVerx - he[i]->source()->m_dVerx),2)+pow((he[i]->pVertex->m_dVery - he[i]->source()->m_dVery),2));
					 pe->m_dLength=lengthedge;
					 pe->m_nTwoVer[0]=he[i]->source()->m_nVerid; pe->m_nTwoVer[1]=he[i]->pVertex->m_nVerid;
					 pe->pLeftHalfEdge=he[i];
					 pe->pRightHalfEdge=NULL;
					 he[i]->pEdge=pe;
					 if(he[i]->source()->m_nVerid!=ph->source()->m_nVerid)
						 ph=ph->pNextHalfedge;

				 }
				 else
				 {
					 CEdge *pe=new CEdge;
					 *pe=*(ph->pEdge);
				     assert(pe->pLeftHalfEdge!=NULL);
					 if(pe->pLeftHalfEdge==ph)
					 {  pe->pLeftHalfEdge=he[i]; 
					 if(ph->dual()!=NULL)
						 ph->dual()->pEdge=pe;
					 }
					 if(pe->pRightHalfEdge==ph)
					 {	 pe->pRightHalfEdge=he[i]; ph->dual()->pEdge=pe;
					 }
					 he[i]->pEdge=pe;
					 ph=ph->pNextHalfedge;
				 }
			 }
			return face;
	
}


char Mesh_InsertTPoint(CMesh &M, CFace insert_face, CVertex T_vertex)
{
	 
	 char       type;
	 CVertex    LDVertex,RUVertex;
     CVertex    *v[2]; 
     CHalfEdge  *ph,*phead;  

	
     phead = insert_face.pHalfedge;
	 ph = phead;
	 do 
	 {
		 if(ph->pVertex->m_nVerid == insert_face.m_nFourIndex[0])
           LDVertex = *(ph->pVertex);
		 if(ph->pVertex->m_nVerid == insert_face.m_nFourIndex[2])
		   RUVertex = *(ph->pVertex);
        ph = ph->pNextHalfedge;

	 } while (ph != phead);

	/* LDVertex=getVertex(M.m_Vertices,insert_face.m_nFourIndex[0]);
	 RUVertex=getVertex(M.m_Vertices,insert_face.m_nFourIndex[2]);*/
	// cout<<"----------- Insert T vertex------ "<<T_vertex.m_nVerid<<" "<<T_vertex.m_dVerx<<" "<<T_vertex.m_dVery<<endl;
	// cout<<LDVertex.m_dVerx<<" "<<LDVertex.m_dVery<<" "<<RUVertex.m_dVerx<<" "<<RUVertex.m_dVery<<endl;


	 v[0] = new CVertex;
	 v[1] = new CVertex;
	 if(T_vertex.m_dVerx > LDVertex.m_dVerx && T_vertex.m_dVerx < RUVertex.m_dVerx)
	 {
		 if(T_vertex.m_dVery > LDVertex.m_dVery)//Àµ√˜¥ÀTµ„‘⁄…œ±ﬂ…�?
		 {
			     v[0]->pHalfEdge=NULL;             v[0]->m_nVerid = M.m_Vertices.size();
				 v[0]->m_dVerx=T_vertex.m_dVerx;   v[0]->m_dVery = LDVertex.m_dVery;
				 v[1]->pHalfEdge=NULL;             v[1]->m_nVerid = M.m_Vertices.size();
				 v[1]->m_dVerx=T_vertex.m_dVerx;   v[1]->m_dVery = T_vertex.m_dVery;
		 }
		 else
		 {
               v[1]->pHalfEdge=NULL; v[1]->m_nVerid=M.m_Vertices.size();
			   v[1]->m_dVerx=T_vertex.m_dVerx;   v[1]->m_dVery=RUVertex.m_dVery;
			   v[0]->pHalfEdge=NULL; v[0]->m_nVerid=M.m_Vertices.size();
			   v[0]->m_dVerx=T_vertex.m_dVerx;   v[0]->m_dVery=T_vertex.m_dVery;
		 }
		//cout<<"insertV is dealed:"<<endl;  //Mesh_InsertV(M,insert_face,v);
		type='V';
		Insert_Mesh_V(M,insert_face,v);
	 }
	 if(T_vertex.m_dVery > LDVertex.m_dVery && T_vertex.m_dVery < RUVertex.m_dVery)
	 {
		 if(T_vertex.m_dVerx > LDVertex.m_dVerx)//Àµ√˜¥ÀTµ„‘⁄right±ﬂ…�?
		 {
			     v[0]->pHalfEdge=NULL; v[0]->m_nVerid=M.m_Vertices.size();
				 v[0]->m_dVerx=LDVertex.m_dVerx;   v[0]->m_dVery=T_vertex.m_dVery;
				 v[1]->pHalfEdge=NULL; v[1]->m_nVerid=M.m_Vertices.size();
				 v[1]->m_dVerx=T_vertex.m_dVerx;   v[1]->m_dVery=T_vertex.m_dVery;
		 }
		 else
		 {
                 v[1]->pHalfEdge=NULL; v[1]->m_nVerid=M.m_Vertices.size();
				 v[1]->m_dVerx=RUVertex.m_dVerx;   v[1]->m_dVery=T_vertex.m_dVery;
				 v[0]->pHalfEdge=NULL; v[0]->m_nVerid=M.m_Vertices.size();
				 v[0]->m_dVerx=T_vertex.m_dVerx;   v[0]->m_dVery=T_vertex.m_dVery;
		 }
		 //cout<<"insertH is dealed:"<<endl; //Mesh_InsertH(M,insert_face,v);
		 type='H';
		 Insert_Mesh_H(M,insert_face,v);
	 }
	 return type;
	
}
/*------------------------------------------------------------------------*/
/*≤Â»Î“ªÃı±ﬂ£¨–Œ≥…µƒÕ¯∏Ò;»Áπ˚≤ª∏˘æ›“—÷™µƒÕ¯∏Ò≤Â»Î±ﬂªπ±»Ωœƒ—∏„£�?/
/* ◊œ»»∑∂®≥ˆ‘ı√¥≤Â»Î±ﬂµƒ¿‡–Õ£¨»Á «“™ƒ≥∏ˆ∑ΩœÚ…œµ»∑÷’‚∏ˆ∞¸«ªªÚ’ﬂ «∏˘æ›Õ¯∏Ò÷�?
  µƒƒ≥“ª∏ˆ¥Ê‘⁄µƒTµ„—”Õ�?------------*/
/*-----------------------------“ª∞Î≤Â»Î£¨∆ ∑÷∞¸«ª-----------------------------------------*/

void Mesh_InsertHalfHV(CMesh &M, CFace insert_face,char insert_type)
{
     CVertex *v[2];
	 CVertex LDVertex,RUVertex;
	 int Mnum = M.m_Vertices.size(); //cout<<"insert before mesh num:"<<Mnum<<endl;
	 
	 v[0]=new CVertex;
	 v[1]=new CVertex;
	
	 LDVertex = M.m_Vertices[insert_face.m_nFourIndex[0]];
	 RUVertex = M.m_Vertices[insert_face.m_nFourIndex[2]];
	 
	 if(insert_type=='V')
	 {
		 //cout<<"insertV is dealed:"<<endl;

		 v[0]->m_dVerx=0.5*(LDVertex.m_dVerx+RUVertex.m_dVerx);
		 v[0]->m_dVery=LDVertex.m_dVery;
		 v[0]->m_nVerid=Mnum;
		 v[0]->pHalfEdge=NULL;
		 v[1]->m_dVerx=0.5*(LDVertex.m_dVerx+RUVertex.m_dVerx);
		 v[1]->m_dVery=RUVertex.m_dVery;
		 v[1]->m_nVerid=Mnum+1;
		 v[1]->pHalfEdge=NULL;

	   //  Mesh_InsertV(M,insert_face,v);
		 Insert_Mesh_V(M,insert_face,v);
	 }
	 if(insert_type=='H')
	 {
		 //cout<<"insertH is dealed:"<<endl;
         
		 v[0]=new CVertex; 
		 v[1]=new CVertex;

		 v[0]->m_dVery=0.5*(LDVertex.m_dVery+RUVertex.m_dVery);   v[1]->m_dVery=0.5*(LDVertex.m_dVery+RUVertex.m_dVery);
		 v[0]->m_dVerx=LDVertex.m_dVerx;                    v[1]->m_dVerx=RUVertex.m_dVerx;
		 v[0]->m_nVerid=Mnum;                  v[1]->m_nVerid=Mnum+1;
		 v[0]->pHalfEdge=NULL;                  v[1]->pHalfEdge=NULL;
		
		 //Mesh_InsertH(M,insert_face,v);
		 Insert_Mesh_H(M,insert_face,v);
	 }

}

void Mesh_Insert_Edge(CMesh &M,CFace insert_face,char insert_type,double val)
{

     CVertex     *v[2];
	 CVertex     LDVertex,RUVertex;
	 
	 int Mnum = M.m_Vertices.size();     cout<<"initial mesh m_Faces output:"<<" ";
	
	
	 v[0]=new CVertex;
	 v[1]=new CVertex;
	 
	 LDVertex = M.m_Vertices[insert_face.m_nFourIndex[0]];
	 RUVertex = M.m_Vertices[insert_face.m_nFourIndex[2]];
	 
	 if(insert_type=='V')
	 {
		 cout<<"insertV,insert value is dealed:"<<endl;
		 v[0]->m_dVerx=val*LDVertex.m_dVerx+(1.0-val)*RUVertex.m_dVerx;
		 v[0]->m_dVery=LDVertex.m_dVery;
		 v[0]->m_nVerid=Mnum;
		 v[0]->pHalfEdge=NULL;
		 v[1]->m_dVerx=val*LDVertex.m_dVerx+(1.0-val)*RUVertex.m_dVerx;
		 v[1]->m_dVery=RUVertex.m_dVery;
		 v[1]->m_nVerid=Mnum+1;
		 v[1]->pHalfEdge=NULL;
	   //  Mesh_InsertV(M,insert_face,v);
		 Insert_Mesh_V(M,insert_face,v);
	 }
	 if(insert_type=='H')
	 {
		 cout<<"insertH is dealed:"<<endl;
         v[0]=new  CVertex;
		 v[1]=new  CVertex;
		 v[0]->m_dVery=val*LDVertex.m_dVery+(1.0-val)*RUVertex.m_dVery;   v[1]->m_dVery=val*LDVertex.m_dVery+(1.0-val)*RUVertex.m_dVery;
		 v[0]->m_dVerx=LDVertex.m_dVerx;                            v[1]->m_dVerx=RUVertex.m_dVerx;
		 v[0]->m_nVerid=Mnum;                         v[1]->m_nVerid=Mnum+1;
		 v[0]->pHalfEdge=NULL;                        v[1]->pHalfEdge=NULL;
		//Mesh_InsertH(M,insert_face,v);
		 Insert_Mesh_H(M,insert_face,v);
	 }

}

/*-----------------------------------------------------------------------------------*/
//insert a cross in face m_Insertface£¨reconstruct the mesh again
/*-----------------------------------------------------------------------------------*/
void CrossInsertMesh(CFace m_Insertface,vector<Gvertex> &gves,vector<Gface> &gfaces,CMesh &M)
{
	int id[5]={-1,-1,-1,-1,-1};
	double dmidx,dmidy;
	Gvertex gv;
	Gface gf;
    CHalfEdge *phead,*ph;

	int m = (int)gfaces.size();
	int n = (int)gves.size();
   phead = m_Insertface.pHalfedge;
   while (phead->pVertex->m_nVerid != m_Insertface.m_nFourIndex[0])//make phead point to left down corner
	   phead = phead->pNextHalfedge;

   
   for (int i=0;i<4;i++)
   {
       gv.m_dGVx = phead->pVertex->m_dVerx;
       gv.m_dGVy = phead->pVertex->m_dVery;
	   ph = phead;
	   while (phead->pVertex->m_nVerid != m_Insertface.m_nFourIndex[(i+1)%4])//make phead point to left down corner
		   phead = phead->pNextHalfedge;

	   if( i%2 == 0 )
	   {
		   gv.m_dGVx += phead->pVertex->m_dVerx;
		   gv.m_dGVx *=0.5;
		   dmidx = gv.m_dGVx;                   // cout<<i<<"dmidx:"<<" "<<dmidx<<endl;
		   while (ph != phead)
		   {
			   if(fabs( ph->pVertex->m_dVerx - dmidx) < TOLERANCE)
			   {
				   id[i] = ph->pVertex->m_nVerid;
				   break;
			   }
			   ph = ph->pNextHalfedge;
		   }
	   }
	   else
	   {
		   gv.m_dGVy += phead->pVertex->m_dVery;
		   gv.m_dGVy *=0.5;
		   dmidy = gv.m_dGVy;             // cout<<i<<"dmidy:"<<" "<<dmidy<<endl;
		   while (ph != phead)
		   {
			   if(fabs( ph->pVertex->m_dVery - dmidy) < TOLERANCE)
			   {
				   id[i] = ph->pVertex->m_nVerid;
				   break;
			   }
			   ph = ph->pNextHalfedge;
		   }
	   }

         if (-1 == id[i])//the midpoint does not exit
         {
			 gv.m_nGvid = n;
			 id[i] = n;
			 gves.push_back(gv);
			 n++; 
         }
		
		 
   }
   
   gv.m_nGvid = n;
   gv.m_dGVx = dmidx;
   gv.m_dGVy = dmidy;
   id[4] = n;
   gves.push_back(gv);
   
	gfaces[m_Insertface.m_nFaceid].m_nGver[0]=m_Insertface.m_nFourIndex[0];
	gfaces[m_Insertface.m_nFaceid].m_nGver[1]=id[0];
	gfaces[m_Insertface.m_nFaceid].m_nGver[2]=id[4];
	gfaces[m_Insertface.m_nFaceid].m_nGver[3]=id[3];

	gf.m_nGfid = m;
	gf.m_nGver[0]=id[0];
	gf.m_nGver[1]=m_Insertface.m_nFourIndex[1];
	gf.m_nGver[2]=id[1];
	gf.m_nGver[3]=id[4];
    m++;
	gfaces.push_back(gf);
    
	gf.m_nGfid = m;
	gf.m_nGver[0]=id[3];
	gf.m_nGver[1]=id[4];
	gf.m_nGver[2]=id[2];
	gf.m_nGver[3]=m_Insertface.m_nFourIndex[3];
	m++;
	gfaces.push_back(gf);
	
	gf.m_nGfid = m;
	gf.m_nGver[0]=id[4];
	gf.m_nGver[1]=id[1];
	gf.m_nGver[2]=m_Insertface.m_nFourIndex[2];
	gf.m_nGver[3]=id[2];
	gfaces.push_back(gf);

	/*for (int i=0;i<5;++i)
		cout<<"five new point:"<<id[i]<<" ";*/
	M.GeneratTMesh(gves,gfaces);

}

void InsertMeshHA(CFace m_Insertface,vector<Gvertex> &gves,vector<Gface> &gfaces,CMesh &M)
{
	int id[2]={-1,-1};
	double  dmidy;
	Gvertex gv;
	Gface gf;
	CHalfEdge *phead,*ph;

	int m = (int)gfaces.size();
	int n = (int)gves.size();
	phead = m_Insertface.pHalfedge;
	while (phead->pVertex->m_nVerid != m_Insertface.m_nFourIndex[1])//make phead point to left down corner
		phead = phead->pNextHalfedge;


	for (int i=0;i<3;i+=2)
	{
		gv.m_dGVx = phead->pVertex->m_dVerx;
		gv.m_dGVy = phead->pVertex->m_dVery;
		ph = phead;
		while (phead->pVertex->m_nVerid != m_Insertface.m_nFourIndex[(i+2)%4])//make phead point to left down corner
			phead = phead->pNextHalfedge;
		{
			gv.m_dGVy += phead->pVertex->m_dVery;
			gv.m_dGVy *=0.5;
			dmidy = gv.m_dGVy;              cout<<i<<"dmidy:"<<" "<<dmidy<<endl;
			while (ph != phead)
			{
				if(fabs( ph->pVertex->m_dVery - dmidy) < TOLERANCE)
				{
					id[i/2] = ph->pVertex->m_nVerid;
					break;
				}
				ph = ph->pNextHalfedge;
			}
		}

		if (-1 == id[i/2])//the midpoint does not exit
		{
			gv.m_nGvid = n;
			id[i/2] = n;
			gves.push_back(gv);
			n++; 
		}

		while (phead->pVertex->m_nVerid != m_Insertface.m_nFourIndex[(i+3)%4])//make phead point to left down corner
			phead = phead->pNextHalfedge;

	}

	gfaces[m_Insertface.m_nFaceid].m_nGver[0]=m_Insertface.m_nFourIndex[0];
	gfaces[m_Insertface.m_nFaceid].m_nGver[1]=m_Insertface.m_nFourIndex[1];
	gfaces[m_Insertface.m_nFaceid].m_nGver[2]=id[0];
	gfaces[m_Insertface.m_nFaceid].m_nGver[3]=id[1];

	gf.m_nGfid = m;
	gf.m_nGver[0]=id[1];
	gf.m_nGver[1]=id[0];
	gf.m_nGver[2]=m_Insertface.m_nFourIndex[2];
	gf.m_nGver[3]=m_Insertface.m_nFourIndex[3];
	
	gfaces.push_back(gf);

	for (int i=0;i<2;++i)
		cout<<"five new point:"<<id[i]<<" ";
	M.GeneratTMesh(gves,gfaces);
	cout<<endl;

}


void InsertMeshVA(CFace m_Insertface,vector<Gvertex> &gves,vector<Gface> &gfaces,CMesh &M)
{
	int id[2]={-1,-1};
	double dmidx;
	Gvertex gv;
	Gface gf;
	CHalfEdge *phead,*ph;

	int m = (int)gfaces.size();
	int n = (int)gves.size();
	phead = m_Insertface.pHalfedge;
	while (phead->pVertex->m_nVerid != m_Insertface.m_nFourIndex[0])//make phead point to left down corner
		phead = phead->pNextHalfedge;


	for (int i=0;i<3;i+=2)
	{
		gv.m_dGVx = phead->pVertex->m_dVerx;
		gv.m_dGVy = phead->pVertex->m_dVery;
		ph = phead;
		while (phead->pVertex->m_nVerid != m_Insertface.m_nFourIndex[(i+1)%4])//make phead point to left down corner
			phead = phead->pNextHalfedge;
		{
			gv.m_dGVx += phead->pVertex->m_dVerx;
			gv.m_dGVx *=0.5;
			dmidx = gv.m_dGVx;                    cout<<i<<"dmidx:"<<" "<<dmidx<<endl;
			while (ph != phead)
			{
				if(fabs( ph->pVertex->m_dVerx - dmidx) < TOLERANCE)
				{
					id[i/2] = ph->pVertex->m_nVerid;
					break;
				}
				ph = ph->pNextHalfedge;
			}
		}

		if (-1 == id[i/2])//the midpoint does not exit
		{
			gv.m_nGvid = n;
			id[i/2] = n;
			gves.push_back(gv);
			n++; 
		}

		while (phead->pVertex->m_nVerid != m_Insertface.m_nFourIndex[(i+2)%4])//make phead point to left down corner
			phead = phead->pNextHalfedge;

	}

	gfaces[m_Insertface.m_nFaceid].m_nGver[0]=m_Insertface.m_nFourIndex[0];
	gfaces[m_Insertface.m_nFaceid].m_nGver[1]=id[0];
	gfaces[m_Insertface.m_nFaceid].m_nGver[2]=id[1];
	gfaces[m_Insertface.m_nFaceid].m_nGver[3]=m_Insertface.m_nFourIndex[3];

	gf.m_nGfid = m;
	gf.m_nGver[0]=id[0];
	gf.m_nGver[1]=m_Insertface.m_nFourIndex[1];
	gf.m_nGver[2]=m_Insertface.m_nFourIndex[2];
	gf.m_nGver[3]=id[1];

	gfaces.push_back(gf);

	for (int i=0;i<2;++i)
		cout<<"five new point:"<<id[i]<<" ";
	M.GeneratTMesh(gves,gfaces);
	cout<<endl;

}


/************************************************************************/
/* Useful functions for Hierarchical Bs-plines                                                                    
/************************************************************************/
int Tver_FaceExtension(CVertex findv, CHalfEdge *directionhe) //for any given T-mesh
{

	int       n = 0;
	bool      flag = false;
	CHalfEdge *phead,*ph;


	if(findv.m_cVerType == 'U')
		n=2; 
	else if(findv.m_cVerType == 'D')
		n=0; 
	else if(findv.m_cVerType == 'L')
		n=3; 
	else
		n=1;  

	if(n==3 || n==1)
	{
		phead = directionhe->pNextHalfedge->dual();
		ph=phead;
		do{
			if( ph->pVertex->m_nVerid == phead->pFace->m_nFourIndex[n] )
				break;
			ph = ph->pNextHalfedge;

		}while(ph!=phead); 

		ph = ph->pNextHalfedge;
		if( fabs(ph->pVertex->m_dVery - findv.m_dVery )< TOLERANCE)
			return  ph->pVertex->m_nVerid;
		if ( 1 == n)
		{
			do{
				if( ph->pVertex->m_dVery > findv.m_dVery ) //”ˆµΩµ⁄“ª∏ˆ¥Û”⁄’‚∏ˆT-µ„µƒ£¨ƒ«√¥’‚∏ˆ√Ê≤≈ «œ¬“ª∏ˆ≤È’“µƒ√Ê
				{
					if( ph->dual() != NULL)   
						phead = ph->dual();
					else                              //»Áπ˚”ˆµΩµƒ «±ﬂΩÁ±�?≤ª–Ë“™ºÃ–¯’“œ¬“ª∏ˆface¡À
						flag = true;
					break;
				}

				ph=ph->pNextHalfedge;

			}while( ph->pVertex->m_nVerid != ph->pFace->m_nFourIndex[n+2] );
		}
		else
		{
			do{
				if( ph->pVertex->m_dVery < findv.m_dVery ) //”ˆµΩµ⁄“ª∏ˆxiao”⁄’‚∏ˆT-µ„µƒ£¨ƒ«√¥’‚∏ˆ√Ê≤≈ «œ¬“ª∏ˆ≤È’“µƒ√Ê
				{
					if( ph->dual() != NULL)   
						phead = ph->dual();
					else                              //»Áπ˚”ˆµΩµƒ «±ﬂΩÁ±�?≤ª–Ë“™ºÃ–¯’“œ¬“ª∏ˆface¡À
						flag = true;
					break;
				}
				ph=ph->pNextHalfedge;
			}while( ph->pVertex->m_nVerid != ph->pFace->m_nFourIndex[(n+2)%4] );

		}
		if (!flag)
		{
			ph=phead;
			do{
				if( ph->pVertex->m_nVerid == phead->pFace->m_nFourIndex[n] )
					break;
				ph = ph->pNextHalfedge;

			}while(ph!=phead); 

			ph = ph->pNextHalfedge;
			if( fabs(ph->pVertex->m_dVery - findv.m_dVery )< TOLERANCE)
				return  ph->pVertex->m_nVerid;
		}


	}
	else
	{
		phead = directionhe->pNextHalfedge->dual();
		ph=phead;
		do{
			if( ph->pVertex->m_nVerid == phead->pFace->m_nFourIndex[n] )
				break;
			ph = ph->pNextHalfedge;

		}while(ph!=phead);   

		ph = ph->pNextHalfedge;

		if( fabs(ph->pVertex->m_dVerx - findv.m_dVerx )< TOLERANCE)
			return  ph->pVertex->m_nVerid;
		if ( 2 == n)
		{
			do{
				if( ph->pVertex->m_dVerx < findv.m_dVerx ) 
				{
					if( ph->dual() != NULL)   
						phead = ph->dual();
					else                              //»Áπ˚”ˆµΩµƒ «±ﬂΩÁ±�?≤ª–Ë“™ºÃ–¯’“œ¬“ª∏ˆface¡À
						flag = true;
					break;
				}

				ph=ph->pNextHalfedge;

			}while( ph->pVertex->m_nVerid != ph->pFace->m_nFourIndex[0] );
		}
		else
		{
			do{
				if( ph->pVertex->m_dVerx > findv.m_dVerx ) //”ˆµΩµ⁄“ª∏ˆxiao”⁄’‚∏ˆT-µ„µƒ£¨ƒ«√¥’‚∏ˆ√Ê≤≈ «œ¬“ª∏ˆ≤È’“µƒ√Ê
				{
					if( ph->dual() != NULL)   
						phead = ph->dual();
					else                              //»Áπ˚”ˆµΩµƒ «±ﬂΩÁ±�?≤ª–Ë“™ºÃ–¯’“œ¬“ª∏ˆface¡À
						flag = true;
					break;
				}
				ph=ph->pNextHalfedge;
			}while( ph->pVertex->m_nVerid != ph->pFace->m_nFourIndex[(n+2)%4] );

		}
		if (!flag)
		{
			ph=phead;
			do{
				if( ph->pVertex->m_nVerid == phead->pFace->m_nFourIndex[n] )
					break;
				ph = ph->pNextHalfedge;

			}while(ph!=phead);   

			ph = ph->pNextHalfedge;
			if( fabs(ph->pVertex->m_dVerx - findv.m_dVerx )< TOLERANCE)
				return  ph->pVertex->m_nVerid;
		}

	}
	return  -1;
}


void  SubdividCell(CMesh &M,int fid)
{

	int         level = M.m_Faces[fid].m_nFaceLevel;
	int         Nf = (int)M.m_Faces.size();
	int         Nv = (int)M.m_Vertices.size();

	Mesh_InsertHalfHV(M,M.m_Faces[fid],'H');
	Mesh_InsertHalfHV(M,M.m_Faces[fid],'V');
	Mesh_InsertHalfHV(M,M.m_Faces[Nf],'V');  
	cout<<"-----Refine---"<<M.m_Vertices.size()<<endl;


	for (int i=Nv;i < (int)M.m_Vertices.size();++i)
		M.m_Vertices[i].m_nvLevel = level+1;


	M.m_Faces[fid].m_nFaceLevel = level+1;
	M.m_Faces[Nf].m_nFaceLevel = level+1;
	M.m_Faces[Nf+1].m_nFaceLevel = level+1;
	M.m_Faces[Nf+2].m_nFaceLevel = level+1;

}

void CrossRefineHierarMesh(CMesh &M, int fid)
{	
 //   vector<int>    neighborcell;

	//Find_Adjacent_Cell(M.m_Faces[fid],neighborcell);
	//
	//for (int i=0;i < (int)neighborcell.size();++i)
	//{
	//	/*cout<<endl<<" level "<<M.m_nMeshLevel<<" "<<fid<<" "<<neighborcell[i]<<" "
	//		<<M.m_Faces[neighborcell[i]].m_nFaceLevel<<" "<<M.m_Faces[fid].m_nFaceLevel;*/
	//	if (M.m_Faces[neighborcell[i]].m_nFaceLevel < M.m_Faces[fid].m_nFaceLevel)
	//	          return;
	//}
	//cout<<endl;


	int   Nf = (int)M.m_Faces.size();
	int   Nv = (int)M.m_Vertices.size();
	int   level = M.m_Faces[fid].m_nFaceLevel;
   

	Mesh_InsertHalfHV(M,M.m_Faces[fid],'H');
	Mesh_InsertHalfHV(M,M.m_Faces[fid],'V');
	Mesh_InsertHalfHV(M,M.m_Faces[Nf], 'V');  //cout<<"-----Refine---"<<M.m_Vertices.size()<<endl;
   

	for (int i=Nv;i < (int)M.m_Vertices.size();++i)
	     M.m_Vertices[i].m_nvLevel = level+1;

	M.m_Faces[fid].m_nFaceLevel = level+1;
    M.m_Faces[Nf].m_nFaceLevel =  level+1;
	M.m_Faces[Nf+1].m_nFaceLevel = level+1;
	M.m_Faces[Nf+2].m_nFaceLevel = level+1;

	M.m_Faces[fid].IsSubdivid = 1;
	
			
}


void find_vertices_in_subcell(CMesh &M,CHalfEdge *ph,vector<int> &vid)
{
	int        k;
	int        fid[4];

	fid[0] = ph->pFace->m_nFaceid;
	fid[1] = ph->pNextHalfedge->pNextHalfedge->dual()->pFace->m_nFaceid;
	fid[2] = ph->pPrevHalfedge->pFace->m_nFaceid;
	fid[3] = ph->pNextHalfedge->pNextHalfedge->dual()->pPrevHalfedge->dual()->pFace->m_nFaceid;

	for (int i=0;i < 4;++i)
	{
		for (int j=0;j < 4;++j)
		{
			k = M.m_Faces[fid[i]].m_nFourIndex[j];
			if (!M.m_Vertices[k].m_bDealWith)
			{
				vid.push_back(k);
				M.m_Vertices[k].m_bDealWith = true;
			}
		}
	}

}

void Find_Adjacent_Cell(CFace f,vector<int> &fid)
{

	int        n = 0;
	CHalfEdge  *phead = f.pHalfedge;
	CHalfEdge  *ph = phead;

	do 
	{
		if (ph->dual() != NULL)
		{
			fid.push_back(ph->dual()->pFace->m_nFaceid);
			//������������
            /*cout<<ph->pVertex->m_dVerx<<" "<<ph->pVertex->m_dVery<<" "<<ph->pVertex->VertexType()<<endl;
            cout<<ph->pNextHalfedge->pVertex->m_dVerx<<" "<<ph->pVertex->m_dVery<<" "<<ph->pNextHalfedge->pVertex->VertexType()<<endl;*/
			if (ph->pVertex->m_nVerid == f.m_nFourIndex[n] )
			{
				//����˵0~8��3*3�棬�������²����������ܶ��޷�������棬��ʱע�͵�
				/*if( 'C' == ph->pVertex->VertexType() )
					fid.push_back(ph->dual()->pPrevHalfedge->dual()->pFace->m_nFaceid);*/
				n++;
			}
		}

		ph =ph->pNextHalfedge;
	} while (ph != phead);

}


//-----------------------------my new function-byQQ-----------------------------------------
char Is_Neighbor(CMesh &M, CFace &f1, CFace &f2)
{
    CHalfEdge  *phead = f1.pHalfedge;
    CHalfEdge  *ph = phead;
    char mychar = 'E';
    do
    {
        if (ph->dual() != NULL)
        {
            // cout<<ph->dual()->pFace->m_nFaceid<<endl;
            if (ph->dual()->pFace->m_nFaceid == f2.pHalfedge->pFace->m_nFaceid)
            {
                if (M.m_Vertices[f1.m_nFourIndex[0]].m_dVery == M.m_Vertices[f2.m_nFourIndex[0]].m_dVery && M.m_Vertices[f1.m_nFourIndex[2]].m_dVery == M.m_Vertices[f2.m_nFourIndex[2]].m_dVery)
                {
                    mychar ='H';
                }
                else if (M.m_Vertices[f1.m_nFourIndex[0]].m_dVerx == M.m_Vertices[f2.m_nFourIndex[0]].m_dVerx && M.m_Vertices[f1.m_nFourIndex[2]].m_dVerx == M.m_Vertices[f2.m_nFourIndex[2]].m_dVerx)
                {
                    mychar ='V';
                }
            }
            
        }
        
        ph =ph->pNextHalfedge;
    } while (ph != phead);
    return mychar ;
    
}





void Unidirectional_Refine(CMesh &M, int fid, char insert_type )
{
    int           Nf = (int)M.m_Faces.size();
    int           Nv = (int)M.m_Vertices.size();
    int           level = M.m_Faces[fid].m_nFaceLevel; 
    
    
    if('H' == insert_type)
    {
        Mesh_InsertHalfHV(M, M.m_Faces[fid],'H');
        for (int i=Nv; i < (int)M.m_Vertices.size(); ++i)
        {
            M.m_Vertices[i].m_nvLevel = level+1;
        }
        
        M.m_Faces[fid].m_nFaceLevel = level+1;
        M.m_Faces[Nf].m_nFaceLevel =  level+1;        
    }
    
    if('V' == insert_type)
    {        
        Mesh_InsertHalfHV(M,M.m_Faces[fid],'V');
        for (int i=Nv; i < (int)M.m_Vertices.size(); ++i)
        {
            M.m_Vertices[i].m_nvLevel = level+1;
        }
        
        M.m_Faces[fid].m_nFaceLevel = level+1;
        M.m_Faces[Nf].m_nFaceLevel =  level+1;
    }	
    
}

void Unidirectional_Refine_new(CMesh &M, int fid, char insert_type )
{
    int           Nf = (int)M.m_Faces.size();
    int           Nv = (int)M.m_Vertices.size();
    int           level = M.m_Faces[fid].m_nFaceLevel; 
    
    
    if('H' == insert_type)
    {
        Mesh_InsertHalfHV(M, M.m_Faces[fid],'H');
        for (int i=Nv; i < (int)M.m_Vertices.size(); ++i)
        {
            M.m_Vertices[i].m_nvLevel = level;
        }
        
        M.m_Faces[fid].m_nFaceLevel = level;
        M.m_Faces[Nf].m_nFaceLevel =  level;        
    }
    
    if('V' == insert_type)
    {        
        Mesh_InsertHalfHV(M,M.m_Faces[fid],'V');
        for (int i=Nv; i < (int)M.m_Vertices.size(); ++i)
        {
            M.m_Vertices[i].m_nvLevel = level;
        }
        
        M.m_Faces[fid].m_nFaceLevel = level;
        M.m_Faces[Nf].m_nFaceLevel =  level;
    }	
    
}


/*void myRefine_QQ(CMesh &M, vector<int> &vid)
{
    vector<int>::iterator i;
    vector<int> vid_half;
	vid_half.clear ();
	int Nf;
    
    for(i=vid.begin();i!=vid.end();i++)
    {
        M.m_Faces[*i].flag_H = 0;
        M.m_Faces[*i].flag_V = 0;
    }  
///////////////////////////////1.give H/V mark according to curvature/////////////////////////    
	for (int i = 0; i < vid.size(); ++i)
	{   
		if(M.m_Faces[vid[i]].m_bAnisoU==1&&M.m_Faces[vid[i]].m_bAnisoV==0)
		{
			M.m_Faces[vid[i]].flag_H = 0;
			M.m_Faces[vid[i]].flag_V = 1;
			
		}
		else if(M.m_Faces[vid[i]].m_bAnisoU==0&&M.m_Faces[vid[i]].m_bAnisoV==1)
		{
			M.m_Faces[vid[i]].flag_H = 1;
			M.m_Faces[vid[i]].flag_V = 0;
			
		}
		
		else if(M.m_Faces[vid[i]].m_bAnisoU==0&&M.m_Faces[vid[i]].m_bAnisoV==0)
		{
			M.m_Faces[vid[i]].flag_H = 1;
			M.m_Faces[vid[i]].flag_V = 1;
			
		}
		else continue;
	}*/
	////////////////////////////2.give H/V mark for iso cell//////////////////////////
	/*for (int i = 0; i < vid.size(); ++i)
	{
		if(M.m_Faces[vid[i]].m_bAnisoU==0&&M.m_Faces[vid[i]].m_bAnisoV==0)
		{
			for (int j = 0; j < vid.size(); ++j )
			{
				char flag=Is_Neighbor(M,M.m_Faces[vid[i]], M.m_Faces[vid[j]]);
				if (M.m_Faces[vid[i]].m_nFaceLevel == M.m_Faces[vid[j]].m_nFaceLevel)
				{
					if( flag == 'H' &&  M.m_Faces[vid[j]].flag_H ==1)
					{
						M.m_Faces[vid[i]].flag_H = 1;
					}
					if( flag == 'V' &&  M.m_Faces[vid[j]].flag_V ==1)
					{
						M.m_Faces[vid[i]].flag_V = 1;
					}
					if( flag == 'H' &&  M.m_Faces[vid[j]].m_bAnisoU==0&&M.m_Faces[vid[j]].m_bAnisoV==0)
					{
						M.m_Faces[vid[i]].flag_H = 1;
						M.m_Faces[vid[j]].flag_H = 1;
					}
					if( flag == 'V' &&  M.m_Faces[vid[j]].m_bAnisoU==0&&M.m_Faces[vid[j]].m_bAnisoV==0)
					{
						M.m_Faces[vid[i]].flag_V = 1;
						M.m_Faces[vid[j]].flag_V = 1;
					}

				}
			}
		}
	}*/
/////////////////////////// 3.give H/V mark according to neighbour cell//////////////////////////    
	/*for (int i = 0; i < vid.size(); ++i)
	{
		for (int j = 0; j < vid.size(); ++j )
		{
			char flag=Is_Neighbor(M,M.m_Faces[vid[i]], M.m_Faces[vid[j]]);
			if (M.m_Faces[vid[i]].m_nFaceLevel == M.m_Faces[vid[j]].m_nFaceLevel)
			 {
				 if( flag == 'H' && M.m_Faces[vid[i]].flag_H + M.m_Faces[vid[j]].flag_H ==1)
				 {
					 M.m_Faces[vid[i]].flag_H = 1;
                     M.m_Faces[vid[j]].flag_H = 1;
				 }
				 if( flag == 'V' && M.m_Faces[vid[i]].flag_V + M.m_Faces[vid[j]].flag_V ==1)
				 {
					 M.m_Faces[vid[i]].flag_V = 1;
                     M.m_Faces[vid[j]].flag_V = 1;
				 }
			 }
		}
	}*/

////////////////////////////4.subdivide cell according to mark///////////////////////////
   /* for(i=vid.begin();i!=vid.end();i++)
    {
		Nf = (int)M.m_Faces.size(); 
        // cout<<M.m_Faces[*i].flag_H<<" vs "<<M.m_Faces[*i].flag_V<<endl;
        if (M.m_Faces[*i].flag_H == 1 && M.m_Faces[*i].flag_V == 0)
        {
        	Unidirectional_Refine(M, *i,'H');////add level 1
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = Nf;
			M.m_Faces[*i].m_nSiblings[2] = -1;
			M.m_Faces[*i].m_nSiblings[3] = -1;
		}        	
        if (M.m_Faces[*i].flag_H == 0 && M.m_Faces[*i].flag_V == 1)
		{
            Unidirectional_Refine(M, *i,'V');//add level 1
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = -1;
			M.m_Faces[*i].m_nSiblings[2] = Nf;
			M.m_Faces[*i].m_nSiblings[3] = -1;
		}
        if(M.m_Faces[*i].flag_H == 1 && M.m_Faces[*i].flag_V == 1)
		{
            CrossRefineHierarMesh(M, *i);//add level 1
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = Nf;
			M.m_Faces[*i].m_nSiblings[2] = Nf+1;
			M.m_Faces[*i].m_nSiblings[3] = Nf+2;
		}
        if(M.m_Faces[*i].flag_H == 0 && M.m_Faces[*i].flag_V == 0)
        {
            CrossRefineHierarMesh(M, *i);
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = Nf;
			M.m_Faces[*i].m_nSiblings[2] = Nf+1;
			M.m_Faces[*i].m_nSiblings[3] = Nf+2;
		}
    } 
}*/

void myRefine_QQ_1(CMesh &M, vector<int> &vid)
{
    vector<int>::iterator i;
    
    for(i=vid.begin();i!=vid.end();i++)
    {
        M.m_Faces[*i].flag_H = 0;
        M.m_Faces[*i].flag_V = 0;
    }  
    
    
    for (int i = 0; i < vid.size(); ++i)
    {
        for (int j = i+1; j < vid.size(); ++j )
        {
            if (M.m_Faces[vid[i]].m_nFaceLevel == M.m_Faces[vid[j]].m_nFaceLevel)
            {
                if(Is_Neighbor(M,M.m_Faces[vid[i]], M.m_Faces[vid[j]]) == 'H' ||
                   Is_Neighbor(M,M.m_Faces[vid[i]], M.m_Faces[vid[j]]) == 'V')
                {
                    if(Is_Neighbor(M,M.m_Faces[vid[i]], M.m_Faces[vid[j]]) == 'H')
                    {
                        M.m_Faces[vid[i]].flag_H = 1;
                        M.m_Faces[vid[j]].flag_H = 1;
                    }
                    
                    else if(Is_Neighbor(M,M.m_Faces[vid[i]], M.m_Faces[vid[j]]) == 'V')
                    {
                        M.m_Faces[vid[i]].flag_V = 1;
                        M.m_Faces[vid[j]].flag_V = 1;
                    }
                }
            }
        }
    }
    
    
    for(i=vid.begin();i!=vid.end();i++)
    {
		int Nf = (int)M.m_Faces.size(); 
        // cout<<M.m_Faces[*i].flag_H<<" vs "<<M.m_Faces[*i].flag_V<<endl;


        if (M.m_Faces[*i].flag_H == 1 && M.m_Faces[*i].flag_V == 0)
        {
        	Unidirectional_Refine(M, *i,'H');
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = Nf;
			M.m_Faces[*i].m_nSiblings[2] = -1;
			M.m_Faces[*i].m_nSiblings[3] = -1;
        }

        	
        if (M.m_Faces[*i].flag_H == 0 && M.m_Faces[*i].flag_V == 1)
		{
            Unidirectional_Refine(M, *i,'V');
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = -1;
			M.m_Faces[*i].m_nSiblings[2] = Nf;
			M.m_Faces[*i].m_nSiblings[3] = -1;
		}

        if(M.m_Faces[*i].flag_H == 1 && M.m_Faces[*i].flag_V == 1)
		{
            CrossRefineHierarMesh(M, *i);
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = Nf;
			M.m_Faces[*i].m_nSiblings[2] = Nf+1;
			M.m_Faces[*i].m_nSiblings[3] = Nf+2;
		}

        if(M.m_Faces[*i].flag_H == 0 && M.m_Faces[*i].flag_V == 0)
        {
            CrossRefineHierarMesh(M, *i);
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = Nf;
			M.m_Faces[*i].m_nSiblings[2] = Nf+1;
			M.m_Faces[*i].m_nSiblings[3] = Nf+2;
		}

    }   
    
}


void myRefine_QQ_local(CMesh &M, vector<int> &cid)
{
	vector<vector<int>> clas;
	classifycells(M, cid, clas);
	for(int i=0;i<clas.size();i++)
	{
		myRefine_QQ(M, clas[i]);
	}
}


void myRefine_QQ(CMesh &M, vector<int> &vid)
{
	vector<int>::iterator i;
	int Nf;

	for(i=vid.begin();i!=vid.end();i++)
	{
		M.m_Faces[*i].flag_H = 0;
		M.m_Faces[*i].flag_V = 0;
	}  
	///////////////////////////////1.give H/V mark/////////////////////////  
	for (int i = 0; i < vid.size(); ++i)
	{   
		if(M.m_Faces[vid[i]].m_bAnisoU==1)
			M.m_Faces[vid[i]].flag_V = 1;

		 if(M.m_Faces[vid[i]].m_bAnisoV==1)
			M.m_Faces[vid[i]].flag_H = 1;
		 if(M.m_Faces[vid[i]].m_bAnisoU==0&&M.m_Faces[vid[i]].m_bAnisoV==0)
		 {
			 M.m_Faces[vid[i]].flag_H = 1;
			 M.m_Faces[vid[i]].flag_V = 1;
		 }
		else continue;
	}
	/////////////////2.if goto refine 1.0
	int sum[2];
	sum[0]=sum[1]=0;
	for (int i = 0; i < vid.size(); ++i)
	{
		sum[0]+=M.m_Faces[vid[i]].flag_H;
		sum[1]+=M.m_Faces[vid[i]].flag_V;
	}
	///////////3.add basis HV/////////////
	if(sum[0]==0||sum[1]==0)
	{
		for (int i = 0; i < vid.size(); ++i)
		{
			int eh=0;
			int ev=0;
			for (int j = 0; j < vid.size(); ++j)
			{
				char flag=Is_Neighbor(M,M.m_Faces[vid[i]], M.m_Faces[vid[j]]);
				if('E'!=flag)
				{
					if(flag == 'V'&&M.m_Faces[vid[j]].flag_V ==1)
						ev=1;
					else if(flag == 'H'&&M.m_Faces[vid[j]].flag_H ==1)
						eh=1;
				}
			}		
			if(M.m_Faces[vid[i]].flag_V ==1&&M.m_Faces[vid[i]].flag_H == 0&&ev==0)
			{
				if(M.m_Faces[vid[i]].IsBoundaryCell_LRUD()=='V'||M.m_Faces[vid[i]].IsBoundaryCell_LRUD()=='N')
					M.m_Faces[vid[i]].flag_H = 1;
			}
			else if(M.m_Faces[vid[i]].flag_V ==0&&M.m_Faces[vid[i]].flag_H == 1&&eh==0)
			{
				if(M.m_Faces[vid[i]].IsBoundaryCell_LRUD()=='H'||M.m_Faces[vid[i]].IsBoundaryCell_LRUD()=='N')
					M.m_Faces[vid[i]].flag_V = 1;
			}
		}
	}
	/////////////////4.if goto refine 1.0
	sum[0]=sum[1]=0;
	for (int i = 0; i < vid.size(); ++i)
	{
		sum[0]+=M.m_Faces[vid[i]].flag_H;
		sum[1]+=M.m_Faces[vid[i]].flag_V;
	}
	if(sum[0]==0||sum[1]==0)
	{
		/////////////////////////// 3. refine//////////////////////////    
		for(i=vid.begin();i!=vid.end();i++)
		{
			Nf = (int)M.m_Faces.size(); 
			// cout<<M.m_Faces[*i].flag_H<<" vs "<<M.m_Faces[*i].flag_V<<endl;
			if (M.m_Faces[*i].flag_H == 1 && M.m_Faces[*i].flag_V == 0)
			{
				Unidirectional_Refine(M, *i,'H');////add level 1
				M.m_Faces[*i].m_nSiblings[0] = *i;
				M.m_Faces[*i].m_nSiblings[1] = Nf;
				M.m_Faces[*i].m_nSiblings[2] = -1;
				M.m_Faces[*i].m_nSiblings[3] = -1;
			}        	
			else if (M.m_Faces[*i].flag_H == 0 && M.m_Faces[*i].flag_V == 1)
			{
				Unidirectional_Refine(M, *i,'V');//add level 1
				M.m_Faces[*i].m_nSiblings[0] = *i;
				M.m_Faces[*i].m_nSiblings[1] = -1;
				M.m_Faces[*i].m_nSiblings[2] = Nf;
				M.m_Faces[*i].m_nSiblings[3] = -1;
			}
			else if(M.m_Faces[*i].flag_H == 1 && M.m_Faces[*i].flag_V == 1)
			{
				CrossRefineHierarMesh(M, *i);//add level 1
				M.m_Faces[*i].m_nSiblings[0] = *i;
				M.m_Faces[*i].m_nSiblings[1] = Nf;
				M.m_Faces[*i].m_nSiblings[2] = Nf+1;
				M.m_Faces[*i].m_nSiblings[3] = Nf+2;
			}
			else if(M.m_Faces[*i].flag_H == 0 && M.m_Faces[*i].flag_V == 0)
			{
				CrossRefineHierarMesh(M, *i);
				M.m_Faces[*i].m_nSiblings[0] = *i;
				M.m_Faces[*i].m_nSiblings[1] = Nf;
				M.m_Faces[*i].m_nSiblings[2] = Nf+1;
				M.m_Faces[*i].m_nSiblings[3] = Nf+2;
			}
		} 
	}
	else
	myRefine_QQ_1(M, vid);
	//myRefine_QQ_Cross(M,vid);
}


void myRefine_QQ_Cross(CMesh &M, vector<int> &vid)
{
    vector<int>::iterator i;
    
    for(i=vid.begin();i!=vid.end();i++)
    {
        M.m_Faces[*i].flag_H = 1;
        M.m_Faces[*i].flag_V = 1;
    }  
    
    
    
    for(i=vid.begin();i!=vid.end();i++)
    {
		int Nf = (int)M.m_Faces.size(); 
        // cout<<M.m_Faces[*i].flag_H<<" vs "<<M.m_Faces[*i].flag_V<<endl;       

        if(M.m_Faces[*i].flag_H == 1 && M.m_Faces[*i].flag_V == 1)
        {
            CrossRefineHierarMesh(M, *i);
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = Nf;
			M.m_Faces[*i].m_nSiblings[2] = Nf+1;
			M.m_Faces[*i].m_nSiblings[3] = Nf+2;
		}

    }   
    
}

void myMark_QQ(CMesh &M, vector<int> &vid,vector<int> &cvid, vector<int> &hvid, vector<int> &vvid )
{
    vector<int>::iterator i;
    vector<int> vid_half;
	vid_half.clear ();
    
    for(i=vid.begin();i!=vid.end();i++)
    {
        M.m_Faces[*i].flag_H = 0;
        M.m_Faces[*i].flag_V = 0;
    }  
///////////////////////////////1.give H/V mark according to curvature/////////////////////////    
	for (int i = 0; i < vid.size(); ++i)
	{   
		if(M.m_Faces[vid[i]].m_bAnisoU==1&&M.m_Faces[vid[i]].m_bAnisoV==0)
		{
			M.m_Faces[vid[i]].flag_H = 0;
			M.m_Faces[vid[i]].flag_V = 1;
			vvid.push_back(vid[i]);
		}
		else if(M.m_Faces[vid[i]].m_bAnisoU==0&&M.m_Faces[vid[i]].m_bAnisoV==1)
		{
			M.m_Faces[vid[i]].flag_H = 1;
			M.m_Faces[vid[i]].flag_V = 0;
			hvid.push_back(vid[i]);
		}
		
		else if(M.m_Faces[vid[i]].m_bAnisoU==0&&M.m_Faces[vid[i]].m_bAnisoV==0)
		{
			for (int  j = 0; j < vid.size(); ++j )
			{
				char flag=Is_Neighbor(M,M.m_Faces[vid[i]], M.m_Faces[vid[j]]);
				if (M.m_Faces[vid[i]].m_nFaceLevel == M.m_Faces[vid[j]].m_nFaceLevel)
				{
					if( flag == 'H' &&  M.m_Faces[vid[j]].flag_H ==1)
					{
						M.m_Faces[vid[i]].flag_H = 1;
					}
					if( flag == 'H' &&  M.m_Faces[vid[j]].m_bAnisoU==0&&M.m_Faces[vid[j]].m_bAnisoV==0)
					{
						M.m_Faces[vid[i]].flag_H = 1;
						M.m_Faces[vid[j]].flag_H = 1;
					}
					if( flag == 'H' &&  M.m_Faces[vid[j]].flag_V ==1)
					{
						M.m_Faces[vid[i]].flag_H = 1;
						M.m_Faces[vid[i]].flag_V = 1;
					}
					if( flag == 'V' &&  M.m_Faces[vid[j]].flag_V ==1)
					{
						M.m_Faces[vid[i]].flag_V = 1;
					}
					if( flag == 'V' &&  M.m_Faces[vid[j]].m_bAnisoU==0&&M.m_Faces[vid[j]].m_bAnisoV==0)
					{
						M.m_Faces[vid[i]].flag_V = 1;
						M.m_Faces[vid[j]].flag_V = 1;
					}
					if( flag == 'V' &&  M.m_Faces[vid[j]].flag_H ==1)
					{
						M.m_Faces[vid[i]].flag_H = 1;
						M.m_Faces[vid[i]].flag_V = 1;
					}

				}
			}
			cvid.push_back(vid[i]);
		}
		else continue;
	}
}

char Is_Neighbor_new(CMesh &M, CFace &f1, CFace &f2)
{
    CHalfEdge  *phead = f1.pHalfedge;
    CHalfEdge  *ph = phead;
    char mychar = 'E';
    do
    {
        if (ph->dual() != NULL)
        {
            if (ph->dual()->pFace->m_nFaceid == f2.pHalfedge->pFace->m_nFaceid)
            {
                if (M.m_Vertices[f1.m_nFourIndex[0]].m_dVery == M.m_Vertices[f2.m_nFourIndex[0]].m_dVery && M.m_Vertices[f1.m_nFourIndex[1]].m_dVery == M.m_Vertices[f2.m_nFourIndex[1]].m_dVery)
                    mychar ='H';
				else if(M.m_Vertices[f1.m_nFourIndex[2]].m_dVery == M.m_Vertices[f2.m_nFourIndex[2]].m_dVery && M.m_Vertices[f1.m_nFourIndex[3]].m_dVery == M.m_Vertices[f2.m_nFourIndex[3]].m_dVery)
					mychar ='H';
				else if (M.m_Vertices[f1.m_nFourIndex[0]].m_dVerx == M.m_Vertices[f2.m_nFourIndex[0]].m_dVerx && M.m_Vertices[f1.m_nFourIndex[3]].m_dVerx == M.m_Vertices[f2.m_nFourIndex[3]].m_dVerx)
                    mychar ='V';
				else if (M.m_Vertices[f1.m_nFourIndex[1]].m_dVerx == M.m_Vertices[f2.m_nFourIndex[1]].m_dVerx && M.m_Vertices[f1.m_nFourIndex[2]].m_dVerx == M.m_Vertices[f2.m_nFourIndex[2]].m_dVerx)
                    mychar ='V';
            }
            
        }
        
        ph =ph->pNextHalfedge;
    } while (ph != phead);
    return mychar ;
    
}

/*void myRefine_QQ(CMesh &M, vector<int> &vid, vector<int> &exo_vid )
{
	vector<int>::iterator i;
	vector<char> vtypes;
	vtypes.clear();
	int Nf;
	int Nv;
   ///////////////initial//////////// 
    for(i=vid.begin();i!=vid.end();i++)
    {
        M.m_Faces[*i].flag_H = 0;
        M.m_Faces[*i].flag_V = 0;
    }  
	///////////////////////////1.save old vertex type/////
	Nv=M.m_Vertices.size();
	//cout<<"# Vertices--old: "<<Nv<<endl;
	for(int i=0; i<Nv; i++)
	{
			vtypes.push_back(M.m_Vertices[i].VertexType());
	}
///////////////////////////////2.give H/V mark according to curvature/////////////////////////    
	//ANTIISO
	for (int i = 0; i < vid.size(); ++i)
	{   
		if(M.m_Faces[vid[i]].m_bAnisoU==1&&M.m_Faces[vid[i]].m_bAnisoV==0)
		{
			M.m_Faces[vid[i]].flag_H = 0;
			M.m_Faces[vid[i]].flag_V = 1;
			
		}
		else if(M.m_Faces[vid[i]].m_bAnisoU==0&&M.m_Faces[vid[i]].m_bAnisoV==1)
		{
			M.m_Faces[vid[i]].flag_H = 1;
			M.m_Faces[vid[i]].flag_V = 0;
			
		}
		else continue;
	}
	//ISO
	for (int i = 0; i < vid.size(); ++i)
	{	
		if(M.m_Faces[vid[i]].m_bAnisoU==0&&M.m_Faces[vid[i]].m_bAnisoV==0)
		{
			for (int j = 0; j < vid.size(); ++j )
			{
				char flag=Is_Neighbor(M,M.m_Faces[vid[i]], M.m_Faces[vid[j]]);
				if (M.m_Faces[vid[i]].m_nFaceLevel == M.m_Faces[vid[j]].m_nFaceLevel)
				{
					if( flag == 'H' &&  M.m_Faces[vid[j]].flag_H ==1)
					{
						M.m_Faces[vid[i]].flag_H = 1;
					}
					if( flag == 'H' &&  M.m_Faces[vid[j]].m_bAnisoU==0 && M.m_Faces[vid[j]].m_bAnisoV==0)
					{
						M.m_Faces[vid[i]].flag_H = 1;
						M.m_Faces[vid[j]].flag_H = 1;
					}
					if( flag == 'V' &&  M.m_Faces[vid[j]].flag_V ==1)
					{
						M.m_Faces[vid[i]].flag_V = 1;
					}
					if( flag == 'V' &&  M.m_Faces[vid[j]].m_bAnisoU==0 && M.m_Faces[vid[j]].m_bAnisoV==0)
					{
						M.m_Faces[vid[i]].flag_V = 1;
						M.m_Faces[vid[j]].flag_V = 1;
					}
				}
			}

		}
		else continue;
	}


////////////////////////////3.subdivide cell according to mark///////////////////////////
    for(i=vid.begin();i!=vid.end();i++)
    {
		Nf = (int)M.m_Faces.size(); 
        // cout<<M.m_Faces[*i].flag_H<<" vs "<<M.m_Faces[*i].flag_V<<endl;
        if (M.m_Faces[*i].flag_H == 1 && M.m_Faces[*i].flag_V == 0)
        {
        	Unidirectional_Refine(M, *i,'H');////add level 1
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = Nf;
			M.m_Faces[*i].m_nSiblings[2] = -1;
			M.m_Faces[*i].m_nSiblings[3] = -1;
		}        	
        if (M.m_Faces[*i].flag_H == 0 && M.m_Faces[*i].flag_V == 1)
		{
            Unidirectional_Refine(M, *i,'V');//add level 1
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = -1;
			M.m_Faces[*i].m_nSiblings[2] = Nf;
			M.m_Faces[*i].m_nSiblings[3] = -1;
		}
        if(M.m_Faces[*i].flag_H == 1 && M.m_Faces[*i].flag_V == 1)
		{
            CrossRefineHierarMesh(M, *i);//add level 1
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = Nf;
			M.m_Faces[*i].m_nSiblings[2] = Nf+1;
			M.m_Faces[*i].m_nSiblings[3] = Nf+2;
		}
        if(M.m_Faces[*i].flag_H == 0 && M.m_Faces[*i].flag_V == 0)
        {
            CrossRefineHierarMesh(M, *i);
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = Nf;
			M.m_Faces[*i].m_nSiblings[2] = Nf+1;
			M.m_Faces[*i].m_nSiblings[3] = Nf+2;
		}
    } 
	/////////////////4.find exotic vertex////////////
	//cout<<"Find Exotic Vertex: "<<endl;
	for(int ni=0; ni<Nv; ni++)
	{
		if(vtypes[ni] != M.m_Vertices[ni].VertexType())
		{
			exo_vid.push_back(ni);
			cout<<ni<<",   "<<vtypes[ni]<<", " <<M.m_Vertices[ni].VertexType()<<endl;
		}
	}
}

void adjustHV(CMesh &M, vector<int> &vid)
{
	/////////////////TODO:adjust Horizontal and vertical////////////
	double hx[2],hy[2];
	int Nf;
	for (int i = 0; i < vid.size(); ++i)// error cells
	{
		Nf = (int)M.m_Faces.size();
		for (int j = 0; j < Nf; ++j )//all cells
		{
			char flag=Is_Neighbor_new(M,M.m_Faces[vid[i]], M.m_Faces[j]);
			//if (M.m_Faces[vid[i]].m_nFaceLevel == M.m_Faces[vid[j]].m_nFaceLevel)
			 //{
				 if(flag!='E')
				 {
					 if( flag == 'H')
					 {
						 hy[0]=M.m_Vertices[M.m_Faces[vid[i]].m_nFourIndex[0]].m_dVery-M.m_Vertices[M.m_Faces[vid[i]].m_nFourIndex[2]].m_dVery;
						 hy[1]=M.m_Vertices[M.m_Faces[j].m_nFourIndex[0]].m_dVery-M.m_Vertices[M.m_Faces[j].m_nFourIndex[2]].m_dVery;
						 if (abs(hy[0])>abs(hy[1]))
						 {cout<<"here0"<<endl;
							 M.m_Faces[vid[i]].flag_V = 1;
							 M.m_Faces[vid[i]].flag_H = 0;
							 break;

						 }

					 }
					 else if(flag == 'V')
					 {
						 hx[0]=M.m_Vertices[M.m_Faces[vid[i]].m_nFourIndex[0]].m_dVerx-M.m_Vertices[M.m_Faces[vid[i]].m_nFourIndex[2]].m_dVerx;
						 hx[1]=M.m_Vertices[M.m_Faces[j].m_nFourIndex[0]].m_dVerx-M.m_Vertices[M.m_Faces[j].m_nFourIndex[2]].m_dVerx;
						 if (abs(hx[0])>abs(hx[1]))
						 {cout<<"here2"<<endl;
							 M.m_Faces[vid[i]].flag_V = 0;
							 M.m_Faces[vid[i]].flag_H = 1;
							 break;
						 }

					 }
				 //}
			}
		}
	}
	int size =vid.size();
	for (int i = 0; i <size ; ++i)// error cells
	{
		for (int j = i+1; j < size; ++j )//all cells
		{
			char flag=Is_Neighbor(M,M.m_Faces[vid[i]], M.m_Faces[j]);
			if (M.m_Faces[vid[i]].m_nFaceLevel == M.m_Faces[vid[j]].m_nFaceLevel)
			{
				 if(flag!='E')
				 {
					 if( flag == 'H')
					 {
						 if(M.m_Faces[vid[i]].flag_H+M.m_Faces[vid[j]].flag_H==1)
						 {
							 if(vid.size()==2)
							 {
							 M.m_Faces[vid[i]].flag_H = 1;
							 M.m_Faces[vid[j]].flag_H = 1;
							 }
							 else
							 {
								vid.erase(vid.begin());
								vid.resize(size-1);
								adjustHV(M, vid);
							 }

						 }

					 }
					 else if(flag == 'V')
					 {
						 if(M.m_Faces[vid[i]].flag_V+M.m_Faces[vid[j]].flag_V==1)
						 {
							 if(vid.size()==2)
							 {
							 M.m_Faces[vid[i]].flag_V = 1;
							 M.m_Faces[vid[j]].flag_V = 1;
							 }
							 else
							 {
								vid.erase(vid.begin());
								vid.resize(size-1);
								adjustHV(M, vid);
							 }

						 }
					 }
				 }
			}
		}
	}
}*/

void myRefine_QQ(CMesh &M, vector<int> &vid, vector<int> &exo_vid )
{
	vector<int>::iterator i;
	vector<int> vid_half;
	vid_half.clear ();
	vector<char> vtypes;
	vtypes.clear();
	int Nf;
	int Nv;
   ///////////////initial//////////// 
    for(i=vid.begin();i!=vid.end();i++)
    {
        M.m_Faces[*i].flag_H = 0;
        M.m_Faces[*i].flag_V = 0;
    }  
	///////////////////////////1.save old vertex type/////
	Nv=M.m_Vertices.size();
	//cout<<"# Vertices--old: "<<Nv<<endl;
	for(int i=0; i<Nv; i++)
	{
			vtypes.push_back(M.m_Vertices[i].VertexType());
	}
///////////////////////////////2.give H/V mark according to curvature/////////////////////////    
	for (int i = 0; i < vid.size(); ++i)
	{   
		if(M.m_Faces[vid[i]].m_bAnisoU==1)
			M.m_Faces[vid[i]].flag_V = 1;

		 if(M.m_Faces[vid[i]].m_bAnisoV==1)
			M.m_Faces[vid[i]].flag_H = 1;
		 if(M.m_Faces[vid[i]].m_bAnisoU==0&&M.m_Faces[vid[i]].m_bAnisoV==0)
		 {
			 M.m_Faces[vid[i]].flag_H = 1;
			 M.m_Faces[vid[i]].flag_V = 1;
		 }
		else continue;
	}
	addbasisHV(M, vid);
	adjustHV(M, vid);
////////////////////////////3.subdivide cells according to mark///////////////////////////
     for(i=vid.begin();i!=vid.end();i++)
    {
		Nf = (int)M.m_Faces.size(); 
         //cout<<M.m_Faces[*i].flag_H<<" vs "<<M.m_Faces[*i].flag_V<<endl;
        if (M.m_Faces[*i].flag_H == 1 && M.m_Faces[*i].flag_V == 0)
        {
        	Unidirectional_Refine(M, *i,'H');////add level 1
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = Nf;
			M.m_Faces[*i].m_nSiblings[2] = -1;
			M.m_Faces[*i].m_nSiblings[3] = -1;
			///save halfcell id
			vid_half.push_back(*i);
			vid_half.push_back(Nf);
		}        	
        if (M.m_Faces[*i].flag_H == 0 && M.m_Faces[*i].flag_V == 1)
		{
            Unidirectional_Refine(M, *i,'V');//add level 1
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = -1;
			M.m_Faces[*i].m_nSiblings[2] = Nf;
			M.m_Faces[*i].m_nSiblings[3] = -1;
			//save halfcell id
			vid_half.push_back(*i);
			vid_half.push_back(Nf);
		}
        if(M.m_Faces[*i].flag_H == 1 && M.m_Faces[*i].flag_V == 1)
		{
            CrossRefineHierarMesh(M, *i);//add level 1
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = Nf;
			M.m_Faces[*i].m_nSiblings[2] = Nf+1;
			M.m_Faces[*i].m_nSiblings[3] = Nf+2;
		}
        if(M.m_Faces[*i].flag_H == 0 && M.m_Faces[*i].flag_V == 0)
        {
            CrossRefineHierarMesh(M, *i);
			M.m_Faces[*i].m_nSiblings[0] = *i;
			M.m_Faces[*i].m_nSiblings[1] = Nf;
			M.m_Faces[*i].m_nSiblings[2] = Nf+1;
			M.m_Faces[*i].m_nSiblings[3] = Nf+2;
		}
    } 

	////////////////////4. new vertex is basis vertex ?  second refine/////////////////
   /* for (int i = 0; i < vid_half.size(); i=2+i)
	{
		 Nf = (int)M.m_Faces.size(); 
		if(M.m_Faces[vid_half[i]].flag_H==1)
		{
			if(M.m_Vertices[M.m_Faces[vid_half[i]].m_nFourIndex[2]].IsBoundaryVertex(0,1,0,1)==false&&M.m_Vertices[M.m_Faces[vid_half[i]].m_nFourIndex[3]].IsBoundaryVertex(0,1,0,1)==false)
			{
				if(M.m_Vertices[M.m_Faces[vid_half[i]].m_nFourIndex[2]].VertexType()=='T'&&M.m_Vertices[M.m_Faces[vid_half[i]].m_nFourIndex[3]].VertexType()=='T')
				{
					for(int j=i;j<i+2;j++)
					{
					Unidirectional_Refine_new(M, vid_half[j],'V');//add level 0
					M.m_Faces[vid_half[j]].m_nSiblings[0] = vid_half[j];
					M.m_Faces[vid_half[j]].m_nSiblings[1] = -1;
					M.m_Faces[vid_half[j]].m_nSiblings[2] = Nf;
					M.m_Faces[vid_half[j]].m_nSiblings[3] = -1;
					}
				}
			}
		}
		if(M.m_Faces[vid_half[i]].flag_V==1)
		{
			if(M.m_Vertices[M.m_Faces[vid_half[i]].m_nFourIndex[2]].IsBoundaryVertex(0,1,0,1)==false&&M.m_Vertices[M.m_Faces[vid_half[i]].m_nFourIndex[1]].IsBoundaryVertex(0,1,0,1)==false)
			{
				if(M.m_Vertices[M.m_Faces[vid_half[i]].m_nFourIndex[2]].VertexType()=='T'&&M.m_Vertices[M.m_Faces[vid_half[i]].m_nFourIndex[1]].VertexType()=='T')
				{
					for(int j=i;j<i+2;j++)
					{
					Unidirectional_Refine_new(M, vid_half[j],'H');//add level 0
					M.m_Faces[vid_half[j]].m_nSiblings[0] = vid_half[j];
					M.m_Faces[vid_half[j]].m_nSiblings[1] = Nf;
					M.m_Faces[vid_half[j]].m_nSiblings[2] = -1;
					M.m_Faces[vid_half[j]].m_nSiblings[3] = -1;
					}
				}
			}
		}
	}*/

	/////////////////5.find exotic vertex////////////
	for(int ni=0; ni<Nv; ni++)
	{
		if(vtypes[ni] != M.m_Vertices[ni].VertexType())
		{
			exo_vid.push_back(ni);
			cout<<"Find Exotic Vertex: "<<endl;
			cout<<ni<<",   "<<vtypes[ni]<<", " <<M.m_Vertices[ni].VertexType()<<endl;
		}
	}
}

void adjustHV(CMesh &M, vector<int> &vid)
{
	/////////////////TODO:adjust Horizontal and vertical////////////
	int size =vid.size();
	vector<int> vid_new;

	for (int i = 0; i <size ; ++i)// error cells
	{
		for (int j = i+1; j < size; ++j )//all cells
		{
			char flag=Is_Neighbor(M,M.m_Faces[vid[i]], M.m_Faces[vid[j]]);
			//cout<<vid[i]<<"  "<<vid[j]<<"  "<<flag<<endl;
			if (M.m_Faces[vid[i]].m_nFaceLevel == M.m_Faces[vid[j]].m_nFaceLevel)
			{
				if(flag!='E')
				{
					if( flag == 'H')
					{
						if(M.m_Faces[vid[i]].flag_H+M.m_Faces[vid[j]].flag_H==1)
						{
							if(size==2)
							{
								M.m_Faces[vid[i]].flag_H = 1;
								M.m_Faces[vid[j]].flag_H = 1;
							}
							else if(size>2)
							{
								M.m_Faces[vid[i]].flag_H = 1;
								M.m_Faces[vid[j]].flag_H = 1;
								vid_new.clear();
								vid_new.resize(size);
								for(int i=0;i<size;i++)
									vid_new[i]=vid[i];

								vid_new.erase(vid_new.begin());
								vid_new.resize(size-1);
								adjustHV(M, vid_new);
								if (size==4)
									continue;
							}

						}
					}
					else if(flag == 'V')
					{
						if(M.m_Faces[vid[i]].flag_V+M.m_Faces[vid[j]].flag_V==1)
						{
							if(size==2)
							{
								M.m_Faces[vid[i]].flag_V = 1;
								M.m_Faces[vid[j]].flag_V = 1;
							}
							else if(size>2)
							{
								M.m_Faces[vid[i]].flag_V = 1;
								M.m_Faces[vid[j]].flag_V = 1;
								vid_new.clear();
								vid_new.resize(size);
								for(int i=0;i<size;i++)
									vid_new[i]=vid[i];

								vid_new.erase(vid_new.begin());
								vid_new.resize(size-1);
								adjustHV(M, vid_new);
								if (size==4)
									continue;
							}
						}
					}
				}
			}
		}
	}
}

void addbasisHV(CMesh &M,  vector<int> &vid)
{	
	/////////////TODO:add H or V mark for cell without basis vertex
	for (int i = 0; i < vid.size(); ++i)
	{
		int eh=0;
		int ev=0;
		for (int j = 0; j < vid.size(); ++j)
		{
			char flag=Is_Neighbor(M,M.m_Faces[vid[i]], M.m_Faces[vid[j]]);
			if('E'!=flag)
			{
				if(flag == 'V'&&M.m_Faces[vid[j]].flag_V ==1)
					ev=1;
				else if(flag == 'H'&&M.m_Faces[vid[j]].flag_H ==1)
					eh=1;
			}
		}		
		if(M.m_Faces[vid[i]].flag_V ==1&&M.m_Faces[vid[i]].flag_H == 0&&ev==0)
		{
			if(M.m_Faces[vid[i]].IsBoundaryCell_LRUD()=='V'||M.m_Faces[vid[i]].IsBoundaryCell_LRUD()=='N')
				M.m_Faces[vid[i]].flag_H = 1;
		}
		else if(M.m_Faces[vid[i]].flag_V ==0&&M.m_Faces[vid[i]].flag_H == 1&&eh==0)
		{
			if(M.m_Faces[vid[i]].IsBoundaryCell_LRUD()=='H'||M.m_Faces[vid[i]].IsBoundaryCell_LRUD()=='N')
				M.m_Faces[vid[i]].flag_V = 1;
		}
	}
}

char CFace::IsBoundaryCell_LRUD()
{
	CHalfEdge  *ph,*phead;
	char flag = 'N';
	phead = pHalfedge;
	ph = phead;
	do 
	{
		if(ph->dual() == NULL)
		{
			if(flag=='N')
				flag=ph->pEdge->edge_type();//H/V
			else
				flag='M';
		}
		ph = ph->pNextHalfedge;
	} while (ph != phead);

	return flag;
}

void classifycells(CMesh &M, vector<int> &vid, vector<vector<int>> &temp2)
{
	vector<int> temp;
	//vector<vector<int>> temp2;
	//////find neighbor vector (just right and up)
	for (int i = 0; i < vid.size(); ++i)
	{
		temp.push_back(vid[i]);
		for (int j = i+1; j < vid.size(); ++j)
		{
			char flag=Is_Neighbor(M,M.m_Faces[vid[i]], M.m_Faces[vid[j]]);
			if('E'!=flag)
			{
				temp.push_back(vid[j]);
			}
		}
		temp2.push_back(temp);
		temp.clear();
	}


	/////////////////merge vector if include same id
	int size;
	for(int i=0;i<temp2.size();i++)
	{
		for(int j=0;j<temp2[i].size();j++)
		{
			for(int k=i+1;k<temp2.size();k++)
			{
				for(int n=0;n<temp2[k].size();n++)
				{
					if(temp2[i][j]==temp2[k][n])
					{
						//cout<<"k    "<<temp2[k][n]<<endl;
						for(int m=0;m<temp2[k].size();m++)
						temp2[i].push_back (temp2[k][m]);

						sort(temp2[i].begin(),temp2[i].end());
						temp2[i].erase(unique(temp2[i].begin(),temp2[i].end()),temp2[i].end());
						//for(vector<int>::iterator it=temp2[i].begin();it!=temp2[i].end();++it)
							//cout<<*it<<endl;
						temp2.erase(temp2.begin()+k);
						size=temp2.size();
						//cout<<"temp2.size"<<size<<endl;	
						break;
					}
					//else continue;
				}
			}
		}
		
	}
	/////// deal with the last vector
	for(int i=0;i<temp2.size()-1;i++)
	{
		for(int j=0;j<temp2[i].size();j++)
		{
			for(int n=0;n<temp2[size-1].size();n++)
			{
				if(temp2[i][j]==temp2[size-1][n])
				{
					for(int m=0;m<temp2[size-1].size();m++)
						temp2[i].push_back (temp2[size-1][m]);

					sort(temp2[i].begin(),temp2[i].end());
					temp2[i].erase(unique(temp2[i].begin(),temp2[i].end()),temp2[i].end());
					//for(vector<int>::iterator it=temp2[i].begin();it!=temp2[i].end();++it)
						//cout<<*it<<endl;
					temp2.erase(temp2.begin()+size-1);
					size=temp2.size();
					cout<<"temp2.size"<<size<<endl;	
					break;
				}
			}
		}
	}
}

void InitialMesh(CMesh& cmesh, int un, int vn, double x0, double x1, double y0, double y1) {

	Gvertex ver;
	Gface face;

	int m_nVertexNum = un * vn;
	int m_nFaceNum = (un - 1) * (vn - 1);//��ˮƽ�������-1��*����ֱ�������-1�������������

	vector<Gvertex> m_Gvers;//������
	vector<Gface> m_Gfaces;//������

	/*m_nVertexNum = un * vn;
	m_nFaceNum = (un - 1) * (vn - 1);
	m_Gvers.clear();
	m_Gfaces.clear();*/

	//�������е�
	for (int j = 0; j < vn; ++j) { //uniform partition
		ver.m_dGVy = y0 + (y1 - y0) / (vn - 1) * j;
		for (int i = 0; i < un; i++) {
			ver.m_dGVx = x0 + (x1 - x0) / (un - 1) * i;
			ver.m_nGvid = j * un + i;//id������j��*un+����i
			m_Gvers.push_back(ver);//��ʼ����������֮��push_back
		}
	}

	for (int i = 0; i < m_nFaceNum; ++i) {
		face.m_nGver[0] = i + i / (un - 1);//���Ͻ�
		face.m_nGver[1] = i + i / (un - 1) + 1;//���Ͻ�
		face.m_nGver[2] = (i / (un - 1) + 1) * un + i % (un - 1) + 1;//���½�
		face.m_nGver[3] = (i / (un - 1) + 1) * un + i % (un - 1);//���½�
		face.m_nGfid = i;//��id
		m_Gfaces.push_back(face);
	}

	//��ʼ����
	cmesh.GeneratTMesh(m_Gvers, m_Gfaces);

}

//void Connected(CMesh& M, vector<int>& remain, vector<vector<int>>& connected, int now, int nowLine, bool haveNext) {
//
//	if (now == -1) {
//		now = remain.back();
//		remain.pop_back();//ɾ��back
//		connected.push_back({ now });
//		nowLine = connected.size() - 1;
//	}
//	vector<int> remainDelete;
//	vector<int> nexts;
//
//	//vector<int> fid;
//	//Find_Adjacent_Cell(M.m_Faces[now], fid);
//	int size = remain.size();
//	for (int i = 0; i < size; i++) {
//		char result = Is_Neighbor_new(M, M.m_Faces[now], M.m_Faces[remain[i]]);
//		if (result == 'V' || result == 'H') {
//			nexts.push_back(remain[i]);
//			connected[nowLine].push_back(remain[i]);
//			remainDelete.push_back(remain[i]);
//		}
//	}
//
//	vector<int> newRemain;
//	for (const int& i : remain) {
//		bool flag = true;
//		for (const int& j : remainDelete) {
//			if (i == j) {
//				flag = false;
//				break;
//			}
//		}
//		if (flag) {
//			newRemain.push_back(i);
//		}
//	}
//	remain = newRemain;
//	
//	if (!remain.empty() && !nexts.empty()) {
//		for (const int& everyNext : nexts) {
//			if (everyNext != nexts.back()) {
//				haveNext = true;
//				Connected(M, remain, connected, everyNext, nowLine, haveNext);
//			}	
//		}
//	}
//	else if (!remain.empty()&& !haveNext) {
//		Connected(M, remain, connected);
//	}
//
//}

void Connected(CMesh& M, vector<int>& remain, vector<vector<int>>& connected, vector<int>& nextStack) {
	//nextStackջ

	int now;
	int nowLine;
	if (nextStack.empty()) {//ȫ����ͨ����
		now = remain.back();
		remain.pop_back();//ɾ��back
		connected.push_back({ now });
	}
	else {//��Ȼ���������һ����ͨ����
		now = nextStack.back();
		nextStack.pop_back();
	}
	nowLine = connected.size() - 1;//��ǰ����ͨ����

	vector<int> remainDelete;//��Ҫɾ����

	int size = remain.size();
	for (int i = 0; i < size; i++) {
		char result = Is_Neighbor_new(M, M.m_Faces[now], M.m_Faces[remain[i]]);
		if (result == 'V' || result == 'H') {//�ж���ͨ
			nextStack.push_back(remain[i]);//����ͨѹ��ջ
			connected[nowLine].push_back(remain[i]);//�����µ�Ԫ�ص���ǰ��ͨ����
			remainDelete.push_back(remain[i]);//��Ҫɾ����Ҳ�����µ�
		}
	}

	vector<int> newRemain;
	for (const int& i : remain) {
		bool flag = true;
		for (const int& j : remainDelete) {//�Ѽ���remainDelete��ɾ�ˣ��õ�newRemain
			if (i == j) {
				flag = false;
				break;
			}
		}
		if (flag) {
			newRemain.push_back(i);
		}
	}
	remain = newRemain;//ע�⵽��������������&remainһֱ��ʹ��

	if (!remain.empty()) {//һ���ж�
		Connected(M, remain, connected, nextStack);
	}

}