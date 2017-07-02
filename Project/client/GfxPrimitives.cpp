/////////////////////////////////////////
//
//   Auxiliary Software class library
//
/////////////////////////////////////////


// Graphics primitives
// Created 12/11/01
// By Jason Boettcher


#include "defs.h"


///////////////////
// Simply draw the image
void DrawImage(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int x, int y)
{
	SDL_Rect	rDest;

	rDest.x = x;
	rDest.y = y;

	//SDL_SetColorKey(bmpSrc, SDL_SRCCOLORKEY, SDL_MapRGB(bmpSrc->format,255,0,255));

	SDL_BlitSurface(bmpSrc,NULL,bmpDest,&rDest);
}


///////////////////
// Draw the image, with more options
void DrawImageEx(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int x, int y, int w, int h)
{
	SDL_Rect	rDest;
	SDL_Rect	rSrc;

	rDest.x = x;
	rDest.y = y;
	rSrc.x = 0;
	rSrc.y = 0;
	rSrc.w = w;
	rSrc.h = h;

	SDL_BlitSurface(bmpSrc,&rSrc,bmpDest,&rDest);
}


///////////////////
// Draw the image with a huge amount of options
void DrawImageAdv(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int sx, int sy, int dx, int dy, int w, int h)
{
	SDL_Rect	rDest;
	SDL_Rect	rSrc;

	rDest.x = dx;
	rDest.y = dy;
	rSrc.x = sx;
	rSrc.y = sy;
	rSrc.w = w;
	rSrc.h = h;

	SDL_BlitSurface(bmpSrc,&rSrc,bmpDest,&rDest);
}


///////////////////
// Draw the image mirrored with a huge amount of options
void DrawImageAdv_Mirror(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int sx, int sy, int dx, int dy, int w, int h)
{
	int x,y,c;
	int ow = w;
	int oh = h;

	// Lock the surfaces
	if(SDL_MUSTLOCK(bmpDest))
		SDL_LockSurface(bmpDest);
	if(SDL_MUSTLOCK(bmpSrc))
		SDL_LockSurface(bmpSrc);


	// Warning: Doesn't do clipping on the source surface
	int cx = bmpDest->clip_rect.x;
	int cy = bmpDest->clip_rect.y;
	int cex = cx + bmpDest->clip_rect.w;
	int cey = cy + bmpDest->clip_rect.h;

	// Colour key
	Uint16 ckey = -1;
	if(bmpSrc->flags & SDL_SRCCOLORKEY)
		ckey = bmpSrc->format->colorkey;

	// Do clipping on the DEST surface
	if(dx+w<cx || dy+h<cy) return;
	if(dx>cex || dy>cey) return;

	if(dx<cx) {
		c = cx-dx;
		dx+=c;
		w-=c;
	}
	if(dy<cy) {
		c = cy-dy;
		dy=cy;
		h-=c;
	}

	if(dx+w > cex) {
		c = dx+w - cex;
		w-=c;
		sx+=c;
	}
	if(dy+h > cey) {
		c = dy+h - cey;
		h-=c;
		sy+=c;
	}

	int TrgBpp = bmpDest->format->BytesPerPixel;

	Uint8 *TrgPix = (Uint8 *)bmpDest->pixels + dy*bmpDest->pitch + dx*TrgBpp;
	Uint8 *SrcPix = (Uint8 *)bmpSrc->pixels +  sy*bmpSrc->pitch + sx*bmpSrc->format->BytesPerPixel;

	Uint16 *sp,*tp;
	for(y=0;y<h;y++) {
		
		sp = (Uint16 *)SrcPix;
		tp = (Uint16 *)TrgPix + w;
		for(x=0;x<w;x++) {
			// Check transparent colour
			if(ckey == *sp) {
				// Skip the pixel
				tp--;
				sp++;
			} else
				*(tp--) = *(sp++);
		}

		SrcPix+=bmpSrc->pitch;
		TrgPix+=bmpDest->pitch;
	}
	

	// Unlock em
	if(SDL_MUSTLOCK(bmpDest))
		SDL_UnlockSurface(bmpDest);
	if(SDL_MUSTLOCK(bmpSrc))
		SDL_UnlockSurface(bmpSrc);
}


