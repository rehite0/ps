#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

typedef BALL _point;
	//redefine the underlying _point type while using
#define ax(p) (p->pos[0])
	//how to acess x coord from _point
#define ay(p) (p->pos[1])
	//how to acess y coord from _point
#define qat(i) (_arena[i])

typedef struct _bod {
	float px,nx,py,ny;
} bod;
	//boundary structure
typedef int qtidx;
typedef struct _qtree{
	int len;
	bod b;
	union p_and_qt {
		_point* p;
		qtidx t;
	} q1,q2,q3,q4;
}qtree;
	//quad tree structure

qtidx qt_create(bod b);
	//create a clean quad tree
int qt_insert(_point* p,qtidx t);
	//p [in] pointer to point to be inserted
	//t [in] pointer to root node of tree
	//ret [evar] 1 if inserted, 0 if insertion denied and panic if unexpected things happned

void qt_pvt_subdivide(qtidx t);
_point** qt_query_range_sq(qtidx t,bod b,int* size);
	//t [in] pointer to root node of tree to search
	//b [in] boundary of area where point need to be search
	//size [out]-size of buff
	//ret [out] pointer to an array of _point* whth 'size' elements
//void qt_free(qtidx t);///////////////////////////////////////////////////////////////////////////////////////////
	//free quad tree

//my memory allocator for better catch preformance
qtree* _arena=NULL;
int _Asize=0,_Aused=0;

qtidx arena_alloc(int s){
	_Aused+=s;
	if (_Asize>_Aused){
		assert(_arena&&"realloc failed");
		return _Aused-s;}
	else{
		_Asize+=100;
		_arena=realloc(_arena,_Asize*sizeof(qtree));
		assert(_arena&&"realloc failed");
		return _Aused-s;

	}

}
void arena_free(){
	_Aused=0;
}
void arena_del(){
	if (_arena){
		free(_arena);
		_arena=NULL;
		_Asize=0;
	}
		
}


qtidx qt_create(bod b){
	qtidx ret=arena_alloc(1);
	qat(ret).len=0;
	qat(ret).b=b;
	qat(ret).q1.p=NULL;
	qat(ret).q2.p=NULL;
	qat(ret).q3.p=NULL;
	qat(ret).q4.p=NULL;
	return ret;
}//fn

int qt_insert(_point* p,qtidx t){
	if (	ax(p) > qat(t).b.px
		||	ax(p) < qat(t).b.nx
		||	ay(p) > qat(t).b.py
		||	ay(p) < qat(t).b.ny
			) return 0;
	switch (qat(t).len){
		case 0:
			qat(t).q1.p=p;
			++(qat(t).len);
			return 1;
		case 1:
			qat(t).q2.p=p;
			++(qat(t).len);
			return 1;
		case 2:
			qat(t).q3.p=p;
			++(qat(t).len);
			return 1;
		case 3:
			qat(t).q4.p=p;
			++(qat(t).len);
			return 1;
		case 4:
			qt_pvt_subdivide(t);
			++(qat(t).len);
			break;
		default:
			++(qat(t).len);
			break;
	}//switch
	return qt_insert(p,qat(t).q1.t)
		|| qt_insert(p,qat(t).q2.t)
		|| qt_insert(p,qat(t).q3.t)
		|| qt_insert(p,qat(t).q4.t);
}//fn

void qt_pvt_subdivide(qtidx t){
	_point* p1=qat(t).q1.p;
	_point* p2=qat(t).q2.p;
	_point* p3=qat(t).q3.p;
	_point* p4=qat(t).q4.p;
	bod b=qat(t).b;
	qtidx q1=qt_create((bod){b.px				,(b.px+b.nx)/2	,b.py			,(b.py+b.ny)/2});
	qtidx q2=qt_create((bod){(b.px+b.nx)/2		,b.nx			,b.py			,(b.py+b.ny)/2});
	qtidx q3=qt_create((bod){(b.px+b.nx)/2		,b.nx			,(b.py+b.ny)/2	,b.ny});
	qtidx q4=qt_create((bod){b.px				,(b.px+b.nx)/2	,(b.py+b.ny)/2	,b.ny});

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
	
	qat(t).q1.t=q1;
	qat(t).q2.t=q2;
	qat(t).q3.t=q3;
	qat(t).q4.t=q4;
}//fn

_point** qt_query_range_sq(qtidx t,bod b,int* num){
	*num=0;
	_point* arr[4]={0,0,0,0};
	_point** ret=NULL;
	if (	b.px < qat(t).b.nx
		||	b.nx > qat(t).b.px
		||	b.py < qat(t).b.ny
		||	b.ny > qat(t).b.py
		||	qat(t).len==0){
		*num=0;
		return NULL;
	}//if
	
	switch (qat(t).len){
		case 4:
			if	(	ax(qat(t).q4.p) <= b.px
				&&	ax(qat(t).q4.p) >= b.nx
				&&	ay(qat(t).q4.p) <= b.py
				&&	ay(qat(t).q4.p) >= b.ny
				){
				arr[(*num)++]=qat(t).q4.p;
			}
		case 3:
			if	(	ax(qat(t).q3.p) <= b.px
				&&	ax(qat(t).q3.p) >= b.nx
				&&	ay(qat(t).q3.p) <= b.py
				&&	ay(qat(t).q3.p) >= b.ny
				){
				arr[(*num)++]=qat(t).q3.p;
			}

		case 2:
			if	(	ax(qat(t).q2.p) <= b.px
				&&	ax(qat(t).q2.p) >= b.nx
				&&	ay(qat(t).q2.p) <= b.py
				&&	ay(qat(t).q2.p) >= b.ny
				){
				arr[(*num)++]=qat(t).q2.p;
			}

		case 1:
			if	(	ax(qat(t).q1.p) <= b.px
				&&	ax(qat(t).q1.p) >= b.nx
				&&	ay(qat(t).q1.p) <= b.py
				&&	ay(qat(t).q1.p) >= b.ny
				){
				arr[(*num)++]=qat(t).q1.p;
			}
		case 0:
			ret=malloc(sizeof(_point*)*(*num));
			for (int i=0;i<*num;++i) ret[i]=arr[i];
			return ret;
		default:
			int s[4]={0,0,0,0};
			_point** buff[4]={0,0,0,0};
			buff[0]=qt_query_range_sq( qat(t).q1.t, b , (int*)s   );
			buff[1]=qt_query_range_sq( qat(t).q2.t, b , (int*)s+1 );
			buff[2]=qt_query_range_sq( qat(t).q3.t, b , (int*)s+2 );
			buff[3]=qt_query_range_sq( qat(t).q4.t, b , (int*)s+3 );
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

void qt_free(qtidx t){
		arena_free();}
//	if (qat(t).len>4 && qat(t).len!=0){
//		qt_free(qat(t).q1.t);
//		qt_free(qat(t).q2.t);
//		qt_free(qat(t).q3.t);
//		qt_free(qat(t).q4.t);
//		free(t);
//	}
//	else{
//		free(t);
//		return;
//	}
//}//fn

