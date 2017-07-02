/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Client class - Drawing routines
// Created 9/7/02
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"
#include "Menu.h"
#include "console.h"


SDL_Surface		*bmpMenuButtons = NULL;
float			fLagFlash;


///////////////////
// Initialize the drawing routines
int CClient::InitializeDrawing(void)
{
	LOAD_IMAGE(bmpMenuButtons,"data/frontend/buttons.png");

	fLagFlash = 0;

    // Set the appropriate chatbox width
    if(tGameInfo.iGameType == GME_LOCAL)
        cChatbox.setWidth(600);
    else
        cChatbox.setWidth(320);

	return true;
}


///////////////////
// Main drawing routines
void CClient::Draw(SDL_Surface *bmpDest)
{
	int i,num;
	float dt = tLX->fDeltaTime;


	num = iNumWorms;
	num = MIN(2,iNumWorms);


	// Check for any communication errors
	if(iServerError) {

		// Show message box, shutdown and quit back to menu
		DrawImage(tMenu->bmpBuffer,tMenu->bmpMainBack_wob,0,0);
		Menu_RedrawMouse(true);
        SDL_ShowCursor(SDL_DISABLE);

		Menu_MessageBox("Connection error", strServerErrorMsg, LMB_OK);        

		QuittoMenu();
		return;
	}

    // Draw the borders    
    DrawRectFill(bmpDest,0,382,640,480,0);
    if(cViewports[1].getUsed())
        DrawRectFill(bmpDest,318,0,322,384,0);

	
	// Draw the viewports
	if(iNetStatus == NET_PLAYING) {        

        // Draw the viewports
        for( i=0; i<NUM_VIEWPORTS; i++ ) {
            if( cViewports[i].getUsed() )
                DrawViewport(bmpDest, &cViewports[i]);
        }

        //
        // Mini-Map
        //

		// Single screen
		if( !cViewports[1].getUsed() )
			cMap->DrawMiniMap( bmpDest, 511,383, dt, cRemoteWorms, iGameType );
        else
            // Split screen
			cMap->DrawMiniMap( bmpDest, 257,383, dt, cRemoteWorms, iGameType);
	}

	
	// Connected
	if(iNetStatus == NET_CONNECTED && iGameReady) {
		int ready = true;

		// Go through and draw the first two worms select menus
		for(i=0;i<num;i++) {
			
			// Draw Map
            if( cViewports[i].getUsed() )
			    cMap->Draw(bmpDest, &cViewports[i]);

			// Select weapons
			if(!cLocalWorms[i]->getWeaponsReady()) {
				ready = false;
				cLocalWorms[i]->SelectWeapons(bmpDest, &cViewports[i]);
			}
		}

		// If we're ready, let the server know
		if(ready && !iReadySent) {
			iReadySent = true;
			CBytestream *bytes = cNetChan.getMessageBS();
			bytes->writeByte(C2S_IMREADY);
			bytes->writeByte(iNumWorms);

			// Send my worm's weapon details
			for(i=0;i<iNumWorms;i++)
				cLocalWorms[i]->writeWeapons(bytes);
		}
	}	
	
	// Draw the chatbox for either a local game, or remote game
	if(tGameInfo.iGameType == GME_LOCAL)
		DrawLocalChat(bmpDest);
	else
		DrawRemoteChat(bmpDest);

	// Game over
    if(iGameOver) {
        if(tLX->fCurTime - fGameOverTime > GAMEOVER_WAIT)
		    DrawGameOver(bmpDest);
        else
            tLX->cOutlineFont.DrawCentre(bmpDest, 320, 200, 0xffff, "Game Over");
    }

	// Game menu
	if(iGameMenu)
		DrawGameMenu(bmpDest);

    // Viewport manager
    if(bViewportMgr)
        DrawViewportManager(bmpDest);

    // Scoreboard
    DrawScoreboard(bmpDest);
       
    Con_Draw(bmpDest);


	// Lag icon
	if(tGameInfo.iGameType != GME_LOCAL) {
		if(cNetChan.getOutSeq() - cNetChan.getInAck() > 127) {
			fLagFlash += dt;
			if(fLagFlash>=2)
				fLagFlash=0;

			// Draw the lag icon
			int y = 0;
			if(fLagFlash>1)
				y = 43;
			//DrawImageAdv(bmpDest, gfxGame.bmpLag, 0,y, 640-gfxGame.bmpLag->w, 0, gfxGame.bmpLag->w,43);
		}
	}


	// FPS on the top right
	if(tLXOptions->iShowFPS) {
		int fps = GetFPS();
		tLX->cOutlineFont.Draw(bmpDest, 570, 0, 0xffff, "FPS: %d",fps);
	}


	// Chatter
	if(iChat_Typing)
		tLX->cOutlineFont.Draw(bmpDest, 4, 366, 0xffff, "Talk: %s_",sChat_Text);

    //tLX->cOutlineFont.Draw(bmpDest, 4,20, 0xffff, "%s",tLX->debug_string);
    //tLX->cOutlineFont.Draw(bmpDest, 4,40, 0xffff, "%f",tLX->debug_float);
}


