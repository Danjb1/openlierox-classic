/////////////////////////////////////////
//
//   Auxiliary Software class library
//
/////////////////////////////////////////


// Font class
// Created 15/7/01
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"


char Fontstr[] = {" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~~"};



///////////////////
// Load a font
int CFont::Load(char *fontname, int _colour, int _width)
{
	LOAD_IMAGE(bmpFont,fontname);
	
	Colour = _colour;
	Width = _width;

	bmpWhite = gfxCreateSurface(bmpFont->w,bmpFont->h);
	bmpGreen = gfxCreateSurface(bmpFont->w,bmpFont->h);

	// Calculate the font width for each character
	CalculateWidth();

	PreCalculate(bmpWhite,MakeColour(255,255,255));
	PreCalculate(bmpGreen,MakeColour(0,255,0));


	// Must do this after
	SDL_SetColorKey(bmpFont, SDL_SRCCOLORKEY, SDL_MapRGB(bmpFont->format,255,0,255));

    // Pre-calculate some colours
    f_pink = SDL_MapRGB(bmpFont->format,255,0,255);
	f_blue = SDL_MapRGB(bmpFont->format,0,0,255);
	f_white = MakeColour(255,255,255);
	f_green = MakeColour(0,255,0);

	return true;
}


///////////////////
// Shutdown the font
void CFont::Shutdown(void)
{
	if(bmpWhite)
		SDL_FreeSurface(bmpWhite);
	if(bmpGreen)
		SDL_FreeSurface(bmpGreen);
}


///////////////////
// Calculate Widths
void CFont::CalculateWidth(void)
{
	unsigned int n;
	Uint32 pixel;
	int i,j;
	int a,b;

	// Lock the surface
	if(SDL_MUSTLOCK(bmpFont))
		SDL_LockSurface(bmpFont);

	Uint32 blue = SDL_MapRGB(bmpFont->format,0,0,255);
	
	for(n=0;n<strlen(Fontstr);n++) {
		a=n*Width;
		for(j=0;j<bmpFont->h;j++) {
			for(i=a,b=0;b<Width;i++,b++) {

				pixel = GetPixel(bmpFont,i,j);
				if(pixel == blue) {
					FontWidth[n] = b;
					break;
				}
			}
		}
	}


	// Unlock the surface
	if(SDL_MUSTLOCK(bmpFont))
		SDL_UnlockSurface(bmpFont);
}


///////////////////
// Precalculate a font's colour
void CFont::PreCalculate(SDL_Surface *bmpSurf, Uint32 colour)
{
	Uint32 pixel;
	int x,y;

	DrawRectFill(bmpSurf,0,0,bmpSurf->w,bmpSurf->h,MakeColour(255,0,255));
	SDL_BlitSurface(bmpFont,NULL,bmpSurf,NULL);


	// Replace black with the appropriate colour
	if(SDL_MUSTLOCK(bmpSurf))
		SDL_LockSurface(bmpSurf);
	
	for(y=0;y<bmpSurf->h;y++) {
		for(x=0;x<bmpSurf->w;x++) {
			pixel = GetPixel(bmpSurf,x,y);

			if(pixel == 0)
				PutPixel(bmpSurf,x,y,colour);
		}
	}


	// Unlock the surface
	if(SDL_MUSTLOCK(bmpSurf))
		SDL_UnlockSurface(bmpSurf);

	SDL_SetColorKey(bmpSurf, SDL_SRCCOLORKEY, SDL_MapRGB(bmpSurf->format,255,0,255));
}


