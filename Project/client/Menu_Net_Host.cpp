/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Net menu - Hosting
// Created 12/8/02
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"
#include "Menu.h"


/*
   The hosting menu is in two parts
     1) Selecting the players _we_ want to play as
     2) The lobby screen were other players can connect
*/



/*
==================================

		Player selection

==================================
*/



CGuiLayout	cHostPly;

// Player gui
enum {
	Back=0,
	Ok,
	PlayerList,
	Playing,
	Servername,
	MaxPlayers,
	Register,
    Password
};


///////////////////
// Initialize the host menu
int Menu_Net_HostInitialize(void)
{
	char buf[64];

	iNetMode = net_host;
	iHostType = 0;

	// Player gui layout
	cHostPly.Shutdown();
	cHostPly.Initialize();

    cHostPly.Add( new CLabel("Player settings",MakeColour(0,138,251)), -1, 350,140,0,0);
	cHostPly.Add( new CLabel("Server settings",MakeColour(0,138,251)), -1, 30, 140,0,0);
	cHostPly.Add( new CButton(BUT_BACK, tMenu->bmpButtons), Back,		25, 440,50, 15);
	cHostPly.Add( new CButton(BUT_OK, tMenu->bmpButtons),   Ok,			585,440,30, 15);

	cHostPly.Add( new CListview(),							PlayerList,	360,160,200,130);
	cHostPly.Add( new CListview(),							Playing,	360,310,200,70);	
	cHostPly.Add( new CLabel("Server name",	0xffff),		-1,			40, 178,0,  0);
	cHostPly.Add( new CTextbox(),							Servername, 160,175,140,20);
	cHostPly.Add( new CLabel("Max Players",	0xffff),		-1,			40, 208,0,  0);
	cHostPly.Add( new CTextbox(),							MaxPlayers, 160,205,140,20);
    //cHostPly.Add( new CLabel("Password",	0xffff),	    -1,			40, 238,0,  0);
	//cHostPly.Add( new CTextbox(),		                    Password,	160,235,140,20);
	cHostPly.Add( new CLabel("Register Server",	0xffff),	-1,			40, 238,0,  0);
	cHostPly.Add( new CCheckbox(0),		                    Register,	160,235,17, 17);    

	// Use previous settings
	cHostPly.SendMessage( MaxPlayers, TXM_SETTEXT, (DWORD)itoa(tLXOptions->tGameinfo.iMaxPlayers,buf,10), 0);
	cHostPly.SendMessage( Servername, TXM_SETTEXT, (DWORD)tLXOptions->tGameinfo.sServerName, 0);	
	cHostPly.SendMessage( Register,   CKM_SETCHECK, tLXOptions->tGameinfo.bRegServer, 0);
    //cHostPly.SendMessage( Password,   TXM_SETTEXT, (DWORD)tLXOptions->tGameinfo.szPassword, 0 );

	// Add columns
	cHostPly.SendMessage( PlayerList,   LVM_ADDCOLUMN, (DWORD)"Players",22);
	cHostPly.SendMessage( PlayerList,   LVM_ADDCOLUMN, (DWORD)"",60);
	cHostPly.SendMessage( Playing,      LVM_ADDCOLUMN, (DWORD)"Playing",22);
	cHostPly.SendMessage( Playing,      LVM_ADDCOLUMN, (DWORD)"",60);	

	// Add players to the list
	profile_t *p = GetProfiles();
	for(;p;p=p->tNext) {
		if(p->iType == PRF_COMPUTER)
			continue;

		cHostPly.SendMessage( PlayerList, LVM_ADDITEM, (DWORD)"", p->iID);
		cHostPly.SendMessage( PlayerList, LVM_ADDSUBITEM, LVS_IMAGE, (DWORD)p->bmpWorm);
		cHostPly.SendMessage( PlayerList, LVM_ADDSUBITEM, LVS_TEXT,  (DWORD)p->sName);
	}


	return true;
}


///////////////////
// Shutdown the host menu
void Menu_Net_HostShutdown(void)
{
	cHostPly.Shutdown();
}


