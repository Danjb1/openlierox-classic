/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Main entry point
// Created 28/6/02
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"
#include "Menu.h"
#include "console.h"


CClient		*cClient = NULL;
CServer		*cServer = NULL;
lierox_t	*tLX = NULL;
game_t		tGameInfo;

int         nDisableSound = false;

keyboard_t	*kb;
SDL_Surface	*Screen;

CVec		vGravity = CVec(0,4);



///////////////////
// Main entry point
int main(int argc, char *argv[])
{
    int     startgame = false;
    float   fMaxFPS = 72;

	if(!LoadOptions())
		return -1;

    // Parse the arguments
    ParseArguments(argc, argv);

	if(!InitializeLieroX())
		return -1;
	kb = GetKeyboard();
	Screen = SDL_GetVideoSurface();

	if(!Menu_Initialize(&startgame)) {
        SystemError("Error: Could not initialize the menu system.\nError when loading graphics files");
		ShutdownLieroX();
		return -1;
	}

	while(!tLX->iQuitGame) {

		startgame = false;

		// Start the menu
		Menu_Start();
		
		if(startgame) {
			// Start the game
			StartGame();

		} else {
			// Quit
			ShutdownLieroX();
			return 0;
		}

		// Pre-game initialization
		Screen = SDL_GetVideoSurface();
		float oldtime = GetMilliSeconds();
        float captime = GetMilliSeconds();
		
		ClearEntities();

		ProcessEvents();
		tLX->iQuitEngine = false;
        fMaxFPS = 1.0f / (float)tLXOptions->nMaxFPS;

        //
        // Main game loop
        //
		while(!tLX->iQuitEngine) {

            tLX->fCurTime = GetMilliSeconds();

            // Cap the FPS
            if(tLX->fCurTime - captime < fMaxFPS)
                continue;
            else
                captime = tLX->fCurTime;

            ProcessEvents();

			// Timing
			tLX->fDeltaTime = tLX->fCurTime - oldtime;
			oldtime = tLX->fCurTime;

			// Main frame
			GameLoop();

			FlipScreen(Screen);
		}
	}


	ShutdownLieroX();

	return 0;
}


///////////////////
// Parse the arguments
void ParseArguments(int argc, char *argv[])
{
    // Parameters passed to liero xtreme overwrite the loaded options
    char *a;
    for(int i=1; i<argc; i++) {
        a = argv[i];

        // -nosound
        // Turns off the sound
        if( stricmp(a, "-nosound") == 0 ) {
            nDisableSound = true;
            tLXOptions->iSoundOn = false;
        }

        // -window
        // Turns fullscreen off
        if( stricmp(a, "-window") == 0 ) {
            tLXOptions->iFullscreen = false;
        }


        // -fullscreen
        // Turns fullscreen on
        if( stricmp(a, "-fullscreen") == 0 ) {
            tLXOptions->iFullscreen = true;
        }
    }
}


///////////////////
// Initialize the game
int InitializeLieroX(void)
{
	// Initialize the aux library
	if(!InitializeAuxLib("Liero Xtreme","config.cfg",16,0))
		return false;

	// Initialize the network
    if(!nlInit()) {
        SystemError("Error: Failed to initialize the network library");
		return false;
    }

    if(!nlSelectNetwork(NL_IP)) {
        SystemError("Error: Failed to initialize the network library\nCould not select IP");
		return false;
    }


	// Allocate the client & server
	cClient = new CClient;
    if(cClient == NULL) {
        SystemError("Error: InitializeLieroX() Out of memory");
		return false;
    }

	//cClient->Clear();

	cServer = new CServer;
    if(cServer == NULL) {
        SystemError("Error: InitializeLieroX() Out of memory");
		return false;
    }

	// Initialize the entities
    if(!InitializeEntities()) {
        SystemError("Error: InitializeEntities() Out of memory");
		return false;
    }


	// Initialize the LieroX structure
	tLX = new lierox_t;
    if(tLX == NULL) {
        SystemError("Error: InitializeLieroX() Out of memory");
		return false;
    }
	tLX->iQuitGame = false;
	tLX->debug_string[0] = 0;

	// Load the graphics
    if(!LoadGraphics()) {
        SystemError("Error: Error loading graphics");
		return false;
    }

    // Initialize the console
    if(!Con_Initialize()) {
        SystemError("Error: Could not initialize the console");
        return false;
    }

    // Add some console commands
    Cmd_AddCommand("kick", Cmd_Kick);

	// Load the sounds
	LoadSounds();


	// Load the profiles
	LoadProfiles();

    // Initialize the game info structure
	tGameInfo.iNumPlayers = 0;
    tGameInfo.sMapRandom.psObjects = NULL;


	return true;
}


///////////////////
// Start the game
void StartGame(void)
{
    // Clear the screen
    DrawRectFill(SDL_GetVideoSurface(), 0,0, 640,480, 0);

	// Local game
	if(tGameInfo.iGameType == GME_LOCAL) {

		// Start the server
		if(!cServer->StartServer( "local", tLXOptions->iNetworkPort, 8, false )) {
			// ERROR
			MessageBox(NULL, "Error: Could not start server", "Liero Xtreme Error", MB_OK);
			return;
		}

		// Setup the client
		if(!cClient->Initialize()) {
			// ERROR

			MessageBox(NULL, "Error: Could not initialize client", "Liero Xtreme Error", MB_OK);
			return;
		}

		// Tell the client to connect to the server
		cClient->Connect("127.0.0.1");
	}
}


///////////////////
// Game loop
void GameLoop(void)
{
    if(tLX->iQuitEngine)
        return;

	// Local
	if(tGameInfo.iGameType == GME_LOCAL) {
		cClient->Frame();

		cServer->Frame();

		// If we are connected, just start the game straight away (bypass lobby in local)
		if(cClient->getStatus() == NET_CONNECTED) {
			if(cServer->getState() == SVS_LOBBY)
				cServer->StartGame();
		}

		cClient->Draw(Screen);
	}


	// Hosting
	if(tGameInfo.iGameType == GME_HOST) {
		cClient->Frame();
		cServer->Frame();

		cClient->Draw(Screen);
	}


	// Joined
	if(tGameInfo.iGameType == GME_JOIN) {
		cClient->Frame();
		cClient->Draw(Screen);
	}
}


///////////////////
// Quit back to the menu
void QuittoMenu(void)
{
	tLX->iQuitEngine = true;
    Menu_SetSkipStart(false);
	cClient->Disconnect();
}


///////////////////
// Shutdown the game
void ShutdownLieroX(void)
{
    Con_Shutdown();

	ShutdownGraphics();

	Menu_Shutdown();
	ShutdownProfiles();

	ShutdownOptions();

    // Free the game info structure
    if(tGameInfo.sMapRandom.psObjects)
        delete[] tGameInfo.sMapRandom.psObjects;

    tGameInfo.sMapRandom.psObjects = NULL;

	// Free the client & server
	if(cClient) {
		cClient->Shutdown();
		delete cClient;
		cClient = NULL;
	}

	if(cServer) {
		cServer->Shutdown();
		delete cServer;
		cServer = NULL;
	}

	ShutdownEntities();
	
	if(tLX) {
		delete tLX;
		tLX = NULL;
	}

	nlShutdown();
	
	ShutdownAuxLib();
}