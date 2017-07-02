/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Worm class
// Created 28/6/02
// Jason Boettcher


#ifndef __CWORM_H__
#define __CWORM_H__


#define		MAX_WEAPONSLOTS		10
#define		MAX_WORMS			32

// Direction
#define		DIR_LEFT			0
#define		DIR_RIGHT			1

#define		WRM_OUT				-1
#define		WRM_UNLIM			-2


// Weapon slot structure
typedef struct {
	weapon_t	*Weapon;
	int			SlotNum;
	float		Charge;
	int			Reloading;
	float		LastFire;
} wpnslot_t;



class CClient;
class CBonus;


// Lobby worm details
#define		LBY_OPEN	0
#define		LBY_CLOSED	1
#define		LBY_USED	2

typedef struct {
	int			iType;
    int         iTeam;
	int			iHost;
	int			iReady;
} lobbyworm_t;



/*

    Artificial Intelligence


*/

// AI states
enum {
    AI_THINK,
    //AI_FINDTARGET,
    AI_MOVINGTOTARGET,
    AI_AIMING,
    AI_SHOOTING
};

// Target types
enum {
    AIT_NONE,
    AIT_WORM,
    AIT_BONUS,
    AIT_POSITION
};


// Path finding node
typedef struct ai_node_s {

    int     nX, nY;
    int     nCost;
    int     nFound;
    int     nCount;
    //int     *nOpenClose;

    struct ai_node_s    *psParent;
    struct ai_node_s    *psPath;
    struct ai_node_s    *psChildren[8];

} ai_node_t;



class CWorm {
public:
	// Constructor
	CWorm() {
		Clear();
	}


private:
	// Attributes

	// General
	int			iUsed;
	int			iID;
	int			iType;
	int			iLocal;
	int			iTeam;
	char		sName[32];
	Uint8		iColComps[3];
    char        szSkin[128];
	uint		iColour;
	int			iRanking;

	// Client info
	int			iClientID;
	int			iClientWormID;

	// Input
	CInput		cUp, cDown, cLeft, cRight,
				cShoot, cJump, cSelWeapon, cInpRope;

	// Simulation
	worm_state_t tState;
	CVec		vPos;
	CVec		vOldPos;
	CVec		vVelocity;
	int			iOnGround;	

	// Score
	int			iKills;
	int			iDeaths;
	int			iSuicides;

	// Game
	float		fLoadingTime;
	int			iDrawMuzzle;
	int			iHealth;
	int			iLives;
	int			iAlive;
	float		fTimeofDeath;
	int			iDirection;
	int			iGotTarget;
	float		fAngle;
    float       fAngleSpeed;
	int			iCarving;
	float		fFrame;
	CNinjaRope	cNinjaRope;
	profile_t	*tProfile;
	float		fRopeTime;

	int			iHooked;
	CWorm		*pcHookWorm;

	int			iRopeDown;
	int			iRopeDownOnce;

	int			iTagIT;
	float		fTagTime;
	float		fLastSparkle;

    int         iDirtCount;

	float		fLastBlood;


	// Owner client
	CClient		*cClient;

	// Network
	float		fFrameTimes[NUM_FRAMES];
	lobbyworm_t tLobbyState;



	// Graphics
	SDL_Surface	*bmpWorm;
	SDL_Surface	*bmpGibs;
	SDL_Surface	*bmpPic;
    SDL_Surface *bmpShadowPic;
	//CViewport	*pcViewport;


	// Arsenal
	int			iWeaponsReady;
	int			iGameReady;
	CGameScript	*cGameScript;
    CWpnRest    *cWeaponRest;
	int			iNumWeaponSlots;
	int			iCurrentWeapon;
	wpnslot_t	tWeapons[MAX_WEAPONSLOTS];
    
    // Force the showing of the current weapon
    bool        bForceWeapon_Name;
    float       fForceWeapon_Time;




