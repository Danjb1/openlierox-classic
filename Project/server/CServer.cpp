/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Server class
// Created 28/6/02
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"
#include "console.h"


///////////////////
// Clear the server
void CServer::Clear(void)
{
	cClients = NULL;
	cMap = NULL;
	//cProjectiles = NULL;
	cWorms = NULL;
	iState = SVS_LOBBY;
	iServerFrame=0;
	iNumPlayers = 0;
	iRandomMap = false;
	iMaxWorms = 8;
	iGameOver = false;
	iGameType = GMT_DEATHMATCH;
	fLastBonusTime = 0;
	tSocket = NL_INVALID;
	tGameLobby.nSet = false;
	bRegServer = false;
	bServerRegistered = false;
	fLastRegister = 0;
	nPort = LX_PORT;

	cShootList.Clear();
}


///////////////////
// Start a server
int CServer::StartServer(char *name, int port, int maxplayers, int regserver)
{
	// Shutdown and clear any previous server settings
	Shutdown();
	Clear();

	strcpy(sName, name);
	iMaxWorms = maxplayers;
	bRegServer = regserver;
	nPort = port;



	// Open the socket
	tSocket = nlOpen(port, NL_UNRELIABLE);
	if(tSocket == NL_INVALID) {
		SystemError("Error: Could not open UDP socket");
		return false;
	}
	if(!nlListen(tSocket)) {
		return false;
	}

	NLaddress addr;    
	nlGetLocalAddr(tSocket,&addr);
	nlAddrToString(&addr, tLX->debug_string);


	// Initialize the clients
	cClients = new CClient[MAX_CLIENTS];
	if(cClients==NULL) {
		SetError("Error: Out of memory!\nsv::Startserver() %d",__LINE__);
		return false;
	}

	// Allocate the worms
	cWorms = new CWorm[MAX_WORMS];
	if(cWorms == NULL) {
		SetError("Error: Out of memory!\nsv::Startserver() %d",__LINE__);
		return false;
	}
	
	// Initialize the bonuses
	for(int i=0;i<MAX_BONUSES;i++)
		cBonuses[i].setUsed(false);

	// Shooting list
	if( !cShootList.Initialize() ) {
		SetError("Error: Out of memory!\nsv::Startserver() %d",__LINE__);
		return false;
	}

	// In zee lobby
	iState = SVS_LOBBY;

	// Setup the register so it happens on the first frame
	bServerRegistered = true;
    fLastRegister = -99999;
	//if(bRegServer)
		//RegisterServer();

	// Initialize the clients
	for(i=0;i<MAX_CLIENTS;i++) {
		cClients[i].Clear();

		// Initialize the shooting list
		if( !cClients[i].getShootList()->Initialize() )
			return false;
	}


	return true;
}