///////////////////
// Draws a sprite doubly stretched
void DrawImageStretch2(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int sx, int sy, int dx, int dy, int w, int h)
{
	int x,y;
	
	// Lock the surfaces
	if(SDL_MUSTLOCK(bmpDest))
		SDL_LockSurface(bmpDest);
	if(SDL_MUSTLOCK(bmpSrc))
		SDL_LockSurface(bmpSrc);


	// Warning: Doesn't do clipping
	// Warning: Assumes 16bpp
	
	Uint8 *TrgPix = (Uint8 *)bmpDest->pixels + dy*bmpDest->pitch + dx*bmpDest->format->BytesPerPixel;
	Uint8 *SrcPix = (Uint8 *)bmpSrc->pixels +  sy*bmpSrc->pitch + sx*bmpSrc->format->BytesPerPixel;

	Uint16 *sp,*tp;
    int desthp = bmpDest->pitch/2;      // We are performing the add on a 16bit int, so we only use half


    for(y=0;y<h;y++) {
		
		sp = (Uint16 *)SrcPix;
		tp = (Uint16 *)TrgPix;
		for(x=0;x<w;x++) {
            // Copy the 1 source pixel into a 4 pixel block on the destination surface
			*(tp) = *sp;
			*(tp+1) = *sp;
            *(tp+desthp) = *sp;
			*(tp+desthp+1) = *sp;
			sp++;
            tp+=2;
		}
		TrgPix += bmpDest->pitch;
		TrgPix += bmpDest->pitch;
		SrcPix += bmpSrc->pitch;
	}

	// Unlock em
	if(SDL_MUSTLOCK(bmpDest))
		SDL_UnlockSurface(bmpDest);
	if(SDL_MUSTLOCK(bmpSrc))
		SDL_UnlockSurface(bmpSrc);
}


///////////////////
// Draws a sprite doubly stretched with colour key
void DrawImageStretch2Key(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int sx, int sy, int dx, int dy, int w, int h, Uint16 key)
{
	int x,y;
	int c;
	
	// Warning: Doesn't do clipping on the source surface
	// Warning: Assumes 16bpp

	int cx = bmpDest->clip_rect.x;
	int cy = bmpDest->clip_rect.y;
	int cex = cx+bmpDest->clip_rect.w;
	int cey = cy+bmpDest->clip_rect.h;


	// Do clipping on the DEST surface
	if(dx+w*2<cx || dy+h*2<cy) return;
	if(dx>cex || dy>cey) return;

	if(dx<cx) {	c=cx-dx;	dx+=c; c/=2; sx+=c;	w-=c; }
	if(dy<cy) {	c=cy-dy;	dy+=c; c/=2; sy+=c;	h-=c; }

	if(dx+w*2>cex) {	c=(dx+w*2)-cex;	c/=2; w-=c; }
	if(dy+h*2>cey) {	c=(dy+h*2)-cey;	c/=2; h-=c; }


	// Lock the surfaces
	if(SDL_MUSTLOCK(bmpDest))
		SDL_LockSurface(bmpDest);
	if(SDL_MUSTLOCK(bmpSrc))
		SDL_LockSurface(bmpSrc);

	
	Uint8 *TrgPix = (Uint8 *)bmpDest->pixels + dy*bmpDest->pitch + dx*bmpDest->format->BytesPerPixel;
	Uint8 *SrcPix = (Uint8 *)bmpSrc->pixels +  sy*bmpSrc->pitch + sx*bmpSrc->format->BytesPerPixel;

	Uint16 *sp,*tp;

	for(y=0;y<h;y++) {

		// First Line
		sp = (Uint16 *)SrcPix;
		tp = (Uint16 *)TrgPix;
		for(x=0;x<w;x++) {
			if(*sp == key)
				tp+=2;
			else {
				*(tp++) = *sp;
				*(tp++) = *sp;
			}
			sp++;
		}
		TrgPix += bmpDest->pitch;

		// Second Line
		sp = (Uint16 *)SrcPix;
		tp = (Uint16 *)TrgPix;
		for(x=0;x<w;x++) {
			if(*sp == key)
				tp+=2;
			else {
				*(tp++) = *sp;
				*(tp++) = *sp;
			}
			sp++;
		}

		TrgPix += bmpDest->pitch;
		SrcPix += bmpSrc->pitch;
	}


	// Unlock em
	if(SDL_MUSTLOCK(bmpDest))
		SDL_UnlockSurface(bmpDest);
	if(SDL_MUSTLOCK(bmpSrc))
		SDL_UnlockSurface(bmpSrc);
}


