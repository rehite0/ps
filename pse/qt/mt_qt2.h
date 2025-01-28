#define _nt 4
int _leaf=1;
qtidx* _leafbuff=NULL;

void* _qt_routine(void* ptr);
int qt_insert_all(qtidx qt);
int qt_insert_mt(_point* p,qtidx t);
void qt_pvt_subdivide_mt(qtidx t);

void* _qt_routine(void* ptr){
	struct arg{
		int start;
		int stop;
		int num;
		qtidx* ts;
		};
	struct arg* a=(struct arg*)ptr;
	for (int i=a->start;i<a->stop;++i){
		for (int j=0;j<a->num;++j){
			if (qt_insert_mt(ball_buff[i],a->ts[j])) break;
		}
	}
	free(a->ts);
}

int qt_insert_all(qtidx qt){
	int idx;
	for (idx=0;(_leaf<(mthreads+1)*_nt)&&(idx<BALL_COUNT);++idx){
		qt_insert_mt(ball_buff[idx],qt);
	}
	if (_leaf<(mthreads+1)*_nt) return 0;

	struct arg{
		int start;
		int stop;
		int num;
		qtidx* ts;
		}a[mthreads+1];
	int nt=_leaf/(mthreads+1);
	for (int i=0;i<mthreads;++i){
		a[i].start=idx;
		a[i].stop=BALL_COUNT;
		a[i].num=nt;
		a[i].ts=malloc(sizeof(qtidx)*nt);
		for (int j=nt*i;j<nt*(i+1);++j) a[i].ts[j-(nt*i)]=_leafbuff[j];
		pthread_create(&(tid[i]),NULL,&_qt_routine,a+i);
	}
	a[mthreads].start=idx;
	a[mthreads].stop=BALL_COUNT;
	a[mthreads].num=_leaf-(mthreads*nt);
	a[mthreads].ts=malloc(sizeof(qtidx)*a[mthreads].num);
	for (int j=nt*mthreads;j<_leaf;++j) a[mthreads].ts[j-(nt*mthreads)]=_leafbuff[j];

	_qt_routine(a+mthreads);
	for (int i=0;i<mthreads;++i)
		pthread_join(tid[i],NULL);
	//sync_len();
	_leaf=1;
	free(_leafbuff);
	_leafbuff=NULL;
}



int qt_insert_mt(_point* p,qtidx t){
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
			qt_pvt_subdivide_mt(t);
			_leaf+=3;
			++(qat(t).len);
			break;
		default:
			++(qat(t).len);
			break;
	}//switch
	return qt_insert_mt(p,qat(t).q1.t)
		|| qt_insert_mt(p,qat(t).q2.t)
		|| qt_insert_mt(p,qat(t).q3.t)
		|| qt_insert_mt(p,qat(t).q4.t);
}//fn

void qt_pvt_subdivide_mt(qtidx t){
	fprintf(stdout,"hi ");////////////////////////////////////
	_point* p1=qat(t).q1.p;
	_point* p2=qat(t).q2.p;
	_point* p3=qat(t).q3.p;
	_point* p4=qat(t).q4.p;
	bod b=qat(t).b;
	qtidx q1=qt_create((bod){b.px				,(b.px+b.nx)/2	,b.py			,(b.py+b.ny)/2});
	qtidx q2=qt_create((bod){(b.px+b.nx)/2		,b.nx			,b.py			,(b.py+b.ny)/2});
	qtidx q3=qt_create((bod){(b.px+b.nx)/2		,b.nx			,(b.py+b.ny)/2	,b.ny});
	qtidx q4=qt_create((bod){b.px				,(b.px+b.nx)/2	,(b.py+b.ny)/2	,b.ny});

	   qt_insert_mt(p1,q1)
	|| qt_insert_mt(p1,q2)
	|| qt_insert_mt(p1,q3)
	|| qt_insert_mt(p1,q4);
	   qt_insert_mt(p2,q1)
	|| qt_insert_mt(p2,q2)
	|| qt_insert_mt(p2,q3)
	|| qt_insert_mt(p2,q4);
	   qt_insert_mt(p3,q1)
	|| qt_insert_mt(p3,q2)
	|| qt_insert_mt(p3,q3)
	|| qt_insert_mt(p3,q4);
	   qt_insert_mt(p4,q1)
	|| qt_insert_mt(p4,q2)
	|| qt_insert_mt(p4,q3)
	|| qt_insert_mt(p4,q4);
	
	qat(t).q1.t=q1;
	qat(t).q2.t=q2;
	qat(t).q3.t=q3;
	qat(t).q4.t=q4;
	
	if (_leafbuff==NULL){
		_leafbuff=realloc(_leafbuff,4*sizeof(qtidx));
		_leafbuff[0]=q1;
		_leafbuff[1]=q2;
		_leafbuff[2]=q3;
		_leafbuff[3]=q4;
		return;
	}
	for (int i=0;i<_leaf;++i){
		if (_leafbuff[i]==t)
			_leafbuff[i]=q1;
	}
	_leafbuff=realloc(_leafbuff,(_leaf+3)*sizeof(qtidx));
	_leafbuff[_leaf]=q2;
	_leafbuff[_leaf+1]=q3;
	_leafbuff[_leaf+2]=q4;

}//fn