///////////////////
// Start the game
int CServer::StartGame(void)
{
	CBytestream bs;
	
	iLives =		 tGameInfo.iLives;
	iGameType =		 tGameInfo.iGameMode;
	iMaxKills =		 tGameInfo.iKillLimit;
	iTimeLimit =	 tGameInfo.iTimeLimit;
	iTagLimit =		 tGameInfo.iTagLimit;
	strcpy(sModName, tGameInfo.sModName);
	iLoadingTimes =	 tGameInfo.iLoadingTimes;
	iBonusesOn =	 tGameInfo.iBonusesOn;
	iShowBonusName = tGameInfo.iShowBonusName;


	// Shutdown any previous map instances
	if(cMap) {
		cMap->Shutdown();
		delete cMap;
		cMap = NULL;
	}

	// Create the map
	cMap = new CMap;
	if(cMap == NULL) {
		SetError("Error: Out of memory!\nsv::Startgame() %d",__LINE__);
		return false;
	}
	
	iRandomMap = false;
	if(stricmp(tGameInfo.sMapname,"_random_") == 0)
		iRandomMap = true;

	if(iRandomMap) {
        char buf[128];
        cMap->New(504,350,"dirt");
		cMap->ApplyRandomLayout( &tGameInfo.sMapRandom );

        // Free the random layout
        if( tGameInfo.sMapRandom.psObjects )
            delete[] tGameInfo.sMapRandom.psObjects;
        tGameInfo.sMapRandom.psObjects = NULL;
        tGameInfo.sMapRandom.bUsed = false;

	} else {

		strcpy(sMapFilename,"levels/");
		strcat(sMapFilename, tGameInfo.sMapname);
		if(!cMap->Load(sMapFilename)) {
			return false;
		}
	}

	// Load the game script
	cGameScript.Shutdown();
	if(!cGameScript.Load(sModName)) {
		printf("Error: Could not load the '%s' game script\n",sModName);
		return false;
	}


    // Load & update the weapon restrictions
    cWeaponRestrictions.loadList("cfg/wpnrest.dat");
    cWeaponRestrictions.updateList(&cGameScript);
    

	// Set some info on the worms
	for(int i=0;i<MAX_WORMS;i++) {
		if(cWorms[i].isUsed()) {
			cWorms[i].setLives(iLives);
            cWorms[i].setKills(0);
			cWorms[i].setGameScript(&cGameScript);
            cWorms[i].setWpnRest(&cWeaponRestrictions);
			cWorms[i].setLoadingTime( (float)iLoadingTimes / 100.0f );
		}
	}

	// Clear bonuses
	for(i=0; i<MAX_BONUSES; i++)
		cBonuses[i].setUsed(false);

	// Clear the shooting list
	cShootList.Clear();



	fLastBonusTime = tLX->fCurTime;

	// Set all the clients to 'not ready'
	for(i=0;i<MAX_CLIENTS;i++) {
		cClients[i].getShootList()->Clear();
		cClients[i].setGameReady(false);
	}


    // If this is the host, and we have a team game: Send all the worm info back so the worms know what 
    // teams they are on
    if( tGameInfo.iGameType == GME_HOST ) {
        if( iGameType == GMT_TEAMDEATH ) {

            CWorm *w = cWorms;
            CBytestream b;

            for( i=0; i<MAX_WORMS; i++, w++ ) {
                if( !w->isUsed() )
                    continue;
            
                // Write out the info
                b.writeByte(S2C_WORMINFO);
			    b.writeInt(w->getID(),1);			    
                w->writeInfo(&b);
            }

            SendGlobalPacket(&b);
        }
    }

	

	iState = SVS_GAME;		// In-game, waiting for players to load
	iServerFrame = 0;
    iGameOver = false;

	bs.writeByte(S2C_PREPAREGAME);
	bs.writeInt(iRandomMap,1);
	if(!iRandomMap)
		bs.writeString(sMapFilename);
	
	// Game info
	bs.writeInt(iGameType,1);
	bs.writeShort(iLives);
	bs.writeShort(iMaxKills);
	bs.writeShort(iTimeLimit);
	bs.writeShort(iLoadingTimes);
	bs.writeInt(iBonusesOn, 1);
	bs.writeInt(iShowBonusName, 1);
	if(iGameType == GMT_TAG)
		bs.writeShort(iTagLimit);
	bs.writeString(sModName);

    cWeaponRestrictions.sendList(&bs);

	SendGlobalPacket(&bs);


	return true;
}


///////////////////
// Begin the match
void CServer::BeginMatch(void)
{
	int i;

	iState = SVS_PLAYING;


	// Initialize some server settings
	fServertime = 0;
	iServerFrame = 0;
    iGameOver = false;
	cShootList.Clear();

	// Send the connected clients a startgame message
	CBytestream bs;
	bs.writeInt(S2C_STARTGAME,1);

	SendGlobalPacket(&bs);

	// If this is a game of tag, find a random worm to make 'it'
	if(iGameType == GMT_TAG)
		TagRandomWorm();


	// Spawn the worms
	for(i=0;i<MAX_WORMS;i++) {
		if(cWorms[i].isUsed())
			SpawnWorm(&cWorms[i]);
	}
}


///////////////////
// Main server frame
void CServer::Frame(void)
{
	// Playing frame
	if(iState == SVS_PLAYING) {
		fServertime += tLX->fDeltaTime;
		iServerFrame++;
	}

	// Process any http requests (register, deregister)
	if(bRegServer && !bServerRegistered )
		ProcessRegister();


	ReadPackets();

	SimulateGame();

	CheckTimeouts();

	CheckRegister();

	SendPackets();
}


///////////////////
// Read packets
void CServer::ReadPackets(void)
{
	CBytestream bs;
	NLaddress adrFrom;
	int c;
	
	while(bs.Read(tSocket)) {

		nlGetRemoteAddr(tSocket,&adrFrom);
		
		// Check for connectionless packets (four leading 0xff's)
		if(*(int *)bs.GetData() == -1) {
			bs.SetPos(4);
			ParseConnectionlessPacket(&bs);
			continue;
		}


		// Read packets
		CClient *cl = cClients;
		for(c=0;c<MAX_CLIENTS;c++,cl++) {
			
			// Player not connected
			if(cl->getStatus() == NET_DISCONNECTED)
				continue;

			// Check if the packet is from this player
			if(!nlAddrCompare(&adrFrom,cl->getChannel()->getAddress()))
				continue;

			
			// TODO: Check ports

			// Process the net channel			
            if(cl->getChannel()->Process(&bs)) {

                // Only process the actual packet for playing clients
                if( cl->getStatus() != NET_ZOMBIE )
				    ParseClientPacket(cl,&bs);
            }
		}
	}
}


