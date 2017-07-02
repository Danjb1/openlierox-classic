/////////////////////////////////////////
//
//   Auxiliary Software class library
//
/////////////////////////////////////////


// Error system
// Created 12/11/01
// By Jason Boettcher


#include "defs.h"


int		GotError = false;
char	ErrorMsg[128];

FILE *ErrorFile = NULL;


///////////////////
// Sets the error message
void SetError(char *fmt, ...)
{
	va_list	va;

	va_start(va,fmt);
	vsprintf(ErrorMsg,fmt,va);
	va_end(va);
	GotError = true;


	if(ErrorFile == NULL) {
		ErrorFile = fopen("Error.txt","wt");
		if(ErrorFile == NULL)
			return;
		fprintf(ErrorFile,"%s error file\n----------------------\n",GetGameName());
	}
	
	fprintf(ErrorFile,"%s\n",ErrorMsg);
}


///////////////////
// Show the error
void ShowError(void)
{
	SDL_ShowCursor(SDL_ENABLE);

	if(GotError)
		MessageBox(NULL,ErrorMsg,"Liero Xtreme",MB_OK | MB_ICONEXCLAMATION);
	else
		MessageBox(NULL,"Unkown Error",GetGameName(),MB_OK | MB_ICONEXCLAMATION);

	SDL_ShowCursor(SDL_DISABLE);
}


///////////////////
// End the error system
void EndError(void)
{
	if(ErrorFile)
		fclose(ErrorFile);
	ErrorFile = NULL;
}


///////////////////
// Show a system error
void SystemError(char *fmt, ...)
{
	char buf[512];
	va_list	va;

	va_start(va,fmt);
	vsprintf(buf,fmt,va);
	va_end(va);

	SDL_ShowCursor(SDL_ENABLE);	
	MessageBox(NULL,buf,"Liero Xtreme",MB_OK | MB_ICONEXCLAMATION);


	// Shutdown the game
	SDL_Quit();
	exit(1);
}