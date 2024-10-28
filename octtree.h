#define _point ball
#define CAP 4

typedef struct _bod {
	float px,nx,py,ny
} bod;	// 
typedef struct _qtree{
	_points p[4];
	bod
}qtree;

void qt_insert(_point* b,qtree* t);
void qt_free(qtree* t);