///////////////////
// Send packets
void CServer::SendPackets(void)
{
	int c;
	CClient *cl = cClients;
	CBytestream *bs;
	static float oldtime =0;

	if(tLX->fCurTime - oldtime < 1.0/72.0)
		return;		// So we don't flood packets out to the clients
	else
		oldtime = tLX->fCurTime;



	// Go through each client and send them a message
	for(c=0;c<MAX_CLIENTS;c++,cl++) {
		if(cl->getStatus() == NET_DISCONNECTED)
			continue;


		if(iState == SVS_PLAYING && cl->getStatus() != NET_ZOMBIE)
			SendUpdate(cl);

		// Send out the packets if we havn't gone over the clients bandwidth
		bs = cl->getUnreliable();
		cl->getChannel()->Transmit(bs);
		
		// Clear the unreliable bytestream
		cl->getUnreliable()->Clear();
	}
}


///////////////////
// Register the server
void CServer::RegisterServer(void)
{
	// Create the url
	char url[1024];
    char svr[1024];
	char buf[512];

	NLaddress addr;

	nlGetLocalAddr(tSocket,&addr);	

	sprintf(url, "%s?port=%d&addr=%s", LX_SVRREG, nPort, nlAddrToString(&addr, buf));

    bServerRegistered = false;

    // Get the first server from the master server list
    FILE *fp = fopen("cfg/masterservers.txt","rt");
    if( !fp ) {
        bRegServer = false;
        notifyLog("Could not register with master server: 'Could not open master server file'");
        return;
    }

    // Find the first line
    svr[0] = '\0';
    while( fgets(svr, 1023, fp) ) {
        strcpy( svr, StripLine(svr) );
        if( strlen(svr) > 0 ) {
            if( !http_InitializeRequest(svr, url) ) {
                bRegServer = false;
                notifyLog("Could not register with master server: 'Could not open TCP socket'");
            }
            break;
        }
    }

    fclose(fp);
}


///////////////////
// Process the registering of the server
void CServer::ProcessRegister(void)
{
    char szError[512];

	if(!bRegServer || bServerRegistered)
		return;

	int result = http_ProcessRequest(szError);

	// Normal, keep going
	if(result == 0)
		return;
	// Failed
    if(result == -1) {
		bRegServer = false;
        notifyLog("Could not register with master server: %s", szError);
    }
	// Completed ok
	if(result == 1) {
		bServerRegistered = true;
		fLastRegister = tLX->fCurTime;
	}
}


///////////////////
// This checks the registering of a server
void CServer::CheckRegister(void)
{
	// If we don't want to register, just leave
	if(!bRegServer)
		return;

	// If we registered over n seconds ago, register again
	// The master server will not add duplicates, instead it will update the last ping time
	// so we will have another 5 minutes before our server is cleared
	if( tLX->fCurTime - fLastRegister > 4*60 ) {
		bServerRegistered = false;
		fLastRegister = tLX->fCurTime;
		RegisterServer();
	}
}


///////////////////
// De-register the server
bool CServer::DeRegisterServer(void)
{
	// If we aren't registered, or we didn't try to register, just leave
	if( !bRegServer || !bServerRegistered )
		return false;

	// Create the url
	char url[1024];
    char svr[1024];
	char buf[512];

	NLaddress addr;

	nlGetLocalAddr(tSocket,&addr);	

	sprintf(url, "%s?port=%d&addr=%s", LX_SVRDEREG, nPort, nlAddrToString(&addr, buf));

	// Initialize the request
	bServerRegistered = false;

	// Get the first server from the master server list
    FILE *fp = fopen("cfg/masterservers.txt","rt");
    if( !fp )
        return false;

    // Find the first line
    svr[0] = '\0';
    while( fgets(svr, 1023, fp) ) {
        strcpy( svr, StripLine(svr) );
        if( strlen(svr) > 0 ) {
            if( !http_InitializeRequest(svr, url) ) {
                fclose(fp);
                return false;
            }
            break;
        }
    }

    fclose(fp);
    return true;
}


///////////////////
// Process the de-registering of the server
bool CServer::ProcessDeRegister(void)
{
	int result = http_ProcessRequest(NULL);

	if( result == 0 )
		return false;

	return true;
}


///////////////////
// Check if any clients haved timed out or are out of zombie state
void CServer::CheckTimeouts(void)
{
	int c;

	float dropvalue = tLX->fCurTime - LX_SVTIMEOUT;

	// Cycle through clients
	CClient *cl = cClients;
	for(c=0;c<MAX_CLIENTS;c++,cl++) {
		// Client not connected
		if(cl->getStatus() == NET_DISCONNECTED)
			continue;

        // Check for a drop
		if(cl->getLastReceived() < dropvalue && cl->getStatus() != NET_ZOMBIE) {
			DropClient(cl, CLL_TIMEOUT);
		}

        // Is the client out of zombie state?
        if(cl->getStatus() == NET_ZOMBIE && tLX->fCurTime > cl->getZombieTime() ) {
            cl->setStatus(NET_DISCONNECTED);
        }
	}
}


