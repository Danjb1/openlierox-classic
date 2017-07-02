/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Text box
// Created 30/6/02
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"
#include "Menu.h"


///////////////////
// Create the text box
void CTextbox::Create(void)
{
	iCurpos = 0;
	iLength = 0;
	sText[0] = 0;
	iMax = MAX_TEXTLENGTH;
	iHolding = false;
	fTimePushed = -9999;
	iLastchar = -1;
}


///////////////////
// Draw the text box
void CTextbox::Draw(SDL_Surface *bmpDest)
{
	char buf[MAX_TEXTLENGTH];
	char text[MAX_TEXTLENGTH];

	strcpy(text, sText);

    Menu_redrawBufferRect(iX,iY, iWidth,iHeight);
	Menu_DrawBoxInset(bmpDest, iX, iY, iX+iWidth, iY+iHeight);

	if(iFlags & TXF_PASSWORD) {

		// Draw astericks for password
		for(int i=0;i<strlen(sText);i++)
			text[i]='*';
		text[i+1] = '\0';
	}

	tLX->cFont.Draw(bmpDest, iX+3, iY+3, 0xffff, text);

	int x = 0;
	if(iFocused) {
		if(iCurpos)
			strncpy(buf,text,iCurpos);
		buf[iCurpos] = '\0';
		x = tLX->cFont.GetWidth(buf);

		DrawVLine(bmpDest, iY+3, iY+iHeight-3, iX+x+3,MakeColour(0,100,150));
	}
}


///////////////////
// Keydown event
int CTextbox::KeyDown(char c)
{
	keyboard_t *kb = GetKeyboard();

    if(c == 0)
        return -1;

	if(iHolding) {
		if(iLastchar != c)
			iHolding = false;
		else {
			if(tLX->fCurTime - fTimePushed < 0.25f)
				return TXT_NONE;
		}
	}

	if(!iHolding) {
		iHolding = true;
		fTimePushed = tLX->fCurTime;
	}

	iLastchar = c;


	// Backspace
	if(c == '\b') {
		Backspace();
		return TXT_CHANGE;
	}

	// Enter
	if(c == '\r') {
		return TXT_ENTER;
	}

	
	// Left arrow
	if(kb->KeyUp[SDLK_LEFT]) {
		if(iCurpos > 0)
			iCurpos--;
		return TXT_NONE;
	}

	// Right arrow
	if(kb->KeyUp[SDLK_RIGHT]) {
		if(iCurpos < iLength)
			iCurpos++;
		return TXT_NONE;
	}

	// Home
	if(kb->KeyUp[SDLK_HOME]) {
		iCurpos=0;
		return TXT_NONE;
	}

	// End
	if(kb->KeyUp[SDLK_END]) {
		iCurpos = iLength;
		return TXT_NONE;
	}

	// Delete
	if(kb->KeyUp[SDLK_DELETE]) {
		Delete();
		return TXT_CHANGE;
	}

    // Ctrl-v (paste)
    if( c == 22 ) {
        PasteText();
        return TXT_CHANGE;
    }

	// Insert character
	Insert(c);
	
	return TXT_CHANGE;
}


///////////////////
// Keyup event
int CTextbox::KeyUp(char c)
{
	iHolding = false;
	
	return TXT_NONE;
}


///////////////////
// Backspace
void CTextbox::Backspace(void)
{
	if(iCurpos<=0)
		return;

	memmove(sText+iCurpos-1,sText+iCurpos,iLength-iCurpos+1);

	iCurpos--;
	iLength--;
}


///////////////////
// Delete
void CTextbox::Delete(void)
{
	if(iCurpos >= iLength)
		return;

	memmove(sText+iCurpos,sText+iCurpos+1,iLength-iCurpos+1);
	
	iLength--;
}


///////////////////
// Insert a character
void CTextbox::Insert(char c)
{
	// Check for the max
	if(iLength >= iMax-2)
		return;

    // If the text size is greater than the textbox size, don't insert any more characters
    if(tLX->cFont.GetWidth(sText) > iWidth-tLX->cFont.GetWidth("W"))
        return;

	if(c==0)
		return;

	memmove(sText+iCurpos+1,sText+iCurpos,iLength-iCurpos+1);
	
	sText[iCurpos++] = c;
	sText[++iLength] = '\0';
}


///////////////////
// This widget is send a message
int CTextbox::SendMessage(int iMsg, DWORD Param1, DWORD Param2)
{
	char *p;

	switch(iMsg) {

		// Get the text
		case TXM_GETTEXT:
			strncpy((char *)Param1, sText, Param2);
			p = (char *)Param1;
			p[Param2-1] = '\0';
			break;

		// Set the text
		case TXM_SETTEXT:
			setText( (char *)Param1 );
			break;

		// Set some flags
		case TXM_SETFLAGS:
			iFlags = Param1;
			break;

		// Set the max length
		case TXM_SETMAX:
			iMax = Param1;
			break;

	}

	return 0;
}


///////////////////
// Paste some text from the clipboard
void CTextbox::PasteText(void)
{
    char text[MAX_TEXTLENGTH];

    int length = GetClipboardText(text, MAX_TEXTLENGTH-1);

    if(length > 0 ) {
        // Insert the text
        for( int i=0; i<length; i++)
            Insert( text[i] );
    }
}