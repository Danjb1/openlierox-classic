/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Client class - Game routines
// Created 21/7/02
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"
#include "console.h"


///////////////////
// Simulation
void CClient::Simulation(void)
{
	frame_t *f = &tFrames[ iServerFrame & FRAME_MASK ];
	int local,i;
	int teamgame = false;
    CWorm *w;
    bool con = Con_IsUsed();

	if(iGameType == GMT_TEAMDEATH)
		teamgame = true;


	// If we're in a menu & a local game, don't do simulation
    if( iGameOver || ((iGameMenu || bViewportMgr) && tGameInfo.iGameType == GME_LOCAL) ) {

        // Clear the input of the local worms
        w = cRemoteWorms;
	    for(i=0;i<MAX_WORMS;i++,w++) {
		    if(!w->isUsed())
			    continue;

		    if( w->getLocal() )
                w->clearInput();
        }
    }

    // We stop a few seconds after the actual game over
    if(iGameOver && tLX->fCurTime - fGameOverTime > GAMEOVER_WAIT)
        return;
    if((iGameMenu || bViewportMgr) && tGameInfo.iGameType == GME_LOCAL)
        return;


    // Process the viewports
    for(i=0;i<NUM_VIEWPORTS;i++) {
        if(cViewports[i].getUsed() && !iGameOver)
            cViewports[i].Process(cRemoteWorms, cViewports, cMap->GetWidth(), cMap->GetHeight(), iGameType);
    }


	// Player simulation
	w = cRemoteWorms;
	for(i=0;i<MAX_WORMS;i++,w++) {
		if(!w->isUsed())
			continue;

		local = w->getLocal();
		
		if(w->getAlive()) {

			/*
				Only get input for this worm on certain conditions:
				1) This worm is a local worm (ie, owned by me)
				2) We're not in a game menu
				3) We're not typing a message
			*/

			if(local && !iGameMenu && !iChat_Typing && !iGameOver && !con) {
				if(w->getType() == PRF_HUMAN)
					w->getInput();
				else
					w->AI_GetInput(iGameType, teamgame, iGameType == GMT_TAG, cMap);
            }

			// Simulate the worm
			w->Simulate(cMap, cRemoteWorms, local, tLX->fDeltaTime);

            if(iGameOver)
                continue;


			// Check if this worm picked up a bonus
			CBonus *b = cBonuses;
			for(int n=0;n<MAX_BONUSES;n++,b++) {
				if(!b->getUsed())
					continue;

				if(w->CheckBonusCollision(b)) {
					if(local) {
						if( w->GiveBonus(b) ) {

							// Pickup noise
							BASS_SamplePlay(sfxGame.smpPickup);

							DestroyBonus(n, local, w->getID());
						}
					}
				}
			}
		}

        if(iGameOver)
            continue;


		// Is this worm shooting?
		if(/*local && */w->getAlive()) {

			// Shoot
			if(w->getWormState()->iShoot) {
				// This is only for client-side weapons, like jetpack
				PlayerShoot(w);
			}

			// If the worm is using a weapon with a laser sight, spawn a laser sight
			if(w->getCurWeapon()->Weapon->LaserSight) {
				LaserSight(w);
			}

		}


		// In a game of tag, increment the tagged worms time
		if(iGameType == GMT_TAG && w->getTagIT())
			w->incrementTagTime(tLX->fDeltaTime);
	}

    // Entities
	SimulateEntities(tLX->fDeltaTime,cMap);

    // Weather
    //cWeather.Simulate(tLX->fDeltaTime, cMap);

	// Projectiles
    SimulateProjectiles(tLX->fDeltaTime);
    
    // Bonuses    
    SimulateBonuses(tLX->fDeltaTime);




	// Debug test
#ifdef _DEBUG
	/*keyboard_t *kb = GetKeyboard();
	if( kb->keys[SDLK_1])
		iDrawingViews[1] = 1;
	if( kb->keys[SDLK_2])
		iDrawingViews[1] = 2;
	if( kb->keys[SDLK_3])
		iDrawingViews[1] = 3;
	if( kb->keys[SDLK_4])
		iDrawingViews[1] = 4;

	cRemoteWorms[iDrawingViews[1]].getViewport()->Setup(322,0,318,382);*/
#endif  // _DEBUG
}


