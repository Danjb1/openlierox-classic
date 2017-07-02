/////////////////////////////////////////
//
//                  LieroX
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Map class
// Created 21/1/02
// Jason Boettcher


#ifndef __CMAP_H__
#define __CMAP_H__



#define		MAP_VERSION	0

// Map types
#define		MPT_PIXMAP	0
#define		MPT_IMAGE	1

// Pixel flags
#define		PX_EMPTY	0x01
#define		PX_DIRT		0x02
#define		PX_ROCK		0x04
#define		PX_SHADOW	0x08


// Object types
#define		OBJ_HOLE	0
#define		OBJ_STONE	1
#define		OBJ_MISC	2

#define		MAX_OBJECTS	8192



class CWorm;



// Object structure
typedef struct {
	int		Type;
	int		Size;
	int     X, Y;

} object_t;



// Random map data
typedef struct {

    bool        bUsed;
    char        szTheme[128];
    int         nNumObjects;
    object_t    *psObjects;

} maprandom_t;



// Theme structure
typedef struct {
	char		name[128];
	Uint32		iDefaultColour;
	SDL_Surface	*bmpFronttile;
	SDL_Surface	*bmpBacktile;

	int			NumStones;
	SDL_Surface	*bmpStones[16];
	SDL_Surface	*bmpHoles[16];
	int			NumMisc;
	SDL_Surface	*bmpMisc[32];

} theme_t;


class CPlayer;

class CMap {
public:
	// Constructor
	CMap() {
		Width = 800;
		Height = 700;
		Type = MPT_PIXMAP;
        nTotalDirtCount = 0;

		Created = false;

		bmpImage = NULL;
		bmpBackImage = NULL;
		bmpMiniMap = NULL;
		PixelFlags = NULL;
        bmpGreenMask = NULL;
        bmpShadowMap = NULL;
        GridFlags = NULL;

		NumObjects = 0;
		Objects = NULL;

		bMiniMapDirty = true;
        sRandomLayout.bUsed = false;
	}


private:
	// Attributes

	char		Name[64];
	int			Type;
	int			Width;
	int			Height;
	theme_t		Theme;
    int         nTotalDirtCount;

	int			Created;

	SDL_Surface	*bmpImage;
	SDL_Surface	*bmpBackImage;    
	SDL_Surface	*bmpMiniMap;
    SDL_Surface *bmpGreenMask;
	uchar		*PixelFlags;    
    SDL_Surface *bmpShadowMap;

    // AI Grid
    int         nGridWidth, nGridHeight;
    int         nGridCols, nGridRows;
    uchar       *GridFlags;

    maprandom_t sRandomLayout;

	bool		bMiniMapDirty;

	// Objects
	int			NumObjects;
	object_t	*Objects;


	// Water
	int			*m_pnWater1;
	int			*m_pnWater2;


public:
	// Methods

	int			New(int _width, int _height, char *_theme);
	int			Load(char *filename);
	int			LoadOriginal(FILE *fp);
	int			Save(char *name, char *filename);
	int			SaveImageFormat(FILE *fp);
	int			LoadImageFormat(FILE *fp);	
	void		Clear(void);

    void		ApplyRandom(void);
    void        ApplyRandomLayout(maprandom_t *psRandom);

	void		Shutdown(void);

	int			LoadTheme(char *_theme);
	int			CreateSurface(void);
	int			CreatePixelFlags(void);
    bool        createGrid(void);
    void        calculateGrid(void);
    void        calculateGridCell(int x, int y, bool bSkipEmpty);
	void		TileMap(void);
    
    void        CalculateDirtCount();
    void        CalculateShadowMap();

    char        *findRandomTheme(char *buf);
    bool        validateTheme(char *name);

    void        PutImagePixel(int x, int y, Uint32 colour);

	void		UpdateMiniMap(int force = false);	

	void		Send(CBytestream *bs);

	void		Draw(SDL_Surface *bmpDest, CViewport *view);
    void        DrawObjectShadow(SDL_Surface *bmpDest, SDL_Surface *bmpObj, int sx, int sy, int w, int h, CViewport *view, int wx, int wy);
    void        DrawPixelShadow(SDL_Surface *bmpDest, CViewport *view, int wx, int wy);
    void		DrawMiniMap(SDL_Surface *bmpDest, int x, int y, float dt, CWorm *worms, int gametype);

	void		SetPixelFlag(int x, int y, int flag);
	uchar		GetPixelFlag(int x, int y);
	uchar		*GetPixelFlags(void)	{ return PixelFlags; }

	SDL_Surface	*GetImage(void)			{ return bmpImage; }
	SDL_Surface	*GetMiniMap(void)		{ return bmpMiniMap; }

	void		AddObject(int type, int size, CVec pos);

	int 		CarveHole(int size, CVec pos);
	int 		PlaceDirt(int size, CVec pos);
	void		PlaceStone(int size, CVec pos);
	void		PlaceMisc(int id, CVec pos);
    int         PlaceGreenDirt(CVec pos);
	void		ApplyShadow(int sx, int sy, int w, int h);

	void		DeleteObject(CVec pos);
	void		DeleteStone(object_t *obj);

	theme_t		*GetTheme(void)		{ return &Theme; }

	void		DEBUG_DrawPixelFlags(void);

    maprandom_t *getRandomLayout(void)  { return &sRandomLayout; }


	int			GetWidth(void)		{ return Width; }
	int			GetHeight(void)		{ return Height; }
    int         GetDirtCount(void)  { return nTotalDirtCount; }

    int         getGridCols(void)   { return nGridCols; }
    int         getGridRows(void)   { return nGridRows; }
    int         getGridWidth(void)  { return nGridWidth; }
    int         getGridHeight(void) { return nGridHeight; }
    uchar       *getGridFlags(void) { return GridFlags; }


};





#endif  //  __CMAP_H__