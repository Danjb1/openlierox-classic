/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Net menu - LAN
// Created 21/8/02
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"
#include "Menu.h"


CGuiLayout	cLan;
char        szLanCurServer[128];


// Lan widgets
enum {
	Join=0,
	ServerList,
	Refresh,
	Back,
    PopupMenu
};



///////////////////
// Initialize the LAN menu
int Menu_Net_LANInitialize(void)
{
	Uint32 blue = MakeColour(0,138,251);

	iNetMode = net_lan;	

	cLan.Shutdown();
	cLan.Initialize();

	cLan.Add( new CButton(BUT_BACK, tMenu->bmpButtons),    Back,       25, 440, 50,  15);
	cLan.Add( new CButton(BUT_REFRESH, tMenu->bmpButtons), Refresh,	   280,440, 83,  15);
	cLan.Add( new CButton(BUT_JOIN, tMenu->bmpButtons),    Join,	   570,440, 43,  15);
	cLan.Add( new CListview(),							   ServerList, 40, 160, 560, 260);
	//cLan.Add( new CLabel("Local Area Network", blue),	   -1,		   40, 140, 0,   0);


	/*
	  Server list columns

      Connection speed
	  Name
	  State
	  Players
	  Ping
	  Address
    */


	cLan.SendMessage( ServerList, LVM_ADDCOLUMN, (DWORD)"", 32);
	cLan.SendMessage( ServerList, LVM_ADDCOLUMN, (DWORD)"Server Name", 180);
	cLan.SendMessage( ServerList, LVM_ADDCOLUMN, (DWORD)"State", 70);
	cLan.SendMessage( ServerList, LVM_ADDCOLUMN, (DWORD)"Players", 80);
	cLan.SendMessage( ServerList, LVM_ADDCOLUMN, (DWORD)"Ping", 60);
	cLan.SendMessage( ServerList, LVM_ADDCOLUMN, (DWORD)"Address", 150);

	// Clear the server list
	Menu_SvrList_Clear();
	Menu_SvrList_PingLAN();	

	return true;
}


///////////////////
// Shutdown the LAN menu
void Menu_Net_LANShutdown(void)
{
	cLan.Shutdown();
}


///////////////////
// Net LAN frame
void Menu_Net_LANFrame(int mouse)
{
	mouse_t		*Mouse = GetMouse();
	gui_event_t *ev;
	char		addr[256];
	

	// Process & Draw the gui
	ev = cLan.Process();
	cLan.Draw( tMenu->bmpScreen );


	// Process the server list
	if( Menu_SvrList_Process() ) {
		// Add the servers to the listview
		Menu_SvrList_FillList( (CListview *)cLan.getWidget( ServerList ) );
	}



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
					cLan.Shutdown();

					// Back to main menu					
					Menu_MainInitialize();
				}
				break;

			// Refresh
			case Refresh:
				if(ev->iEventMsg == BTN_MOUSEUP) {

					// Click!
					BASS_SamplePlay(sfxGeneral.smpClick);

					// Send out a ping over the lan
					Menu_SvrList_Clear();
					Menu_SvrList_PingLAN();
				}
				break;

			// Join
			case Join:
				if(ev->iEventMsg == BTN_MOUSEUP) {				

					addr[0] = 0;
					int result = cLan.SendMessage(ServerList, LVM_GETCURSINDEX, (DWORD)addr, sizeof(addr));
					if(result != -1 && addr[0]) {
						
						// Click!
						BASS_SamplePlay(sfxGeneral.smpClick);

						// Join
						Menu_Net_LANJoinServer(addr);
						return;
					}
				}
				break;

			// Serverlist
			case ServerList:
				if(ev->iEventMsg == LV_DOUBLECLK) {

					/*
					  Now.... Should a double click refresh the server (like tribes)?
					  Or should it join the server like other games???
					*/

					// Just join for the moment
					addr[0] = 0;
					int result = cLan.SendMessage(ServerList, LVM_GETCURSINDEX, (DWORD)addr, sizeof(addr));
					if(result != -1 && addr[0]) {
						Menu_Net_LANJoinServer(addr);
						return;
					}
				}

                // Right click
                if( ev->iEventMsg == LV_RIGHTCLK ) {
                    addr[0] = 0;
					int result = cLan.SendMessage(ServerList, LVM_GETCURSINDEX, (DWORD)addr, sizeof(addr));
					if(result && addr[0]) {
                        // Display a menu
                        strcpy(szLanCurServer, addr);
                        mouse_t *m = GetMouse();
                        
                        cLan.Add( new CMenu(m->X, m->Y), PopupMenu, 0,0, 640,480 );
                        cLan.SendMessage( PopupMenu, MNM_ADDITEM, 0, (DWORD)"Delete server" );
                        cLan.SendMessage( PopupMenu, MNM_ADDITEM, 1, (DWORD)"Refresh server" );
                        cLan.SendMessage( PopupMenu, MNM_ADDITEM, 2, (DWORD)"Join server" );
                        cLan.SendMessage( PopupMenu, MNM_ADDITEM, 3, (DWORD)"Server details" );
                    }
                }
				break;

            // Popup menu
            case PopupMenu:
                switch( ev->iEventMsg ) {
                    // Delete the server
                    case MNU_USER+0:
                        Menu_SvrList_RemoveServer(szLanCurServer);
                        break;

                    // Refresh the server
                    case MNU_USER+1:
                        {
                            server_t *sv = Menu_SvrList_FindServerStr(szLanCurServer);
                            if(sv)
                                Menu_SvrList_RefreshServer(sv);
                        }
                        break;

                    // Join a server
                    case MNU_USER+2:
                        // Save the list
                        Menu_SvrList_SaveList("cfg/svrlist.dat");
						Menu_Net_LANJoinServer(szLanCurServer);
                        return;

                    // Show server details
                    case MNU_USER+3:
                        Menu_Net_LanShowServer(szLanCurServer);
                        break;
                }

                // Re-Fill the server list
                Menu_SvrList_FillList( (CListview *)cLan.getWidget( ServerList ) );

                // Remove the menu widget
                cLan.SendMessage(PopupMenu, MNM_REDRAWBUFFER, 0, 0);
                cLan.removeWidget(PopupMenu);
                break;
		}

	}


	// Draw the mouse
	DrawImage(tMenu->bmpScreen,gfxGUI.bmpMouse[mouse], Mouse->X,Mouse->Y);
}