///////////////////
// Simulate the projectiles
void CClient::SimulateProjectiles(float dt)
{
	CProjectile *prj = cProjectiles;
	int a,i;
	CVec sprd;
	int explode = false;
	int timer = false;
	int shake = 0;
	int dirt = false;
    int grndirt = false;
	int damage = 0;
    int result = 0;
    int wormid = -1;

    
    for(int p=0;p<nTopProjectile;p++,prj++) {
		if(!prj->isUsed())
			continue;
		explode = false;
		timer = false;
		dirt = false;
        grndirt = false;
		shake = 0;
		int spawnprojectiles = false;
        
		// Check if the timer is up
		proj_t *pi = prj->GetProjInfo();
        float f = prj->getTimeVarRandom();
		if(pi->Timer_Time > 0 && (pi->Timer_Time+pi->Timer_TimeVar*f) < prj->getLife()) {            

			// Run the end timer function
			if(pi->Timer_Type == PJ_EXPLODE) {
				explode = true;
				timer = true;

				if(pi->Timer_Projectiles)
					spawnprojectiles = true;
				if(pi->Timer_Shake > shake)
					shake = pi->Timer_Shake;
			}

			// Create some dirt
			if(pi->Timer_Type == PJ_DIRT) {
				dirt = true;
				if(pi->Timer_Projectiles)
					spawnprojectiles = true;
				if(pi->Timer_Shake > shake)
					shake = pi->Timer_Shake;
				damage = pi->Timer_Damage;
			}

            // Create some green dirt
			if(pi->Timer_Type == PJ_GREENDIRT) {
				grndirt = true;
				if(pi->Timer_Projectiles)
					spawnprojectiles = true;
				if(pi->Timer_Shake > shake)
					shake = pi->Timer_Shake;
				damage = pi->Timer_Damage;
			}

            // Carve
			if(pi->Timer_Type == PJ_CARVE) {                
				int d = cMap->CarveHole(pi->Timer_Damage,prj->GetPosition());
                prj->setUsed(false);

                if(pi->Timer_Projectiles)
					spawnprojectiles = true;

                // Increment the dirt count
                cRemoteWorms[prj->GetOwner()].incrementDirtCount( d );

                CheckDemolitionsGame();
			}
		}

		// Simulate the projectile
        wormid = -1;
        result = prj->Simulate(dt,cMap, cRemoteWorms, &wormid);

        /*
        ===================
         Terrain Collision
        ===================
        */
        if( result & PJC_TERRAIN ) {

			// Explosion
			if(pi->Hit_Type == PJ_EXPLODE) {
				explode = true;

				if(pi->Hit_Shake > shake)					
					shake = pi->Hit_Shake;

				// Play the hit sound
				if(pi->Hit_UseSound)
					BASS_SamplePlay(pi->smpSample);
			}

			// Bounce
			if(pi->Hit_Type == PJ_BOUNCE) {
				prj->Bounce(pi->Hit_BounceCoeff);

				// Do we do a bounce-explosion (bouncy larpa uses this)
				if(pi->Hit_BounceExplode > 0)
					Explosion(prj->GetPosition(), pi->Hit_BounceExplode, false, prj->GetOwner());
			}

			// Carve
			if(pi->Hit_Type == PJ_CARVE) {                
				int d = cMap->CarveHole(pi->Hit_Damage,prj->GetPosition());
                prj->setUsed(false);

                // Increment the dirt count
                cRemoteWorms[prj->GetOwner()].incrementDirtCount( d );

                CheckDemolitionsGame();
			}

			// Dirt
			if(pi->Hit_Type == PJ_DIRT) {
				dirt = true;
				damage = pi->Hit_Damage;
			}

            // Green Dirt
            if(pi->Hit_Type == PJ_GREENDIRT) {
                grndirt = true;
            }


			if(pi->Hit_Projectiles)
				spawnprojectiles = true;
		}


        /*
        ===================
          Explosion Event
        ===================
        */
        /*if( result & PJC_EXPLODE ) {
            
            // Explosion
			if(pi->Exp_Type == PJ_EXPLODE) {
				explode = true;

				if(pi->Exp_Shake > shake)					
					shake = pi->Exp_Shake;

				// Play the Explode sound
				if(pi->Exp_UseSound)
					BASS_SamplePlay(pi->smpSample);
			}

			// Carve
			if(pi->Exp_Type == PJ_CARVE) {                
				int d = cMap->CarveHole(pi->Exp_Damage,prj->GetPosition());
				prj->setUsed(false);

                // Increment the dirt count
                cRemoteWorms[prj->GetOwner()].incrementDirtCount( d );

                CheckDemolitionsGame();
			}

			// Dirt
			if(pi->Exp_Type == PJ_DIRT) {
				dirt = true;
				damage = pi->Exp_Damage;
			}

            // Green Dirt
            if(pi->Exp_Type == PJ_GREENDIRT) {
                grndirt = true;
            }

            // Spawn projectiles?
            if(pi->Exp_Projectiles)
				spawnprojectiles = true;
        }*/


		// Check if we need to spawn any trail projectiles
		if(prj->getSpawnPrjTrl()) {
			prj->setSpawnPrjTrl(false);

			for(i=0;i<pi->PrjTrl_Amount;i++) {				
				sprd = CVec(0,0);
				
				if(pi->PrjTrl_UsePrjVelocity) {
					sprd = prj->GetVelocity();
					float l = NormalizeVector(&sprd);
					sprd = sprd * (l*0.3f);		// Slow it down a bit.
													// It can be sped up by the speed variable in the script
				} else
					GetAngles((int)((float)pi->PrjTrl_Spread * prj->getRandomFloat()),&sprd,NULL);
				
				CVec v = sprd*(float)pi->PrjTrl_Speed + CVec(1,1)*(float)pi->PrjTrl_SpeedVar*prj->getRandomFloat();

				SpawnProjectile(prj->GetPosition(), v, 0, prj->GetOwner(), pi->PrjTrl_Proj, prj->getRandomIndex()+1, false,0);
			}
		}


        /*
        ===================
           Worm Collision
        ===================
        */
		if( result & PJC_WORM && wormid >= 0 && !explode && !timer) {

			// Explode
			if(pi->PlyHit_Type == PJ_EXPLODE)
				explode = true;

			// Injure
			if(pi->PlyHit_Type == PJ_INJURE) {
				
				// Add damage to the worm
				InjureWorm(&cRemoteWorms[wormid], pi->PlyHit_Damage, prj->GetOwner());
				prj->setUsed(false);
			}

			if(pi->PlyHit_Type != PJ_BOUNCE && pi->PlyHit_Type != PJ_NOTHING) {

				// Push the worm back
				CVec d = prj->GetVelocity();
				NormalizeVector(&d);
				CVec *v = cRemoteWorms[wormid].getVelocity();
				*v = *v + (d*100)*dt;
			}

			// Bounce
			if(pi->PlyHit_Type == PJ_BOUNCE)
				prj->Bounce(pi->PlyHit_BounceCoeff);

			if(pi->PlyHit_Projectiles)
				spawnprojectiles = true;

			// Dirt
			if(pi->PlyHit_Type == PJ_DIRT) {
				dirt = true;
				damage = pi->PlyHit_Damage;
			}

            // Green Dirt
            if(pi->Hit_Type == PJ_GREENDIRT) {
                grndirt = true;
            }
		}


		// Explode?
		if(explode) {
		
			// Explosion
			damage = pi->Hit_Damage;
			if(timer)
				damage = pi->Timer_Damage;
			if(pi->PlyHit_Type == PJ_EXPLODE)
				damage = pi->PlyHit_Damage;

			if(damage != -1)
				Explosion(prj->GetPosition(), damage, shake, prj->GetOwner());
			prj->setUsed(false);
		}

		// Dirt
		if(dirt) {
			damage = 5;
            int d = 0;
			d += cMap->PlaceDirt(damage,prj->GetPosition()-CVec(6,6));
			d += cMap->PlaceDirt(damage,prj->GetPosition()+CVec(6,-6));
			d += cMap->PlaceDirt(damage,prj->GetPosition()+CVec(0,6));
            
            // Remove the dirt count on the worm
            cRemoteWorms[prj->GetOwner()].incrementDirtCount( -d );
			prj->setUsed(false);
		}

        // Green dirt
        if(grndirt) {
            int d = cMap->PlaceGreenDirt(prj->GetPosition());
            
            // Remove the dirt count on the worm
            cRemoteWorms[prj->GetOwner()].incrementDirtCount( -d );
            prj->setUsed(false);
        }

		// Spawn any projectiles?
		if(spawnprojectiles) {

			CVec v = prj->GetVelocity();
			NormalizeVector(&v);

			// Calculate the angle of the direction the projectile is heading
			float heading = 0;
			if(pi->ProjUseangle) {
				heading = (float)( -atan2(v.GetX(),v.GetY()) * (180.0f/PI) );
				heading+=90;
				if(heading < 0)
					heading+=360;
				if(heading > 360)
					heading-=360;
				if(heading == 360)
					heading=0;
			}

			for(i=0;i<pi->ProjAmount;i++) {
				a = (int)( (float)pi->ProjAngle + heading + prj->getRandomFloat()*(float)pi->ProjSpread );
				GetAngles(a,&sprd,NULL);

				float speed = (float)pi->ProjSpeed + (float)pi->ProjSpeedVar*prj->getRandomFloat();				

				SpawnProjectile(prj->GetPosition(), sprd*speed, 0, prj->GetOwner(), pi->Projectile, prj->getRandomIndex()+1, false,0);
			}
		}
	}
}


