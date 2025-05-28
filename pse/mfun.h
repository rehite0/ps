#pragma once
#include "vars.h"

#define abs2(o)		(o[0]*o[0] + o[1]*o[1])
#define mod(x) 		((x>0)? x:-1*x)
#define sig(x) 		((x>=0)? 1:-1)
#define vtop(v,p)	((p)-(v)*fdt)
#define ptov(pp,p)	(((p)-(pp))/fdt)
#define ckflg(v,f)	(((v)&(f))!=0)