///////////////////
// Draw a viewport
void CClient::DrawViewport(SDL_Surface *bmpDest, CViewport *v)
{
    Uint32 grey = MakeColour(128,128,128);

    // If the viewport is null, or not used: exit
    if( !v )
        return;
    if( !v->getUsed() )
        return;

    //CWorm *worm = v->getTarget();

	// Set the clipping
	SDL_Rect r = v->getRect();
	SDL_SetClipRect(bmpDest,&r);

	cMap->Draw(bmpDest, v);

    // Weather
    //cWeather.Draw(bmpDest, v);

    // Draw the projectile shadows
    if( tLXOptions->iShadows ) {
        DrawProjectileShadows(bmpDest, v);
        
        // Draw the worm shadows
        CWorm *w = cRemoteWorms;
        for(int i=0;i<MAX_WORMS;i++,w++) {		
            if(w->isUsed() && w->getAlive())
                w->DrawShadow(bmpDest, cMap, v);		
        }
    }


	// Draw the entities
	DrawEntities(bmpDest, v);

	// Draw the projectiles
	DrawProjectiles(bmpDest, v);

	// Draw the bonuses
	DrawBonuses(bmpDest, v);

    

	// Draw all the worms in the game
	CWorm *w = cRemoteWorms;
	for(int i=0;i<MAX_WORMS;i++,w++) {
		if(w->isUsed() && w->getAlive())
			w->Draw(bmpDest, cMap, v);		
	}


	// Disable the special clipping
	SDL_SetClipRect(bmpDest,NULL);

	// Draw the worm details
	int x = v->GetLeft();
	int y = v->GetTop() + v->GetVirtH();

	if(x > 0)
		x = 386;

	// The following is only drawn for viewports with a worm target
    if( v->getType() != VW_FOLLOW && v->getType() != VW_CYCLE )
        return;

    CWorm *worm = v->getTarget();
	
	// Health
	tLX->cFont.Draw(bmpDest, x+2, y+2, 0xffff,"Health:");
	DrawRectFill(bmpDest,x+63,y+6,x+165,y+13,grey);
	DrawRectFill(bmpDest,x+64,y+7,x+64+worm->getHealth(),y+12,MakeColour(64,255,64));

	// Weapon
	wpnslot_t *Slot = worm->getCurWeapon();
	tLX->cFont.Draw(bmpDest, x+2, y+20,0xffff,"Weapon:");
	DrawRectFill(bmpDest,x+63,y+24,x+165,y+31,grey);
	Uint32 col = MakeColour(64,64,255);
	if(Slot->Reloading)
		col = MakeColour(128,64,64);

	float c = 1;
	c = Slot->Charge;
	DrawRectFill(bmpDest,x+64,y+25,x+64+(int)(c*100.0f),y+30,col);


	// Lives
	tLX->cFont.Draw(bmpDest,x+2, y+38, 0xffff, "Lives:");
	if(worm->getLives() >= 0)
		tLX->cFont.Draw(bmpDest,x+61,y+38, 0xffff, "%d",worm->getLives());
	else if(worm->getLives() == WRM_OUT)
		tLX->cFont.Draw(bmpDest,x+61,y+38, 0xffff, "Out");	
	else if(worm->getLives() == WRM_UNLIM)
		DrawImage(bmpDest, gfxGame.bmpInfinite, x+61,y+41);

	// Kills
	tLX->cFont.Draw(bmpDest,x+2, y+56, 0xffff, "Kills:");
	tLX->cFont.Draw(bmpDest,x+61,y+56, 0xffff, "%d",worm->getKills());

	// Am i IT?
	if(worm->getTagIT() && iGameType == GMT_TAG)
		tLX->cFont.Draw(bmpDest, x+2, y+75, 0xffff, "You are IT!!");

    // Dirt count
    if( iGameType == GMT_DEMOLITION ) {
        tLX->cFont.Draw(bmpDest, x+2, y+75, 0xffff, "Dirt Count:");
        char buf[64];
        int count = worm->getDirtCount();

        // Draw short versions
        sprintf(buf,"%d",count);
        if( count >= 1000 )
            sprintf(buf,"%dk",count/1000);
        
        tLX->cFont.Draw(bmpDest,x+85,y+75, 0xffff, "%s",buf);
    }

	// Debug
	/*CViewport *view = worm->getViewport();
	int wx = view->GetWorldX();
	int wy = view->GetWorldY();
	int l = view->GetLeft();
	int t = view->GetTop();
	
	CVec vPosition = tLX->debug_pos;
	x=((int)vPosition.GetX()-wx)*2+l;
	y=((int)vPosition.GetY()-wy)*2+t;

	DrawRectFill(bmpDest, x-2,y-2,x+2,y+2,0xffff);*/
}