///////////////////
// Explosion
void CClient::Explosion(CVec pos, int damage, int shake, int owner)
{
	int		x,y,i,px;
	CWorm	*w;
	Uint32	Colour = cMap->GetTheme()->iDefaultColour;
    int     gotDirt = false;
	
	// Go through until we find dirt to throw around
	y = MIN((int)pos.GetY(),cMap->GetHeight()-1);
	y = MAX(y,0);	

	px = (int)pos.GetX();

	for(x=px-2; x<px+2; x++) {
		// Clipping
		if(x<0)	continue;
		if(x>=cMap->GetWidth())	break;

		if(cMap->GetPixelFlag(x,y) & PX_DIRT) {
			Colour = GetPixel(cMap->GetImage(),x,y);
            gotDirt = true;
			break;
		}
	}

	// Go through bonuses. If any were next to an explosion, destroy the bonus explosivly
	CBonus *b = cBonuses;
	for(i=0;i<MAX_BONUSES;i++,b++) {
		if(!b->getUsed())
			continue;

        if( fabs(b->getPosition().GetX() - pos.GetX()) > 15 )
            continue;
        if( fabs(b->getPosition().GetY() - pos.GetY()) > 15 )
            continue;

		b->setUsed(false);
		Explosion(pos,15,5,owner);
	}


    // Go through projectiles. If any were next to an explosion, set the projectile's explode event to true
    CProjectile *prj = cProjectiles;
    /*for( i=0; i<MAX_PROJECTILES; i++, prj++ ) {
        if( !prj->isUsed() )
            continue;

        if( fabs(prj->GetPosition().GetX() - pos.GetX()) > 15 )
            continue;
        if( fabs(prj->GetPosition().GetY() - pos.GetY()) > 15 )
            continue;

        prj->setExplode( tLX->fCurTime + CalculateDistance(prj->GetPosition(),pos) / 500.0f, true);
    }*/


	// Particles
    if(gotDirt) {
	    for(x=0;x<2;x++)
		    SpawnEntity(ENT_PARTICLE,0,pos,CVec(GetRandomNum()*30,GetRandomNum()*10),Colour,NULL);
    }


	int expsize = 8;
	if(damage > 5)
		expsize = damage;

	// Explosion
	SpawnEntity(ENT_EXPLOSION, expsize, pos, CVec(0,0),0,NULL);
	
	int d = cMap->CarveHole(damage,pos);
    
    // Increment the dirt count
    cRemoteWorms[owner].incrementDirtCount( d );

	// If this is within a viewport, shake the viewport
	for(i=0; i<NUM_VIEWPORTS; i++) {
		if(!cViewports[i].getUsed())
            continue;
		
		if(shake) {
			if(cViewports[i].inView(pos))
				cViewports[i].Shake(shake);
		}
	}


	//
	// Server Side?
	//

	// Check if any of _my_ worms are near the explosion
	for(i=0;i<iNumWorms;i++) {
		w = cLocalWorms[i];

		if(!w->getAlive())
			continue;

		float dist = CalculateDistance(pos,w->getPos());
		if(dist <= 5) {

			// Injure him
			InjureWorm(w, damage,owner);
		}
	}

    CheckDemolitionsGame();
}


