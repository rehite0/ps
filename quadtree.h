#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

typedef BALL _point;
#define CAP 4
#define ax(p) (p->pos[0])
#define ay(p) (p->pos[1])

typedef struct _bod {
	float px,nx,py,ny;
} bod;

typedef struct _qtree{
	int len;
	bod b;
	union p_and_qt {
		_point* p;
		struct _qtree* t;
	} q1,q2,q3,q4;
}qtree;

qtree* qt_create(bod b);
int qt_insert(_point* p,qtree* t);
	//p [in] pointer to point to be inserted
	//t [in] pointer to root node of tree
	//ret [evar] 1 if inserted, 0 if insertion denied and panic if unexpected things happned

void qt_pvt_subdivide(qtree* t);
_point** qt_query_range_sq(qtree* t,bod b,int* size);
	//t [in] pointer to root node of tree to search
	//b [in] boundary of area where point need to be search
	//size [out]-size of buff
	//ret [out] pointer to an array of _point* whth 'size' elements
void qt_free(qtree* t);

qtree* qt_create(bod b){
	qtree* ret=malloc(sizeof(qtree));
	ret->len=0;
	ret->b=b;
	ret->q1.p=NULL;
	ret->q2.p=NULL;
	ret->q3.p=NULL;
	ret->q4.p=NULL;
	return ret;
}//fn

int qt_insert(_point* p,qtree* t){
	if (	ax(p) > t->b.px
		||	ax(p) < t->b.nx
		||	ay(p) > t->b.py
		||	ay(p) < t->b.ny
			) return 0;
	switch (t->len){
		case 0:
			t->q1.p=p;
			++(t->len);
			return 1;
		case 1:
			t->q2.p=p;
			++(t->len);
			return 1;
		case 2:
			t->q3.p=p;
			++(t->len);
			return 1;
		case 3:
			t->q4.p=p;
			++(t->len);
			return 1;
		case 4:
			qt_pvt_subdivide(t);
			++(t->len);
			break;
		default:
			++(t->len);
			break;
	}//switch
	return qt_insert(p,t->q1.t)
		|| qt_insert(p,t->q2.t)
		|| qt_insert(p,t->q2.t)
		|| qt_insert(p,t->q2.t);
}//fn

void qt_pvt_subdivide(qtree* t){
	_point* p1=t->q1.p;
	_point* p2=t->q2.p;
	_point* p3=t->q3.p;
	_point* p4=t->q4.p;
	bod b=t->b;
	qtree* q1=qt_create((bod){b.px				,(b.px+b.nx)/2	,b.py			,(b.py+b.ny)/2});
	qtree* q2=qt_create((bod){(b.px+b.nx)/2	,b.nx			,b.py			,(b.py+b.ny)/2});
	qtree* q3=qt_create((bod){(b.px+b.nx)/2	,b.nx			,(b.py+b.ny)/2	,b.ny});
	qtree* q4=qt_create((bod){b.px				,(b.px+b.nx)/2	,(b.py+b.ny)/2	,b.ny});

	   qt_insert(p1,q1)
	|| qt_insert(p1,q2)
	|| qt_insert(p1,q3)
	|| qt_insert(p1,q4);
	   qt_insert(p2,q1)
	|| qt_insert(p2,q2)
	|| qt_insert(p2,q3)
	|| qt_insert(p2,q4);
	   qt_insert(p3,q1)
	|| qt_insert(p3,q2)
	|| qt_insert(p3,q3)
	|| qt_insert(p3,q4);
	   qt_insert(p4,q1)
	|| qt_insert(p4,q2)
	|| qt_insert(p4,q3)
	|| qt_insert(p4,q4);

	t->q1.t=q1;
	t->q2.t=q2;
	t->q3.t=q3;
	t->q4.t=q4;
}//fn

_point** qt_query_range_sq(qtree* t,bod b,int* num){
	*num=0;
	_point* arr[4]={0,0,0,0};
	_point** ret=NULL;
	if (	b.px < t->b.nx
		||	b.nx > t->b.px
		||	b.py < t->b.ny
		||	b.ny > t->b.py
		||	t->len==0){
		*num=0;
		return NULL;
	}//if
	
	switch (t->len){
		case 4:
			if	(	ax(t->q4.p) <= b.px
				&&	ax(t->q4.p) >= b.nx
				&&	ay(t->q4.p) <= b.py
				&&	ay(t->q4.p) >= b.ny
				){
				arr[(*num)++]=t->q1.p;
			}
		case 3:
			if	(	ax(t->q3.p) <= b.px
				&&	ax(t->q3.p) >= b.nx
				&&	ay(t->q3.p) <= b.py
				&&	ay(t->q3.p) >= b.ny
				){
				arr[(*num)++]=t->q3.p;
			}

		case 2:
			if	(	ax(t->q2.p) <= b.px
				&&	ax(t->q2.p) >= b.nx
				&&	ay(t->q2.p) <= b.py
				&&	ay(t->q2.p) >= b.ny
				){
				arr[(*num)++]=t->q2.p;
			}

		case 1:
			if	(	ax(t->q1.p) <= b.px
				&&	ax(t->q1.p) >= b.nx
				&&	ay(t->q1.p) <= b.py
				&&	ay(t->q1.p) >= b.ny
				){
				arr[(*num)++]=t->q1.p;
			}
			ret=malloc(sizeof(_point*)*(*num));
			for (int i=0;i<*num;++i) ret[i]=arr[i];
			return ret;
		case 0:
			*num=0;
			return NULL;
		default:
			int s[4]={0,0,0,0};
			_point** buff[4]={0,0,0,0};
			buff[0]=qt_query_range_sq( t->q1.t, b , (int*)s   );
			buff[1]=qt_query_range_sq( t->q2.t, b , (int*)s+1 );
			buff[2]=qt_query_range_sq( t->q3.t, b , (int*)s+2 );
			buff[3]=qt_query_range_sq( t->q4.t, b , (int*)s+3 );
			//merge buff
			*num=s[0]+s[1]+s[2]+s[3];
			ret=malloc(sizeof(_point*)*(*num));
			assert(ret && "malloc failed");
			int k=0;
			for (int i=0 ; i<4 ;++i){
				for (int j=0 ; j<s[i] ; ++j){
					ret[k]=buff[i][j];
					++k;
				}//for j
				free(buff[i]);
			}//for i
			return ret;
	}//switch	
}//fn

void qt_free(qtree* t){
	if (t->len>4 && t->len!=0){
		qt_free(t->q1.t);
		qt_free(t->q2.t);
		qt_free(t->q3.t);
		qt_free(t->q4.t);
		free(t);
	}
	else{
		free(t);
		return;
	}
}//fn