///////////////////
// Draws a sprite mirrored doubly stretched with colour key
void DrawImageStretchMirrorKey(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int sx, int sy, int dx, int dy, int w, int h, Uint16 key)
{
	int x,y;
	int c;
	
	// Warning: Doesn't do clipping on the source surface
	// Warning: Assumes 16bpp

	int cx = bmpDest->clip_rect.x;
	int cy = bmpDest->clip_rect.y;
	int cex = cx+bmpDest->clip_rect.w;
	int cey = cy+bmpDest->clip_rect.h;

	// Do clipping on the DEST surface
	if(dx+w*2<cx || dy+h*2<cy) return;
	if(dx>cex || dy>cey) return;

	if(dx<cx) {	c=cx-dx; dx+=c; w-=c;}
	if(dy<cy) {	c=cy-dy; dy+=c; h-=c;}

	if(dx+w*2>cex) {	c=(dx+w*2)-cex;	c/=2; sx+=c; w-=c;}
	if(dy+h*2>cey) {	c=(dy+h*2)-cey;	c/=2; sy+=c; h-=c;}


	// Lock the surfaces
	if(SDL_MUSTLOCK(bmpDest))
		SDL_LockSurface(bmpDest);
	if(SDL_MUSTLOCK(bmpSrc))
		SDL_LockSurface(bmpSrc);

	
	Uint8 *TrgPix = (Uint8 *)bmpDest->pixels + dy*bmpDest->pitch + dx*bmpDest->format->BytesPerPixel;
	Uint8 *SrcPix = (Uint8 *)bmpSrc->pixels + sy*bmpSrc->pitch + sx*bmpSrc->format->BytesPerPixel;

	Uint16 *sp,*tp;

	for(y=0;y<h;y++) {

		// First Line
		sp = (Uint16 *)SrcPix;
		tp = (Uint16 *)TrgPix + w*2;
		for(x=0;x<w;x++) {
			if(*sp == key)
				tp-=2;
			else {
				*(tp--) = *sp;
				*(tp--) = *sp;
			}
			sp++;
		}
		TrgPix += bmpDest->pitch;

		// Second Line
		sp = (Uint16 *)SrcPix;
		tp = (Uint16 *)TrgPix + w*2;
		for(x=0;x<w;x++) {
			if(*sp == key)
				tp-=2;
			else {
				*(tp--) = *sp;
				*(tp--) = *sp;
			}
			sp++;
		}

		TrgPix += bmpDest->pitch;
		SrcPix += bmpSrc->pitch;
	}


	// Unlock em
	if(SDL_MUSTLOCK(bmpDest))
		SDL_UnlockSurface(bmpDest);
	if(SDL_MUSTLOCK(bmpSrc))
		SDL_UnlockSurface(bmpSrc);
}



///////////////////
// Draws a sprite doubly stretched, with a colour key and not so advanced
void DrawImageStretchKey(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int dx, int dy, Uint16 key)
{
	DrawImageStretch2Key(bmpDest,bmpSrc,0,0,dx,dy,bmpSrc->w,bmpSrc->h,key);
}


///////////////////
// Draws a sprite doubly stretched but not so advanced
void DrawImageStretch(SDL_Surface *bmpDest, SDL_Surface *bmpSrc, int dx, int dy)
{
	DrawImageStretch2(bmpDest,bmpSrc,0,0,dx,dy,bmpSrc->w,bmpSrc->h);
}



///////////////////
// Creates a buffer with the same details as the screen
SDL_Surface *gfxCreateSurface(int width, int height)
{
	SDL_Surface *screen = SDL_GetVideoSurface();
	SDL_PixelFormat *fmt = screen->format;

	return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, fmt->BitsPerPixel, 
								fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
}


///////////////////
// Draw a rectangle
void DrawRect(SDL_Surface *bmpDest, int x, int y, int x2, int y2, int colour)
{
	DrawHLine(bmpDest,x,x2,y,colour);
	DrawHLine(bmpDest,x,x2,y2,colour);

	DrawVLine(bmpDest,y,y2,x,colour);
	DrawVLine(bmpDest,y,y2,x2,colour);
}


///////////////////
// Draws a filled rectangle
void DrawRectFill(SDL_Surface *bmpDest, int x, int y, int x2, int y2, int colour)
{
	SDL_Rect Rect;

	Rect.x = x;
	Rect.y = y;
	Rect.w = x2-x;
	Rect.h = y2-y;

	SDL_FillRect(bmpDest,&Rect,colour);
}


///////////////////
// Draw an alpha filled rectangle
void DrawRectFillA(SDL_Surface *bmpDest, int x, int y, int x2, int y2, int color, int alpha)
{
    // Note: Unoptimized
    // Does NOT do clipping
    
    SDL_Surface *src = SDL_GetVideoSurface();

    Uint32 Rmask = bmpDest->format->Rmask, Gmask = bmpDest->format->Gmask, Bmask = bmpDest->format->Bmask, Amask = bmpDest->format->Amask;
	Uint32 R,G,B,A = 0;

    for(int j=y; j<=y2; j++) {

        for(int i=x; i<=x2; i++) {
            Uint16 *pixel = (Uint16 *)bmpDest->pixels + j*bmpDest->pitch/2 + i;
	        Uint16 *srcpixel = (Uint16 *)src->pixels + j*src->pitch/2 + i;
	        Uint32 dc = *srcpixel;

	        R = ((dc & Rmask) + (( (color & Rmask) - (dc & Rmask) ) * alpha >> 8)) & Rmask;
	        G = ((dc & Gmask) + (( (color & Gmask) - (dc & Gmask) ) * alpha >> 8)) & Gmask;
	        B = ((dc & Bmask) + (( (color & Bmask) - (dc & Bmask) ) * alpha >> 8)) & Bmask;
	        if( Amask )
		        A = ((dc & Amask) + (( (color & Amask) - (dc & Amask) ) * alpha >> 8)) & Amask;

	        *pixel= R | G | B | A;
        }
    }
}