///////////////////
// Injure a worm
void CClient::InjureWorm(CWorm *w, int damage, int owner)
{
	int me = false;
	int n,i;
	int localid=0;

	int numworms = MIN(iNumWorms,2);

	// Make sure this is one of our worms
	for(i=0;i<iNumWorms;i++) {
		if(cLocalWorms[i]->getID() == w->getID()) {
			me=true;
			localid = i;
			break;
		}
	}


	if(w->Injure(damage)) {
		// His dead Jim
		
		// Kill me
		if(me) {
			w->setAlive(false);
			w->Kill();
            w->clearInput();

			// Let the server know that i am dead
			SendDeath(w->getID(), owner);


			// If this worm is now out of the game and this viewport was used for a worm,
			// change the viewport
			/*if(w->getLives() == WRM_OUT && iNumWorms > 1) {
				for(i=0;i<numworms;i++) {
					if(iDrawingViews[i] == localid) {

						cRemoteWorms[iDrawingViews[i]].getViewport()->setUsed(false);

						// Find another worm that is alive
						for(n=0;n<iNumWorms;n++) {
							if(cLocalWorms[n]->getLives() != WRM_OUT && !cLocalWorms[n]->getViewport()->getUsed()) {
								iDrawingViews[i] = n;
								break;
							}
						}

						// Setup the viewport
						CViewport *v = cRemoteWorms[iDrawingViews[i]].getViewport();
						v->setUsed(true);

						if(i==0)
							v->Setup(0,0,318,382);
						else
							v->Setup(322,0,318,382);
					}
				}
			}*/
		}
	}

	// Spawn some blood
	float amount = ((float)tLXOptions->iBloodAmount / 100.0f);
	for(i=0;i<amount;i++) {
		float sp = GetRandomNum()*50;
		SpawnEntity(ENT_BLOODDROPPER,0,w->getPos(),CVec(GetRandomNum()*sp,GetRandomNum()*sp*4),MakeColour(128,0,0),NULL);
		SpawnEntity(ENT_BLOOD,0,w->getPos(),CVec(GetRandomNum()*sp,GetRandomNum()*sp),MakeColour(128,0,0),NULL);
		SpawnEntity(ENT_BLOOD,0,w->getPos(),CVec(GetRandomNum()*sp,GetRandomNum()*sp),MakeColour(200,0,0),NULL);
	}
}

///////////////////
// Send a carve to the server
void CClient::SendCarve(CVec pos)
{
	int x,y,n,px;
	Uint32 Colour = cMap->GetTheme()->iDefaultColour;
	
	// Go through until we find dirt to throw around
	y = MIN((int)pos.GetY(),cMap->GetHeight()-1);
	y = MAX(y,0);
	px = (int)pos.GetX();

	for(x=px-2; x<=px+2; x++) {
		// Clipping
		if(x<0)	continue;
		if(x>=cMap->GetWidth())	break;

		if(cMap->GetPixelFlag(x,y) & PX_DIRT) {
			Colour = GetPixel(cMap->GetImage(),x,y);
			for(n=0;n<3;n++)
				SpawnEntity(ENT_PARTICLE,0,pos,CVec(GetRandomNum()*30,GetRandomNum()*10),Colour,NULL);
			break;
		}
	}

	/*for(x=0;x<3;x++)
		SpawnEntity(ENT_PARTICLE,0,pos,CVec(GetRandomNum()*30,GetRandomNum()*10),Colour);*/

	// Just carve a hole for the moment
	cMap->CarveHole(4,pos);
}


