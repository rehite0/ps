
//create mutex
pthread_rwlock_t rwl =PTHREAD_RWLOCK_INITIALIZER;

void* _qt_routine(void* ptr){
	int start=((int*)ptr)[0],stop=((int*)ptr)[1];

	for (int i=start;i<stop;++i){
		qtidx t=qt;
		BALL* p=ball_buff[i];
		pthread_rwlock_rdlock(&rwl);
		while(1){
			if (qat(t).len>4){
				if (	ax(p) <= qat(qat(t).q1.t).b.px
					&&	ax(p) >= qat(qat(t).q1.t).b.nx
					&&	ay(p) <= qat(qat(t).q1.t).b.py
					&&	ay(p) >  qat(qat(t).q1.t).b.ny
						) {
					++(qat(t).len);
					t=qat(t).q1.t;
					continue;
				}if (	ax(p) <  qat(qat(t).q2.t).b.px
					&&	ax(p) >= qat(qat(t).q2.t).b.nx
					&&	ay(p) <= qat(qat(t).q2.t).b.py
					&&	ay(p) >= qat(qat(t).q2.t).b.ny
						) {
					++(qat(t).len);
					t=qat(t).q2.t;
					continue;
				}if (	ax(p) <= qat(qat(t).q3.t).b.px
					&&	ax(p) >= qat(qat(t).q3.t).b.nx
					&&	ay(p) <  qat(qat(t).q3.t).b.py
					&&	ay(p) >= qat(qat(t).q3.t).b.ny
						) {
					++(qat(t).len);
					t=qat(t).q3.t;
					continue;
				}if (	ax(p) <= qat(qat(t).q4.t).b.px
					&&	ax(p) >  qat(qat(t).q4.t).b.nx
					&&	ay(p) <= qat(qat(t).q4.t).b.py
					&&	ay(p) >= qat(qat(t).q4.t).b.ny
						) {
					++(qat(t).len);
					t=qat(t).q4.t;
					continue;
				}
				break;///////////////////problem due to floating point precision may be
			}//if
			else{
				pthread_rwlock_unlock(&rwl);
				pthread_rwlock_wrlock(&rwl);
				if (qat(t).len>4){
					pthread_rwlock_unlock(&rwl);
					pthread_rwlock_rdlock(&rwl);
					continue;}

				switch (qat(t).len){
					case 0:
						qat(t).q1.p=p;
						++(qat(t).len);
						break;
					case 1:
						qat(t).q2.p=p;
						++(qat(t).len);
						break;
					case 2:
						qat(t).q3.p=p;
						++(qat(t).len);
						break;
					case 3:
						qat(t).q4.p=p;
						++(qat(t).len);
						break;

					case 4:
						qt_pvt_subdivide(t);
						++(qat(t).len);
						qt_insert(p,qat(t).q1.t)
						|| qt_insert(p,qat(t).q2.t)
						|| qt_insert(p,qat(t).q3.t)
						|| qt_insert(p,qat(t).q4.t);
						break;
				}//switch
				pthread_rwlock_unlock(&rwl);
				pthread_rwlock_rdlock(&rwl);
				break;
			}//else
		}//while
		pthread_rwlock_unlock(&rwl);
	}//for
}

int qt_insert_all(){
	int work_idx[mthreads+1][2];
	int num_work=BALL_COUNT/(mthreads+1);
	for (int i=0;i<mthreads;++i){
		work_idx[i][0]=i*num_work;
		work_idx[i][1]=(i+1)*num_work;
		pthread_create(&(tid[i]),NULL,&_qt_routine,(void*)(work_idx[i]));
	}
	work_idx[mthreads][0]=mthreads*num_work;
	work_idx[mthreads][1]=BALL_COUNT;
	_qt_routine(work_idx[mthreads]);
	for (int i=0;i<mthreads;++i)
		pthread_join(tid[i],NULL);
}
		