///////////////////
// Draw a font
void CFont::Draw(SDL_Surface *dst, int x, int y, int col, char *fmt,...)
{
	char buf[512];
	va_list arg;
	int pos=0;
	int n,l;
	Uint32 pixel;
	int i,j;
	int w;
	int a,b;
	int length = strlen(Fontstr);

	va_start(arg, fmt);
	vsprintf(buf, fmt, arg);
	va_end(arg);

	int txtlen = strlen(buf);


	// Clipping rectangle
	SDL_Rect rect = dst->clip_rect;

	int	top = rect.y;
	int left = rect.x;
	int right = rect.x + rect.w;
	int bottom = rect.y + rect.h;


	// Lock the surfaces
	if(SDL_MUSTLOCK(dst))
		SDL_LockSurface(dst);
	if(SDL_MUSTLOCK(bmpFont))
		SDL_LockSurface(bmpFont);

	
	Uint32 col2 = (Uint32)col;

	pos=0;
	for(n=0;n<txtlen;n++) {

		l = buf[n]-32;

        // Ignore unkown characters
        if(l >= length-1 || l < 0 )
            continue;
		
		w=0;
		a=l*Width;

		if(!Colour) {
			SDL_SetColorKey(bmpFont, SDL_SRCCOLORKEY, SDL_MapRGB(bmpFont->format,255,0,255));
			DrawImageAdv(dst,bmpFont,a,0,x+pos,y,FontWidth[l],bmpFont->h);
			pos+=FontWidth[l];
			continue;
		}

		if(col2 == f_white) {
			DrawImageAdv(dst,bmpWhite,a,0,x+pos,y,FontWidth[l],bmpFont->h);
			pos+=FontWidth[l];
			continue;
		}

		if(col2 == 0) {
			DrawImageAdv(dst,bmpFont,a,0,x+pos,y,FontWidth[l],bmpFont->h);
			pos+=FontWidth[l];
			continue;
		}

		if(col2 == f_green) {
			DrawImageAdv(dst,bmpGreen,a,0,x+pos,y,FontWidth[l],bmpFont->h);
			pos+=FontWidth[l];
			continue;
		}

		/*if(!Colour) {
			SDL_SetColorKey(bmpFont, SDL_SRCCOLORKEY, SDL_MapRGB(bmpFont->format,255,0,255));
			DrawImageAdv(dst,bmpFont,a,0,x+pos,y,FontWidth[l],bmpFont->h);
		}
		else */{
			Uint8 *src = (Uint8 *)bmpFont->pixels + a * bmpFont->format->BytesPerPixel;
			Uint8 *p;
			int bpp = bmpFont->format->BytesPerPixel;
			for(j=0;j<bmpFont->h;j++) {
				p = src;
				for(i=a,b=0;b<FontWidth[l];i++,b++,p+=bpp) {

					// Clipping
					if(x+pos+b < left)
						continue;
					if(y+j < top)
						break;

					if(y+j >= bottom)
						break;
					if(x+pos+b >= right)
						break;
						
					pixel = GetPixelFromAddr(p,bpp);

					if(pixel == f_pink)
						continue;

					if(Colour)
						PutPixel(dst,x+pos+b,y+j,col);
				}
				src+= bmpFont->pitch;
			}
		}

		pos+=FontWidth[l];
	}


	// Unlock the surfaces
	if(SDL_MUSTLOCK(dst))
		SDL_UnlockSurface(dst);
	if(SDL_MUSTLOCK(bmpFont))
		SDL_UnlockSurface(bmpFont);

}


///////////////////
// Calculate the width of a string of text
int CFont::GetWidth(char *buf)
{
	unsigned int n,l;
	int length = 0;

	// Calculate the length of the text
	for(n=0;n<strlen(buf);n++) {
        int l = buf[n]-32;
        if( l >= strlen(Fontstr) || l < 0 )
            continue;

		length += FontWidth[l];
	}
	
	return length;
}


///////////////////
// Draw's the text in centre alignment
void CFont::DrawCentre(SDL_Surface *dst, int x, int y, int col, char *fmt, ...)
{
	char buf[512];
	va_list arg;
	int pos;
	int length=0;
	unsigned int n,l;
	
	va_start(arg, fmt);
	vsprintf(buf, fmt, arg);
	va_end(arg);

	// Calculate the length of the text
	for(n=0;n<strlen(buf);n++) {
		/*for(l=0;l<strlen(Fontstr);l++) {
			if(Fontstr[l] == buf[n])
				break;
		}*/

		l = buf[n]-32;
		if(l>=strlen(Fontstr))
			continue;

		length+=FontWidth[l];
	}

	pos = x-length/2;

	Draw(dst,pos,y,col,buf);
}