///////////////////
// Player shoot
void CClient::PlayerShoot(CWorm *w)
{
	wpnslot_t *Slot = w->getCurWeapon();
	
	if(Slot->Reloading)
		return;

	if(Slot->LastFire>0)
		return;

	Slot->LastFire = Slot->Weapon->ROF;


	// Special weapons get processed differently
	if(Slot->Weapon->Type == WPN_SPECIAL) {
		ShootSpecial(w);
		return;
	}

	// Beam weapons get processed differently
	if(Slot->Weapon->Type == WPN_BEAM) {
		//ShootBeam(w);
		return;
	}


	// Shots are now handled by the server 
	return;



	
	// Play the weapon's sound
	//if(Slot->Weapon->UseSound)
	//	StartSound(Slot->Weapon->smpSample, w->getPos(), w->getLocal(), 100, cLocalWorms[0]);
	
	/*CVec dir;
	GetAngles(Angle,&dir,NULL);
	CVec pos = w->getPos() + dir*8;

	pos.SetX( (int)pos.GetX() - (int)pos.GetX() % 2 );
	pos.SetY( (int)pos.GetY() - (int)pos.GetY() % 2 );

	int rot = 0;

	// Spawn the projectiles
	for(int n=0;n<Slot->Weapon->ProjAmount;n++) {
		rot = 0;

		// Spread
		a = Angle + GetRandomNum()*(float)Slot->Weapon->ProjSpread;

		if(a < 0)
			a+=360;
		if(a>360)
			a-=360;

		GetAngles(a,&sprd,NULL);

		// Calculate a random starting angle for the projectile rotation (if used)
		if(Slot->Weapon->Projectile) {
			if(Slot->Weapon->Projectile->Rotating) 
			   rot = GetRandomInt( 360 / Slot->Weapon->Projectile->RotIncrement ) * Slot->Weapon->Projectile->RotIncrement;
		}

		float speed = Slot->Weapon->ProjSpeed + Slot->Weapon->ProjSpeedVar*GetRandomNum();

		CVec p = sprd*speed + *w->getVelocity();
		//d_printf("Projectile vector = %f, %f\n", p.GetX(), p.GetY() );
		//d_printf("Worm vector = %f, %f\n", w->getVelocity()->GetX(), w->getVelocity()->GetY() );

		SpawnProjectile(pos, sprd*speed + *w->getVelocity(), rot, w->getID(), Slot->Weapon->Projectile);
	}

	//
	// Note: Drain does NOT have to use a delta time, because shoot timing is controlled by the ROF
	//

	// Drain the Weapon charge
	Slot->Charge -= Slot->Weapon->Drain / 100;
	if(Slot->Charge < 0) {
		Slot->Charge = 0;
		Slot->Reloading = true;
	}*/

	// Add the recoil
	/*CVec *vel = w->getVelocity();
	GetAngles(Angle,&sprd,NULL);

	*vel = *vel + -sprd*(float)Slot->Weapon->Recoil;

	// Draw the muzzle flash
	w->setDrawMuzzle(true);*/
}


///////////////////
// Shoot a special weapon
void CClient::ShootSpecial(CWorm *w)
{
	wpnslot_t *Slot = w->getCurWeapon();
	float dt = tLX->fDeltaTime;

	// Safety
	if(!Slot->Weapon)
		return;

	switch(Slot->Weapon->Special) {

		// Jetpack
		case SPC_JETPACK:
			CVec *v = w->getVelocity();
			*v = *v + CVec(0,-50) * ((float)Slot->Weapon->tSpecial.Thrust * dt);

			Uint32 blue = MakeColour(80,150,200);
			CVec s = CVec(15,0) * GetRandomNum();
			SpawnEntity(ENT_JETPACKSPRAY, 0, w->getPos(), s + CVec(0,1) * (float)Slot->Weapon->tSpecial.Thrust, blue, NULL);
			break;
	}


	// Drain the Weapon charge
	Slot->Charge -= Slot->Weapon->Drain / 100;
	if(Slot->Charge < 0) {
		Slot->Charge = 0;
		Slot->Reloading = true;
	}
}


///////////////////
// Shoot a beam
void CClient::ShootBeam(CWorm *w)
{
	wpnslot_t *Slot = w->getCurWeapon();

	// Safety
	if(!Slot->Weapon)
		return;

	// Get the direction angle
	float Angle = w->getAngle();
	if(w->getDirection() == DIR_LEFT)
		Angle=180-Angle;


	// Trace a line from the worm to length or until it hits something
	CVec pos = w->getPos();
	CVec dir;
	GetAngles((int)Angle,&dir,NULL);

	int divisions = 1;			// How many pixels we go through each check (more = slower)

	if( Slot->Weapon->Bm_Length < divisions)
		divisions = Slot->Weapon->Bm_Length;

	// Make sure we have at least 1 division
	divisions = MAX(divisions,1);

	int stopbeam = false;

	for(int i=0; i<Slot->Weapon->Bm_Length; i+=divisions) {
		uchar px = cMap->GetPixelFlag( (int)pos.GetX(), (int)pos.GetY() );

		if(px & PX_DIRT || px & PX_ROCK) {
			SpawnEntity(ENT_EXPLOSION, 5, pos, CVec(0,0), 0, NULL);
			int d = cMap->CarveHole(Slot->Weapon->Bm_Damage, pos);
            w->incrementDirtCount(d);
			break;
		}

		// Check if it has hit any of the worms
		CWorm *w2 = cRemoteWorms;
		for(int n=0;n<MAX_WORMS;n++,w2++) {
			if(!w2->isUsed() || !w2->getAlive())
				continue;

			// Don't check against me
			if(w2->getID() == w->getID())
				continue;

			float wormsize = 5;
			if(CalculateDistance(pos, w2->getPos()) < wormsize) {
				InjureWorm(w2, Slot->Weapon->Bm_PlyDamage, w->getID());
				stopbeam = true;
				break;
			}
		}

		if(stopbeam)
			break;

		pos = pos + dir*(float)divisions;
	}

	// Spawn a beam entity
	// Don't draw the beam if it is 255,0,255
	if(Slot->Weapon->Bm_Colour[0] != 255 && Slot->Weapon->Bm_Colour[1] != 0 && Slot->Weapon->Bm_Colour[2] != 255) {
		Uint32 col = MakeColour(Slot->Weapon->Bm_Colour[0], Slot->Weapon->Bm_Colour[1], Slot->Weapon->Bm_Colour[2]);
		SpawnEntity(ENT_BEAM, i, w->getPos(), dir, col, NULL);
	}


	// Drain the Weapon charge
	Slot->Charge -= Slot->Weapon->Drain / 100;
	if(Slot->Charge < 0) {
		Slot->Charge = 0;
		Slot->Reloading = true;
	}

    CheckDemolitionsGame();
}