///////////////////
// The net host menu frame
void Menu_Net_HostFrame(int mouse)
{
	switch(iHostType) {

		// Player selection
		case 0:
			Menu_Net_HostPlyFrame(mouse);
			break;

		// Lobby
		case 1:
			Menu_Net_HostLobbyFrame(mouse);
			break;
	}
}


///////////////////
// Player selection frame
void Menu_Net_HostPlyFrame(int mouse)
{
	mouse_t		*Mouse = GetMouse();
	gui_event_t *ev;
	CListview	*lv, *lv2;
	profile_t	*ply;

	// Process & Draw the gui
	ev = cHostPly.Process();
	cHostPly.Draw( tMenu->bmpScreen );


	// Process any events
	if(ev) {

		// Mouse type
		if(ev->cWidget->getType() == wid_Button)
			mouse = 1;
		if(ev->cWidget->getType() == wid_Textbox)
			mouse = 2;


		switch(ev->iControlID) {

			// Back
			case Back:
				if(ev->iEventMsg == BTN_MOUSEUP) {

					// Click!
					BASS_SamplePlay(sfxGeneral.smpClick);

					// Shutdown
					cHostPly.Shutdown();

					// Back to net menu
					Menu_MainInitialize();
				}
				break;


			// Player list
			case PlayerList:
				if(ev->iEventMsg == LV_DOUBLECLK) {
					// Add the item to the players list
					lv = (CListview *)cHostPly.getWidget(PlayerList);
					lv2 = (CListview *)cHostPly.getWidget(Playing);
					int index = lv->getCurIndex();

					// Make sure there is 0-1 players in the list
					if(lv2->getItemCount() < 1) {

						// Remove the item from the list
						lv->RemoveItem(index);

						ply = FindProfile(index);

						if(ply) {
							lv2->AddItem("",index,0xffff);					
							lv2->AddSubitem(LVS_IMAGE, "", ply->bmpWorm);
							lv2->AddSubitem(LVS_TEXT, ply->sName, NULL);
						}
					}
				}
				break;


			// Playing list
			case Playing:
				if(ev->iEventMsg == LV_DOUBLECLK) {
					// Add the item to the players list
					lv = (CListview *)cHostPly.getWidget(Playing);
					lv2 = (CListview *)cHostPly.getWidget(PlayerList);
					int index = lv->getCurIndex();

					// Remove the item from the list
					lv->RemoveItem(index);

					ply = FindProfile(index);

					if(ply) {
						lv2->AddItem("",index,0xffff);					
						lv2->AddSubitem(LVS_IMAGE, "", ply->bmpWorm);
						lv2->AddSubitem(LVS_TEXT, ply->sName, NULL);
					}
				}
				break;

			// Ok
			case Ok:
				if(ev->iEventMsg == BTN_MOUSEUP) {

					lv = (CListview *)cHostPly.getWidget(Playing);

					// Make sure there is 1-2 players in the list
					if(lv->getItemCount() > 0 && lv->getItemCount() < 3) {

						tGameInfo.iNumPlayers = lv->getItemCount();

						// Fill in the game structure												
						lv_item_t *item = lv->getItems();
		
						// Add the players to the list
						int count=0;
						for(;item;item=item->tNext) {
							if(item->iIndex < 0)
								continue;

							profile_t *ply = FindProfile(item->iIndex);
	
							if(ply)
								tGameInfo.cPlayers[count++] = ply;
						}

						// Get the server name
						cHostPly.SendMessage( Servername, TXM_GETTEXT, (DWORD)tGameInfo.sServername, sizeof(tGameInfo.sServername));
                        cHostPly.SendMessage( Password, TXM_GETTEXT, (DWORD)tGameInfo.sPassword, sizeof(tGameInfo.sPassword));
						
						// Save the info
						char buf[64];
						cHostPly.SendMessage( Servername, TXM_GETTEXT, (DWORD)tLXOptions->tGameinfo.sServerName, sizeof(tLXOptions->tGameinfo.sServerName));
                        //cHostPly.SendMessage( Password, TXM_GETTEXT, (DWORD)tLXOptions->tGameinfo.szPassword, sizeof(tLXOptions->tGameinfo.szPassword));
						cHostPly.SendMessage( MaxPlayers, TXM_GETTEXT, (DWORD)buf, sizeof(buf));
						
						tLXOptions->tGameinfo.iMaxPlayers = atoi(buf);
						// At least 2 players, and max 8
						tLXOptions->tGameinfo.iMaxPlayers = MAX(tLXOptions->tGameinfo.iMaxPlayers,2);
						tLXOptions->tGameinfo.iMaxPlayers = MIN(tLXOptions->tGameinfo.iMaxPlayers,8);
						tLXOptions->tGameinfo.bRegServer = cHostPly.SendMessage( Register, CKM_GETCHECK, 0, 0);

						cHostPly.Shutdown();

						// Click
						BASS_SamplePlay(sfxGeneral.smpClick);

						// Start the lobby
						Menu_Net_HostLobbyInitialize();
					}
				}
				break;
		}
	}


	// Draw the mouse
	DrawImage(tMenu->bmpScreen,gfxGUI.bmpMouse[mouse], Mouse->X,Mouse->Y);
}





