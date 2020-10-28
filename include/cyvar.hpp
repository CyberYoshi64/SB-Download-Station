#ifndef CYVAR_HPP
#define CYVAR_HPP
#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

extern C3D_RenderTarget* top;
extern C3D_RenderTarget* bottom;
extern float viewportwt;
extern float viewportwb;
extern float viewporth;
extern float gnrscl;
extern int topscrfade;
extern int botscrfade;
extern u32 topscrcol;
extern u32 botscrcol;
extern bool doAntiAlias;

namespace CYvar
{
	Result init(float scl);
	Result chvp(float scl); // synonym for init()
	Result aliasing(bool doIt);
	void updateCanvas(void);
}

#endif //CYVAR_HPP