///////////////////
// Spawn a projectile
void CClient::SpawnProjectile(CVec pos, CVec vel, int rot, int owner, proj_t *_proj, int _random, int _remote, float remotetime)
{
	CProjectile *proj = cProjectiles;
	int p=0;

	for(p=0;p<MAX_PROJECTILES;p++,proj++) {
		if(!proj->isUsed())
			break;
	}

	if(p >= MAX_PROJECTILES-1) {
		// Warning: Out of space for a projectile
		return;
	}

    if( p >= nTopProjectile )
        nTopProjectile = p+1;
    nTopProjectile = MIN(nTopProjectile,MAX_PROJECTILES);    
    
    // Safety
    _random %= 255;

	proj->Spawn(_proj,pos,vel,rot,owner,_random,_remote,remotetime);
}


///////////////////
// Update the scoreboard
void CClient::UpdateScoreboard(void)
{
	CWorm *w = cRemoteWorms;
	int p,i,j,s;

	// Clear the team scores
	for(i=0;i<4;i++) {
		iTeamScores[i]=-1;		// -1 means no players on team
		iTeamList[i]=i;
	}

	// Add the worms to the list
	iScorePlayers = 0;
	for(p=0; p<MAX_WORMS; p++,w++) {
		if(!w->isUsed())
			continue;

		iScoreboard[iScorePlayers++] = p;

		// Add to the team score
		if(iGameType == GMT_TEAMDEATH) {
			// Make the score at least zero to say we have 
			iTeamScores[w->getTeam()] = MAX(0,iTeamScores[w->getTeam()]);

			if(w->getLives() != WRM_OUT && w->getLives() != WRM_UNLIM)
				iTeamScores[w->getTeam()] += w->getLives();
		}
	}


	// Sort the team lists
	if(iGameType == GMT_TEAMDEATH) {
		for(i=0;i<4;i++) {
			for(j=0;j<3-i;j++) {
				if(iTeamScores[iTeamList[j]] < iTeamScores[iTeamList[j+1]]) {

					// Swap the team list entries
					s = iTeamList[j];
					iTeamList[j] = iTeamList[j+1];
					iTeamList[j+1] = s;
				}
			}
		}
	}


	if(iScorePlayers < 2)
		return;


	// Sort the array
	for(i=0; i<iScorePlayers; i++) {
		for(j=0; j<iScorePlayers-1-i; j++) {
			if(iGameType == GMT_TAG) {

				// TAG
				if(cRemoteWorms[iScoreboard[j]].getTagTime() < cRemoteWorms[iScoreboard[j + 1]].getTagTime()) {
				   // Swap the 2 scoreboard entries
					s = iScoreboard[j];
					iScoreboard[j] = iScoreboard[j+1];
					iScoreboard[j+1] = s;
				}

            } else if( iGameType == GMT_DEMOLITION ) {

                // DEMOLITIONS
                if(cRemoteWorms[iScoreboard[j]].getDirtCount() < cRemoteWorms[iScoreboard[j + 1]].getDirtCount()) {
				    // Swap the 2 scoreboard entries
					s = iScoreboard[j];
					iScoreboard[j] = iScoreboard[j+1];
					iScoreboard[j+1] = s;
				}
            
			} else {
				// DEATHMATCH or TEAM DEATHMATCH

				if(cRemoteWorms[iScoreboard[j]].getLives() < cRemoteWorms[iScoreboard[j + 1]].getLives()) {
				
					// Swap the 2 scoreboard entries
					s = iScoreboard[j];
					iScoreboard[j] = iScoreboard[j+1];
					iScoreboard[j+1] = s;
				} else if(cRemoteWorms[iScoreboard[j]].getLives() == cRemoteWorms[iScoreboard[j + 1]].getLives()) {
	
					// Equal lives, so compare kills
					if(cRemoteWorms[iScoreboard[j]].getKills() < cRemoteWorms[iScoreboard[j + 1]].getKills()) {
	
						// Swap the 2 scoreboard entries
						s = iScoreboard[j];
						iScoreboard[j] = iScoreboard[j+1];
						iScoreboard[j+1] = s;
					}
				}
			}
		}
	}
}


///////////////////
// Simulate the bonuses
void CClient::SimulateBonuses(float dt)
{
	CBonus *b = cBonuses;

	for(int i=0;i<MAX_BONUSES;i++,b++) {
		if(!b->getUsed())
			continue;

		b->Simulate(cMap, dt);
	}
}


///////////////////
// Destroy a bonus (not explosively)
void CClient::DestroyBonus(int id, int local, int wormid)
{
	cBonuses[id].setUsed(false);


	// Tell the server the bonus is gone if we grabbed it
	if(local) {
		CBytestream bs;
		bs.writeByte(C2S_GRABBONUS);
		bs.writeByte(id);
		bs.writeByte(wormid);
		bs.writeByte(cRemoteWorms[wormid].getCurrentWeapon());

		cNetChan.getMessageBS()->Append(&bs);
	}
}