/*
==================================

		Hosting Lobby

==================================
*/


// Lobby gui
enum {
	Back2=0,
	Start,
	ChatText,
	ChatList,
	LevelList,
	Lives,
	MaxKills,
	ModName,
	Gametype,
	GameSettings,
    WeaponOptions,
    PopupMenu
};

bool		bHostGameSettings = false;
bool		bHostWeaponRest = false;
CGuiLayout	cHostLobby;
CChatBox	cChat;
int			iSpeaking=-1;
int         g_nLobbyWorm = -1;


///////////////////
// Initialize the hosting lobby
int Menu_Net_HostLobbyInitialize(void)
{
	tGameInfo.iGameType = GME_HOST;
	iNetMode = net_host;
	iHostType = 1;
	bHostGameSettings = false;
    bHostWeaponRest = false;
    iSpeaking = -1;

    // Draw the lobby
	Menu_Net_HostLobbyDraw();

	// Fill in some game details
	tGameInfo.iLoadingTimes = tLXOptions->tGameinfo.iLoadingTime;
	tGameInfo.iLives = tLXOptions->tGameinfo.iLives;
	tGameInfo.iKillLimit = tLXOptions->tGameinfo.iKillLimit;
	tGameInfo.iBonusesOn = tLXOptions->tGameinfo.iBonusesOn;
	tGameInfo.iShowBonusName = tLXOptions->tGameinfo.iShowBonusName;
    tGameInfo.iGameMode = tLXOptions->tGameinfo.nGameType;

    cClient->Clear();


	// Start the server
	if(!cServer->StartServer( tGameInfo.sServername, tLXOptions->iNetworkPort, tLXOptions->tGameinfo.iMaxPlayers, tLXOptions->tGameinfo.bRegServer )) {
		// Crappy
		printf("Server wouldn't start\n");
		cHostLobby.Shutdown();
		return false;
	}


	// Lets connect me to the server
	if(!cClient->Initialize()) {
		// Crappy
		printf("Client wouldn't initialize\n");
		cHostLobby.Shutdown();
		return false;
	}


	cClient->Connect("127.0.0.1");

	cClient->setChatbox(&cChat);
	cChat.Clear();
    cChat.setWidth(590);

	// Set up the server's lobby details
	game_lobby_t *gl = cServer->getLobby();
	gl->nSet = true;
	gl->nGameMode = tLXOptions->tGameinfo.nGameType;
	gl->nLives = tLXOptions->tGameinfo.iLives;
	gl->nMaxKills = tLXOptions->tGameinfo.iKillLimit;
	gl->nLoadingTime = tLXOptions->tGameinfo.iLoadingTime;
    gl->nBonuses = tLXOptions->tGameinfo.iBonusesOn;


    // Create the GUI
    Menu_Net_HostLobbyCreateGui();

	return true;
}


///////////////////
// Draw the lobby screen
void Menu_Net_HostLobbyDraw(void)
{
    // Create the buffer
	DrawImage(tMenu->bmpBuffer,tMenu->bmpMainBack_wob,0,0);
    Menu_DrawBox(tMenu->bmpBuffer, 5,5, 635, 475);
	Menu_DrawBox(tMenu->bmpBuffer, 460,29, 593, 130);
    DrawImageAdv(tMenu->bmpBuffer, tMenu->bmpMainBack_wob, 281,0, 281,0, 79,20);

    tLX->cFont.DrawCentre(tMenu->bmpBuffer, 320, -1, 0xffff, "[  Lobby  ]");

	// Chat box
    DrawRectFill(tMenu->bmpBuffer, 16, 270, 624, 417, 0);

    // Player box
    Menu_DrawBox(tMenu->bmpBuffer, 15, 29, 340, 235);

	Menu_RedrawMouse(true);
}


