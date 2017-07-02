/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Custom browser class
// Created 7/6/02
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"
#include "Menu.h"


///////////////////
// The create event
void CBrowser::Create(void)
{
	// Destroy any previous data
	Destroy();

	iLines = -1;
	iUseScroll = false;

	iProperties = 0;
	iTextColour = 0xffff;

	tObjects = NULL;
	
	iPos = iLength = 0;
	sData = NULL;


	cScrollbar.Create();

	cScrollbar.Setup(0, iX+iWidth-16, iY+2, 14, iHeight-3);
	cScrollbar.setMin(0);
	cScrollbar.setValue(0);
	cScrollbar.setItemsperbox(iHeight);
	cScrollbar.setMax(0);
}


///////////////////
// This widget is send a message
int CBrowser::SendMessage(int iMsg, DWORD Param1, DWORD Param2)
{
	switch(iMsg) {

		// Load a file
		case BRM_LOAD:
			return Load( (char *)Param1 );

	}

	return 0;
}


///////////////////
// Load the cmht file
int CBrowser::Load(char *sFilename)
{
	FILE *fp;
	
	// Open the file
	fp = fopen(sFilename,"rb");
	if(fp == NULL)
		return false;

	fseek(fp,0,SEEK_END);
	iLength = ftell(fp);
	fseek(fp,0,SEEK_SET);

	// Allocate room for the data
	sData = new char[iLength];
	if(sData == NULL) {
		fclose(fp);
		return false;
	}

	// Load the data
	fread(sData,sizeof(char),iLength,fp);
	fclose(fp);


	// Go through the file
	iPos=0;
	iLines=-1;
	while(iPos<iLength) {
		ReadObject();
		iPos++;
	}


	// Free the data
	if(sData) {
		delete[] sData;
		sData = NULL;
	}
	
	return true;
}


///////////////////
// Destroy function
void CBrowser::Destroy(void)
{
	// Go through freeing each object
	ht_object_t *obj = tObjects;
	ht_object_t *o;

	for(; obj ; obj=o) {
		o = obj->tNext;
		
		if(obj->strText) {
			delete[] obj->strText;
			obj->strText = NULL;
		}
		
		if(obj)
			delete obj;
	}
	
	tObjects = NULL;
}


///////////////////
// Read an object
void CBrowser::ReadObject(void)
{
	// Check first character
	char c = sData[iPos];

	// Comment
	if(c == '/' && sData[iPos+1] == '/') {
		// Read until the end of the line
		ReadNewline();
		return;
	}

	// Tag
	if(c == '<') {
		ReadTag();
		return;
	}

	// Newline
	if(c == '\n' || c == '\r') {
		return;
	}

	// Normal text
	ReadText();
}


///////////////////
// Set the position to the next line
void CBrowser::ReadNewline(void)
{
	while(iPos < iLength) {
		if(sData[iPos] == '\n') {
			iPos--;
			return;
		}
		iPos++;
	}
}


///////////////////
// Read a tag
void CBrowser::ReadTag(void)
{
	char sName[32];
	char sVal[32];
	int i = 0;
	int val = 0;
	int end = false;
	
	sName[0] = 0;
	sVal[0] = 0;

	// Tag list
	char	*sTags[] = {"b","u","shadow","box","colour","tab","stab","nl","line"};
	int		iNumTags = 9;

	iPos++;

	// Check if it's an end tag
	if(sData[iPos] == '/') {
		end=true;
		iPos++;
	}

	// Read the tag name until a space or an arrow
	while(iPos < iLength) {
		if(sData[iPos] == ' ' || sData[iPos] == '>') {
			sName[i] = '\0';
			break;
		}

		sName[i++] = sData[iPos++];
	}

			
	// Some tags contain a value (and not the end tags)
	if(stricmp(sName,"tab") == 0 ||
		stricmp(sName,"stab") == 0 ||
	   stricmp(sName,"colour") == 0 ||
	   stricmp(sName,"box") == 0 &&
	   !end) {
		
		iPos++;
		i=0;

		// Read the value
		while(iPos < iLength) {
			if(sData[iPos] == ' ' || sData[iPos] == '>') {
				sVal[i] = '\0';
				break;
			}

			sVal[i++] = sData[iPos++];
		}
	}

	// Find the tag id
	int id = -1;
	for(i=0;i<iNumTags;i++) {
		if(stricmp(sTags[i],sName) == 0) {
			id = i+HTO_BOLD;
			break;
		}
	}

	if(id == -1) {
		// Error
		return;
	}


	// Add the object to the list
	AddObject(sName,sVal,id,end);
}