///////////////////
// Draw the projectiles
void CClient::DrawProjectiles(SDL_Surface *bmpDest, CViewport *v)
{
	CProjectile *prj = cProjectiles;

    prj = cProjectiles;
	for(int p=0;p<nTopProjectile;p++,prj++) {
		if(!prj->isUsed())
			continue;

		prj->Draw(bmpDest, v);
	}    
}


///////////////////
// Draw the projectile shadows
void CClient::DrawProjectileShadows(SDL_Surface *bmpDest, CViewport *v)
{
    CProjectile *prj = cProjectiles;
    
    for(int p=0;p<nTopProjectile;p++,prj++) {
        if(!prj->isUsed())
            continue;
        
        prj->DrawShadow(bmpDest, v, cMap);
    }
}


///////////////////
// Simulate the hud
void CClient::SimulateHud(void)
{
	float dt = tLX->fDeltaTime;
	float ScrollSpeed=5;
    bool  con = Con_IsUsed();

    if(!iGameReady)
        return;

	for(int i=0;i<iChat_Numlines;i++) {
		tChatLines[i].fScroll += dt*ScrollSpeed;
		tChatLines[i].fScroll = MIN(1,tChatLines[i].fScroll);
		
		if(tChatLines[i].fTime + 4 < tLX->fCurTime) {
			iChat_Numlines--;
			iChat_Numlines = MAX(0,iChat_Numlines);
		}
	}

    // Console
    if(!iChat_Typing && !iGameMenu && !bViewportMgr && tGameInfo.iGameType == GME_HOST)
        Con_Process(tLX->fDeltaTime);


	// Game Menu
    if(GetKeyboard()->KeyUp[SDLK_ESCAPE] && !iChat_Typing && !con) {
        if( !bViewportMgr )
		    iGameMenu = !iGameMenu;
        else
            bViewportMgr = false;
    }

    // Viewport manager
    if(GetKeyboard()->KeyUp[SDLK_F2] && !iChat_Typing && !iGameMenu && !con)
        InitializeViewportManager();

    // Process Chatter
    if(!iGameMenu && !bViewportMgr && !con)
	    processChatter();
}


///////////////////
// Draw the game over
void CClient::DrawGameOver(SDL_Surface *bmpDest)
{
	mouse_t *Mouse = GetMouse();
	int mouse;

	DrawScore(bmpDest,gfxGame.bmpGameover);

	// Network games have a different game over screen
	if(tGameInfo.iGameType != GME_LOCAL) {
		DrawRemoteGameOver(bmpDest);
		return;
	}


	// Buttons
	mouse = 0;
	int width = gfxGame.bmpGameover->w;
	int height = gfxGame.bmpGameover->h;

	int x = 320 - width/2;
	int y = 200 - height/2;
	
	int j = y+height-27;
	CButton ok = CButton(BUT_OK,bmpMenuButtons);
	
	ok.Setup(0, 305,j, 30, 15);
    ok.Create();

	// OK
	if(ok.InBox(Mouse->X,Mouse->Y)) {
		ok.MouseOver(Mouse);
		mouse=1;
	}
	ok.Draw2(bmpDest);


	if(Mouse->Up) {
		if(ok.InBox(Mouse->X,Mouse->Y)) {
			// Quit
			tLX->iQuitEngine = true;
		}
	}
	
	// Draw the mouse
	DrawImage(bmpDest,gfxGUI.bmpMouse[mouse], Mouse->X,Mouse->Y);
}