///////////////////
// Create the lobby gui
void Menu_Net_HostLobbyCreateGui(void)
{
    Uint32 blue = MakeColour(0,138,251);

    // Lobby gui layout
	cHostLobby.Shutdown();
	cHostLobby.Initialize();

	cHostLobby.Add( new CButton(BUT_LEAVE, tMenu->bmpButtons),Back2,	15,  450, 60,  15);
	cHostLobby.Add( new CButton(BUT_START, tMenu->bmpButtons),Start,	560, 450, 60,  15);
	cHostLobby.Add( new CLabel("Players",blue),				  -1,		15,  15,  0,   0);
	cHostLobby.Add( new CTextbox(),							  ChatText, 15,  421, 610, 20);
    cHostLobby.Add( new CListview(),                          ChatList, 15,  253, 610, 165);

	cHostLobby.Add( new CButton(BUT_GAMESETTINGS, tMenu->bmpButtons), GameSettings, 360, 210, 170,15);
    cHostLobby.Add( new CButton(BUT_WEAPONOPTIONS,tMenu->bmpButtons), WeaponOptions,360, 235, 185,15);

	cHostLobby.Add( new CLabel("Mod",0xffff),	    -1,         360, 180, 0,   0);
	cHostLobby.Add( new CCombobox(),				ModName,    440, 179, 170, 17);
	cHostLobby.Add( new CLabel("Game type",0xffff),	-1,         360, 158, 0,   0);
	cHostLobby.Add( new CCombobox(),				Gametype,   440, 157, 170, 17);
    cHostLobby.Add( new CLabel("Level",0xffff),	    -1,         360, 136, 0,   0);
    cHostLobby.Add( new CCombobox(),				LevelList,  440, 135, 170, 17);

	// Fill in the game details
	cHostLobby.SendMessage(Gametype,    CBM_ADDITEM,   GMT_DEATHMATCH, (DWORD)"Deathmatch");
	cHostLobby.SendMessage(Gametype,    CBM_ADDITEM,   GMT_TEAMDEATH,  (DWORD)"Team Deathmatch");
	cHostLobby.SendMessage(Gametype,    CBM_ADDITEM,   GMT_TAG,        (DWORD)"Tag");

	// Fill in the mod list
	Menu_Local_FillModList( (CCombobox *)cHostLobby.getWidget(ModName));

	// Fill in the levels list
	Menu_FillLevelList( (CCombobox *)cHostLobby.getWidget(LevelList), false);
	Menu_HostShowMinimap();
    
    // Don't show chat box selection
    CListview *lv = (CListview *)cHostLobby.getWidget(ChatList);
    lv->setShowSelect(false);

    game_lobby_t *gl = cServer->getLobby();
    cHostLobby.SendMessage(LevelList, CBM_GETCURSINDEX, (DWORD)gl->szMapName, sizeof(gl->szMapName));
    cHostLobby.SendMessage(ModName, CBM_GETCURNAME, (DWORD)gl->szModName, sizeof(gl->szModName));
    cHostLobby.SendMessage(ModName, CBM_GETCURSINDEX, (DWORD)gl->szModDir, sizeof(gl->szModDir));
    cHostLobby.SendMessage(Gametype, CBM_SETCURINDEX, gl->nGameMode, 0);
}


