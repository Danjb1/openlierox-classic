/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Profile system
// Created 13/8/02
// Jason Boettcher


#ifndef __PROFILESYSTEM_H__
#define __PROFILESYSTEM_H__


#define		PROFILE_VERSION		5

#define		PRF_HUMAN			0
#define		PRF_COMPUTER		1

// AI Difficulty
#define		AI_EASY			0
#define		AI_MEDIUM		1
#define		AI_HARD			2
#define		AI_XTREME		3



// Player profile structure
typedef struct profile_s {
	int				iID;

	int				iType;
	char			sName[32];
    char            szSkin[128];
    int             nDifficulty;
	char			sUsername[16];
	char			sPassword[16];
	Uint8			R,G,B;
	char			sWeaponSlots[5][64];
	int				iTeam;
	SDL_Surface		*bmpWorm;

	struct profile_s *tNext;
} profile_t;



int		LoadProfiles(void);
void	ShutdownProfiles(void);

void    AddDefaultPlayers(void);
void	SaveProfile(FILE *fp, profile_t *p);
void    AddProfile(char *name, char *skin, char *username, char *password,  int R, int G, int B, int type, int difficulty);
void	LoadProfile(FILE *fp, int id);
int		FindProfileID(void);
void	DeleteProfile(int id);
int		LoadProfileGraphics(profile_t *p);

profile_t *GetProfiles(void);
profile_t *FindProfile(int id);

// General function for all to use
SDL_Surface *LoadSkin(char *szSkin, int colR, int colG, int colB);




#endif  //  __PROFILESYSTEM_H__