///////////////////
// Draw a remote game over screen
void CClient::DrawRemoteGameOver(SDL_Surface *bmpDest)
{
	mouse_t *Mouse = GetMouse();
	int mouse;
	
	// Buttons
	mouse = 0;
	int width = gfxGame.bmpGameover->w;
	int height = gfxGame.bmpGameover->h;

	int x = 320 - width/2;
	int y = 200 - height/2;
	
	int j = y+height-27;
	CButton leave = CButton(BUT_LEAVE,bmpMenuButtons);
		
	leave.Setup(0, x+20,j, 60, 15);
    leave.Create();

	// OK
	if(leave.InBox(Mouse->X,Mouse->Y)) {
		leave.MouseOver(Mouse);
		mouse=1;
	}
	leave.Draw(bmpDest);


	if(Mouse->Up) {
		if(leave.InBox(Mouse->X,Mouse->Y)) {
			
			// If this is a host, we go back to the lobby
			// The host can only quit the game via the lobby
			if(tGameInfo.iGameType == GME_HOST) {
				
				cServer->gotoLobby();

			} else {

				// Quit
				QuittoMenu();
			}
		}
	}

	// Draw a timer when we're going back to the lobby
	float timeleft = LX_ENDWAIT - (tLX->fCurTime - fGameOverTime);
	timeleft = MAX(timeleft,0);
	tLX->cFont.Draw(bmpDest, x+width-180, j+2, MakeColour(200,200,200), "Returning to lobby in %d", (int)timeleft);

	
	// Draw the mouse
	DrawImage(bmpDest,gfxGUI.bmpMouse[mouse], Mouse->X,Mouse->Y);
	
}


///////////////////
// Draw the in-game menu
void CClient::DrawGameMenu(SDL_Surface *bmpDest)
{
	mouse_t *Mouse = GetMouse();
	int mouse;
	
	DrawScore(bmpDest, gfxGame.bmpScoreboard);

	// Buttons
	mouse = 0; 

	CButton quit = CButton(BUT_QUITGAME,bmpMenuButtons);
	CButton resume = CButton(BUT_RESUME,bmpMenuButtons);

	int width = gfxGame.bmpScoreboard->w;
	int height = gfxGame.bmpScoreboard->h;

	int x = 320 - width/2;
	int y = 200 - height/2;
	
	int j = y+height-27;
	quit.Setup(0, x+width-130,j, 105, 15);
	resume.Setup(1, x+20,j, 75, 15);
    quit.Create();
    resume.Create();

	// Quit
	if(quit.InBox(Mouse->X,Mouse->Y)) {
		quit.MouseOver(Mouse);
		mouse=1;
	}
	quit.Draw2(bmpDest);

	// Resume
	if(resume.InBox(Mouse->X,Mouse->Y)) {
		resume.MouseOver(Mouse);
		mouse=1;
	}
	resume.Draw2(bmpDest);

	if(Mouse->Up) {
		if(quit.InBox(Mouse->X,Mouse->Y)) {

			// If we're the host tell all the clients that we're going back to the lobby
			// We don't directly go back to the lobby, instead we send the message
			// and the client on this machine does the goto lobby when it gets the packet
			if(tGameInfo.iGameType == GME_HOST) {
				
				cServer->gotoLobby();

			} else {

				// Quit
				QuittoMenu();
			}
		}
		
		if(resume.InBox(Mouse->X,Mouse->Y))
			// Resume
			iGameMenu = false;
	}
	
	// Draw the mouse
	DrawImage(bmpDest,gfxGUI.bmpMouse[mouse], Mouse->X,Mouse->Y);
}