///////////////////
// Check the demolitions game
void CClient::CheckDemolitionsGame(void)
{
    // If the map has less then a 1/5th of the dirt it once had, the game is over
    // And the worm with the highest dirt count wins

    if( iGameType != GMT_DEMOLITION || tGameInfo.iGameType != GME_LOCAL )
        return;

    // Add up all the worm's dirt counts
    int nDirtCount = 0;
    int highest = -99999;
    int winner = -1;

    CWorm *w = cRemoteWorms;
    for( int i=0; i<MAX_WORMS; i++,w++ ) {
        if( !w->isUsed() )
            continue;

        nDirtCount += w->getDirtCount();

        // Get the highest dirt count
        if( w->getDirtCount() > highest ) {
            highest = w->getDirtCount();
            winner = i;
        }
    }

    if( nDirtCount > (float)cMap->GetDirtCount()*0.8f ) {

        // Make the server trigger a game over
        cServer->DemolitionsGameOver(winner);
    }
}


///////////////////
// Show a laser sight
void CClient::LaserSight(CWorm *w)
{
	wpnslot_t *Slot = w->getCurWeapon();

	// Safety
	if(!Slot->Weapon)
		return;

	// Only show the sight if the player is NOT reloading
	if(Slot->Reloading)
		return;


	// Get the direction angle
	float Angle = w->getAngle();
	if(w->getDirection() == DIR_LEFT)
		Angle=180-Angle;


	// Trace a line from the worm to length or until it hits something
	CVec pos = w->getPos();
	CVec dir;
	GetAngles((int)Angle,&dir,NULL);

	int divisions = 3;			// How many pixels we go through each check (more = slower)

	int stopbeam = false;

	for(int i=0; i<9999; i+=divisions) {
		uchar px = cMap->GetPixelFlag( (int)pos.GetX(), (int)pos.GetY() );

		if(px & PX_DIRT || px & PX_ROCK)			
			break;

		// Check if it has hit any of the worms
		CWorm *w2 = cRemoteWorms;
		for(int n=0;n<MAX_WORMS;n++,w2++) {
			if(!w2->isUsed() || !w2->getAlive())
				continue;

			// Don't check against me
			if(w2->getID() == w->getID())
				continue;

			float wormsize = 5;
			if(CalculateDistance(pos, w2->getPos()) < wormsize) {
				stopbeam = true;

				// We have a target
				w->setTarget(true);
				break;
			}
		}

		if(stopbeam)
			break;

		pos = pos + dir*(float)divisions;
	}

	// Spawn a laser sight entity
	SpawnEntity(ENT_LASERSIGHT, i, w->getPos(), dir, 0, NULL);
}


///////////////////
// Process the shots
void CClient::ProcessShots(void)
{
	int num = cShootList.getNumShots();

	for(int i=0; i<num; i++) {
		shoot_t *sh = cShootList.getShot(i);

		if(sh)
			ProcessShot(sh);
	}
}


///////////////////
// Process a shot
void CClient::ProcessShot(shoot_t *shot)
{
	CWorm *w = &cRemoteWorms[shot->nWormID];

    // If this worm is dead, ignore the shot
    if(!w->getAlive())
        return;

	weapon_t *wpn = cGameScript.GetWeapons() + shot->nWeapon;

	// Process beam weapons differently
	if(wpn->Type == WPN_BEAM) {
		ProcessShot_Beam(shot);
		return;
	}


	CVec dir;
	GetAngles(shot->nAngle,&dir,NULL);
	CVec pos = shot->cPos + dir*8;

	
	// Play the weapon's sound
	if(wpn->UseSound)
		StartSound(wpn->smpSample, w->getPos(), w->getLocal(), 100, cLocalWorms[0]);

	// Add the recoil
	CVec *vel = w->getVelocity();
	*vel = *vel + -dir*(float)wpn->Recoil;

	// Draw the muzzle flash
	w->setDrawMuzzle(true);


	// This is assuming that the client time is greater then the projectile time
	float time = fServerTime - shot->fTime;	

	CVec sprd;

	for(int i=0; i<wpn->ProjAmount; i++) {
		
		int rot = 0;		
		
		// Spread
		float a = (float)shot->nAngle + GetFixedRandomNum(shot->nRandom)*(float)wpn->ProjSpread;
                
		if(a < 0)
			a+=360;
		if(a>360)
			a-=360;

		GetAngles((int)a,&sprd,NULL);

		// Calculate a random starting angle for the projectile rotation (if used)
		if(wpn->Projectile) {
			if(wpn->Projectile->Rotating) {

				// Prevent div by zero
				if(wpn->Projectile->RotIncrement == 0)
					wpn->Projectile->RotIncrement = 1;
				rot = GetRandomInt( 360 / wpn->Projectile->RotIncrement ) * wpn->Projectile->RotIncrement;
			}
		}

        shot->nRandom++;
		shot->nRandom %= 255;

		float speed = (float)wpn->ProjSpeed + (float)wpn->ProjSpeedVar * GetFixedRandomNum(shot->nRandom);

        shot->nRandom *= 5;
		shot->nRandom %= 255;

        CVec v = sprd*speed + shot->cWormVel;

		SpawnProjectile(pos, sprd*speed + shot->cWormVel, rot, w->getID(), wpn->Projectile, shot->nRandom, true, time);

		shot->nRandom++;
		shot->nRandom %= 255;
	}
}