    /*
	    Artificial Intelligence
    */
    int         nAIState;
    int         nAITargetType;
    CWorm       *psAITarget;
    CBonus      *psBonusTarget;
    CVec        cPosTarget;
    int         nPathStart[2];
    int         nPathEnd[2];
    float       fLastCarve;
    CVec        cStuckPos;
    float       fStuckTime;
    bool        bStuck;
    float       fStuckPause;
    float       fLastThink;
    CVec        cNinjaShotPos;


    // Path Finding
    int         nGridCols, nGridRows;
    ai_node_t   *psPath;
    int         *pnOpenCloseGrid;
    ai_node_t   *psCurrentNode;
	float       fLastPathUpdate;


public:
	// Methods


	//
	// General
	//
	void		Clear(void);
	void		Init(void);
	//void		CopyProfile(plyprofile_t *profile);
	void		Shutdown(void);
	

	//
	// Network
	//
	void		writeInfo(CBytestream *bs);
	void		readInfo(CBytestream *bs);
	void		writeScore(CBytestream *bs);
	void		readScore(CBytestream *bs);
	void		writePacket(CBytestream *bs);
	void		readPacket(CBytestream *bs, CWorm *worms);
	void		readPacketState(CBytestream *bs, CWorm *worms);
	void		writeWeapons(CBytestream *bs);
	void		readWeapons(CBytestream *bs);
	void		writeStatUpdate(CBytestream *bs);
	void		readStatUpdate(CBytestream *bs);

	void		setupLobby(void);


	//
	// Weapon & Input
	//
	void		SetupInputs(char Inputs[32][8]);
	void		InitWeaponSelection(void);
	void		SelectWeapons(SDL_Surface *bmpDest, CViewport *v);


	//
	// Graphics
	//
	int			LoadGraphics(int gametype);
	void		FreeGraphics(void);
	SDL_Surface	*ChangeGraphics(char *filename, int team);
	void		Draw(SDL_Surface *bmpDest, CMap *map, CViewport *v);
    void        DrawShadow(SDL_Surface *bmpDest, CMap *map, CViewport *v);


	//
	// Game
	//
	void		Prepare(CMap *pcMap);
	void		Spawn(CVec position);
	int			Injure(int damage);
	int			Kill(void);
	int			CheckBonusCollision(CBonus *b);
	int			GiveBonus(CBonus *b);
	


	//
	// Simulation
	//
	void		getInput(void);
	void		getMouseInput(void);
    void        clearInput(void);
	void		getGamepadInput(void);
	void		Simulate(CMap *map, CWorm *worms, int local, float dt);
	void		SimulateWeapon( float dt );
	int			CheckWormCollision( float dt, CMap *map, CVec pos, CVec *vel, int jump );
    int			CheckOnGround(CMap *map);



    //
    // AI
    //
    bool        AI_Initialize(CMap *pcMap);
    void        AI_Shutdown(void);

	void		AI_GetInput(int gametype, int teamgame, int taggame, CMap *pcMap);
    void        AI_Think(int gametype, int teamgame, int taggame, CMap *pcMap);
    bool        AI_FindHealth(CMap *pcMap);
    bool        AI_SetAim(CVec cPos);
    CVec        AI_GetTargetPos(void);
    
    void        AI_InitMoveToTarget(CMap *pcMap);
    void        AI_MoveToTarget(CMap *pcMap);
    void        AI_SimpleMove(CMap *pcMap);
    void        AI_PreciseMove(CMap *pcMap);
    
    ai_node_t   *AI_ProcessNode(CMap *pcMap, ai_node_t *psParent, int curX, int curY, int tarX, int tarY);
    void        AI_CleanupPath(ai_node_t *node);
    
    int         AI_FindClearingWeapon(void);
    bool        AI_CanShoot(CMap *pcMap, int nGameType);
    void        AI_Shoot(CMap *pcMap);
    int         AI_GetBestWeapon(int nGameType, float fDistance, bool bDirect, CMap *pcMap, float fTraceDist);
    void        AI_ReloadWeapons(void);
    int         cycleWeapons(void);

    void        AI_DEBUG_DrawPath(CMap *pcMap, ai_node_t *node);