///////////////////
// Display the score
void CClient::DrawScore(SDL_Surface *bmpDest, SDL_Surface *bmpImage)
{
	int i,j,n;

	// Teams
	Uint8 teamcolours[] = {102,153,255,  255,51,0,  51,153,0,  255,255,0};
	char *teamnames[] = {"Blue", "Red", "Green", "Yellow"};
//    char buf[64];

	int width = bmpImage->w;
	int height = bmpImage->h;

	int x = 320 - width/2;
	int y = 200 - height/2;

	// Draw the back image
	DrawImage(bmpDest,bmpImage,x,y);

	// Deathmatch scoreboard
	if(iGameType == GMT_DEATHMATCH) {

		tLX->cFont.Draw(bmpDest, x+15, y+45, 0xffff,"Players");
		if(iLives != WRM_UNLIM)
			tLX->cFont.Draw(bmpDest, x+300, y+45, 0xffff,"Lives");
		tLX->cFont.Draw(bmpDest, x+380, y+45, 0xffff,"Kills");
		DrawHLine(bmpDest, x+15,x+width-15, y+60,0xffff);
		DrawHLine(bmpDest, x+15,x+width-15, y+height-30,0xffff);

		// Draw the players
		j = y+65;
		for(i=0;i<iScorePlayers;i++) {
			CWorm *p = &cRemoteWorms[iScoreboard[i]];

			DrawImage(bmpDest, p->getPicimg(), x+15, j);

			tLX->cFont.Draw(bmpDest, x+40, j, 0xffff, p->getName());

			if(p->getLives() >= 0)
				tLX->cFont.DrawCentre(bmpDest, x+317, j, 0xffff, "%d",p->getLives());
			else if(p->getLives() == WRM_OUT)
				tLX->cFont.DrawCentre(bmpDest, x+317, j, 0xffff, "out");
			
			tLX->cFont.DrawCentre(bmpDest, x+393, j, 0xffff, "%d",p->getKills());

			j+=20;
		}
	}


    // Demolitions scoreboard
	if(iGameType == GMT_DEMOLITION) {

		tLX->cFont.Draw(bmpDest, x+15, y+45, 0xffff,"Players");
		if(iLives != WRM_UNLIM)
			tLX->cFont.Draw(bmpDest, x+270, y+45, 0xffff,"Lives");
		tLX->cFont.Draw(bmpDest, x+340, y+45, 0xffff,"Dirt Count");
		DrawHLine(bmpDest, x+15,x+width-15, y+60,0xffff);
		DrawHLine(bmpDest, x+15,x+width-15, y+height-30,0xffff);

        int dirtcount = 0;

		// Draw the players
		j = y+65;
		for(i=0;i<iScorePlayers;i++) {
			CWorm *p = &cRemoteWorms[iScoreboard[i]];

			DrawImage(bmpDest, p->getPicimg(), x+15, j);

			tLX->cFont.Draw(bmpDest, x+40, j, 0xffff, p->getName());

			if(p->getLives() >= 0)
				tLX->cFont.DrawCentre(bmpDest, x+287, j, 0xffff, "%d",p->getLives());
			else if(p->getLives() == WRM_OUT)
				tLX->cFont.DrawCentre(bmpDest, x+287, j, 0xffff, "out");

            
			tLX->cFont.DrawCentre(bmpDest, x+372, j, 0xffff, "%d",p->getDirtCount());
            dirtcount += p->getDirtCount();

			j+=20;
		}

        // Draw the total
        j+=10;
        DrawHLine(bmpDest,x+15,x+width-15, j, 0xffff);
        j+=5;
        tLX->cFont.Draw(bmpDest, x+250, j, 0xffff,"Total");
        tLX->cFont.DrawCentre(bmpDest, x+372, j, 0xffff,"%dk / %.0fk", dirtcount / 1000, ((float)cMap->GetDirtCount()*0.8f) / 1000);
	}


	// Tag scoreboard
	if(iGameType == GMT_TAG) {

		tLX->cFont.Draw(bmpDest, x+15, y+45, 0xffff,"Players");
		if(iLives != WRM_UNLIM)
			tLX->cFont.Draw(bmpDest, x+220, y+45, 0xffff,"Lives");
		tLX->cFont.Draw(bmpDest, x+290, y+45, 0xffff,"Kills");
		tLX->cFont.Draw(bmpDest, x+360, y+45, 0xffff,"Tag time");
		DrawHLine(bmpDest, x+15,x+width-15, y+60,0xffff);
		DrawHLine(bmpDest, x+15,x+width-15, y+height-30,0xffff);

		// Draw the players
		j = y+65;
		for(i=0;i<iScorePlayers;i++) {
			CWorm *p = &cRemoteWorms[iScoreboard[i]];

			DrawImage(bmpDest, p->getPicimg(), x+15, j);

			tLX->cFont.Draw(bmpDest, x+40, j, 0xffff, p->getName());

			// Check if it
			if(p->getTagIT())
				tLX->cFont.Draw( bmpDest, x+160, j, MakeColour(255,0,0), "IT");

			if(p->getLives() >= 0)
				tLX->cFont.DrawCentre(bmpDest, x+237, j, 0xffff, "%d",p->getLives());
			else if(p->getLives() == WRM_OUT)
				tLX->cFont.DrawCentre(bmpDest, x+237, j, 0xffff, "out");
			
			tLX->cFont.DrawCentre(bmpDest, x+303, j, 0xffff, "%d",p->getKills());

			// Total time of being IT
			int h,m,s;
			char buf[32];
			ConvertTime(p->getTagTime(), &h,&m,&s);
			sprintf(buf,"%d:%s%d",m,s<10 ? "0" : "",s);
			Uint32 col = 0xffff;
			if(p->getTagIT())
				col = MakeColour(255,0,0);
			tLX->cFont.Draw(bmpDest, x+375, j, col, "%s", buf);

			j+=20;
		}
	}


	// Team deathmatch scoreboard
	if(iGameType == GMT_TEAMDEATH) {

		DrawHLine(bmpDest, x+15,x+width-15, y+height-30,0xffff);

		// Go through each team
		j = y+50;
		for(n=0;n<4;n++) {
			int team = iTeamList[n];
			int score = iTeamScores[team];

			// Check if the team has any players
			if(score == -1)
				continue;

			Uint32 colour = MakeColour( teamcolours[team*3], teamcolours[team*3+1],teamcolours[team*3+2]);

			tLX->cFont.Draw(bmpDest, x+15, j, colour, "%s team  (%d)",teamnames[team],score);
			if(iLives != WRM_UNLIM)
				tLX->cFont.Draw(bmpDest, x+300, j, colour,"Lives");
			tLX->cFont.Draw(bmpDest, x+380, j, colour,"Kills");
			DrawHLine(bmpDest, x+15,x+width-15, j+15,0xffff);
			j+=20;


			// Draw the players
			for(i=0;i<iScorePlayers;i++) {
				CWorm *p = &cRemoteWorms[iScoreboard[i]];

				if(p->getTeam() != team)
					continue;

				DrawImage(bmpDest, p->getPicimg(), x+15, j);

				tLX->cFont.Draw(bmpDest, x+40, j, 0xffff, p->getName());

				if(p->getLives() >= 0)
					tLX->cFont.DrawCentre(bmpDest, x+317, j, 0xffff, "%d",p->getLives());
				else if(p->getLives() == WRM_OUT)
					tLX->cFont.DrawCentre(bmpDest, x+317, j, 0xffff, "out");

				tLX->cFont.DrawCentre(bmpDest, x+393, j, 0xffff, "%d",p->getKills());

				j+=20;
			}

			j+=15;
		}
	}
}


