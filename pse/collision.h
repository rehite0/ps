#pragma once
#include "gtypes.h"
#include "globals.h"

extern void coll_resolver(BALL* a,BALL* b);
extern void coll_dect();
#if defined(use_qt) && !defined(mthreads)
extern 	void coll_dect_qt();
#endif
#ifdef mthreads
extern 	void coll_dect_mt();
#endif