    CWorm       *findTarget(int gametype, int teamgame, int taggame, CMap *pcMap);   
    int         traceLine(CVec target, CMap *pcMap, float *fDist, int *nType);
    //void        moveToTarget(CWorm *pcTarget, CMap *pcMap);


    //int         getBestWeapon(int nGameType, float fDistance, CVec cTarget, CMap *pcMap);
    
    





	//
	// Variables
	//
	int			isUsed(void)				{ return iUsed; }
	void		setUsed(int _u)				{ iUsed = _u; }

	char		*getName(void)				{ return sName; }
	uint		getColour(void)				{ return iColour; }
	void		setColour(uint c)			{ iColour = c; }

	void		setLocal(int _l)			{ iLocal = _l; }
	int			getLocal(void)				{ return iLocal; }

	int			getHealth(void)				{ return iHealth; }
	int			getLives(void)				{ return iLives; }
	int			getKills(void)				{ return iKills; }
	void		setLives(int l)				{ iLives = l; }

	void		AddKill(void)				{ iKills++; }
    void        setKills(int k)             { iKills = k; }

	void		setID(int i)				{ iID = i; }
	int			getID(void)					{ return iID; }

	int			getType(void)				{ return iType; }
    void        setType(int t)              { iType = t; }

	int			getAlive(void)				{ return iAlive; }
	void		setAlive(int _a)			{ iAlive = _a; }

	float		getTimeofDeath(void)		{ return fTimeofDeath; }

	void		setHooked(int h, CWorm *w)	{ iHooked=h; pcHookWorm=w; }
	void		setClient(CClient *cl)		{ cClient = cl; }
    CClient     *getClient(void)            { return cClient; }

	CInput		*getShoot(void)				{ return &cShoot; }

	CVec		getPos(void)				{ return vPos; }
	void		setPos(CVec v)				{ vPos = v; }

	CVec		*getVelocity(void)			{ return &vVelocity; }

	worm_state_t *getWormState(void)		{ return &tState; }

	float		getAngle(void)				{ return fAngle; }
	int			getDirection(void)			{ return iDirection; }

	void		setLoadingTime(float l)		{ fLoadingTime = l; }

	void		setGameScript(CGameScript *gs)	{ cGameScript = gs; }
    void        setWpnRest(CWpnRest *wr)    { cWeaponRest = wr; }

	void		setDrawMuzzle(int _d)		{ iDrawMuzzle = _d; }

	int			getWeaponsReady(void)		{ return iWeaponsReady; }
	void		setWeaponsReady(int _w)		{ iWeaponsReady = _w; }
	wpnslot_t	*getCurWeapon(void)			{ return &tWeapons[iCurrentWeapon]; }
	int			getCurrentWeapon(void)		{ return iCurrentWeapon; }
	wpnslot_t	*getWeapon(int id)			{ return &tWeapons[id]; }

	void		setGameReady(int _g)		{ iGameReady = _g; }
	int			getGameReady(void)			{ return iGameReady; }

	void		setProfile(profile_t *p)	{ tProfile = p; }

	void		setTeam(int _t)				{ iTeam = _t; }
	int			getTeam(void)				{ return iTeam; }

	SDL_Surface	*getGibimg(void)			{ return bmpGibs; }
	SDL_Surface	*getPicimg(void)			{ return bmpPic; }

	lobbyworm_t	*getLobby(void)				{ return &tLobbyState; }

	int			getTagIT(void)				{ return iTagIT; }
	void		setTagIT(int _t)			{ iTagIT = _t; }

    void        incrementDirtCount(int d)   { iDirtCount += d; }
    int         getDirtCount(void)          { return iDirtCount; }

	void		setTarget(int _t)			{ iGotTarget = _t; }

	float		getTagTime(void)			{ return fTagTime; }
	void		setTagTime(float _t)		{ fTagTime += _t; }
	void		incrementTagTime(float dt)	{ fTagTime+=dt; }


};




#endif  //  __CWORM_H__