/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Projectile Class
// Created 11/2/02
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"


///////////////////
// Spawn the projectile
void CProjectile::Spawn(proj_t *_proj, CVec _pos, CVec _vel, int _rot, int _owner, int _random, int _remote, float _remotetime)
{
	tProjInfo = _proj;
	fLife = 0;
	fExtra = 0;
	vOldPos = _pos;
	vPosition = _pos;
	vVelocity = _vel;
	fRotation = (float)_rot;
	iOwner = _owner;
	iUsed = true;
	fLastTrailProj = -99999;
	iRandom = _random;
	iRemote = _remote;

	if(iRemote)
		fRemoteFrameTime = _remotetime;


	fSpeed = VectorLength(_vel);

	fFrame = 0;
	iFrameDelta = true;

	firstbounce = true;

	// Choose a colour
	if(tProjInfo->Type == PRJ_PIXEL) {
		int c = GetRandomInt(tProjInfo->NumColours-1);
		if(c == 0)
			iColour = MakeColour(tProjInfo->Colour1[0], tProjInfo->Colour1[1], tProjInfo->Colour1[2]);
		else if(c==1)
			iColour = MakeColour(tProjInfo->Colour2[0], tProjInfo->Colour2[1], tProjInfo->Colour2[2]);
	}
}


///////////////////
// Gets a random float from a special list
float CProjectile::getRandomFloat(void)
{
	float r = GetFixedRandomNum(iRandom++);

	iRandom %= 255;

	return r;
}


///////////////////
// Simulate the projectile
//
// Returns true if it has exploded
int CProjectile::Simulate(float dt, CMap *map)
{
	// If this is a remote projectile, the first frame is simulated with a longer delta time
	if(iRemote) {
		iRemote = false;

		// Only do it for a positive delta time
		if(fRemoteFrameTime>0) {
			if(Simulate(fRemoteFrameTime, map))
				return true;
		}

		// Don't leave, coz we still need to process it with a normal delta time
	}


	fLife += dt;
	fExtra += dt;

	
	vOldPos = vPosition;
	if(tProjInfo->UseCustomGravity)
		vVelocity = vVelocity + CVec(0,(float)tProjInfo->Gravity)*dt;
	else
		vVelocity = vVelocity + CVec(0,100)*dt;

	vPosition = vPosition + vVelocity*dt;

	// Dampening
	if(tProjInfo->Dampening != 1)
		vVelocity = vVelocity * tProjInfo->Dampening;

	if(tProjInfo->Rotating)
		fRotation += (float)tProjInfo->RotSpeed*dt;
	if(fRotation < 0)
		fRotation = 360;
	if(fRotation > 360)
		fRotation = 0;

	// Animation
	if(tProjInfo->Animating) {
		if(iFrameDelta)
			fFrame += (float)tProjInfo->AnimRate*dt;
		else
			fFrame -= (float)tProjInfo->AnimRate*dt;

		tLX->debug_float = fFrame;

		if(tProjInfo->bmpImage) {
			int NumFrames = tProjInfo->bmpImage->w / tProjInfo->bmpImage->h;
			if(fFrame >= NumFrames) {
				if(tProjInfo->AnimType == ANI_ONCE)
					iUsed = false;
				if(tProjInfo->AnimType == ANI_LOOP)
					fFrame = 0;
				if(tProjInfo->AnimType == ANI_PINGPONG) {
					iFrameDelta = !iFrameDelta;
					fFrame = (float)NumFrames-1;
				}
			}
		}
	}


	// Trails
	if(tProjInfo->Trail == TRL_SMOKE) {
		if(fExtra > 0.075f) {
			fExtra=0;
			SpawnEntity(ENT_SMOKE,0,vPosition,CVec(0,0),0,NULL);
		}
	}

	if(tProjInfo->Trail == TRL_CHEMSMOKE) {
		if(fExtra > 0.075f) {
			fExtra=0;
			SpawnEntity(ENT_CHEMSMOKE,0,vPosition,CVec(0,0),0,NULL);
		}
	}

	if(tProjInfo->Trail == TRL_DOOMSDAY) {
		if(fExtra > 0.05f) {
			fExtra=0;			
			SpawnEntity(ENT_DOOMSDAY,0,vPosition,vVelocity,0,NULL);
		}
	}

	if(tProjInfo->Trail == TRL_EXPLOSIVE) {
		if(fExtra > 0.05f) {
			fExtra=0;			
			SpawnEntity(ENT_EXPLOSION,10,vPosition,CVec(0,0),0,NULL);
		}
	}

	// Projectile trail
	iSpawnPrjTrl = false;
	if(tProjInfo->Trail == TRL_PROJECTILE) {
		if(tLX->fCurTime > fLastTrailProj) {
			fLastTrailProj = tLX->fCurTime + tProjInfo->PrjTrl_Delay;

			// Set the spawning to true so the upper layers of code (client) will spawn the projectiles
			iSpawnPrjTrl = true;
		}
	}


	// Hack!!!
	if(tProjInfo->Hit_Type == PJ_EXPLODE && tProjInfo->Type == PRJ_PIXEL) {
		uchar pf = map->GetPixelFlag((int)vPosition.GetX(), (int)vPosition.GetY());
		if(pf & PX_DIRT || pf & PX_ROCK) {
			return true;
		} else
			return false;
	}


	// Check for collisions
	return CheckCollision(dt, map, vOldPos, vVelocity);	
}