///////////////////
// Go straight to the lobby, without clearing the server & client
void Menu_Net_HostGotoLobby(void)
{
	Uint32 blue = MakeColour(0,138,251);

	tGameInfo.iGameType = GME_HOST;
	iNetMode = net_host;
	iHostType = 1;
	bHostGameSettings = false;
    bHostWeaponRest = false;
    iSpeaking =- 1;

    // Draw the lobby
	Menu_Net_HostLobbyDraw();

	// Fill in some game details
	tGameInfo.iLoadingTimes = tLXOptions->tGameinfo.iLoadingTime;
	tGameInfo.iLives = tLXOptions->tGameinfo.iLives;
	tGameInfo.iKillLimit = tLXOptions->tGameinfo.iKillLimit;
	tGameInfo.iBonusesOn = tLXOptions->tGameinfo.iBonusesOn;
	tGameInfo.iShowBonusName = tLXOptions->tGameinfo.iShowBonusName;
    tGameInfo.iGameMode = tLXOptions->tGameinfo.nGameType;

	cClient->setChatbox(&cChat);
	cChat.Clear();

	// Set up the server's lobby details
	game_lobby_t *gl = cServer->getLobby();
	gl->nSet = true;
	gl->nGameMode = tLXOptions->tGameinfo.nGameType;
	gl->nLives = tLXOptions->tGameinfo.iLives;
	gl->nMaxKills = tLXOptions->tGameinfo.iKillLimit;
	gl->nLoadingTime = tLXOptions->tGameinfo.iLoadingTime;

    // Create the GUI
    Menu_Net_HostLobbyCreateGui();
	
	cServer->UpdateGameLobby();
}


