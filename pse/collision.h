#pragma once
#include "gtypes.h"
#include "globals.h"

void coll_resolver(BALL* a,BALL* b);
void coll_dect();
#if defined(use_qt) && !defined(mthreads)
	void coll_dect_qt();
#endif
#ifdef mthreads
	void coll_dect_mt();
#endif