///////////////////
// Draw the bonuses
void CClient::DrawBonuses(SDL_Surface *bmpDest, CViewport *v)
{
	CBonus *b = cBonuses;

	for(int i=0;i<MAX_BONUSES;i++,b++) {
		if(!b->getUsed())
			continue;

		b->Draw(bmpDest, v, iShowBonusName);
	}
}


///////////////////
// Draw text that is shadowed
void CClient::DrawText(SDL_Surface *bmpDest, int centre, int x, int y, Uint32 fgcol, char *fmt, ...)
{
	char buf[512];
	va_list arg;
	
	va_start(arg, fmt);
	vsprintf(buf, fmt, arg);
	va_end(arg);

	if(centre) {
		//tLX->cOutlineFont.DrawCentre(bmpDest, x+1, y+1, 0, buf);
		tLX->cOutlineFont.DrawCentre(bmpDest, x, y, fgcol, buf);
	}
	else {
		//tLX->cOutlineFont.Draw(bmpDest, x+1, y+1, 0, buf);
		tLX->cOutlineFont.Draw(bmpDest, x, y, fgcol, buf);
	}
}


///////////////////
// Draw the local chat
void CClient::DrawLocalChat(SDL_Surface *bmpDest)
{
	int y = 0;
	int i;
	for(i=0;i<MAX_CLINES;i++) {
		line_t *l = cChatbox.GetLine(i);

		// This chat times out after a few seconds AND is on the top of the screen
		if(l->iUsed && tLX->fCurTime - l->fTime < 3) {
			tLX->cFont.Draw(bmpDest, 6, y+1, 0, l->strLine);
			tLX->cFont.Draw(bmpDest, 5, y, l->iColour, l->strLine);
			y+=18;
		}
	}
}


///////////////////
// Draw the remote chat
void CClient::DrawRemoteChat(SDL_Surface *bmpDest)
{
	int y = 386;
	int i;
	
	for(i=0;i<MAX_CLINES;i++) {
		line_t *l = cChatbox.GetLine(i);

		// This chat is in the black region of the screen
		if(l->iUsed) {
			tLX->cFont.Draw(bmpDest, 190, y+1, 0, l->strLine);
			tLX->cFont.Draw(bmpDest, 190, y, l->iColour, l->strLine);
			y+=15;
		}
	}
}




CGuiLayout ViewportMgr;
enum {
    v1_On,
    v1_Type,
    v2_On,
    v2_Type,
    v_ok
};