///////////////////
// Draw a horizontal line
void DrawHLine(SDL_Surface *bmpDest, int x, int x2, int y, int colour)
{
	int l,r;
	SDL_Surface *bmpScreen = SDL_GetVideoSurface();

	SDL_Rect rect = bmpDest->clip_rect;

	int	top = rect.y;
	int left = rect.x;
	int right = rect.x + rect.w - 1;
	int bottom = rect.y + rect.h - 1;

	if(SDL_MUSTLOCK(bmpDest))
		SDL_LockSurface(bmpDest);
	
	l = MIN(x,x2);
	r = MAX(x,x2);

	l = MAX(l,left);
	r = MIN(r,right);

	int bpp = bmpDest->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)bmpDest->pixels + y * bmpDest->pitch + l * bpp;


	// Clipping
	if(y > bottom || y < top) {
		if(SDL_MUSTLOCK(bmpDest))
			SDL_UnlockSurface(bmpDest);
		return;
	}

	for(int n=0;n<=r-l;n++) {

		// Only 2 bpp's supported
		switch(bpp) {

			// 16 bpp
			case 2:
				*(Uint16 *)p = colour;
				break;

			// 32 bpp
			case 4:
				*(Uint32 *)p = colour;
				break;
		}

		p+=bpp;
    }

	if(SDL_MUSTLOCK(bmpDest))
		SDL_UnlockSurface(bmpDest);
}


///////////////////
// Draw a vertical line
void DrawVLine(SDL_Surface *bmpDest, int y, int y2, int x, int colour)
{
	int t,b;
	SDL_Surface *bmpScreen = SDL_GetVideoSurface();

	SDL_Rect rect = bmpDest->clip_rect;

	int	top = rect.y;
	int left = rect.x;
	int right = rect.x + rect.w;
	int bottom = rect.y + rect.h;

	if(SDL_MUSTLOCK(bmpDest))
		SDL_LockSurface(bmpDest);

	int bpp = bmpDest->format->BytesPerPixel;
	Uint8 *p;

	t = MIN(y,y2);
	b = MAX(y,y2);

	t = MAX(t,top);
	b = MIN(b,bottom);
	
	// Clipping
	if(x > right || x < left) {
		
		if(SDL_MUSTLOCK(bmpDest))
			SDL_UnlockSurface(bmpDest);
		return;
	}

	for(int n=t;n<=b;n++) {

		p = (Uint8 *)bmpDest->pixels + n * bmpDest->pitch + x * bpp;

		// Only 2 bpp's supported
		switch(bpp) {

			// 16 bpp
			case 2:
				*(Uint16 *)p = colour;
				break;

			// 32 bpp
			case 4:
				*(Uint32 *)p = colour;
				break;
		}
	}

	if(SDL_MUSTLOCK(bmpDest))
		SDL_UnlockSurface(bmpDest);
}


///////////////////
// Put a pixel on the surface
void PutPixel(SDL_Surface *bmpDest, int x, int y, int colour)
{
	int bpp = bmpDest->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)bmpDest->pixels + y * bmpDest->pitch + x * bpp;


    switch(bpp) {

		// 8 bpp
		case 1:
			*p = colour;
			break;

		// 16 bpp
		case 2:
			*(Uint16 *)p = colour;
			break;

		// 24 bpp
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (colour >> 16) & 0xff;
				p[1] = (colour >> 8) & 0xff;
				p[2] = colour & 0xff;
			} else {
				p[0] = colour & 0xff;
				p[1] = (colour >> 8) & 0xff;
				p[2] = (colour >> 16) & 0xff;
			}
			break;

		// 32 bpp
		case 4:
			*(Uint32 *)p = colour;
			break;
    }
}

int ropecolour = 0;
int ropealt = 0;