///////////////////
// Read text as an object
void CBrowser::ReadText(void)
{
	char	sText[4096];
	int i=0;

	// Read the text until a tag, or a newline
	while(iPos < iLength) {
		if(sData[iPos] == '<' || sData[iPos] == '\n') {
			iPos--;
			break;
		}

		sText[i++] = sData[iPos++];

		if(i>=4095)
			break;
	}
	sText[i] = '\0';

	AddObject(sText,"",HTO_TEXT,0);
}


///////////////////
// Add an object to the list
void CBrowser::AddObject(char *sText, char *sVal, int iType, int iEnd)
{
	ht_object_t *obj;
	int r,g,b;

	// Allocate a new object
	obj = new ht_object_t;
	if(obj == NULL) {
		// Out of memory
		return;
	}

	// Allocate room for the text
	obj->strText = new char[strlen(sText)+1];
	if(obj->strText == NULL) {
		// Out of memory
		return;
	}

	// Set the properties
	strcpy(obj->strText,sText);
	obj->iType = iType;
	obj->iEnd = iEnd;
	obj->tNext = NULL;

	// Values get translated differently for different types of objects
	// End tags don't get translated
	if(!iEnd) {
		switch(iType) {

			// Normal integer
			case HTO_TAB:
			case HTO_STAB:
				obj->iValue = atoi(sVal);
				break;

			// Triple value
			case HTO_COLOUR:
			case HTO_BOX:
				char *tok = strtok(sVal,",");
				if(tok)	r = atoi(tok);	tok = strtok(NULL,",");
				if(tok)	g = atoi(tok);  tok = strtok(NULL,",");
				if(tok)	b = atoi(tok);

				obj->iValue = MakeColour(r,g,b);
				break;
		}
	}


	// Add this object to the list
	if(tObjects) {
		ht_object_t *o = tObjects;
		for(;o; o = o->tNext) {
			if(o->tNext == NULL) {
				o->tNext = obj;
				break;
			}
		}
	} else
		tObjects = obj;
}



///////////////////
// Mouse down event
int CBrowser::MouseDown(mouse_t *tMouse, int nDown)
{
	if(iUseScroll && tMouse->X > iX+iWidth-20)
		return cScrollbar.MouseDown(tMouse, nDown);

	return BRW_NONE;
}


///////////////////
// Mouse over event
int CBrowser::MouseOver(mouse_t *tMouse)
{
	if(iUseScroll && tMouse->X > iX+iWidth-20)
		return cScrollbar.MouseOver(tMouse);

	return BRW_NONE;
}


