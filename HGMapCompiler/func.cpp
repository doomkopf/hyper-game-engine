#include "stdafx.h"
#include "func.h"

bool Equal(const float &f1,const float &f2,const float &epsilon)
{
	float dif = abs_f(f1 - f2);

	if(dif <= epsilon)
		return true;

	return false;
}

int ClassifyPoint(const CVector3f &p,const CPlane &plane,const float &epsilon)
{
	float d = DotProduct(p,plane.normal) + plane.d;

	if(d > epsilon)
		return FRONT;
	if(d < -epsilon)
		return BACK;

	return COPLANAR;
}

int ClassifyLineseg(const CVector3f &p1,const CVector3f &p2,const CPlane &plane,const float &epsilon)
{
	int d1,d2;

	d1 = ClassifyPoint(p1,plane,epsilon);
	d2 = ClassifyPoint(p2,plane,epsilon);

	if(d1 == FRONT && d2 == BACK)
		return SPANNING_FB;

	if(d1 == BACK && d2 == FRONT)
		return SPANNING_BF;

	//if((d1 == COPLANAR && d2 == COPLANAR) || (d1 == FRONT || d2 == FRONT))
		//return FRONT;

	if(((d1 == FRONT || d1 == COPLANAR) && d2 == FRONT) ||
		((d2 == FRONT || d2 == COPLANAR) && d1 == FRONT))
		return FRONT;

	if(d1 == COPLANAR && d2 == COPLANAR)
		return COPLANAR;

	return BACK;
}

int ClassifyPolygon(const CPoly &poly,const CPlane &plane,const float &epsilon)
{
	int front,back,coplanar,c = 0;
	float dist;
	CVertex *vertex;

	front = back = coplanar = 0;

	for(CNode<CVertex> *node=poly.vertices.GFDN();node;node=node->next)
	{
		vertex = node->GetData();

		dist = DotProduct(plane.normal,vertex->p) + plane.d;

		if(dist > epsilon)
			front++;
		else if(dist < -epsilon)
			back++;
		else
		{
			coplanar++;
			front++;
			back++;
		}

		c++;
	}

	if(coplanar == c)
		return COPLANAR;
	if(front == c)
		return FRONT;
	if(back == c)
		return BACK;

	return SPANNING;
}

void ClipPolygon(const CPoly &poly,const CPlane &plane,CPoly *front,CPoly *back)
{
	float d1,d2;
	CVertex *v1,*v2;
	CVertex v_new;

	if(front)
	{
		front->plane = poly.plane;
		front->tex = poly.tex;
		front->issplitter = poly.issplitter;
	}

	if(back)
	{
		back->plane = poly.plane;
		back->tex = poly.tex;
		back->issplitter = poly.issplitter;
	}

	for(CNode<CVertex> *node=poly.vertices.GFDN();node;node=node->next)
	{
		v1 = node->GetData();
		if(!node->next)
			v2 = poly.vertices.GFDN()->GetData();
		else
			v2 = node->next->GetData();

		d1 = DotProduct(plane.normal,v1->p) + plane.d;
		d2 = DotProduct(plane.normal,v2->p) + plane.d;

		if(d1 > 0 && d2 < 0) // v1=front v2=back
		{
			// adding new vertex to both polys
			GetIntersectionPoint_lineseg_plane(
				v1->p,
				v2->p,
				plane,
				&v_new.p);
			float l1 = (v2->p - v1->p).Length();
			float l2 = (v_new.p - v1->p).Length();
			float percent = l2 / l1;
			float deltax = v2->tu - v1->tu;
			float deltay = v2->tv - v1->tv;
			v_new.tu = v1->tu + (deltax * percent);
			v_new.tv = v1->tv + (deltay * percent);
			if(front)
				front->vertices.Add(v_new);
			if(back)
				back->vertices.Add(v_new);

			// adding v2 to back poly
			if(back)
				back->vertices.Add(*v2);
		}
		else if(d1 < 0 && d2 > 0) // v1=back v2=front
		{
			// adding new vertex to both polys
			GetIntersectionPoint_lineseg_plane(
				v2->p,
				v1->p,
				plane,
				&v_new.p);
			float l1 = (v2->p - v1->p).Length();
			float l2 = (v_new.p - v1->p).Length();
			float percent = l2 / l1;
			float deltax = v2->tu - v1->tu;
			float deltay = v2->tv - v1->tv;
			v_new.tu = v1->tu + (deltax * percent);
			v_new.tv = v1->tv + (deltay * percent);
			if(front)
				front->vertices.Add(v_new);
			if(back)
				back->vertices.Add(v_new);

			// adding v2 to front poly
			if(front)
				front->vertices.Add(*v2);
		}
		else if(d1 < 0 && d2 == 0) // v1=back v2=onplane
		{
			// adding v2 to both
			if(front)
				front->vertices.Add(*v2);
			if(back)
				back->vertices.Add(*v2);
		}
		else if(d1 > 0 && d2 == 0) // v1=front v2==onplane
		{
			// adding v2 to both
			if(front)
				front->vertices.Add(*v2);
			if(back)
				back->vertices.Add(*v2);
		}
		else if(d1 == 0 && d2 > 0) // v1=onplane v2=front
		{
			// adding v2 to front poly
			if(front)
				front->vertices.Add(*v2);
		}
		else if(d1 == 0 && d2 < 0) // v1=onplane v2=back
		{
			// adding v2 to back poly
			if(back)
				back->vertices.Add(*v2);
		}
		else if(d1 > 0 && d2 > 0) // v1=front v2=front
		{
			// adding v2 to front poly
			if(front)
				front->vertices.Add(*v2);
		}
		else if(d1 < 0 && d2 < 0) // v1=back v2=back
		{
			// adding v2 to back poly
			if(back)
				back->vertices.Add(*v2);
		}
		else
		{
			//printf("Error clipping poly %f %f\n",d1,d2);
		}
	}
}

// both polys should be coplanar and intersecting
/*void ClipPolyToPoly(const CPoly &poly1,const CPoly &poly2,CList<CPoly> *new_polys,const float &epsilon)
{
	CPlane plane;
	CPoly poly_t,front,back;

	if(poly1.VerticesEqual(poly2,epsilon))
	{
		return;
	}

	poly_t.Copy(poly1);

	for(CNode<CVertex> *v_node=poly2.vertices.GFDN();v_node;v_node=v_node->next)
	{
		CVertex *v1 = v_node->GetData(),*v2;
		if(!v_node->next)
			v2 = poly2.vertices.GFDN()->GetData();
		else
			v2 = v_node->next->GetData();

		plane.normal = v2->p - v1->p;
		plane.normal = CrossProduct(plane.normal,poly2.plane.normal);
		plane.normal.Normalize();
		plane.d = -DotProduct(plane.normal,v1->p);

		switch(ClassifyPolygon(poly_t,plane,epsilon))
		{
		case FRONT:
			new_polys->Add(poly_t);
			return;
			break;

		case BACK:
			break;

		//case COPLANAR: // must not be
			//break;

		case SPANNING:
			ClipPolygon(poly_t,plane,&front,&back);

			if(front.vertices.GFDN())
				new_polys->Add(front);
			poly_t.Copy(back);

			front.vertices.Clear();
			back.vertices.Clear();
			break;
		}
	}
}*/