///////////////////
// Check for a collision
int CProjectile::CheckCollision(float dt, CMap *map, CVec pos, CVec vel)
{
	float maxspeed = 90;

	if(tProjInfo->Hit_Type == PJ_BOUNCE)
		maxspeed = 40;

	// If the projectile is going too fast, divide the speed by 2 and perform 2 collision checks
	if( VectorLength(vel) > maxspeed) {
		vel = vel / 2;

		if(CheckCollision(dt,map,pos,vel))
			return true;

		pos = pos + vel*dt;

		if(CheckCollision(dt,map,pos,vel))
			return true;

		return false;
	}

	pos = pos + vel*dt;



	// Check if it hit the terrain
	int mw = map->GetWidth();
	int mh = map->GetHeight();
	int w,h;
	int px,py,x,y;
	
	if(tProjInfo->Type == PRJ_PIXEL)
		w=h=1;

	w=h=2;

	px=(int)pos.GetX();
	py=(int)pos.GetY();

	CollisionSide = 0;
	int top,bottom,left,right;
	top=bottom=left=right=0;
	
	// Hit edges
	if(px-w<0 || py-h<0 || px+w>=mw || py+h>=mh) {

		// Check the collision side
		if(px-w<0) {
			px = w;
			CollisionSide |= COL_LEFT;
		}
		if(py-h<0) {
			py = h;
			CollisionSide |= COL_TOP;
		}
		if(px+w>=mw) {
			px = mw-w;
			CollisionSide |= COL_RIGHT;
		}
		if(py+h>=mh) {
			py = mh-h;
			CollisionSide |= COL_BOTTOM;
		}

		//vPosition = CVec((int)px,(int)py);

		return Collision(PX_ROCK);
	}


	for(y=py-h;y<=py+h;y++) {

		// Clipping means that it has collided
		if(y<0)	{
			CollisionSide |= COL_TOP;
			return Collision(PX_ROCK);
		}
		if(y>=mh) {
			CollisionSide |= COL_BOTTOM;
			return Collision(PX_ROCK);
		}

		
		uchar *pf = map->GetPixelFlags() + y*mw + px-w;

		for(x=px-w;x<=px+w;x++) {

			// Clipping
			if(x<0) {
				CollisionSide |= COL_LEFT;
				return Collision(PX_ROCK);
			}
			if(x>=mw) {
				CollisionSide |= COL_RIGHT;
				return Collision(PX_ROCK);
			}

			if(*pf & PX_DIRT || *pf & PX_ROCK) {
				if(y<py)
					top++;
				if(y>py)
					bottom++;
				if(x<px)
					left++;
				if(x>px)
					right++;

				//return Collision(*pf);
			}

			pf++;
		}
	}


	// Check for a collision
	if(top || bottom || left || right) {
		CollisionSide = 0;

		if(tProjInfo->Hit_Type == PJ_EXPLODE) {
			//vPosition = pos;
			return true;
		}

		
		// Bit of a hack
		if(tProjInfo->Hit_Type == PJ_BOUNCE)
			pos = vOldPos;
		else
			vPosition = pos;
		
		// Find the collision side
		if( (left>right || left>2) && left>1 && vVelocity.GetX() < 0) {
			if(tProjInfo->Hit_Type == PJ_BOUNCE)
				vPosition.SetX( pos.GetX() );
			CollisionSide |= COL_LEFT;
		}
		
		if( (right>left || right>2) && right>1 && vVelocity.GetX() > 0) {
			if(tProjInfo->Hit_Type == PJ_BOUNCE)
				vPosition.SetX( pos.GetX() );
			CollisionSide |= COL_RIGHT;
		}

		if(top>1 && vVelocity.GetY() < 0) {
			if(tProjInfo->Hit_Type == PJ_BOUNCE)
				vPosition.SetY( pos.GetY() );
			CollisionSide |= COL_TOP;
		}

		if(bottom>1 && vVelocity.GetY() > 0) {
			if(tProjInfo->Hit_Type == PJ_BOUNCE)
				vPosition.SetY( pos.GetY() );
			CollisionSide |= COL_BOTTOM;
		}		

		// If the velocity is too low, just stop me
		/*if(fabs(vVelocity.GetX()) < 2)
			vVelocity.SetX(0);
		if(fabs(vVelocity.GetY()) < 2)
			vVelocity.SetY(0);*/

		return true;
	}

	return false;
}