///////////////////
// Host lobby frame
void Menu_Net_HostLobbyFrame(int mouse)
{
	mouse_t		*Mouse = GetMouse();
	gui_event_t *ev;

	// Process the server & client frames
	cServer->Frame();
	cClient->Frame();


    // Game settings
	if(bHostGameSettings) {
		if(Menu_GameSettings_Frame()) {
			Menu_Net_HostLobbyDraw();
			Menu_HostShowMinimap();

			bHostGameSettings = false;

			// Move the settings into the server
			cServer->getLobby()->nLives = tGameInfo.iLives;
			cServer->getLobby()->nMaxKills = tGameInfo.iKillLimit;
			cServer->getLobby()->nLoadingTime = tGameInfo.iLoadingTimes;
            cServer->getLobby()->nBonuses = tGameInfo.iBonusesOn;
			cServer->UpdateGameLobby();
		}
		return;
	}


    // Weapons Restrictions
    if(bHostWeaponRest) {
		if(Menu_WeaponsRestrictions_Frame()) {
			Menu_Net_HostLobbyDraw();
			Menu_HostShowMinimap();

			bHostWeaponRest = false;
		}
		return;
	}


    // Add chat to the listbox
    for(int i=0;i<MAX_CLINES;i++) {
		line_t *l = cChat.GetLine(i);
		if(l->iUsed) {
            l->iUsed = false;
            CListview *lv = (CListview *)cHostLobby.getWidget(ChatList);

            if(lv->getLastItem())
                lv->AddItem("", lv->getLastItem()->iIndex+1, l->iColour);
            else
                lv->AddItem("", 0, l->iColour);
            lv->AddSubitem(LVS_TEXT, l->strLine, NULL);
            lv->setShowSelect(false);

            // If there are too many lines, remove the top line
            if(lv->getItemCount() > 256) {
                if(lv->getItems())
                    lv->RemoveItem(lv->getItems()->iIndex);
            }

            lv->scrollLast();           
		}
	}



    // Clear the chat box & player list
    //Menu_redrawBufferRect(25, 315, 590, 100);
    Menu_redrawBufferRect(15, 40,  315, 185);


    // Draw the host lobby details
	Menu_HostDrawLobby(tMenu->bmpScreen);
	
	// Process & Draw the gui
	ev = cHostLobby.Process();
	cHostLobby.Draw( tMenu->bmpScreen );

	

	// Process any events
	if(ev) {

		// Mouse type
		if(ev->cWidget->getType() == wid_Button)
			mouse = 1;
		if(ev->cWidget->getType() == wid_Textbox)
			mouse = 2;


		switch(ev->iControlID) {

			// Back
			case Back2:
				if(ev->iEventMsg == BTN_MOUSEUP) {
					// Click!
					BASS_SamplePlay(sfxGeneral.smpClick);

					// Shutdown
					cHostLobby.Shutdown();

					// Tell any clients that we're leaving
					cServer->SendDisconnect();

					// De-register the server
					Menu_Net_HostDeregister();

					// Shutdown server & client
					cServer->Shutdown();
					cClient->Shutdown();

					// Back to main net menu
					Menu_NetInitialize();
				}
				break;


			// Chat textbox
			case ChatText:
				if(ev->iEventMsg == TXT_ENTER && iSpeaking >= 0) {
					// Send the msg to the server

					// Get the text
					char buf[128];
					cHostLobby.SendMessage(ChatText, TXM_GETTEXT, (DWORD)buf, sizeof(buf));
                    
                    // Don't send empty messages
                    if(strlen(buf) == 0)
                        break;

					// Clear the text box
					cHostLobby.SendMessage(ChatText, TXM_SETTEXT, (DWORD)"", 0);

					// Get name
					char text[256];
					CWorm *rw = cClient->getRemoteWorms() + iSpeaking;
					sprintf(text, "%s: %s",rw->getName() ,buf);
					cServer->SendGlobalText(text,TXT_CHAT);
				}
				break;

			// Level change
			case LevelList:
				if(ev->iEventMsg == CMB_CHANGED) {
					Menu_HostShowMinimap();
					
					cHostLobby.SendMessage(LevelList, CBM_GETCURSINDEX, (DWORD)cServer->getLobby()->szMapName, sizeof(cServer->getLobby()->szMapName));
					cServer->UpdateGameLobby();
				}
				break;

            // Mod change
            case ModName:
                if(ev->iEventMsg == CMB_CHANGED) {
                    cHostLobby.SendMessage(ModName, CBM_GETCURNAME, (DWORD)cServer->getLobby()->szModName, sizeof(cServer->getLobby()->szModName));
                    cHostLobby.SendMessage(ModName, CBM_GETCURSINDEX, (DWORD)cServer->getLobby()->szModDir, sizeof(cServer->getLobby()->szModDir));
					cServer->UpdateGameLobby();
                }
                break;

			// Game type change
			case Gametype:
				if(ev->iEventMsg == CMB_CHANGED) {
					cServer->getLobby()->nGameMode = cHostLobby.SendMessage(Gametype, CBM_GETCURINDEX, 0, 0);
					cServer->UpdateGameLobby();
				}
				break;

			// Lives change
			case Lives:
				if(ev->iEventMsg == TXT_CHANGE) {
					char buf[128];
					cHostLobby.SendMessage(Lives, TXM_GETTEXT, (DWORD)buf, sizeof(buf));
					if(*buf)
						cServer->getLobby()->nLives = atoi(buf);
					else
						cServer->getLobby()->nLives = -2;

					cServer->UpdateGameLobby();
				}
				break;


			// Max Kills
			case MaxKills:
				if(ev->iEventMsg == TXT_CHANGE) {
					char buf[128];
					cHostLobby.SendMessage(MaxKills, TXM_GETTEXT, (DWORD)buf, sizeof(buf));
					if(*buf)
						cServer->getLobby()->nMaxKills = atoi(buf);
					else
						cServer->getLobby()->nMaxKills = -2;

					cServer->UpdateGameLobby();
				}
				break;

			// Game Settings
			case GameSettings:
				if(ev->iEventMsg == BTN_MOUSEUP) {

					// Draw the lobby screen to the buffer
					cHostLobby.Draw( tMenu->bmpBuffer);
					Menu_HostDrawLobby(tMenu->bmpBuffer);

					Menu_GameSettings();
					bHostGameSettings = true;
				}
				break;

            // Weapon restrictions
            case WeaponOptions:
                if(ev->iEventMsg == BTN_MOUSEUP) {

                    // Draw the lobby screen to the buffer
					cHostLobby.Draw( tMenu->bmpBuffer);
					Menu_HostDrawLobby(tMenu->bmpBuffer);

                    cb_item_t *it = (cb_item_t *)cHostLobby.SendMessage(ModName,CBM_GETCURITEM,0,0);
                    if(it) {
		                bHostWeaponRest = true;
					    Menu_WeaponsRestrictions(it->sIndex);
                    }
                }
                break;


			// Start the game
			case Start:
				if(ev->iEventMsg == BTN_MOUSEUP) {

					// Get the mod
					cb_item_t *it = (cb_item_t *)cHostLobby.SendMessage(ModName,CBM_GETCURITEM,0,0);
                    if(it) {
		                strcpy(tGameInfo.sModName,it->sIndex);
                        strcpy(tLXOptions->tGameinfo.szModName, it->sIndex);
                    }

                    // Get the game type
                    tGameInfo.iGameMode = cHostLobby.SendMessage(Gametype, CBM_GETCURINDEX, 0, 0);
                    tLXOptions->tGameinfo.nGameType = tGameInfo.iGameMode;
					
					// Get the map name
					cHostLobby.SendMessage(LevelList, CBM_GETCURSINDEX, (DWORD)tGameInfo.sMapname, sizeof(tGameInfo.sMapname));
					// Save the current level in the options
					cHostLobby.SendMessage(LevelList, CBM_GETCURSINDEX, (DWORD)tLXOptions->tGameinfo.sMapName, sizeof(tLXOptions->tGameinfo.sMapName));
					cHostLobby.Shutdown();

                    // Setup the client
                    cClient->SetupViewports();

					// Start the game
					cServer->StartGame();
					
					// Leave the frontend
					*iGame = true;
					tMenu->iMenuRunning = false;
					tGameInfo.iGameType = GME_HOST;					

					return;
				}
				break;

            // Popup menu
            case PopupMenu:
                switch( ev->iEventMsg ) {
                    // Kick the player
                    case MNU_USER+0:
                        // Kick the player
                        if( g_nLobbyWorm > 0 )
                            cServer->kickWorm( g_nLobbyWorm );
                        break;
                }

                // Remove the menu widget
                cHostLobby.SendMessage( PopupMenu, MNM_REDRAWBUFFER, 0, 0);
                cHostLobby.removeWidget(PopupMenu);
                break;
		}
	}


	// Draw the mouse
	DrawImage(tMenu->bmpScreen,gfxGUI.bmpMouse[mouse], Mouse->X,Mouse->Y);
}


