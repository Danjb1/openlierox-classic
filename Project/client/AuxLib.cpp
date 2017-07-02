/////////////////////////////////////////
//
//   Auxiliary Software class library
//
/////////////////////////////////////////


// Auxiliary library 
// Created 12/11/01
// By Jason Boettcher


#include "defs.h"
#include <SDL/SDL_syswm.h>
#include "LieroX.h"
#include "2xsai.h"


// Game info
char		GameName[32];
int         nFocus = true;

// Config file
char		ConfigFile[64];

// Keyboard, Mouse, & Event
keyboard_t	Keyboard;
mouse_t		Mouse;
SDL_Event	Event;

SDL_Surface *bmpIcon=NULL;


///////////////////
// Initialize the standard Auxiliary Library
int InitializeAuxLib(char *gname, char *config, int bpp, int vidflags)
{
	// Set the game info
	strcpy(GameName,gname);

	strcpy(ConfigFile,config);

	
	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE) == -1) {
		SystemError("Failed to initialize the SDL system!\nErrorMsg: %s",SDL_GetError());
		return false;
	}

	bmpIcon = SDL_LoadBMP("data/icon.bmp");
	if(bmpIcon)
		SDL_WM_SetIcon(bmpIcon,NULL);


	if(!SetVideoMode())
		return false;

    // Enable the system events
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);


	// Initialize the 2xsai system
	Init_2xSaI(565);

    if( !nDisableSound ) {
	    // Initialize BASS
	    if(!BASS_Init(-1,44100,0,GetForegroundWindow())) {
		    printf("Warning: Failed the initialize the sound library");
	    }
    }

	if( tLXOptions->iSoundOn ) {
		BASS_Start();
		BASS_SetVolume( tLXOptions->iSoundVolume );
	}


	// Give a seed to the random number generator
	srand((unsigned int)time(NULL));


	// Initialize the cache
	if(!InitializeCache())
		return false;

	

	// Initialize the keyboard & mouse
	ProcessEvents();
	for(int k = 0;k<SDLK_LAST;k++)
		Keyboard.KeyUp[k] = false;
	
	Mouse.Button = 0;
	Mouse.Up = 0;



	/*
		Temp thing to create a random number table
	*/
	/*FILE *fp = fopen("randomnum.cpp", "wt");

	fprintf(fp, "float RandomNumbers[] = { ");
	for(int i=0;i<256; i++) {
		if(i % 8 == 0 && i>0)
			fprintf(fp, "\n                          ");

		float f = GetRandomNum();
		if(f>=0)
			fprintf(fp, " ");
		fprintf(fp, "%ff", f );
		if(i!= 255)
			fprintf(fp, ", ");		
	}
	fprintf(fp, " };\n\n");
	fclose(fp);*/
	
	
	return true;
}


///////////////////
// Set the video mode
int SetVideoMode(void)
{
	int HardwareBuf = false;
	int DoubleBuf = false;
	int vidflags = 0;

	// Initialize the video
	if(tLXOptions->iFullscreen)
		vidflags |= SDL_FULLSCREEN;

	vidflags |= SDL_SWSURFACE;
	
	/*if(HardwareBuf)
		vidflags |= SDL_HWSURFACE;
	else
		vidflags |= SDL_SWSURFACE;

	if(DoubleBuf)
		vidflags |= SDL_DOUBLEBUF;*/


	if( SDL_SetVideoMode(640,480, 16,vidflags) == NULL) {
		SystemError("Failed to set the video mode %dx%dx%d\nErrorMsg: %s", 640, 480, 16,SDL_GetError());
		return false;
	}


	SDL_EnableUNICODE(1);
	SDL_WM_SetCaption(GameName,NULL);
	SDL_ShowCursor(SDL_DISABLE);

	return true;
}