///////////////////
// Drop a client
void CServer::DropClient(CClient *cl, int reason)
{
    char cl_msg[256];
    strcpy(cl_msg, "");

	// Tell everyone that the client's worms have left both through the net & text
	CBytestream bs;
	bs.writeByte(S2C_CLLEFT);
	bs.writeByte(cl->getNumWorms());

	char buf[128];
	int i;    
	for(i=0; i<cl->getNumWorms(); i++) {
		bs.writeByte(cl->getWorm(i)->getID());

        switch(reason) {

            // Quit
            case CLL_QUIT:
                sprintf(buf, "%s has left", cl->getWorm(i)->getName());
                strcpy(cl_msg, "You have quit");
                break;

            // Timeout
            case CLL_TIMEOUT:
                sprintf(buf, "%s has timed out", cl->getWorm(i)->getName());
                strcpy(cl_msg, "You timed out");
                break;

            // Kicked
            case CLL_KICK:
                sprintf(buf, "%s has been kicked out", cl->getWorm(i)->getName());
                strcpy(cl_msg, "You have been kicked");
                break;
        }

		SendGlobalText(buf,TXT_NETWORK);

		cl->getWorm(i)->setUsed(false);
		cl->getWorm(i)->setAlive(false);
	}


    // Go into a zombie state for a while so the reliable channel can still get the 
    // reliable data to the client
    cl->setStatus(NET_ZOMBIE);
    cl->setZombieTime(tLX->fCurTime + 3);
    
	SendGlobalPacket(&bs);

    // Send the client directly a dropped packet
    bs.Clear();
    bs.writeByte(S2C_DROPPED);
    bs.writeString(cl_msg);
    cl->getChannel()->getMessageBS()->Append(&bs);


	// Re-Calculate number of players
	iNumPlayers=0;
	CWorm *w = cWorms;
	for(i=0;i<MAX_WORMS;i++,w++) {
		if(w->isUsed())
			iNumPlayers++;
	}

    // Now that a player has left, re-check the game status
    RecheckGame();

    // If we're waiting for players to be ready, check again
    if(iState == SVS_GAME)
        CheckReadyClient();
}


///////////////////
// Kick a worm out of the server
void CServer::kickWorm(int wormID)
{
    if( wormID < 0 || wormID >= MAX_PLAYERS )
        return;

    // Get the worm
    CWorm *w = cWorms + wormID;
    if( !w->isUsed() )
        return;

    // Get the client
    CClient *cl = w->getClient();
    if( !cl )
        return;

    // Drop the client
    DropClient(cl, CLL_KICK);
}


///////////////////
// Kick a worm out of the server (by name)
void CServer::kickWorm(char *szWormName)
{
    // Find the worm name
    CWorm *w = cWorms;
    for(int i=0; i<MAX_WORMS; i++, w++) {
        if(!w->isUsed())
            continue;

        if(stricmp(w->getName(), szWormName) == 0) {
            kickWorm(i);
            return;            
        }
    }

    // Didn't find the worm
    Con_Printf(CNC_NOTIFY, "Could not find worm '%s'",szWormName);
}


///////////////////
// Notify the host about stuff
void CServer::notifyLog(char *fmt, ...)
{
    char buf[512];
	va_list	va;

	va_start(va,fmt);
	vsprintf(buf,fmt,va);
	va_end(va);

    // Local hosting?
    // Add it to the clients chatbox
    if(cClient) {
        CChatBox *c = cClient->getChatbox();
        if(c)
            c->AddText(buf, MakeColour(200,200,200), tLX->fCurTime);
    }

}


///////////////////
// Shutdown the server
void CServer::Shutdown(void)
{
	int i;
	if(cMap) {
		cMap->Shutdown();
		delete cMap;
		cMap = NULL;
	}

	if(tSocket != NL_INVALID)
		nlClose(tSocket);
	tSocket = NL_INVALID;

	if(cClients) {
		for(i=0;i<MAX_CLIENTS;i++)
			cClients[i].Shutdown();
		delete[] cClients;
		cClients = NULL;
	}

	if(cWorms) {
		for(i=0;i<MAX_WORMS;i++)
			cWorms[i].Shutdown();
		delete[] cWorms;
		cWorms = NULL;
	}

	cShootList.Shutdown();
    
    cWeaponRestrictions.Shutdown();


	/*if(cProjectiles) {
		delete[] cProjectiles;
		cProjectiles = NULL;
	}*/

	cGameScript.Shutdown();
}