///////////////////
// Draw the projectile
void CProjectile::Draw(SDL_Surface *bmpDest, CViewport *view)
{
	int wx = view->GetWorldX();
	int wy = view->GetWorldY();
	int l = view->GetLeft();
	int t = view->GetTop();
	float framestep;

	int x=((int)vPosition.GetX()-wx)*2+l;
	int y=((int)vPosition.GetY()-wy)*2+t;

	// Clipping on the viewport
	if(x<l || x>l+view->GetVirtW())
		return;
	if(y<t || y>t+view->GetVirtH())
		return;

	if(tProjInfo->Type == PRJ_PIXEL)
		DrawRectFill(bmpDest,x-1,y-1,x+1,y+1,iColour);

	if(tProjInfo->Type == PRJ_IMAGE) {
		if(tProjInfo->bmpImage == NULL) {
			printf("Warning: Bad image on projectile\n");
			return;
		}

		// Spinning projectile only when moving
		if(tProjInfo->Rotating && (fabs(vVelocity.GetX()) > 1 || fabs(vVelocity.GetY()) > 1))
			framestep = fRotation / (float)tProjInfo->RotIncrement;
		else
			framestep = 0;

		// Directed in the direction the projectile is travelling
		if(tProjInfo->UseAngle) {
			CVec dir = vVelocity;
			float angle = (float)( -atan2(dir.GetX(),dir.GetY()) * (180.0f/PI) );
			float offset = 360.0f / (float)tProjInfo->AngleImages;

			if(angle < 0)
				angle+=360;
			if(angle > 360)
				angle-=360;

			if(angle == 360)
				angle=0;
		
			framestep = angle / offset;
		}

		// Special angle
		// Basically another way of organising the angles in images
		// Straight up is in the middle, rotating left goes left, rotating right goes right in terms
		// of image index's from the centre
		if(tProjInfo->UseSpecAngle) {
			CVec dir = vVelocity;
			float angle = (float)( -atan2(dir.GetX(),dir.GetY()) * (180.0f/PI) );
			int direct = 0;

			if(angle > 0)
				angle=180-angle;
			if(angle < 0) {
				angle=180+angle;
				direct = 1;
			}
			if(angle == 0)
				direct = 0;


			int num = (tProjInfo->AngleImages - 1) / 2;
			int middle = num*tProjInfo->bmpImage->h;
			if(direct == 0)
				// Left side
				framestep = (float)(151-angle) / 151 * (float)num;
			else {
				// Right side
				framestep = (float)angle / 151 * (float)num;
				framestep += num+1;
			}
		}

		if(tProjInfo->Animating)
			framestep = fFrame;

		int size = tProjInfo->bmpImage->h;
		int half = size/2;
		DrawImageAdv(bmpDest, tProjInfo->bmpImage, (int)framestep*size, 0, x-half, y-half, size,size);
	}
}


