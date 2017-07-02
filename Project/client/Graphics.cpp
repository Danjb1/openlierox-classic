/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Graphics header file
// Created 30/6/02
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"


gfxgui_t	gfxGUI;
gfxgame_t	gfxGame;

///////////////////
// Load the graphics
int LoadGraphics(void)
{
	int i;
	LOAD_IMAGE_BPP(gfxGUI.bmpMouse[0], "data/frontend/mouse.png");
	LOAD_IMAGE_BPP(gfxGUI.bmpMouse[1], "data/frontend/mouse_hand.png");
	LOAD_IMAGE_BPP(gfxGUI.bmpMouse[2], "data/frontend/mouse_text.png");

	LOAD_IMAGE_BPP(gfxGUI.bmpScrollbar,"data/frontend/scrollbar.png");
	LOAD_IMAGE_BPP(gfxGUI.bmpSliderBut,"data/frontend/sliderbut.png");

	LOAD_IMAGE_BPP(gfxGame.bmpCrosshair,"data/gfx/crosshair.bmp");
	LOAD_IMAGE_BPP(gfxGame.bmpMuzzle,"data/gfx/muzzle.bmp");
	LOAD_IMAGE_BPP(gfxGame.bmpExplosion,"data/gfx/explosion.png");
	LOAD_IMAGE_BPP(gfxGame.bmpSmoke,"data/gfx/smoke.png");
	LOAD_IMAGE_BPP(gfxGame.bmpChemSmoke,"data/gfx/chemsmoke.png");
	LOAD_IMAGE_BPP(gfxGame.bmpSpawn,"data/gfx/spawn.png");
	LOAD_IMAGE_BPP(gfxGame.bmpHook,"data/gfx/hook.bmp");
	LOAD_IMAGE(gfxGame.bmpGameover,"data/gfx/gameover.png");
	LOAD_IMAGE_BPP(gfxGame.bmpInGame,"data/gfx/ingame.png");
	LOAD_IMAGE(gfxGame.bmpScoreboard,"data/gfx/scoreboard.png");
    LOAD_IMAGE_BPP(gfxGame.bmpViewportMgr,"data/gfx/viewportmgr.png");
	LOAD_IMAGE_BPP(gfxGame.bmpSparkle, "data/gfx/sparkle.png");
	LOAD_IMAGE(gfxGame.bmpInfinite,"data/gfx/infinite.png");
	LOAD_IMAGE_BPP(gfxGame.bmpLag, "data/gfx/lag.png");

	LOAD_IMAGE_BPP(gfxGame.bmpBonus, "data/gfx/bonus.png");
	LOAD_IMAGE_BPP(gfxGame.bmpHealth, "data/gfx/health.png");


	if(!tLX->cFont.Load("data/gfx/font.png",true,15))
		return false;
	if(!tLX->cOutlineFont.Load("data/gfx/out_font.png",false,15))
		return false;
	if(!tLX->cOutlineFontGrey.Load("data/gfx/out_fontgrey.png",false,15))
		return false;

	// Set the colour keys
	Uint32 pink = MakeColour(255,0,255);
	for(i=0;i<3;i++)
		SDL_SetColorKey(gfxGUI.bmpMouse[i], SDL_SRCCOLORKEY, pink);

	SDL_SetColorKey(gfxGame.bmpCrosshair, SDL_SRCCOLORKEY, pink);
	SDL_SetColorKey(gfxGame.bmpMuzzle, SDL_SRCCOLORKEY, pink);
	SDL_SetColorKey(gfxGame.bmpExplosion, SDL_SRCCOLORKEY, pink);
	SDL_SetColorKey(gfxGame.bmpSmoke, SDL_SRCCOLORKEY, pink);
	SDL_SetColorKey(gfxGame.bmpChemSmoke, SDL_SRCCOLORKEY, pink);
	SDL_SetColorKey(gfxGame.bmpSpawn, SDL_SRCCOLORKEY, pink);
	SDL_SetColorKey(gfxGame.bmpHook, SDL_SRCCOLORKEY, pink);
	SDL_SetColorKey(gfxGame.bmpBonus, SDL_SRCCOLORKEY, pink);
	SDL_SetColorKey(gfxGame.bmpHealth, SDL_SRCCOLORKEY, pink);
	SDL_SetColorKey(gfxGame.bmpSparkle, SDL_SRCCOLORKEY, pink);
    SDL_SetColorKey(gfxGame.bmpViewportMgr, SDL_SRCCOLORKEY, pink);


	return true;
}


///////////////////
// Shutdown the graphics
void ShutdownGraphics(void)
{
	tLX->cFont.Shutdown();
	tLX->cOutlineFont.Shutdown();
	tLX->cOutlineFontGrey.Shutdown();
}