///////////////////
// Render the browser
void CBrowser::Draw(SDL_Surface *bmpDest)
{
	int x,y,s,p,w,c;
	ht_object_t *obj = tObjects;
	CFont *fnt = &tLX->cFont;
	char buf[64];
	int lcount = 0;

	DrawRectFill(bmpDest, iX+1, iY+1, iX+iWidth-1, iY+iHeight-1, 0xffff);
	
	Menu_DrawBoxInset(bmpDest, iX, iY, iX+iWidth, iY+iHeight);

	// Setup the clipping rectangle
	SDL_Rect r;
	r.x = iX+2;
	r.y = iY+2;
	r.w = iWidth-3;
	r.h = iHeight-3;
	SDL_SetClipRect(bmpDest,&r);


	// 10 pixel margin
	int Margin = 6;
	x=iX + Margin;
	y=iY + Margin - cScrollbar.getValue();

	int RightWidth = iX + iWidth - Margin;
	if(iUseScroll)
		RightWidth -= 14;


	iProperties=0;
	iTextColour = 0;

	if(iUseScroll)
		cScrollbar.Draw(bmpDest);

	// Go through rendering each object
	for(;obj;obj=obj->tNext) {

		if(y > iY+iHeight+Margin && iLines>=0)
			break;

		switch(obj->iType) {

			// Property - Bold
			case HTO_BOLD:
				if(obj->iEnd)
					iProperties &= ~PRP_BOLD;
				else
					iProperties |= PRP_BOLD;
				break;

			// Property - Underline
			case HTO_UNDERLINE:
				if(obj->iEnd)
					iProperties &= ~PRP_UNDERLINE;
				else
					iProperties |= PRP_UNDERLINE;
				break;

			// Property - Shadow
			case HTO_SHADOW:
				if(obj->iEnd)
					iProperties &= ~PRP_SHADOW;
				else
					iProperties |= PRP_SHADOW;
				break;

			// Colour
			case HTO_COLOUR:
				iTextColour = obj->iValue;
				break;

			// Tab
			case HTO_TAB:
				s =	fnt->GetWidth(" ");
				p = x + (obj->iValue*4)*s;
				p/=4;
				p*=4;
				x = p;
				break;

			// Start line Tab
			case HTO_STAB:
				s =	fnt->GetWidth(" ");
				p = (obj->iValue*4)*s;
				p/=4;
				p*=4;
				x = iX+p;
				break;

			// Line
			case HTO_LINE:
				if(y > iY-fnt->GetHeight() && iLines>=0) {
					DrawHLine(bmpDest,iX+Margin,RightWidth,y,MakeColour(28,36,47));
					DrawHLine(bmpDest,iX+Margin,RightWidth,y+1,MakeColour(50,65,82));
				}
				break;

			// Box
			case HTO_BOX:
				if(y > iY-fnt->GetHeight() && iLines>=0)
					DrawRectFill(bmpDest,iX+Margin,y,RightWidth,y+fnt->GetHeight(),obj->iValue);
				break;

			// Newline
			case HTO_NEWLINE:
				x = iX + Margin;
				y += fnt->GetHeight();
				lcount++;
				break;
			

			// Text
			case HTO_TEXT:

				// Go through each word in the text
				// If a word is going to be over the window border, move the word down a line and
				// to the start

				p=0;
				s = strlen(obj->strText);
				while(p<s) {

					// Go through until a space
					for(c=p;c<s;c++) {
						if(obj->strText[c] == ' ') {
							c++;
							break;
						}
					}
					strncpy(buf,obj->strText+p,c-p);
					buf[c-p]='\0';
					p=c;

					w = fnt->GetWidth(buf);
					if(x+w > RightWidth) {
						// Drop down a line
						x = iX+Margin;
						y += fnt->GetHeight();
						lcount++;
					}

					// Make sure it's within the window
					if(y > iY-fnt->GetHeight() && iLines>=0) {

						// Draw the properties
						if(iProperties & PRP_SHADOW)
							fnt->Draw(bmpDest,x+1,y+1,MakeColour(200,200,200),"%s",buf);					
						if(iProperties & PRP_BOLD)
							fnt->Draw(bmpDest,x+1,y+1,iTextColour,"%s",buf);
						if(iProperties & PRP_UNDERLINE)
							DrawHLine(bmpDest,x,x+w,y+fnt->GetHeight()-1,iTextColour);
				
						// Draw the text
						fnt->Draw(bmpDest,x,y,iTextColour,"%s",buf);
					}
				
					x+=w;
				}
				break;
		}
	}


	// If this is the first render, set the scrollbar to show the number of lines
	if(iLines == -1) {
		cScrollbar.setMax((lcount+1) * fnt->GetHeight() + Margin);
		iLines = lcount;

		if(cScrollbar.getMax() > iHeight)
			iUseScroll = true;

		//printl("Browser size: %d",iHeight);
		//printl("Page size: %d",lcount * fnt->GetHeight());
	}


	// Reset the clipping rectangle to full size
	SDL_SetClipRect(bmpDest,NULL);
}