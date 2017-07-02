/////////////////////////////////////////
//
//             Carnage Marines
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Console source code
// Created 7/4/02
// Jason Boettcher


#include "defs.h"
#include "lierox.h"
#include "console.h"

console_t	*Console = NULL;


/* TODO:
[X] Add history
[X] Console doesn't drop at startup every time
[ ] Sometimes commands don't get recognised
[X] Input buffer problem
*/


///////////////////
// Initialize the console
int Con_Initialize(void)
{
	int n;
	Console = (console_t *)malloc(sizeof(console_t));
	if(Console == NULL)
		return false;

	Console->fPosition = 1.0f;
	Console->iState = CON_HIDDEN;
	Console->iCurLength = 0;
	Console->iLastchar = 0;
	Console->icurHistory = -1;
	Console->iNumHistory = 0;
	
	for(n=0;n<MAX_CONLINES;n++) {
		Console->Line[n].strText[0] = 0;
		Console->Line[n].Colour = CNC_NORMAL;
	}

	for(n=0;n<MAX_CONHISTORY;n++)
		Console->History[n].strText[0] = 0;

    Console->bmpConPic = LoadImage("data/gfx/console.png", 16);
    if(!Console->bmpConPic)
        return false;

	return true;
}


///////////////////
// Toggle the console
void Con_Toggle(void)
{
	if(Console->iState == CON_HIDDEN || Console->iState == CON_HIDING) {
		Console->iState = CON_DROPPING;
        if(!tLXOptions->iFullscreen)
		    SDL_ShowCursor(SDL_ENABLE);
	}

	else if(Console->iState == CON_DROPPING || Console->iState == CON_DOWN) {
		Console->iState = CON_HIDING;
        if(!tLXOptions->iFullscreen)
		    SDL_ShowCursor(SDL_DISABLE);
	}
}


///////////////////
// Hide the console
void Con_Hide(void)
{
	Console->iState = CON_HIDDEN;
	Console->fPosition = 1;
}


///////////////////
// Process the console
void Con_Process(float dt)
{
	keyboard_t *kb = GetKeyboard();
	int		History = 0;
	SDL_Event *Ev = GetEvent();

	if(kb->KeyUp[SDLK_BACKQUOTE])
		Con_Toggle();

	if(Console->iState == CON_DROPPING)
		Console->fPosition -= 3.0f*dt;
	if(Console->iState == CON_HIDING)
		Console->fPosition += 3.0f*dt;

	if(Console->fPosition < 0.0f) {
		Console->iState = CON_DOWN;
		Console->fPosition = 0.0f;
	}
	if(Console->fPosition > 1) {
		Console->iState = CON_HIDDEN;
		Console->fPosition = 1;
		
		Console->iCurLength = 0;
		Console->Line[0].strText[0] = '\0';
	}

	if(Console->iState != CON_DOWN && Console->iState != CON_DROPPING)
		return;
	

	// Add text to the console
	//ProcessEvents();
	//SDL_Event *Ev = GetEvent();

	// Make sure a key event happened
	if(Ev->type != SDL_KEYUP && Ev->type != SDL_KEYDOWN)
		return;
		

	// Check the characters
	if(Ev->key.state == SDL_PRESSED) {
		int input = (char)(Ev->key.keysym.unicode & 0x007F);

		
		// Handle the character
		if(Console->iLastchar != input) {
			Console->iLastchar = input;


			if(input == '\b') {
				if(Console->iCurLength > 0)
					Console->Line[0].strText[--Console->iCurLength] = '\0';
				Console->icurHistory = -1;
			}


			// Enter key
			if(input == '\n' || input == '\r') {
				
				Con_Printf(CNC_NORMAL,"]%s",Console->Line[0].strText);

				// Parse the line
				Cmd_ParseLine(Console->Line[0].strText);
				Con_AddHistory(Console->Line[0].strText);


				Console->Line[0].strText[0] = '\0';
				Console->iCurLength = 0;

				return;
			}

			// Tab
			if(input == '\t') {
				// Auto-complete
				Cmd_AutoComplete(Console->Line[0].strText,&Console->iCurLength);
					//CV_AutoComplete(Console->Line[0].strText,&Console->iCurLength);
				Console->icurHistory = -1;
			}

			// Up


			// Normal key
			if(isprint(input)) {
				Console->Line[0].Colour = CNC_NORMAL;
				Console->Line[0].strText[Console->iCurLength++] = input;
				Console->Line[0].strText[Console->iCurLength] = '\0';
				Console->icurHistory = -1;
			}

		}
	}

	// Up arrow
	if(kb->KeyUp[SDLK_UP]) {
		Console->icurHistory++;
		Console->icurHistory = MIN(Console->icurHistory,Console->iNumHistory-1);
		
		if(Console->icurHistory >= 0) {
			Console->Line[0].Colour = CNC_NORMAL;
			strcpy(Console->Line[0].strText, Console->History[Console->icurHistory].strText);
			Console->iCurLength = strlen(Console->Line[0].strText);
		}
	}

	// Down arrow
	if(kb->KeyUp[SDLK_DOWN]) {
		Console->icurHistory--;
		if(Console->icurHistory >= 0) {
			Console->Line[0].Colour = CNC_NORMAL;
			strcpy(Console->Line[0].strText, Console->History[Console->icurHistory].strText);
			Console->iCurLength = strlen(Console->Line[0].strText);
		} else {
			Console->Line[0].strText[0] = 0;
			Console->iCurLength=0;
		}

		Console->icurHistory = MAX(Console->icurHistory,-1);
	}

	if(Ev->key.state == SDL_RELEASED && Ev->type == SDL_KEYUP)
		Console->iLastchar = 0;
}


