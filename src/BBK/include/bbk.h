#ifndef _BBK_H
#define _BBK_H

/* Framework libraries */
#include "compiler.h"
#include "fileio/fileio.h"
#include "graphics/graphics.h"
#include "intersect/intersect.h"
#include "math/mathlib.h"
#include "platform/platform.h"
#include "utils.h"

/* Framework engine modules */
#include "framework/gamestatemgr.h"
#include "framework/gamestate.h"
#include "framework/BObject.h"
#include "framework/baseobjs/DisParticle.h"
#include "framework/baseobjs/DisClothParticle.h"
#include "framework/baseobjs/DisClothParticle8.h"
#include "framework/baseobjs/perspcam.h"

namespace bbk
{
bool InitFramework();
void RunFramework();
} // namespace bbk

#endif /* _BBK_H */
