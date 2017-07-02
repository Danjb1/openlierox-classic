/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Options
// Created 21/7/02
// Jason Boettcher


#ifndef __OPTIONS_H__
#define __OPTIONS_H__


// Setup input id's
enum {
	// Movement
	SIN_UP=0,
	SIN_DOWN,
	SIN_LEFT,
	SIN_RIGHT,

	SIN_SHOOT,
	SIN_JUMP,
	SIN_SELWEAP,
	SIN_ROPE
};

// General controls
enum {
	SIN_CHAT,
    SIN_SCORE
};


// Network speed types
enum {
	NST_MODEM=0,
	NST_ISDN,
	NST_LAN,
	NST_LOCAL			// Hidden speed, only for local games
};



// Options structure
typedef struct {

	// Video
	int		iFullscreen;
	int		iShowFPS;
	int		iFiltered;

	// Network
	int		iNetworkPort;
	int		iNetworkSpeed;

	// Audio
	int		iSoundOn;
	int		iSoundVolume;

	// Controls
	char	sPlayer1Controls[32][8];
	char	sPlayer2Controls[32][8];
	char	sGeneralControls[32][5];

	// Game
	int		iBloodAmount;
	int		iShadows;
	int		iParticles;
	int		iOldSkoolRope;

    // Advanced
    int     nMaxFPS;

	// Last used game details
	struct {
		int		iLives;
		int		iKillLimit;
		int		iTimeLimit;
		int		iTagLimit;
		int		iLoadingTime;
		int		iBonusesOn;
		int		iShowBonusName;
		int		iMaxPlayers;
		char	sServerName[32];
		char	sMapName[128];
        int     nGameType;
        char    szModName[128];
        char    szPassword[32];
		int		bRegServer;
	} tGameinfo;

} options_t;


// Option Routines
int		LoadOptions(void);
void    SaveOptions(void);
void	ShutdownOptions(void);




#endif  //  __OPTIONS_H__