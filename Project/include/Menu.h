/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Menu header file
// Created 30/6/02
// Jason Boettcher


#ifndef __MENU_H__
#define __MENU_H__


#include "CWidget.h"
#include "CGuiLayout.h"
#include "CTitleButton.h"
#include "CButton.h"
#include "CScrollbar.h"
#include "CListview.h"
#include "CTextbox.h"
#include "CLabel.h"
#include "CSlider.h"
#include "CCheckbox.h"
#include "CInputBox.h"
#include "CBrowser.h"
#include "CCombobox.h"
#include "CMenu.h"


// Menu sockets
#define		SCK_LAN		0
#define		SCK_NET		1

// Menu types
enum {
	MNU_MAIN=0,
	MNU_LOCAL,
	MNU_NETWORK,
	MNU_PLAYER,
	MNU_OPTIONS,
	MNU_MAPED
};


// Sub title id's
enum {
	SUB_LOCAL=0,
	SUB_NETWORK,
	SUB_PLAYER,
	SUB_MAPED,
	SUB_OPTIONS,
	SUB_LOBBY
};


// Message box types
enum {
	LMB_OK = 0,
	LMB_YESNO,

	// Return types
	MBR_OK=0,
	MBR_YES,
	MBR_NO
};


// Buttons
enum {
	BUT_MAIN=0,
	BUT_INTERNET,
	BUT_LAN,
	BUT_HOST,
	BUT_CHAT,
	BUT_BACK,
	BUT_OK,
	BUT_CREATE,
	BUT_NEW,
	BUT_RANDOM,
	BUT_LOAD,
	BUT_SAVE,
	BUT_QUIT,
	BUT_CANCEL,
	BUT_QUITGAME,
	BUT_PLAYAGAIN,
	BUT_YES,
	BUT_NO,
	BUT_CONTROLS,
	BUT_GAME,
	BUT_SYSTEM,
	BUT_APPLY,
	BUT_RESUME,
	BUT_NEWPLAYER,
	BUT_VIEWPLAYERS,
	BUT_DELETE,
	BUT_START,
	BUT_JOIN,
	BUT_REFRESH,
	BUT_ADD,
	BUT_READY,
	BUT_LEAVE,
	BUT_GAMESETTINGS,
	BUT_UPDATELIST,
    BUT_WEAPONOPTIONS,
    BUT_RESET,
    BUT_DEFAULT
};

// Menu structure
typedef struct {

	// Graphics
	//SDL_Surface		*bmpMainBack;
    //SDL_Surface		*bmpMainBack_lg;
    SDL_Surface		*bmpMainBack_wob;
	SDL_Surface		*bmpBuffer;
	SDL_Surface		*bmpScreen;
	SDL_Surface		*bmpMsgBuffer;
    SDL_Surface     *bmpMiniMapBuffer;
	
	SDL_Surface		*bmpLieroXtreme;
	SDL_Surface		*bmpMainTitles;
	SDL_Surface		*bmpTitles;
	SDL_Surface		*bmpSubTitles;
	SDL_Surface		*bmpButtons;
	SDL_Surface		*bmpCheckbox;
	SDL_Surface		*bmpInputbox;

	SDL_Surface		*bmpMainLocal;
	SDL_Surface		*bmpMainNet;
	SDL_Surface		*bmpLobbyState;
	SDL_Surface		*bmpHost;
	SDL_Surface		*bmpConnectionSpeeds[4];
	SDL_Surface		*bmpSpeech;
	SDL_Surface		*bmpTeamColours[4];
    SDL_Surface     *bmpHandicap;

	SDL_Surface		*bmpAI;
	SDL_Surface		*bmpWorm;

	SDL_Surface		*bmpMapEdTool;

	SDL_Surface		*bmpMouse;

	// Other
	int				iMenuRunning;
	int				iMenuType;

	// Map Editor
	int				iEditMode;
	int				iCurHole;
	int				iCurStone;
	int				iCurMisc;
	int				iCurDirt;

	// Socket for pinging
	NLsocket		tSocket[2];

} menu_t;


// Net menu types
enum {
	net_main=0,
	net_internet,
	net_lan,
	net_host,
	net_join,
	net_chat
};


