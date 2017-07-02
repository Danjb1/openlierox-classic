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


#ifndef __CTEXTBOX_H__
#define __CTEXTBOX_H__


#define		MAX_TEXTLENGTH		64

// Event types
enum {
	TXT_NONE=-1,
	TXT_CHANGE=0,
	TXT_MOUSEOVER,
	TXT_ENTER
};


// Messages
enum {
	TXM_GETTEXT=0,
	TXM_SETTEXT,
	TXM_SETFLAGS,
	TXM_SETMAX
};


// Flags
#define		TXF_PASSWORD	0x0001


class CTextbox : public CWidget {
public:
	// Constructor
	CTextbox() {
		Create();
		iType = wid_Textbox;
		iFlags = 0;
	}


private:
	// Attributes

	char	sText[MAX_TEXTLENGTH];

	int		iCurpos;
	int		iLength;
	int		iFlags;

	int		iMax;

	int		iHolding;
	float	fTimePushed;
	char	iLastchar;


public:
	// Methods

	void	Create(void);
	void	Destroy(void) { }

	//These events return an event id, otherwise they return -1
	int		MouseOver(mouse_t *tMouse)			{ return TXT_MOUSEOVER; }
	int		MouseUp(mouse_t *tMouse, int nDown)		{ return TXT_NONE; }
	int		MouseDown(mouse_t *tMouse, int nDown)	{ return TXT_MOUSEOVER; }
	int		KeyDown(char c);
	int		KeyUp(char c);

	void	Draw(SDL_Surface *bmpDest);

	int		SendMessage(int iMsg, DWORD Param1, DWORD Param2);

	void	Backspace(void);
	void	Delete(void);
	void	Insert(char c);

	char	*getText(void)						{ return sText; }
	void	setText(char *buf)					{ strcpy(sText,buf); iCurpos=iLength=strlen(buf); }

    void    PasteText(void);


};




#endif  //  __CTEXTBOX_H__