///////////////////
// Initialize the viewport manager
void CClient::InitializeViewportManager(void)
{
    int x = 320-gfxGame.bmpViewportMgr->w/2;
    int y = 200-gfxGame.bmpViewportMgr->h/2;
    int x2 = x+gfxGame.bmpViewportMgr->w/2+40;


    bViewportMgr = true;

    // Initialize the gui
    ViewportMgr.Initialize();

    bool v2On = true;
    // If there is only 1 player total, turn the second viewport off
    int count = 0;
    for(int i=0; i<MAX_WORMS; i++ ) {
        if(cRemoteWorms[i].isUsed())
            count++;
    }

    if( count <= 1 )
        v2On = false;
    
    // Viewport 1
    ViewportMgr.Add( new CLabel("Used",0xffff), -1,     x+15,  y+80,  0,   0);
    ViewportMgr.Add( new CLabel("Type",0xffff), -1,     x+15,  y+110,  0,   0);
    //ViewportMgr.Add( new CCheckbox(true),       v1_On,  x+75,  y+80,  20,  20);    
    ViewportMgr.Add( new CCombobox(),           v1_Type,x+75,  y+110, 150, 17);
    ViewportMgr.Add( new CCheckbox(v2On),       v2_On,  x2,    y+80,  20,  20);    
    ViewportMgr.Add( new CCombobox(),           v2_Type,x2,    y+110, 150, 17);
    ViewportMgr.Add( new CButton(BUT_OK, tMenu->bmpButtons),    v_ok,310,y+gfxGame.bmpViewportMgr->h-25,30,15);

    // Fill in the combo boxes

    // If the first player is a human, and is still playing: Only show the follow option    
    ViewportMgr.SendMessage( v1_Type, CBM_ADDITEM, VW_FOLLOW, (DWORD)"Follow" );
    if( cLocalWorms[0]->getLives() == WRM_OUT || cLocalWorms[0]->getType() == PRF_COMPUTER ) {
        ViewportMgr.SendMessage( v1_Type, CBM_ADDITEM, VW_CYCLE, (DWORD)"Cycle" );
        ViewportMgr.SendMessage( v1_Type, CBM_ADDITEM, VW_FREELOOK, (DWORD)"Free Look" );
        ViewportMgr.SendMessage( v1_Type, CBM_ADDITEM, VW_ACTIONCAM, (DWORD)"Action Cam" );
    }
    
    // If the second player is a human and is still playing: Only show the follow option
    bool show = true;
    if( iNumWorms > 1 )
        if( cLocalWorms[1]->getLives() != WRM_OUT && cLocalWorms[1]->getType() == PRF_HUMAN )
            show = false;

    ViewportMgr.SendMessage( v2_Type, CBM_ADDITEM, VW_FOLLOW, (DWORD)"Follow" );
    if( show ) {    
        ViewportMgr.SendMessage( v2_Type, CBM_ADDITEM, VW_CYCLE, (DWORD)"Cycle" );
        ViewportMgr.SendMessage( v2_Type, CBM_ADDITEM, VW_FREELOOK, (DWORD)"Free Look" );
        ViewportMgr.SendMessage( v2_Type, CBM_ADDITEM, VW_ACTIONCAM, (DWORD)"Action Cam" );
    }
    
    
    // Restore old settings
    ViewportMgr.SendMessage( v1_Type, CBM_SETCURINDEX, cViewports[0].getType(), 0);
    ViewportMgr.SendMessage( v2_On, CKM_SETCHECK, cViewports[1].getUsed(), 0);
    if( cViewports[1].getUsed() )
        ViewportMgr.SendMessage( v2_Type, CBM_SETCURINDEX, cViewports[1].getType(), 0);


    // Draw the background into the menu buffer
    DrawImage(tMenu->bmpBuffer,gfxGame.bmpViewportMgr,x,y);
}


