/////////////////////////////////////////
//
//   Auxiliary Software class library
//
/////////////////////////////////////////


// Cache system
// Created 7/11/01
// By Jason Boettcher


#ifndef __CACHE_H__
#define __CACHE_H__


// Cached item types
#define		CCH_IMAGE		0
#define		CCH_SOUND		1			// MP3's not included, coz they stream
#define		CCH_TEXTURE		2

// Incorperate Textures? Animations?

#define		MAX_CACHE		1024




// The item class (can be surface, sample)
class CCache {
public:
	// Constructor
	CCache() {
		Used = false;
		Type = CCH_IMAGE;
		
		Image = NULL;
	}


private:
	// Attributes

	int		Used;
	int		Type;

	char	Filename[128];

	// Image
	SDL_Surface		*Image;

	// Sample
	HSAMPLE			Sample;



public:
	// Methods

	
	// Loading
	SDL_Surface		*LoadImg(char *_file);
	SDL_Surface		*LoadImgBPP(char *_file, int bpp);
	HSAMPLE			LoadSample(char *_file, int maxplaying);


	// Shutdowning
	void			Shutdown(void);


	// Variables
	int				isUsed(void)			{ return Used; }
	int				getType(void)			{ return Type; }
	char			*getFilename(void)		{ return Filename; }

	SDL_Surface		*GetImage(void)			{ return Image; }
	HSAMPLE			GetSample(void)			{ return Sample; }
};





//////////////////////////////////////
//			The cache system
//////////////////////////////////////


int		InitializeCache(void);
void	ShutdownCache(void);

SDL_Surface     *_LoadImage(char *filename);

SDL_Surface		*LoadImage(char *_filename, int correctbpp);
HSAMPLE			LoadSample(char *_filename, int maxplaying);






#endif  //  __CACHE_H__