///////////////////
// Print a formatted string to the console
void Con_Printf(int colour, char *fmt, ...)
{
	char buf[512];
	va_list	va;

	va_start(va,fmt);
	vsprintf(buf,fmt,va);
	va_end(va);

	Con_AddText(colour, buf);

    //printf("Con: %s\n",buf);
}


///////////////////
// Add a string of text to the console
void Con_AddText(int colour, char *text)
{
	// Move all the text up, losing the last line
	for(int n=MAX_CONLINES-2;n>=1;n--) {
		strcpy(Console->Line[n+1].strText,Console->Line[n].strText);
		Console->Line[n+1].Colour = Console->Line[n].Colour;
	}

	strcpy(Console->Line[1].strText,text);
	Console->Line[1].Colour = colour;
}


///////////////////
// Add a command to the history
void Con_AddHistory(char *text)
{
	// Move the history up one, dropping the last
	for(int n=MAX_CONHISTORY-2;n>=0;n--)
		strcpy(Console->History[n+1].strText, Console->History[n].strText);

	Console->icurHistory=-1;
	Console->iNumHistory++;
	Console->iNumHistory = MIN(Console->iNumHistory,MAX_CONHISTORY-1);

	strcpy(Console->History[0].strText,text);
}


///////////////////
// Draw the console
void Con_Draw(SDL_Surface *bmpDest)
{
	int y = (int)(-Console->fPosition * (float)Console->bmpConPic->h);
	int texty = y+Console->bmpConPic->h-28;
	char buf[256];

	Uint32 Colours[6] = {0xffff, MakeColour(200,200,200), MakeColour(255,0,0), MakeColour(200,128,128),
		                 MakeColour(100,100,255), MakeColour(100,255,100) };
	
	if(Console->iState == CON_HIDDEN)
		return;

	DrawImage(bmpDest,Console->bmpConPic,0,y);


	// Draw the lines of text
	for(int n=0;n<MAX_CONLINES;n++,texty-=15) {
		buf[0] = 0;


		if(n==0) {
			buf[0] = ']'; buf[1] = 0;
		}
		strcat(buf,Console->Line[n].strText);
		if(n==0)
			strcat(buf,"_");
		
		tLX->cFont.Draw(bmpDest,12,texty,Colours[Console->Line[n].Colour],buf);
	}
}


///////////////////
// Returns if the console is in use
int Con_IsUsed(void)
{
	return Console->iState != CON_HIDDEN;
}


///////////////////
// Shutdown the console
void Con_Shutdown(void)
{
	if(Console)
		free(Console);
	
	Console = NULL;
}