#define     MAX_QUERIES     3


// Server structure
typedef struct server_s {
	bool	bIgnore;
	bool	bProcessing;
    bool    bManual;
	int		nPings;
	int		nQueries;
	bool	bgotPong;
	bool	bgotQuery;
	float	fLastPing;
	float	fLastQuery;
    float   fQueryTimes[MAX_QUERIES+1];

	// Server address
	char	szAddress[256];
	NLaddress	sAddress;

	// Server details
	char	szName[32];
	int		nState;
	int		nNumPlayers;
	int		nMaxPlayers;
	int		nPing;

    struct	server_s	*psPrev;
	struct	server_s	*psNext;
} server_t;


// Local player structure
typedef struct {
    bool        bUsed;
    profile_t   *psProfile;
    int         nTeam;
    int         nHealth;
} local_ply_t;




// Menu globals
extern	menu_t	*tMenu;
extern	int		*iGame;
extern	int		iNetMode;
extern	int		iJoinMenu;
extern	int		iHostType;
extern	int		iSkipStart;


// Routines
int		Menu_Initialize(int *game);
void	Menu_Shutdown(void);
void	Menu_Start(void);
void	Menu_RedrawMouse(int total);
void	Menu_Loop(void);
void    Menu_SetSkipStart(int s);
void	Menu_DrawSubTitle(SDL_Surface *bmpDest, int id);
void    Menu_DrawSubTitleAdv(SDL_Surface *bmpDest, int id, int y);
void	Menu_DrawBox(SDL_Surface *bmpDest, int x, int y, int x2, int y2);
void    Menu_DrawBoxInset(SDL_Surface *bmpDest, int x, int y, int x2, int y2);
void    Menu_DrawWinButton(SDL_Surface *bmpDest, int x, int y, int w, int h, bool down);
int		Menu_LoadWormGfx(profile_t *ply);
int		Menu_MessageBox(char *sTitle, char *sText, int type);
void	Menu_FillLevelList(CCombobox *cmb, int random);
void    Menu_redrawBufferRect(int x, int y, int w, int h);

// Server list
void		Menu_SvrList_Clear(void);
void        Menu_SvrList_ClearAuto(void);
void		Menu_SvrList_Shutdown(void);
void		Menu_SvrList_PingLAN(void);
server_t	*Menu_SvrList_AddServer(char *address, bool bManual);
server_t    *Menu_SvrList_FindServerStr(char *szAddress);
void        Menu_SvrList_RemoveServer(char *szAddress);
bool		Menu_SvrList_Process(void);
int			Menu_SvrList_ParsePacket(CBytestream *bs, NLsocket sock);
server_t	*Menu_SvrList_FindServer(NLaddress *addr);
void		Menu_SvrList_PingServer(server_t *svr);
void		Menu_SvrList_QueryServer(server_t *svr);
void		Menu_SvrList_ParseQuery(server_t *svr, CBytestream *bs);
void		Menu_SvrList_RefreshList(void);
void        Menu_SvrList_RefreshServer(server_t *s);
void		Menu_SvrList_UpdateList(void);
void		Menu_SvrList_FillList(CListview *lv);
void        Menu_SvrList_SaveList(char *szFilename);
void        Menu_SvrList_LoadList(char *szFilename);
void        Menu_SvrList_DrawInfo(char *szAddress);

// Main menu
void	Menu_MainInitialize(void);
void	Menu_MainShutdown(void);
void	Menu_MainFrame(void);
void	Menu_MainDrawTitle(int x, int y, int id, int selected);


// Local menu
void	Menu_LocalInitialize(void);
void	Menu_LocalFrame(void);
void	Menu_LocalAddProfiles(void);
void	Menu_LocalStartGame(void);
int		Menu_LocalCheckPlaying(int index);
int		Menu_LocalGetTeam(int count);
void	Menu_Local_FillModList( CCombobox *cb );
void	Menu_LocalShowMinimap(bool bReload);
void    Menu_LocalDrawPlayingList(void);