///////////////////
// Process a shot - Beam
void CClient::ProcessShot_Beam(shoot_t *shot)
{
	CWorm *w = &cRemoteWorms[shot->nWormID];
	weapon_t *wpn = cGameScript.GetWeapons() + shot->nWeapon;

	// Trace a line from the worm to length or until it hits something
	CVec dir;
	GetAngles(shot->nAngle,&dir,NULL);
	CVec pos = shot->cPos;
	
	
	int divisions = 1;			// How many pixels we go through each check (more = slower)

	if( wpn->Bm_Length < divisions)
		divisions = wpn->Bm_Length;

	// Make sure we have at least 1 division
	divisions = MAX(divisions,1);

	int stopbeam = false;

	for(int i=0; i<wpn->Bm_Length; i+=divisions) {
		uchar px = cMap->GetPixelFlag( (int)pos.GetX(), (int)pos.GetY() );

		if(px & PX_DIRT || px & PX_ROCK) {
			// No explosion if damage is -1
			if(wpn->Bm_Damage != -1) {
				SpawnEntity(ENT_EXPLOSION, 5, pos, CVec(0,0), 0, NULL);
				int d = cMap->CarveHole(wpn->Bm_Damage, pos);
				w->incrementDirtCount(d);
			}

			// Stop the beam regardless of explosion being shown
			break;
		}

		// Check if it has hit any of the worms
		CWorm *w2 = cRemoteWorms;
		for(int n=0;n<MAX_WORMS;n++,w2++) {
			if(!w2->isUsed() || !w2->getAlive())
				continue;

			// Don't check against the creator
			if(w2->getID() == w->getID())
				continue;

			float wormsize = 5;
			if(CalculateDistance(pos, w2->getPos()) < wormsize) {
				InjureWorm(w2, wpn->Bm_PlyDamage, w->getID());
				stopbeam = true;
				break;
			}
		}

		if(stopbeam)
			break;

		pos = pos + dir*(float)divisions;
	}

	// Spawn a beam entity
	if(wpn->Bm_Colour[0] != 255 || wpn->Bm_Colour[1] != 0 || wpn->Bm_Colour[2] != 255) {
		Uint32 col = MakeColour(wpn->Bm_Colour[0], wpn->Bm_Colour[1], wpn->Bm_Colour[2]);
		SpawnEntity(ENT_BEAM, i, w->getPos(), dir, col, NULL);
	}

    CheckDemolitionsGame();
}



int ChatMaxLength = 48;

///////////////////
// Process any chatter
void CClient::processChatter(void)
{	
    keyboard_t *kb = GetKeyboard();

	// If we're currently typing a message, add any keys to it
	if(iChat_Typing) {

        // Escape
        if(GetKeyboard()->keys[SDLK_ESCAPE] || GetKeyboard()->KeyUp[SDLK_ESCAPE]) {
            // Stop typing
            iChat_Typing = false;
            
            GetKeyboard()->keys[SDLK_ESCAPE] = false;
            GetKeyboard()->KeyDown[SDLK_ESCAPE] = false;
            GetKeyboard()->KeyUp[SDLK_ESCAPE] = false;
            return;
        }

        // Go through the keyboard queue
        for(int i=0; i<kb->queueLength; i++) {
            int c = kb->keyQueue[i];
            bool down = true;

            // Was it an up keypress?
            if(c < 0) {
                c = -c;
                down = false;
            }

            processChatCharacter(c, down);
        }

        return;
    }

    
	// Check if we have hit the chat key and we're in a network game
	if(cChat_Input.isUp() && tGameInfo.iGameType != GME_LOCAL) {

		// Initialize the chatter
		iChat_Typing = true;
		iChat_Pos = 0;
		sChat_Text[0] = 0;
		iChat_Lastchar = -1;
		iChat_Holding = false;
		fChat_TimePushed = -9999;
	}
}


///////////////////
// Process a single character chat
void CClient::processChatCharacter(char c, bool bDown)
{
    // Up?
    if(!bDown) {
        iChat_Holding = false;
        return;
    }

    // Must be down
    
    if(iChat_Holding) {
        if(iChat_Lastchar != c)
            iChat_Holding = false;
        else {
            if(tLX->fCurTime - fChat_TimePushed < 0.4f)
                return;
        }
    }
    
    if(!iChat_Holding) {
        iChat_Holding = true;
        fChat_TimePushed = tLX->fCurTime;
    }
    
    iChat_Lastchar = c;
    
    // Backspace
    if(c == '\b') {
        if(iChat_Pos > 0)
            iChat_Pos--;
        else
            iChat_Pos = 0;
        
        sChat_Text[iChat_Pos] = 0;
        return;
    }
    
    // Enter
    if(c == '\r') {
        iChat_Typing = false;
        
        // Send chat message to the server
        if(sChat_Text[0]) {
            char buf[256];
            sprintf(buf, "%s: %s",cLocalWorms[0]->getName(), sChat_Text);
            SendText(buf);
        }
        return;
    }
    
    // Normal key
    if(iChat_Pos < ChatMaxLength-1 && c !=0 ) {
        sChat_Text[iChat_Pos++] = c;
        sChat_Text[iChat_Pos] = 0;
    }
}