///////////////////
// Put a pixel on the surface (while checking for clipping)
void RopePutPixel(SDL_Surface *bmpDest, int x, int y, int colour)
{	
	ropealt = !ropealt;

	if(ropealt)
		return;
	
	if(!bmpDest)
		return;

	// Snap to nearest 2nd pixel
	x -= x % 2;
	y -= y % 2;

	if( x < 0 || y < 0 )
		return;
	if( x < bmpDest->clip_rect.x || y < bmpDest->clip_rect.y )
		return;
	if( x >= bmpDest->clip_rect.x+bmpDest->clip_rect.w || y >= bmpDest->clip_rect.y+bmpDest->clip_rect.h )
		return;

	Uint32 cols[2];
	cols[0] = MakeColour(160,80,0);
	cols[1] = MakeColour(200,100,0);

	int bpp = bmpDest->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)bmpDest->pixels + y * bmpDest->pitch + x * bpp;

	if(ropecolour == 0)
		ropecolour = 1;
	else
		ropecolour = 0;

	colour = cols[ropecolour];


    switch(bpp) {

		// 8 bpp
		case 1:
			*p = colour;
			break;

		// 16 bpp
		case 2: {
			Uint8 *a = p+bmpDest->pitch;
			*(Uint16 *)p = colour;
			p+=2;
			*(Uint16 *)p = colour;
			
			*(Uint16 *)a = colour;
			a+=2;
			*(Uint16 *)a = colour;
				}
			break;

		// 24 bpp
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (colour >> 16) & 0xff;
				p[1] = (colour >> 8) & 0xff;
				p[2] = colour & 0xff;
			} else {
				p[0] = colour & 0xff;
				p[1] = (colour >> 8) & 0xff;
				p[2] = (colour >> 16) & 0xff;
			}
			break;

		// 32 bpp
		case 4:
			*(Uint32 *)p = colour;
			break;
    }
}


int beamalt = 0;

///////////////////
// Put a pixel on the surface (while checking for clipping)
void BeamPutPixel(SDL_Surface *bmpDest, int x, int y, int colour)
{	
	beamalt = !beamalt;

	if(beamalt)
		return;
	
	if(!bmpDest)
		return;

	// Snap to nearest 2nd pixel
	x -= x % 2;
	y -= y % 2;

	if( x < 0 || y < 0 )
		return;
	if( x < bmpDest->clip_rect.x || y < bmpDest->clip_rect.y )
		return;
	if( x >= bmpDest->clip_rect.x+bmpDest->clip_rect.w || y >= bmpDest->clip_rect.y+bmpDest->clip_rect.h )
		return;

	int bpp = bmpDest->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)bmpDest->pixels + y * bmpDest->pitch + x * bpp;

    switch(bpp) {

		// 8 bpp
		case 1:
			*p = colour;
			break;

		// 16 bpp
		case 2: {
			Uint8 *a = p+bmpDest->pitch;
			*(Uint16 *)p = colour;
			p+=2;
			*(Uint16 *)p = colour;
			
			*(Uint16 *)a = colour;
			a+=2;
			*(Uint16 *)a = colour;
				}
			break;

		// 24 bpp
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (colour >> 16) & 0xff;
				p[1] = (colour >> 8) & 0xff;
				p[2] = colour & 0xff;
			} else {
				p[0] = colour & 0xff;
				p[1] = (colour >> 8) & 0xff;
				p[2] = (colour >> 16) & 0xff;
			}
			break;

		// 32 bpp
		case 4:
			*(Uint32 *)p = colour;
			break;
    }
}


int laseralt = 0;

///////////////////
// Put a pixel on the surface (while checking for clipping)
void LaserSightPutPixel(SDL_Surface *bmpDest, int x, int y, int colour)
{	
	laseralt++;
	laseralt %= GetRandomInt(35)+1;

	if(laseralt)
		return;
	
	if(!bmpDest)
		return;

	Uint32 Colours[] = { MakeColour(190,0,0), MakeColour(160,0,0) };
	colour = Colours[ GetRandomInt(1) ];

	// Snap to nearest 2nd pixel
	x -= x % 2;
	y -= y % 2;

	if( x < 0 || y < 0 )
		return;
	if( x < bmpDest->clip_rect.x || y < bmpDest->clip_rect.y )
		return;
	if( x >= bmpDest->clip_rect.x+bmpDest->clip_rect.w || y >= bmpDest->clip_rect.y+bmpDest->clip_rect.h )
		return;

	int bpp = bmpDest->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)bmpDest->pixels + y * bmpDest->pitch + x * bpp;

    switch(bpp) {

		// 8 bpp
		case 1:
			*p = colour;
			break;

		// 16 bpp
		case 2: {
			Uint8 *a = p+bmpDest->pitch;
			*(Uint16 *)p = colour;
			p+=2;
			*(Uint16 *)p = colour;
			
			*(Uint16 *)a = colour;
			a+=2;
			*(Uint16 *)a = colour;
				}
			break;

		// 24 bpp
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (colour >> 16) & 0xff;
				p[1] = (colour >> 8) & 0xff;
				p[2] = colour & 0xff;
			} else {
				p[0] = colour & 0xff;
				p[1] = (colour >> 8) & 0xff;
				p[2] = (colour >> 16) & 0xff;
			}
			break;

		// 32 bpp
		case 4:
			*(Uint32 *)p = colour;
			break;
    }
}