///////////////////
// Join a server
void Menu_Net_LANJoinServer(char *sAddress)
{
	// Shutdown
	cLan.Shutdown();

	Menu_Net_JoinInitialize(sAddress);
}

extern CButton	cNetButtons[5];

///////////////////
// Show a server's details
void Menu_Net_LanShowServer(char *szAddress)
{
    mouse_t     *Mouse = GetMouse();
    int         nMouseCur = 0;
    CGuiLayout  cDetails;

    // Create the buffer
    DrawImage(tMenu->bmpBuffer,tMenu->bmpMainBack_wob,0,0);
    Menu_DrawBox(tMenu->bmpBuffer, 15,130, 625, 465);
	Menu_DrawSubTitle(tMenu->bmpBuffer,SUB_NETWORK);
    cLan.Draw(tMenu->bmpBuffer);

    Menu_SvrList_DrawInfo(szAddress);

	for(int i=1;i<4;i++)
		cNetButtons[i].Draw(tMenu->bmpBuffer);
	Menu_RedrawMouse(true);

    cDetails.Initialize();
    cDetails.Add( new CButton(BUT_OK, tMenu->bmpButtons),	    1,      300,420, 40,15);


    while(!GetKeyboard()->KeyUp[SDLK_ESCAPE]) {
		nMouseCur = 0;
		Menu_RedrawMouse(false);
		ProcessEvents();
		DrawImageAdv(tMenu->bmpScreen,tMenu->bmpBuffer, 200,220, 200,220, 240, 240);

        cDetails.Draw(tMenu->bmpScreen);
        gui_event_t *ev = cDetails.Process();
        if(ev) {
            if(ev->cWidget->getType() == wid_Button)
                nMouseCur = 1;

            if(ev->iControlID == 1 && ev->iEventMsg == BTN_MOUSEUP) {
                break;
            }
        }


        DrawImage(tMenu->bmpScreen,gfxGUI.bmpMouse[nMouseCur], Mouse->X,Mouse->Y);
		FlipScreen(tMenu->bmpScreen);	
    }


    // Redraw the background    
	DrawImage(tMenu->bmpBuffer,tMenu->bmpMainBack_wob,0,0);
    Menu_DrawBox(tMenu->bmpBuffer, 15,130, 625, 465);
	Menu_DrawSubTitle(tMenu->bmpBuffer,SUB_NETWORK);
	Menu_RedrawMouse(true);
}