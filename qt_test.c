typedef struct _ball{float x,y;} ball;
#include "quadtree.h"

int main(){
	ball a1 ={0.5,0.5}
		,a2 ={0.1,0.1}
		,a3 ={-0.1,0.5}
		,a4 ={-0.5,-0.5}
		,a5 ={0.0,0.0}
		,a6 ={0.0,0.0}
		,a7 ={0.0,0.0}
		,a8 ={0.0,0.0}
		,a9 ={0.0,0.0}
		,a10={0.0,0.0};
	qtree* q=qt_create((bod){1.0,-1.0,1.0,-1.0});
	qt_insert(&a1,q);
	qt_insert(&a2,q);
	qt_insert(&a3,q);
	qt_insert(&a4,q);
	qt_insert(&a5,q);
//	printf("pointer:%p,%p,%p,%p,%p,%p\n",&a1,&a2,&a3,&a4,&a5,&a6,&a7);
//	printf("%p:%i:%p:%p:%p:%p\n",q,q->len,q->q1.p,q->q2.p,q->q3.p,q->q4.p);
	int n;
	ball** b=qt_query_range_sq(q,(bod){0.6,-0.1,0.6,-0.2},&n);
	printf("%i\n",n);
//	for(int i=0 ;i<n;i++){
//		printf("%f:%f\n",	b[i]->x,b[i]->y);
//	}
	qt_free(q);
}