///////////////////
// Get a pixel from the surface
Uint32 GetPixel(SDL_Surface *bmpSrc, int x, int y)
{
    int bpp = bmpSrc->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)bmpSrc->pixels + y * bmpSrc->pitch + x * bpp;

    switch(bpp) {

		// 8 bpp
		case 1:
			return *p;

		// 16 bpp
		case 2:
			return *(Uint16 *)p;

		// 24 bpp
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;

		// 32 bpp
		case 4:
			return *(Uint32 *)p;

		default:
			return 0;
    }
}


///////////////////
// Get a pixel from an 8bit address
Uint32 GetPixelFromAddr(Uint8 *p, int bpp)
{
	switch(bpp) {
		// 8 bpp
		case 1:
			return *p;

		// 16 bpp
		case 2:
			return *(Uint16 *)p;

		// 24 bpp
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;

		// 32 bpp
		case 4:
			return *(Uint32 *)p;

		default:
			return 0;
	}
}


///////////////////
// Creates a int colour based on the 3 components
Uint32 MakeColour(Uint8 r, Uint8 g, Uint8 b)
{
	return SDL_MapRGB(SDL_GetVideoSurface()->format,r,g,b);
}


///////////////////
// Extract 4 colour components from a packed int
void GetColour4(Uint32 pixel, SDL_Surface *img, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a)
{
	SDL_GetRGBA(pixel,img->format,r,g,b,a);
}



/* --------- Clipping routines for box/line */

/* Clipping based heavily on code from                       */
/* http://www.ncsa.uiuc.edu/Vis/Graphics/src/clipCohSuth.c   */

#define CLIP_LEFT_EDGE   0x1
#define CLIP_RIGHT_EDGE  0x2
#define CLIP_BOTTOM_EDGE 0x4
#define CLIP_TOP_EDGE    0x8
#define CLIP_INSIDE(a)   (!a)
#define CLIP_REJECT(a,b) (a&b)
#define CLIP_ACCEPT(a,b) (!(a|b))

static int clipEncode (Sint16 x, Sint16 y, Sint16 left, Sint16 top, Sint16 right, Sint16 bottom)
{
 int code = 0;
 if (x < left) {
  code |= CLIP_LEFT_EDGE;
 } else if (x > right) {
  code |= CLIP_RIGHT_EDGE;
 }
 if (y < top) {   
  code |= CLIP_TOP_EDGE;
 } else if (y > bottom) {
  code |= CLIP_BOTTOM_EDGE;
 }
 return code;
}


static int clipLine(SDL_Surface *dst, Sint16 *x1, Sint16 *y1, Sint16 *x2, Sint16 *y2)
{
 Sint16 left,right,top,bottom;
 int code1, code2;
 int draw = 0;
 Sint16 swaptmp;
 float m;

 /* Get clipping boundary */
 left = dst->clip_rect.x;
 right = dst->clip_rect.x+dst->clip_rect.w-1;
 top = dst->clip_rect.y;
 bottom = dst->clip_rect.y+dst->clip_rect.h-1;

 while (1) {
  code1 = clipEncode (*x1, *y1, left, top, right, bottom);
  code2 = clipEncode (*x2, *y2, left, top, right, bottom);
  if (CLIP_ACCEPT(code1, code2)) {
   draw = 1;
   break;
  } else if (CLIP_REJECT(code1, code2))
   break;
  else {
   if(CLIP_INSIDE (code1)) {
    swaptmp = *x2; *x2 = *x1; *x1 = swaptmp;
    swaptmp = *y2; *y2 = *y1; *y1 = swaptmp;
    swaptmp = code2; code2 = code1; code1 = swaptmp;
   }
   if (*x2 != *x1) {      
    m = (*y2 - *y1) / (float)(*x2 - *x1);
   } else {
    m = 1.0f;
   }
   if (code1 & CLIP_LEFT_EDGE) {
    *y1 += (Sint16)((left - *x1) * m);
    *x1 = left; 
   } else if (code1 & CLIP_RIGHT_EDGE) {
    *y1 += (Sint16)((right - *x1) * m);
    *x1 = right; 
   } else if (code1 & CLIP_BOTTOM_EDGE) {
    if (*x2 != *x1) {
     *x1 += (Sint16)((bottom - *y1) / m);
    }
    *y1 = bottom;
   } else if (code1 & CLIP_TOP_EDGE) {
    if (*x2 != *x1) {
     *x1 += (Sint16)((top - *y1) / m);
    }
    *y1 = top;
   } 
  }
 }
 
 return draw;
}


/* ----- Line */

/* Non-alpha line drawing code adapted from routine          */
/* by Pete Shinners, pete@shinners.org                       */
/* Originally from pygame, http://pygame.seul.org            */

#define ABS(a) (((a)<0) ? -(a) : (a))

