#define _point ball
#define CAP 4

typedef struct _bod {
	float px,nx,py,ny
} bod;

typedef struct _qtree{
	int len;
	bod b;
	union p_and_qt {
		_point* p;
		struct _qtree* t;
	} q1,q2,q3,q4;
}qtree;

//qtree* qt_create();
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

int qt_insert(_point* p,qtree* t){
	if (	p->x > t->b.px
		||	p->x < t->b.nx
		||	p->y > t->b.py
		||	p->y < t->b.ny
			) return 0;
	switch (t->len){
		case 0:

			return 1;
		case 1:
			return 1;
		case 2:
			return 1;
		case 3:
			return 1;
		case 4:
			subdivide(t);
		default:
			++(t->len);
	}//switch
	return insert(t->i1.t,p)
		|| insert(t->i2.t,p)
		|| insert(t->i2.t,p)
		|| insert(t->i2.t,p)
		|| assert(0 && "insertion failed unexpected behaviour!");
}//fn