///////////////////
// Collision handling
//
// Returns true if we exploded
int CProjectile::Collision(uchar pf)
{
	
	return true;
}


///////////////////
// Bounce
void CProjectile::Bounce(float fCoeff)
{
	float x,y;
	x=y=1;

	float Bounce = fCoeff;

	/*if(firstbounce) {
		printf("Bounce; %d, %d, %d, %d\n",(CollisionSide & COL_TOP)==COL_TOP,
										  (CollisionSide & COL_BOTTOM)==COL_BOTTOM,
										  (CollisionSide & COL_LEFT)==COL_LEFT,
										  (CollisionSide & COL_RIGHT)==COL_RIGHT);
		firstbounce = false;
	}*/

	int w,h;
	w=h=3;

	CVec pos = vPosition;
	if(CollisionSide & COL_TOP) {
		x=Bounce; y=-Bounce;
		//vPosition.SetY( vOldPos.GetY() );
	}
	if(CollisionSide & COL_BOTTOM) {
		x=Bounce; y=-Bounce;
		//vPosition.SetY( vOldPos.GetY() );
	}

	if(CollisionSide & COL_LEFT) {
		x=-Bounce; y=Bounce;
		//vPosition.SetX( vOldPos.GetX() );
	}
	if(CollisionSide & COL_RIGHT) {
		x=-Bounce; y=Bounce;
		//vPosition.SetX( vOldPos.GetX() );
	}

	vVelocity = CVec(vVelocity.GetX()*x, vVelocity.GetY()*y);

	CVec dir = vVelocity;
	NormalizeVector(&dir);

	//vPosition = vPosition + dir*2;
}


///////////////////
// Check for collisions with worms
int CProjectile::CheckWormCollision(CWorm *worms)
{
	CWorm *w = worms;

	float divisions = 5;
	CVec dir = vPosition - vOldPos;
	float length = NormalizeVector(&dir);

	// Length must be at least 'divisions' in size so we do at least 1 check
	// So stationary projectiles also get checked (mines)
	length = MAX(length, divisions);

	// Go through at fixed positions
	for(float p=0; p<length; p+=divisions) {
		CVec pos = vOldPos + dir*p;

		int w = ProjWormColl(pos, worms);
		if( w >= 0)
			return w;
	}

	// No worms hit
	return -1;
}


///////////////////
// Lower level projectile-worm collision test
int CProjectile::ProjWormColl(CVec pos, CWorm *worms)
{
	int px = (int)pos.GetX();
	int py = (int)pos.GetY();
	int wx,wy;
	CWorm *w = worms;

	int wsize = 4;

	for(int i=0;i<MAX_WORMS;i++,w++) {
		if(!w->isUsed() || !w->getAlive())
			continue;

		wx = (int)w->getPos().GetX();
		wy = (int)w->getPos().GetY(); 

		// AABB - Point test
		if( abs(wx-px) < wsize && abs(wy-py) < wsize) {		

			CollisionSide = 0;

			// Calculate the side of the collision (obsolete??)
			if(px < wx-2)
				CollisionSide |= COL_LEFT;
			if(px > wx+2)
				CollisionSide |= COL_RIGHT;
			if(py < wy-2)
				CollisionSide |= COL_TOP;
			if(py > wy+2)
				CollisionSide |= COL_BOTTOM;

			return i;
		}		
	}

	// No worm was hit
	return -1;
}