int DrawLine(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color)
{
	int pixx, pixy;
	int x,y;
	int dx,dy;
//	int ax,ay;
	int sx,sy;
	int swaptmp;
	Uint8 *pixel;
	Uint8 *colorptr;

	/* Clip line and test if we have to draw */
	if (!(clipLine(dst,&x1,&y1,&x2,&y2))) {
		return(0);
	}

	/* Test for special cases of straight lines or single point */
	if (x1==x2) {
		if (y1<y2) {
			DrawVLine(dst, y1, y2, x1, color);
			DrawVLine(dst, y1, y2, x1+1, color);
			return 0;
		} else if (y1>y2) {
			DrawVLine(dst, y2, y1, x1, color);
			DrawVLine(dst, y2, y1, x1+1, color);
			return 0;
		} else {
			DrawRectFill(dst,x1,y1,x1+2,y1+2,color);
			return 0;
		}
	}
	if (y1==y2) { 
		if (x1<x2) {
			DrawHLine(dst, x1, x2, y1, color);
			DrawHLine(dst, x1, x2, y1+1, color);
			return 0;
		} else if (x1>x2) { 
			DrawHLine(dst, x2, x1, y1, color);
			DrawHLine(dst, x2, x1, y1+1, color);
			return 0;
		}
	}

 /* Variable setup */
 dx = x2 - x1;
 dy = y2 - y1;
 sx = (dx >= 0) ? 1 : -1;
 sy = (dy >= 0) ? 1 : -1;

 
  /* No alpha blending - use fast pixel routines */

  /* Setup color */
  colorptr=(Uint8 *)&color;
  if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
   //color=SDL_MapRGBA(dst->format, colorptr[0], colorptr[1], colorptr[2], colorptr[3]);
  } else {
   //color=SDL_MapRGBA(dst->format, colorptr[3], colorptr[2], colorptr[1], colorptr[0]);
  }
  
  /* Lock surface */
  SDL_LockSurface(dst);

  /* More variable setup */
  dx = sx * dx + 1;
  dy = sy * dy + 1;
  pixx = dst->format->BytesPerPixel;
  pixy = dst->pitch;
  pixel = ((Uint8*)dst->pixels) + pixx * (int)x1 + pixy * (int)y1;
  pixx *= sx;
  pixy *= sy;
  if (dx < dy) {
   swaptmp = dx; dx = dy; dy = swaptmp;
   swaptmp = pixx; pixx = pixy; pixy = swaptmp;
  }

  /* Draw */
  x=0;
  y=0;
	switch(dst->format->BytesPerPixel) {

		// 8bpp
		case 1:
			for(; x < dx; x++, pixel += pixx) {
				*pixel = color;
				y += dy; 
				if (y >= dx) {
					y -= dx; pixel += pixy;
				}
			}
			break;
	
		// 16bpp
		case 2:
			for (; x < dx; x++, pixel += pixx) {

				// Assumes this is the ninja rope
				*(Uint16*)pixel = color;
				*(Uint16*)(pixel+2) = color;
				*(Uint16*)(pixel+pixy) = color;
				*(Uint16*)(pixel+pixy+2) = color;
				y += dy; 
				if (y >= dx) {
					y -= dx;
					pixel += pixy;
				}
			}
			break;

		// 24bpp
		case 3:
			for(; x < dx; x++, pixel += pixx) {
				if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
					pixel[0] = (color >> 16) & 0xff;
					pixel[1] = (color >> 8) & 0xff;
					pixel[2] = color & 0xff;
				} else {
					pixel[0] = color & 0xff;
					pixel[1] = (color >> 8) & 0xff;
					pixel[2] = (color >> 16) & 0xff;
				}
				y += dy; 
				if (y >= dx) {
					y -= dx; 
					pixel += pixy;
				}
			}
			break;


   default: /* case 4 */
     for(; x < dx; x++, pixel += pixx) {
      *(Uint32*)pixel = color;
      y += dy; 
      if (y >= dx) {
       y -= dx; 
       pixel += pixy;
      }
     }
     break;
  }

  /* Unlock surface */
  SDL_UnlockSurface(dst);
  
  return 0;
}


/*
 * Perform a line draw using a put pixel callback
 * Grabbed from allegro
 */
