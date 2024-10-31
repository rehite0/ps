#define _point ball
#define CAP 4

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

//void qt_pvt_subdivide(qtree* t);
_point* qt_query_range_sq(qtree* t,bod b,int* size);
	//t [in] pointer to root node of tree to search
	//b [in] boundary of area where point need to be search
	//size [out]-size of buff
	//ret [out] pointer to an array of _point* whth 'size' elements
//void qt_free(qtree* t);

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
	if (	p->x > t->b.px
		||	p->x < t->b.nx
		||	p->y > t->b.py
		||	p->y < t->b.ny
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
			subdivide(t);
			++(t->len);
			break;
		default:
			++(t->len);
			break;
	}//switch
	return insert(t->i1.t,p)
		|| insert(t->i2.t,p)
		|| insert(t->i2.t,p)
		|| insert(t->i2.t,p)
		|| assert(0 && "insertion failed unexpected behaviour!");
}//fn

_point* qt_query_range_sq(qtree* t,bod b,int* num){
	_point* ret=NULL;
	if (	p->x > t->b.px
		||	p->x < t->b.nx
		||	p->y > t->b.py
		||	p->y < t->b.ny
		){
		*num=0;
		return NULL;
	}//if
	switch (t->len){
		case 4:
			if(ret==0){ 
				ret=malloc(sizeof(_point)*4);
				*num=4;	}
			if	(	t->q4.p.x <= b.px
				||	t->q4.p.x >= b.nx
				||	t->q4.p.y <= b.py
				||	t->q4.p.y >= b.ny
				){
				ret[*num-4]=t->q1.p;
			}
		case 3:
			if(ret==0){ 
				ret=malloc(sizeof(_point)*3);
				*num=3;	}
			if	(	t->q3.p.x <= b.px
				||	t->q3.p.x >= b.nx
				||	t->q3.p.y <= b.py
				||	t->q3.p.y >= b.ny
				){
				ret[*num-3]=t->q3.p;
			}

		case 2:
			if(ret==0){ 
				ret=malloc(sizeof(_point)*2);
				*num=2;	}
			if	(	t->q2.p.x <= b.px
				||	t->q2.p.x >= b.nx
				||	t->q2.p.y <= b.py
				||	t->q2.p.y >= b.ny
				){
				ret[*num-2]=t->q2.p;
			}

		case 1:
			if(ret==0){ 
				ret=malloc(sizeof(_point)*1);
				*num=1;	}
			if	(	t->q1.p.x <= b.px
				||	t->q1.p.x >= b.nx
				||	t->q1.p.y <= b.py
				||	t->q1.p.y >= b.ny
				){
				ret[*num-1]=t->q1.p;
			}
			return ret;
		case 0:
			*num=0;
			return NULL;
		default:
			int s[4]={0,0,0,0};
			_point* buff[4]={0,0,0,0};
			buff[0]=qt_query_range_sq( t->q1.t, b , (int*)s   );
			buff[1]=qt_query_range_sq( t->q2.t, b , (int*)s+1 );
			buff[2]=qt_query_range_sq( t->q3.t, b , (int*)s+2 );
			buff[3]=qt_query_range_sq( t->q4.t, b , (int*)s+3 );
			//merge buff
			*num=s[0]+s[1]+s[2]+s[3];
			ret=malloc(sizeof(_point)*(*num));
			assert(ret && "malloc failed");
			int k=0;
			for (int i=0 ; i<4 ;++i){
				for (int j=0 ; j<s[i] ; ++i){
					ret[k]=buff[i][j];
					++k;
				}//for j
				free(buff[i]);
			}//for i
			return ret;
	}//switch
	
}//fn