// Player menu
void	Menu_PlayerInitialize(void);
void	Menu_PlayerFrame(void);
void    Menu_Player_NewPlayerInit(void);
void    Menu_Player_ViewPlayerInit(void);
void	Menu_Player_NewPlayer(int mouse);
void	Menu_Player_ViewPlayers(int mouse);
void	Menu_Player_AddPlayer(char *sName, Uint8 r, Uint8 g, Uint8 b);
void    Menu_Player_DrawWormImage(SDL_Surface *bmpDest, int Frame, int dx, int dy, int ColR, int ColG, int ColB);
void	Menu_Player_DeletePlayer(int index);
void    Menu_Player_FillSkinCombo(CCombobox *cb);

// Map editor
int		Menu_MapEdInitialize(void);
void	Menu_MapEdFrame(SDL_Surface *bmpDest, int process);
void	Menu_MapEd_New(void);
void	Menu_MapEd_LoadSave(int save);
int		Menu_MapEd_OkSave(char *szFilename);

// Game Settings
void	Menu_GameSettings(void);
bool	Menu_GameSettings_Frame(void);
void	Menu_GameSettings_GrabInfo(void);
void    Menu_GameSettings_Default(void);


// Weapons Restrictions
void    Menu_WeaponsRestrictions(char *szMod);
bool    Menu_WeaponsRestrictions_Frame(void);


// Options
int		Menu_OptionsInitialize(void);
void	Menu_OptionsShutdown(void);
void	Menu_OptionsFrame(void);
void	Menu_OptionsWaitInput(int ply, char *name, CInputbox *b);


// Main net
int		Menu_NetInitialize(void);
void	Menu_Net_GotoHostLobby(void);
void	Menu_Net_GotoJoinLobby(void);
void	Menu_NetFrame(void);


// Net::Main menu
int		Menu_Net_MainInitialize(void);
void	Menu_Net_MainFrame(int mouse);
void	Menu_Net_MainShutdown(void);


// Net::Host menu
int		Menu_Net_HostInitialize(void);
void	Menu_Net_HostShutdown(void);
void	Menu_Net_HostFrame(int mouse);
void	Menu_Net_HostPlyFrame(int mouse);

int		Menu_Net_HostLobbyInitialize(void);
void    Menu_Net_HostLobbyDraw(void);
void    Menu_Net_HostLobbyCreateGui(void);
void	Menu_Net_HostGotoLobby(void);
void	Menu_Net_HostLobbyFrame(int mouse);
void	Menu_HostDrawLobby(SDL_Surface *bmpDest);
void	Menu_HostShowMinimap(void);
void	Menu_Net_HostDeregister(void);


// Net::LAN menu
int		Menu_Net_LANInitialize(void);
void	Menu_Net_LANShutdown(void);
void	Menu_Net_LANFrame(int mouse);
void	Menu_Net_LANSendPing(void);
void	Menu_Net_LANJoinServer(char *sAddress);
void    Menu_Net_LanShowServer(char *szAddress);


// Net::Joining menu
int		Menu_Net_JoinInitialize(char *sAddress);
void	Menu_Net_JoinShutdown(void);
void	Menu_Net_JoinFrame(int mouse);

int		Menu_Net_JoinInitializePlayers(void);
void	Menu_Net_JoinPlayersFrame(int mouse);

int		Menu_Net_JoinConnectionInitialize(void);
void	Menu_Net_JoinConnectionFrame(int mouse);

int		Menu_Net_JoinLobbyInitialize(void);
void    Menu_Net_JoinDrawLobby(void);
void    Menu_Net_JoinLobbyCreateGui(void);
void	Menu_Net_JoinGotoLobby(void);
void	Menu_Net_JoinLobbyFrame(int mouse);


// Net::Internet menu
int		Menu_Net_NETInitialize(void);
void	Menu_Net_NETShutdown(void);
void	Menu_Net_NETFrame(int mouse);
void	Menu_Net_NETJoinServer(char *sAddress);
void	Menu_Net_NETAddServer(void);
void	Menu_Net_NETUpdateList(void);
void	Menu_Net_NETParseList(void);
void    Menu_Net_NETShowServer(char *szAddress);

#endif  //  __MENU_H__