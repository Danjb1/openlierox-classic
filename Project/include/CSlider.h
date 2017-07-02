/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Slider
// Created 30/6/02
// Jason Boettcher


#ifndef __CSLIDER_H__
#define __CSLIDER_H__


// Slider events
enum {
	SLD_NONE=-1,
	SLD_CHANGE=0
};


// Slider messages
enum {
	SLM_GETVALUE=0,
	SLM_SETVALUE
};


class CSlider : public CWidget {
public:
	// Constructor
	CSlider(int max) {
		Create();
		iMax = max;
		iType = wid_Slider;
	}


private:
	// Attributes

	int		iValue;
	int		iMax;


public:
	// Methods

	void	Create(void) { iValue=0; }
	void	Destroy(void) { }

	//These events return an event id, otherwise they return -1
	int		MouseOver(mouse_t *tMouse)			{ return SLD_NONE; }
	int		MouseUp(mouse_t *tMouse, int nDown)		{ return SLD_NONE; }
	int		MouseDown(mouse_t *tMouse, int nDown);
	int		KeyDown(char c)						{ return SLD_NONE; }
	int		KeyUp(char c)						{ return SLD_NONE; }

	void	Draw(SDL_Surface *bmpDest);

	int		SendMessage(int iMsg, DWORD Param1, DWORD Param2);

	int		getValue(void)						{ return iValue; }
	void	setValue(int v)						{ iValue = v; }
};



#endif  //  __CSLIDER_H__