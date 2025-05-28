#pragma once
//globel constants & flags
#include "globals.h"
//macro functions
#include "mfun.h"
//types
#include "gtypes.h"

#ifdef use_qt
	#include "qt/quadtree.h"
#endif

#include "forces_and_constrains.h"
#include "model.h"
#include "vars.h"

//	#include "qt/mt_qt2.h" ///////dependent in project

#include "collision.h"
