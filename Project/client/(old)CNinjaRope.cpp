/////////////////////////////////////////
//
//                  LieroX
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Ninja rope class
// Created 6/2/02
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"


///////////////////
// Clear the ninja rope vars
void CNinjaRope::Clear(void)
{
	Released = false;
	HookShooting = false;
	HookAttached = false;
	//RopeLength = false;
}


///////////////////
// Shoot the fucka
void CNinjaRope::Shoot(CVec pos, CVec dir)
{
	Clear();

	Released = true;
	HookShooting = true;
	HookAttached = false;
	//RopeLength = 300;
	//RestLength = 20;

	HookPos = pos;
	HookDir = dir;
}


///////////////////
// Setup the details from the gamescript
void CNinjaRope::Setup(CGameScript *gs)
{
	assert( gs );

	RopeLength = gs->getRopeLength();
	RestLength = gs->getRestLength();
	Strength = gs->getStrength();
}


///////////////////
// Simulate the thing
void CNinjaRope::Simulate(float dt, CMap *map, CVec playerpos)
{
	if(!Released)
		return;

	float length;
	float speed = 250;
	int firsthit = !HookAttached;

	// Still flying in the air
	if(HookShooting) {

		HookPos = HookPos + HookDir*speed*dt;

		length = CalculateDistance(playerpos,HookPos);

		// Check if it's too long
		if(length > RopeLength)
			HookShooting = false;
	}
	// Failing
	if(!HookShooting && !HookAttached) {

		// Going towards the player
		/*length = CalculateDistance(playerpos,HookPos);
		if(length > RestLength) {
			HookDir = playerpos - HookPos;
			NormalizeVector(&HookDir);
		}

		HookPos = HookPos + HookDir*speed*dt;*/
		HookPos = HookPos + CVec(0,170*dt);
	}
	
	// Hack to see if the hook went out of the map
	if(HookPos.GetX() < 0 || HookPos.GetY() < 0 ||
	   HookPos.GetX() >= map->GetWidth() || HookPos.GetY() >= map->GetHeight()) {
		HookShooting = false;
		HookAttached = true;

		// Make the hook stay at an edge
		HookPos.SetX( MAX(0,HookPos.GetX()) );
		HookPos.SetY( MAX(0,HookPos.GetY()) );

		HookPos.SetX( MIN(map->GetWidth()-1,HookPos.GetX()) );
		HookPos.SetY( MIN(map->GetHeight()-1,HookPos.GetY()) );
	}


	// Check if the hook has hit anything on the map
	HookAttached = false;
	int px = map->GetPixelFlag((int)HookPos.GetX(),(int)HookPos.GetY());
	if(px & PX_ROCK || px & PX_DIRT) {
		HookShooting = false;
		HookAttached = true;

		if(px & PX_DIRT && firsthit) {
			Uint32 col = GetPixel(map->GetImage(),(int)HookPos.GetX(),(int)HookPos.GetY());
			SpawnEntity(ENT_PARTICLE,0,HookPos+CVec(0,2),CVec(GetRandomNum()*10,GetRandomNum()),col,NULL);
			SpawnEntity(ENT_PARTICLE,0,HookPos+CVec(0,2),CVec(GetRandomNum()*10,GetRandomNum()),col,NULL);
			SpawnEntity(ENT_PARTICLE,0,HookPos+CVec(0,2),CVec(GetRandomNum()*10,GetRandomNum()),col,NULL);
		}
	}
}


///////////////////
// Draw the thing
void CNinjaRope::Draw(SDL_Surface *bmpDest, CViewport *view, CVec ppos)
{
	if(!Released)
		return;

	int l = view->GetLeft();
	int t = view->GetTop();
	int wx = view->GetWorldX();
	int wy = view->GetWorldY();

	int hx = HookPos.GetX();
	int hy = HookPos.GetY();

	int px = ppos.GetX();
	int py = ppos.GetY();

	px = ((int)ppos.GetX()-wx)*2+l;
	py = ((int)ppos.GetY()-wy)*2+t;

	hx = (HookPos.GetX()-wx)*2+l;
	hy = (HookPos.GetY()-wy)*2+t;


	// Rope
	//DrawLine(bmpDest,px,py,hx,hy,SDL_MapRGB(bmpDest->format,159,79,0));
	px -= px % 2;
	py -= py % 2;
	hx -= hx % 2;
	hy -= hy % 2;
	DrawRope(bmpDest, hx,hy,px,py,SDL_MapRGB(bmpDest->format,159,79,0));


	// Check clipping against viewport
	if(hx>=l && hx<=l+view->GetVirtW() &&
	   hy>=t && hy<=t+view->GetVirtH()) {
		
		// Hook
		DrawImage(bmpDest,gfxGame.bmpHook,hx-2,hy-2);
	}

	// The clipping on the viewport is done in the line function
}


///////////////////
// Return the pulling force
CVec CNinjaRope::GetForce(CVec playerpos)
{
	if(!HookAttached)
		return CVec(0,0);

	float length = CalculateDistance(playerpos,HookPos);

	CVec dir;

	dir = playerpos-HookPos;
	NormalizeVector(&dir);

	float l = MIN(0,RestLength-length);

	if(length < RestLength)
		return CVec(0,0);

	// Make sure the pull isn't huge
	//l = MAX(-40,l);
	l = -Strength;

	dir = dir * l;//*Strength;

	return dir * 100;
}



///////////////////
// Write out the rope details to a bytestream
void CNinjaRope::write(CBytestream *bs)
{
	int type = ROP_NONE;

	if(HookShooting)
		type = ROP_SHOOTING;
	else if(HookAttached)
		type = ROP_HOOKED;
	else
		type = ROP_FALLING;

	bs->writeByte( type );

	// Position
	int x = (int)HookPos.GetX();
	int y = (int)HookPos.GetY();
	
	// Write out position of the hook
	bs->writeByte( x );
	bs->writeByte( (x>>8) | (y<<4) );
	bs->writeByte( (y>>4) );


	// Calculate the heading angle that the hook is travelling
	float heading = (float)-atan2(HookDir.GetX(),HookDir.GetY()) * (180.0f/PI);
	heading+=90;
	if(heading < 0)
		heading+=360;
	if(heading > 360)
		heading-=360;
	if(heading == 360)
		heading=0;

	// Write out the direction is shooting out
	if(type == ROP_SHOOTING) {
		// Convert angle to fixed 256
		int a = (int)(256*heading/360)&255;
		bs->writeByte( a );
	}
}


///////////////////
// Read rope details from a bytestream
void CNinjaRope::read(CBytestream *bs)
{
	int type = bs->readByte();
	Released = true;

	switch(type) {
		case ROP_SHOOTING:
			HookShooting = true;
			HookAttached = false;
			break;
		case ROP_HOOKED:
			HookShooting = false;
			HookAttached = true;
			break;
		case ROP_FALLING:
			HookShooting = false;
			HookAttached = false;
			break;
	}

	// Position
	uchar pos[3];
	for(int i=0;i<3;i++)
		pos[i] = bs->readByte();

	int x = pos[0] + ((pos[1]&15)<<8);
	int y = (pos[1]>>4) + (pos[2]<<4);

	HookPos.SetX( (float)x );
	HookPos.SetY( (float)y );

	// Angle
	if(type == ROP_SHOOTING) {
		int a = bs->readByte();
		float angle = 360*a/256;
		GetAngles(angle, &HookDir, NULL);
	}

}