///////////////////
// Process the events
void ProcessEvents(void)
{
    // Clear the queue
    Keyboard.queueLength = 0;
	
	while(SDL_PollEvent(&Event)) {

        // Quit event
		if(Event.type == SDL_QUIT) {
			// Quit out in a very ugly way
			tLX->iQuitGame = true;
			tLX->iQuitEngine = true;
			//tMenu->iMenuRunning = false;
		}

        // System events
        if(Event.type == SDL_SYSWMEVENT) {
            SDL_SysWMmsg *msg = Event.syswm.msg;

            switch(msg->msg) {

                // Lose focus event
                case WM_KILLFOCUS:
			        nFocus = false;
                    break;

                // Gain focus event
                case WM_SETFOCUS:
                    nFocus = true;
                    break;

            }
        }


        // Keyboard events
		if(Event.type == SDL_KEYUP || Event.type == SDL_KEYDOWN) {

			// Check the characters
			if(Event.key.state == SDL_PRESSED || Event.key.state == SDL_RELEASED) {
				
				char input = (char)(Event.key.keysym.unicode & 0x007F);

                // If we're going to over the queue length, shift the list down and remove the oldest key
                if(Keyboard.queueLength+1 >= MAX_KEYQUEUE) {
                    for(int i=0; i<Keyboard.queueLength-1; i++)
                        Keyboard.keyQueue[i] = Keyboard.keyQueue[i+1];
                    Keyboard.queueLength--;
                }

                // Key down
                if(Event.type == SDL_KEYDOWN) {
                    Keyboard.keyQueue[Keyboard.queueLength++] = input;
                }

				// Key up
				if(Event.type == SDL_KEYUP || Event.key.state == SDL_RELEASED)
                    Keyboard.keyQueue[Keyboard.queueLength++] = -input;

                
            }
        }
	}


    // If we don't have focus, don't update as often
    if(!nFocus)
        Sleep(10);


	// Mouse
	Mouse.Button = SDL_GetMouseState(&Mouse.X,&Mouse.Y);
	Mouse.Up = 0;
    Mouse.FirstDown = 0;
	
	if(!(Mouse.Button & SDL_BUTTON(1)) && Mouse.Down & SDL_BUTTON(1))
		Mouse.Up |= SDL_BUTTON(1);

	if(!(Mouse.Button & SDL_BUTTON(3)) && Mouse.Down & SDL_BUTTON(3))
		Mouse.Up |= SDL_BUTTON(3);

    for( int i=0; i<3; i++ ) {
        if( !(Mouse.Down & SDL_BUTTON(i)) && (Mouse.Button & SDL_BUTTON(i)) )
            Mouse.FirstDown |= SDL_BUTTON(i);
    }
		
	Mouse.Down = Mouse.Button;

    // SAFETY HACK: If we get any mouse presses, we must have focus
    if(Mouse.Down)
        nFocus = true;



	// Keyboard
	Keyboard.keys = SDL_GetKeyState(NULL);

	// Update the key up's
	for(int k=0;k<SDLK_LAST;k++) {
		Keyboard.KeyUp[k] = false;
		
		if(!Keyboard.keys[k] && Keyboard.KeyDown[k])
			Keyboard.KeyUp[k] = true;
		Keyboard.KeyDown[k] = Keyboard.keys[k];        
	}
}


///////////////////
// Flip the screen
void FlipScreen(SDL_Surface *psScreen)
{
    // Take a screenshot?
    // We do this here, because there are so many graphics loops, but this function is common
    // to all of them
    if( Keyboard.KeyUp[SDLK_F12] )
        TakeScreenshot();

    SDL_Flip( psScreen );
}


///////////////////
// Shutdown the standard Auxiliary Library
void ShutdownAuxLib(void)
{
	if(bmpIcon)
		SDL_FreeSurface(bmpIcon);

	// Shutdown the error system
	EndError();

	// Shutdown the cache
	ShutdownCache();

	// Shutdown the SDL system
	SDL_Quit();
}



///////////////////
// Return the game name
char *GetGameName(void)
{
	return GameName;
}

///////////////////
// Return the keyboard structure
keyboard_t *GetKeyboard(void)
{
	return &Keyboard;
}

///////////////////
// Return the mouse structure
mouse_t *GetMouse(void)
{
	return &Mouse;
}

///////////////////
// Return the config filename
char *GetConfigFile(void)
{
	return ConfigFile;
}


///////////////////
// Return the event
SDL_Event *GetEvent(void)
{
	return &Event;
}


///////////////////
// Get text from the clipboard
// Returns the length of the text (0 for no text)
int GetClipboardText(char *szText, int nMaxLength)
{
    if( !szText )
        return 0;

    // Get the window handle
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if(!SDL_GetWMInfo(&info))
		return 0;

	HWND    WindowHandle = info.window;
    HANDLE  CBDataHandle; // handle to the clipboard data
    LPSTR   CBDataPtr;    // pointer to data to send

    // Windows version
    if( IsClipboardFormatAvailable(CF_TEXT) ) {
            
        if( OpenClipboard(WindowHandle) ) {
            CBDataHandle = GetClipboardData(CF_TEXT);
                
            if(CBDataHandle) {
                CBDataPtr = (LPSTR)GlobalLock(CBDataHandle);
                int TextSize = strlen(CBDataPtr);

                strncpy(szText, CBDataPtr, nMaxLength);
                if( TextSize < nMaxLength )
                    szText[TextSize] = '\0';
                else
                    szText[nMaxLength-1] = '\0';

                GlobalUnlock(CBDataHandle);
                CloseClipboard();

                return TextSize < nMaxLength ? TextSize : nMaxLength;
            }
        }
    }

    return 0;
}


///////////////////
// Take a screenshot
void TakeScreenshot(void)
{
	char		picname[80]; 
	char		checkname[_MAX_PATH];
	int			i;
	FILE		*f;

    // Create the 'scrshots' directory if it doesn't exist
    _mkdir("scrshots");

	strcpy(picname,"lierox00.bmp");

    for(i=0; i<100; i++) {

		picname[6] = i/10 + '0'; 
		picname[7] = i%10 + '0'; 
		sprintf(checkname, "scrshots/%s", picname);
		f = fopen(checkname, "rb");
		if (!f)
			break;	// file doesn't exist
		fclose(f);
	}


	if(i == 100) {
		// Couldn't take screenshot
		return;
 	}

    // Save the screen surface
    SDL_SaveBMP( SDL_GetVideoSurface(), checkname );
}