///////////////////
// Draw the host lobby screen
void Menu_HostDrawLobby(SDL_Surface *bmpDest)
{
	int		i,y,local;
	mouse_t *Mouse = GetMouse();

	// Draw the connected players
	CWorm *w = cClient->getRemoteWorms();
	game_lobby_t *gl = cClient->getGameLobby();
	lobbyworm_t *l;
	local = false;
    y = 40;
	for(i=0; i<gl->nMaxWorms; i++, w++) {
        if( !w->isUsed() )
            continue;

		l = w->getLobby();

		local = false;
		if(w->isUsed())
			local = w->getLocal();

		if(local && iSpeaking == -1)
			iSpeaking = i;

		// Check for a click
		if(Mouse->Up & SDL_BUTTON(1)) {
			if(Mouse->Y > y && Mouse->Y < y+18) {

				// Speech
				/*if(Mouse->X > 20 && Mouse->X < 34) {
					if(l->iType == LBY_USED &&
					   local &&
					   cClient->getNumWorms()>1 &&
					   iSpeaking != i)
					      iSpeaking = i;
				}*/

				// Name
				if(Mouse->X > 77 && Mouse->X < 120) {

					// Open/Closed
					/*if(l->iType == LBY_OPEN)
						l->iType = LBY_CLOSED;
					else if(l->iType == LBY_CLOSED)
						l->iType = LBY_OPEN;*/

					// Name selection
				}
			}
		}

        int x = 20;

		switch(l->iType) {

			// Open
			case LBY_OPEN:
				//tLX->cFont.Draw(bmpDest, 77, y, 0xffff, "------");

                //DrawHLine(bmpDest, x, x+315, y+20, MakeColour(64,64,64));
				break;

			// Closed
			case LBY_CLOSED:
				//tLX->cFont.Draw(bmpDest, 77, y, 0xffff, "Closed");
				break;

			// Used
			case LBY_USED:

                //                
                // Draw the worm details
                //

                // Function button
                bool down = false;
                if( MouseInRect(x,y, 10,10) && Mouse->Down )
                    down = true;
                Menu_DrawWinButton(bmpDest, x,y, 10,10, down);

                // Check if the function button was clicked
                // Note: Function button only used on clients, not the host worm
                if( MouseInRect(x,y,10,10) && Mouse->Up && i > 0 ) {
                    g_nLobbyWorm = i;
                    // Remove old popup menu
                    cHostLobby.removeWidget(PopupMenu);

                    cHostLobby.Add( new CMenu(Mouse->X, Mouse->Y), PopupMenu, 0,0, 640,480 );
                    cHostLobby.SendMessage( PopupMenu, MNM_ADDITEM, 0, (DWORD)"Kick player" );
                }

                // Ready icon
                if(l->iReady)
					DrawImageAdv(bmpDest, tMenu->bmpLobbyState, 0,0, x+15,y-1,12,12);
				else
					DrawImageAdv(bmpDest, tMenu->bmpLobbyState, 0,12,x+15,y-1,12,12);

                // Worm
                DrawImage(bmpDest, w->getPicimg(), x+30, y-2);
				tLX->cFont.Draw(bmpDest, x+55, y-2, 0xffff, w->getName());    
                
                // Team
                if( gl->nGameMode == GMT_TEAMDEATH ) {
                    CWorm *sv_w = cServer->getWorms() + i;
                    sv_w->getLobby()->iTeam = l->iTeam;

                    DrawImage(bmpDest, tMenu->bmpTeamColours[l->iTeam], x+200, y-2);

                    // Check if the team colour was clicked on
                    if( MouseInRect(x+200, y-2, 18,16) && Mouse->Up ) {                        
                        sv_w->getLobby()->iTeam = (l->iTeam+1) % 4;

                        // Send the worm state updates
                        cServer->SendWormLobbyUpdate();
                    }

                    sv_w->setTeam(sv_w->getLobby()->iTeam);

                }
				break;
		}

        // Dividing line
        DrawHLine(bmpDest, x, x+315, y+20, MakeColour(64,64,64));

        y+=25;
	}

}