///////////////////
// Draw the viewport manager
void CClient::DrawViewportManager(SDL_Surface *bmpDest)
{
    mouse_t *Mouse = GetMouse();
    int mcursor = 0;
    
    int x = 320-gfxGame.bmpViewportMgr->w/2;
    int y = 200-gfxGame.bmpViewportMgr->h/2;
    
	// Draw the back image
	DrawImage(bmpDest,gfxGame.bmpViewportMgr,x,y);

    tLX->cFont.Draw(bmpDest, x+75,y+50, 0xffff,"Viewport 1");
    tLX->cFont.Draw(bmpDest, x+gfxGame.bmpViewportMgr->w/2+40,y+50, 0xffff,"Viewport 2");
    
    ViewportMgr.Draw(bmpDest);
    gui_event_t *ev = ViewportMgr.Process();

    if( ViewportMgr.getWidget(v_ok)->InBox(Mouse->X,Mouse->Y) )
        mcursor = 1;

    // Draw the mouse
    DrawImage(bmpDest,gfxGUI.bmpMouse[mcursor], Mouse->X,Mouse->Y);

    if(!ev)
        return;


    // Get the worm count    
    int Wormcount = 0;
    for(int i=0; i<MAX_WORMS; i++ ) {
        if(cRemoteWorms[i].isUsed())
            Wormcount++;
    }

    switch(ev->iControlID) {

        // V2 On
        case v2_On:
            if(ev->iEventMsg == CHK_CHANGED) {
                // If there is only one worm, disable the 2nd viewport
                if( Wormcount <= 1 )
                    ViewportMgr.SendMessage(v2_On, CKM_SETCHECK,false,0);
            }
            break;

        // OK
        case v_ok:
            if(ev->iEventMsg == BTN_MOUSEUP) {

                // If there is only one worm, disable the 2nd viewport
                if( Wormcount <= 1 )
                    ViewportMgr.SendMessage(v2_On, CKM_SETCHECK,false,0);

                // Grab settings
                int a_type = ViewportMgr.SendMessage(v1_Type, CBM_GETCURINDEX, 0,0);
                int b_on = ViewportMgr.SendMessage(v2_On, CKM_GETCHECK, 0,0);
                int b_type = ViewportMgr.SendMessage(v2_Type, CBM_GETCURINDEX, 0,0);

                for( int i=0; i<NUM_VIEWPORTS; i++ ) {
                    cViewports[i].setUsed(false);
                    cViewports[i].reset();
                }

                // If both viewports are used, resize the viewports
                if( !b_on ) {
                    cViewports[0].Setup(0,0,640,382,a_type);
		            cViewports[0].setUsed(true);
                    cViewports[0].setTarget(cLocalWorms[0]);
                }

                if( b_on ) {
                    cViewports[0].Setup(0,0,318,382,a_type);
                    cViewports[0].setTarget(cLocalWorms[0]);
		            cViewports[0].setUsed(true);

		            cViewports[1].Setup(322,0,318,382,b_type);
		            cViewports[1].setUsed(true);
                    if( iNumWorms >= 2 )
                        cViewports[1].setTarget(cLocalWorms[1]);
                    else
                        cViewports[1].setTarget(NULL);

                }

                // Shutdown & leave
                ViewportMgr.Shutdown();
                bViewportMgr = false;
                return;
            }
            break;
    }   
}


///////////////////
// Draw the scoreboard
void CClient::DrawScoreboard(SDL_Surface *bmpDest)
{
    bool bShowScore = false;
    bool bShowReady = false;

    // Do checks on whether or not to show the scoreboard
    if(iGameMenu || bViewportMgr || iGameOver || Con_IsUsed())
        return;
    if(cShowScore.isDown())
        bShowScore = true;
    if(iNetStatus == NET_CONNECTED && iGameReady && tGameInfo.iGameType != GME_LOCAL) {
        bShowScore = true;
        bShowReady = true;
    }
    if(!bShowScore)
        return;

    int y = 180;
    int x = 0;
    int w = 220;
    int h = 185;
    DrawRectFill(bmpDest, x+1, y, x+w-1, y+h-1, 0);
    Menu_DrawBox(bmpDest, x, y, x+w, y+h);

    tLX->cFont.Draw(bmpDest, x+5, y+4, 0xffff, "Players");
    if(!bShowReady) {
        tLX->cFont.Draw(bmpDest, x+165, y+4, 0xffff, "L");
        tLX->cFont.Draw(bmpDest, x+195, y+4, 0xffff, "K");
        //tLX->cFont.Draw(bmpDest, x+195, y+4, 0xffff, "P");
    }
    DrawHLine(bmpDest, x+4, x+w-4, y+20, 0xffff);


    // Draw the players
	int j = y+25;
    int i;
    for(i=0;i<iScorePlayers;i++) {
        CWorm *p = &cRemoteWorms[iScoreboard[i]];

        // If this player is local & human, highlight it
        if(p->getType() == PRF_HUMAN && p->getLocal())
            DrawRectFill(bmpDest, x+2,j-2, x+w-1, j+18, MakeColour(32,32,32));
        
        // Pic & Name
        DrawImage(bmpDest, p->getPicimg(), x+5, j);
        tLX->cFont.Draw(bmpDest, x+30, j, 0xffff, p->getName());
        
        // Score
        if(!bShowReady) {
            if(p->getLives() >= 0)
                tLX->cFont.DrawCentre(bmpDest, x+170, j, 0xffff, "%d",p->getLives());
            else if(p->getLives() == WRM_OUT)
                tLX->cFont.DrawCentre(bmpDest, x+170, j, 0xffff, "out");

            tLX->cFont.DrawCentre(bmpDest, x+200, j, 0xffff, "%d",p->getKills());

        } else {
            // Ready state
            if(p->getGameReady())
                tLX->cFont.Draw(bmpDest, x+150, j, MakeColour(0,200,0), "Ready");
            else
                tLX->cFont.Draw(bmpDest, x+150, j, 0xffff, "Waiting");
        }

        // TODO: Ping
        
        j+=20;
    }
}