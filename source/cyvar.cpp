#include "cyvar.hpp"

float viewportwt;
float viewportwb;
float viewporth;
float gnrscl;
int topscrfade;
int botscrfade;
u32 topscrcol;
u32 botscrcol;
bool doAntiAlias;

Result CYvar::init(float scl){
	viewportwt = 400 * scl;
	viewportwb = 320 * scl;
	viewporth = 240 * scl;
	gnrscl = scl;
	return true;
}

Result CYvar::chvp(float scl){
	return init(scl);
}

Result CYvar::aliasing(bool doIt){
	doAntiAlias = doIt;
	return true;
}

void CYvar::updateCanvas(){
	u32 transFlags=	GX_TRANSFER_FLIP_VERT(false)|
					GX_TRANSFER_OUT_TILED(false)|
					GX_TRANSFER_RAW_COPY(false)|
					GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8)|
					GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8);
					
	switch(doAntiAlias){
		case true: transFlags=transFlags | GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_XY); break;
		case false: transFlags=transFlags | GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO); break;
	}
	//top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	//bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	
	top = C3D_RenderTargetCreate(viewporth, viewportwt, GPU_RB_RGBA8, GPU_RB_DEPTH16);
	C3D_RenderTargetClear(top, C3D_CLEAR_ALL, 0, 0);
	C3D_RenderTargetSetOutput(top, GFX_TOP, GFX_LEFT, transFlags);
	
	bottom = C3D_RenderTargetCreate(viewporth, viewportwb, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetClear(bottom, C3D_CLEAR_ALL, 0, 0);
	C3D_RenderTargetSetOutput(bottom, GFX_BOTTOM, GFX_LEFT, transFlags);

}