///////////////////
// Show the minimap
void Menu_HostShowMinimap(void)
{
	CMap map;
	char buf[256];
	char blah[256];

	cHostLobby.SendMessage(LevelList, CBM_GETCURSINDEX, (DWORD)buf, sizeof(buf));					

	// Draw a background over the old minimap
	DrawImageAdv(tMenu->bmpBuffer, tMenu->bmpMainBack_wob, 463,32,463,32,128,96);
	
	// Load the map
	sprintf(blah, "levels/%s",buf);
	if(map.Load(blah)) {

		// Draw the minimap
		map.UpdateMiniMap(true);
		DrawImage(tMenu->bmpBuffer, map.GetMiniMap(), 463,32);		
		map.Shutdown();
	}

	// Update the screen
	DrawImageAdv(tMenu->bmpScreen, tMenu->bmpBuffer, 457,30,457,30,140,110);
}


///////////////////
// Deregister the server
void Menu_Net_HostDeregister(void)
{
	// If the server wasn't registered, just leave
	if( !tLXOptions->tGameinfo.bRegServer )
		return;
	
	// Initialize the request
	if( !cServer->DeRegisterServer() )
		return;


	// Setup the background & show a messagebox
	int x = 160;
	int y = 170;
	int w = 320;
	int h = 140;
	int cx = x+w/2;
	int cy = y+h/2;
	mouse_t *Mouse = GetMouse();


	DrawImage(tMenu->bmpBuffer,tMenu->bmpMainBack_wob,0,0);
    Menu_DrawBox(tMenu->bmpBuffer, 15,60, 625, 465);
	Menu_DrawBox(tMenu->bmpBuffer, x, y, x+w, y+h);
	DrawRectFill(tMenu->bmpBuffer, x+2,y+2, x+w-1,y+h-1,0);

	tLX->cFont.DrawCentre(tMenu->bmpBuffer, cx, cy, 0xffff, "De-Registering server...");

	Menu_RedrawMouse(true);

	float starttime = tLX->fCurTime;

	while(1) {
		Menu_RedrawMouse(false);
		ProcessEvents();
		tLX->fCurTime = GetMilliSeconds();

		// If we have gone over a 4 second limit, just leave
		if( tLX->fCurTime - starttime > 4.0f ) {
			http_Quit();
			break;
		}

		if( cServer->ProcessDeRegister() ) {
			http_Quit();
			break;
		}

		DrawImage(tMenu->bmpScreen,gfxGUI.bmpMouse[0], Mouse->X,Mouse->Y);
		FlipScreen(tMenu->bmpScreen);
	}
}