void perform_line(SDL_Surface *bmp, int x1, int y1, int x2, int y2, int d, void (*proc)(SDL_Surface *, int, int, int))
{
   int dx = x2-x1;
   int dy = y2-y1;
   int i1, i2;
   int x, y;
   int dd;

   /* worker macro */
   #define DO_LINE(pri_sign, pri_c, pri_cond, sec_sign, sec_c, sec_cond)     \
   {                                                                         \
      if (d##pri_c == 0) {                                                   \
	 proc(bmp, x1, y1, d);                                               \
	 return;                                                             \
      }                                                                      \
									     \
      i1 = 2 * d##sec_c;                                                     \
      dd = i1 - (sec_sign (pri_sign d##pri_c));                              \
      i2 = dd - (sec_sign (pri_sign d##pri_c));                              \
									     \
      x = x1;                                                                \
      y = y1;                                                                \
									     \
      while (pri_c pri_cond pri_c##2) {                                      \
	 proc(bmp, x, y, d);                                                 \
									     \
	 if (dd sec_cond 0) {                                                \
	    sec_c sec_sign##= 1;                                             \
	    dd += i2;                                                        \
	 }                                                                   \
	 else                                                                \
	    dd += i1;                                                        \
									     \
	 pri_c pri_sign##= 1;                                                \
      }                                                                      \
   }

   if (dx >= 0) {
      if (dy >= 0) {
	 if (dx >= dy) {
	    /* (x1 <= x2) && (y1 <= y2) && (dx >= dy) */
	    DO_LINE(+, x, <=, +, y, >=);
	 }
	 else {
	    /* (x1 <= x2) && (y1 <= y2) && (dx < dy) */
	    DO_LINE(+, y, <=, +, x, >=);
	 }
      }
      else {
	 if (dx >= -dy) {
	    /* (x1 <= x2) && (y1 > y2) && (dx >= dy) */
	    DO_LINE(+, x, <=, -, y, <=);
	 }
	 else {
	    /* (x1 <= x2) && (y1 > y2) && (dx < dy) */
	    DO_LINE(-, y, >=, +, x, >=);
	 }
      }
   }
   else {
      if (dy >= 0) {
	 if (-dx >= dy) {
	    /* (x1 > x2) && (y1 <= y2) && (dx >= dy) */
	    DO_LINE(-, x, >=, +, y, >=);
	 }
	 else {
	    /* (x1 > x2) && (y1 <= y2) && (dx < dy) */
	    DO_LINE(+, y, <=, -, x, <=);
	 }
      }
      else {
	 if (-dx >= -dy) {
	    /* (x1 > x2) && (y1 > y2) && (dx >= dy) */
	    DO_LINE(-, x, >=, -, y, <=);
	 }
	 else {
	    /* (x1 > x2) && (y1 > y2) && (dx < dy) */
	    DO_LINE(-, y, >=, -, x, <=);
	 }
      }
   }
}


///////////////////
// Draws a rope line
void DrawRope(SDL_Surface *bmp, int x1, int y1, int x2, int y2, Uint32 color)
{
	int sx, sy, dx, dy, t;
	ropealt = 0;
	ropecolour = 0;

	SDL_Rect rect = bmp->clip_rect;
	int	ct = rect.y;
	int cl = rect.x;
	int cr = rect.x + rect.w;
	int cb = rect.y + rect.h;

	sx = x1;
    sy = y1;
    dx = x2;
	dy = y2;

    if (sx > dx) {
		t = sx;
		sx = dx;
		dx = t;
    }

    if (sy > dy) {
		t = sy;
	    sy = dy;
	    dy = t;
    }

    if ((sx >= cr) || (sy >= cb) || (dx < cl) || (dy < ct))
		return;

	t = TRUE;


	perform_line(bmp, x1, y1, x2, y2, color, RopePutPixel);
}


///////////////////
// Draws a beam
void DrawBeam(SDL_Surface *bmp, int x1, int y1, int x2, int y2, Uint32 color)
{
	int sx, sy, dx, dy, t;
	beamalt = 0;

	SDL_Rect rect = bmp->clip_rect;
	int	ct = rect.y;
	int cl = rect.x;
	int cr = rect.x + rect.w;
	int cb = rect.y + rect.h;

	sx = x1;
    sy = y1;
    dx = x2;
	dy = y2;

    if (sx > dx) {
		t = sx;
		sx = dx;
		dx = t;
    }

    if (sy > dy) {
		t = sy;
	    sy = dy;
	    dy = t;
    }

    if ((sx >= cr) || (sy >= cb) || (dx < cl) || (dy < ct))
		return;

	t = TRUE;
	
	perform_line(bmp, x1, y1, x2, y2, color, BeamPutPixel);
}


///////////////////
// Draws a laser sight
void DrawLaserSight(SDL_Surface *bmp, int x1, int y1, int x2, int y2, Uint32 color)
{
	int sx, sy, dx, dy, t;
	laseralt = GetRandomInt(2);

	SDL_Rect rect = bmp->clip_rect;
	int	ct = rect.y;
	int cl = rect.x;
	int cr = rect.x + rect.w;
	int cb = rect.y + rect.h;

	sx = x1;
    sy = y1;
    dx = x2;
	dy = y2;

    if (sx > dx) {
		t = sx;
		sx = dx;
		dx = t;
    }

    if (sy > dy) {
		t = sy;
	    sy = dy;
	    dy = t;
    }

    if ((sx >= cr) || (sy >= cb) || (dx < cl) || (dy < ct))
		return;

	t = TRUE;
	
	perform_line(bmp, x1, y1, x2, y2, color, LaserSightPutPixel);
}