/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Game header file
// Created 28/6/02
// Jason Boettcher


#ifndef __LIEROX_H__
#define __LIEROX_H__

#define		LX_PORT			23400
#define		SPAWN_HOLESIZE	4
#define		LX_VERSION		0.55
#define		LX_ENDWAIT		9.0f


// Game types
enum {
	GMT_DEATHMATCH,
	GMT_TEAMDEATH,
	GMT_TAG,
    GMT_DEMOLITION
};


const float	D2R(1.745329e-2f); // degrees to radians
const float	R2D(5.729578e+1f); // radians to degrees

#define DEG2RAD(a)  (a * D2R)
#define RAD2DEG(a)  (a * R2D)


// Game includes
#include "2xsai.h"
#include "ProfileSystem.h"
#include "Networking.h"
#include "CGameScript.h"
#include "CChatbox.h"
#include "Frame.h"
#include "CViewport.h"
#include "CMap.h"
#include "CSimulation.h"
#include "CNinjaRope.h"
#include "Command.h"
#include "CBonus.h"
#include "CWpnRest.h"
#include "CWorm.h"
#include "CProjectile.h"
#include "CShootList.h"
#include "Entity.h"
#include "CWeather.h"
#include "CClient.h"
#include "CServer.h"
#include "Sounds.h"
#include "Graphics.h"
#include "Protocol.h"
#include "Options.h"


// LieroX structure
typedef struct {
	float	fCurTime;
	float	fDeltaTime;
	CFont	cFont;
	CFont	cOutlineFont;
	CFont	cOutlineFontGrey;

	int		iQuitGame;
	int		iQuitEngine;

	int		debug_int;
	float	debug_float;
	CVec	debug_pos;


	char	debug_string[32];
} lierox_t;


// Game types
enum {
	GME_LOCAL=0,
	GME_HOST,
	GME_JOIN
};



// Game structure
typedef struct {
	int			iGameType;		// Local, remote, etc
	int			iGameMode;		// DM, team DM, etc
	char		sModName[128];
	char		sMapname[128];
    char        sPassword[32];
    maprandom_t sMapRandom;
	int			iLoadingTimes;
	char		sServername[32];
	bool		bRegServer;

	int			iLives;
	int			iKillLimit;
	int			iTimeLimit;
	int			iTagLimit;
	int			iBonusesOn;
	int			iShowBonusName;
	
	int			iNumPlayers;
	profile_t	*cPlayers[MAX_WORMS];
} game_t;


extern	lierox_t	*tLX;
extern	game_t		tGameInfo;
extern	CVec		vGravity;
extern	options_t	*tLXOptions;
extern	CServer		*cServer;
extern	CClient		*cClient;
extern  int         nDisableSound;




// Main Routines
void    ParseArguments(int argc, char *argv[]);
int		InitializeLieroX(void);
void	StartGame(void);
void	ShutdownLieroX(void);
void	GameLoop(void);
void	QuittoMenu(void);



// Miscellanous routines
int		CheckCollision(CVec pos, CVec vel, int width, int height, CMap *map);
void	ConvertTime(float time, int *hours, int *minutes, int *seconds);
int 	CarveHole(CMap *cMap, CVec pos);
void	StripQuotes(char *dest, char *src);
void    lx_strncpy(char *dest, char *src, int count);
bool    MouseInRect(int x, int y, int w, int h);
char    *StripLine(char *szLine);
char    *TrimSpaces(char *szLine);


#endif  //  __LIEROX_H__