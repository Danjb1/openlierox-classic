/////////////////////////////////////////
//
//   Auxiliary Software class library
//
/////////////////////////////////////////


// Input class
// Created 10/12/01
// By Jason Boettcher


#include "defs.h"

keys_t Keys[] = {
	{ "", 0 },
	{ "a", SDLK_a },
	{ "b", SDLK_b },
	{ "c", SDLK_c },
	{ "d", SDLK_d },
	{ "e", SDLK_e },
	{ "f", SDLK_f },
	{ "g", SDLK_g },
	{ "h", SDLK_h },
	{ "i", SDLK_i },
	{ "j", SDLK_j },
	{ "k", SDLK_k },
	{ "l", SDLK_l },
	{ "m", SDLK_m },
	{ "n", SDLK_n },
	{ "o", SDLK_o },
	{ "p", SDLK_p },
	{ "q", SDLK_q },
	{ "r", SDLK_r },
	{ "s", SDLK_s },
	{ "t", SDLK_t },
	{ "u", SDLK_u },
	{ "v", SDLK_v },
	{ "w", SDLK_w },
	{ "x", SDLK_x },
	{ "y", SDLK_y },
	{ "z", SDLK_z },
	{ "1", SDLK_1 },
	{ "2", SDLK_2 },
	{ "3", SDLK_3 },
	{ "4", SDLK_4 },
	{ "5", SDLK_5 },
	{ "6", SDLK_6 },
	{ "7", SDLK_7 },
	{ "8", SDLK_8 },
	{ "9", SDLK_9 },
	{ "0", SDLK_0 },
	{ "kp 0", SDLK_KP0},
	{ "kp 1", SDLK_KP1},
	{ "kp 2", SDLK_KP2},
	{ "kp 3", SDLK_KP3},
	{ "kp 4", SDLK_KP4},
	{ "kp 5", SDLK_KP5},
	{ "kp 6", SDLK_KP6},
	{ "kp 7", SDLK_KP7},
	{ "kp 8", SDLK_KP8},
	{ "kp 9", SDLK_KP9},
	{ "kp .",SDLK_KP_PERIOD },
	{ "kp enter",SDLK_KP_ENTER },
	{ "kp /",SDLK_KP_DIVIDE },
	{ "kp *",SDLK_KP_MULTIPLY },
	{ "kp -",SDLK_KP_MINUS },
	{ "kp +",SDLK_KP_PLUS },
	{ "kp =",SDLK_KP_EQUALS },
	{ "[",SDLK_LEFTBRACKET },
	{ "]",SDLK_RIGHTBRACKET },
	{ "\\",SDLK_BACKSLASH },
	{ "/",SDLK_SLASH},
	{ ":", SDLK_COLON},
	{ ";", SDLK_SEMICOLON},		
	{ "<", SDLK_LESS},
	{ "=", SDLK_EQUALS},
	{ ">", SDLK_GREATER},
	{ "?", SDLK_QUESTION},
	{ "-", SDLK_MINUS},
	{ "'", SDLK_QUOTE},
	{ ",", SDLK_COMMA},
	{ ".", SDLK_PERIOD},
	{ "tab", SDLK_TAB},
	{ "space",SDLK_SPACE },
	{ "up",SDLK_UP },
	{ "down",SDLK_DOWN },
	{ "right",SDLK_RIGHT },
	{ "left",SDLK_LEFT },	
	{ "lctrl",SDLK_LCTRL },
	{ "lshift",SDLK_LSHIFT },
	{ "lalt",SDLK_LALT },
	{ "rctrl",SDLK_RCTRL },
	{ "rshift",SDLK_RSHIFT },
	{ "ralt",SDLK_RALT },
	{ "insert", SDLK_INSERT},
	{ "home", SDLK_HOME},
	{ "pg up", SDLK_PAGEUP},
	{ "end", SDLK_END},
	{ "pg Dn", SDLK_PAGEDOWN},
	{ "delete", SDLK_DELETE},
	{ "num lk", SDLK_NUMLOCK},
	{ "caps", SDLK_CAPSLOCK},
	{ "scr lk", SDLK_SCROLLOCK},
	{ "F1", SDLK_F1 },
	{ "F2", SDLK_F2 },
	{ "F3", SDLK_F3 },
	{ "F4", SDLK_F4 },
	{ "F5", SDLK_F5 },
	{ "F6", SDLK_F6 },
	{ "F7", SDLK_F7 },
	{ "F8", SDLK_F8 },
	{ "F9", SDLK_F9 },
	{ "F10", SDLK_F10 },
	{ "F11", SDLK_F11 },
	{ "F12", SDLK_F12 }
	};


joystick_t Joysticks[] = {
	{ "joy1_up",JOY_UP,0},
	{ "joy1_down",JOY_DOWN,0},
	{ "joy1_left",JOY_LEFT,0},
	{ "joy1_right",JOY_RIGHT,0},
	{ "joy1_but1",JOY_BUTTON,0},
	{ "joy1_but2",JOY_BUTTON,1},
	{ "joy1_but3",JOY_BUTTON,2},
	{ "joy1_but4",JOY_BUTTON,3},
	{ "joy1_but5",JOY_BUTTON,4},
	{ "joy1_but6",JOY_BUTTON,5},
	{ "joy1_but7",JOY_BUTTON,6},
	{ "joy1_but8",JOY_BUTTON,7},
	{ "joy1_but9",JOY_BUTTON,8},
	{ "joy2_up",JOY_UP,0},
	{ "joy2_down",JOY_DOWN,0},
	{ "joy2_left",JOY_LEFT,0},
	{ "joy2_right",JOY_RIGHT,0},
	{ "joy2_but1",JOY_BUTTON,0},
	{ "joy2_but2",JOY_BUTTON,1},
	{ "joy2_but3",JOY_BUTTON,2},
	{ "joy2_but4",JOY_BUTTON,3},
	{ "joy2_but5",JOY_BUTTON,4},
	{ "joy2_but6",JOY_BUTTON,5},
	{ "joy2_but7",JOY_BUTTON,6},
	{ "joy2_but8",JOY_BUTTON,7},
	{ "joy2_but9",JOY_BUTTON,8},
};

SDL_Joystick *joy1,*joy2;


///////////////////
// Load the input from a config file
int CInput::Load(char *name, char *section)
{
	char	string[64];

	Down = false;

	if(!ReadString(GetConfigFile(),section,name,string,""))
		return false;

	return Setup(string);
}


///////////////////
// Waits for any input (used in a loop)
int CInput::Wait(char *strText)
{
	mouse_t *Mouse = GetMouse();
	keyboard_t *kb = GetKeyboard();
	int n,i;

	// First check the mouse
	for(n=1;n<4;n++) {
		i=n;
		if(Mouse->Up & SDL_BUTTON(n)) {
			// Swap rmb id wih mmb (mouse buttons)
			if(n==2) i=3;
			if(n==3) i=2;
			sprintf(strText,"ms%d",i);
			return true;
		}
	}
	
	// Keyboard
	for(n=0;n<sizeof(Keys) / sizeof(keys_t);n++) {
		if(kb->KeyUp[Keys[n].value]) {
			strcpy(strText,Keys[n].text);
			return true;
		}
	}



	return false;
}


///////////////////
// Setup
int CInput::Setup(char *string)
{
	int n;

	Down = false;

	// Check if it's a mouse
	if(stricmp(string,"ms1") == 0) {
		Type = INP_MOUSE;
		Data = 1;
		return true;
	}
	if(stricmp(string,"ms2") == 0) {
		Type = INP_MOUSE;
		Data = 3;
		return true;
	}
	if(stricmp(string,"ms3") == 0) {
		Type = INP_MOUSE;
		Data = 2;
		return true;
	}


	// Check if it's a joystick #1
	if(strncmp(string,"joy1_",5) == 0) {
		Type = INP_JOYSTICK1;
		Data = 0;

		// Make sure there is a joystick present
		if(SDL_NumJoysticks()<=0) {
			SetError("Could not open joystick1");
			return false;
		}

		// Open the joystick if it hasn't been already opened
		if(!SDL_JoystickOpened(0)) {
			joy1 = SDL_JoystickOpen(0);
			if(!joy1) {
				SetError("Could not open joystick1");
				return false;
			}
		}
   

		// Go through the joystick list
		for(n=0;n<sizeof(Joysticks) / sizeof(joystick_t);n++) {
			if(stricmp(Joysticks[n].text,string) == 0) {
				Data = Joysticks[n].value;
				Extra = Joysticks[n].extra;
				return true;
			}
		}
	}

	// Check if it's a joystick #2
	if(strncmp(string,"joy2_",5) == 0) {
		Type = INP_JOYSTICK2;
		Data = 0;

		// Make sure there is a joystick present
		if(SDL_NumJoysticks()<=1)
			return false;

		// Open the joystick if it hasn't been already opened
		if(!SDL_JoystickOpened(1)) {
			joy2 = SDL_JoystickOpen(1);
			if(!joy1) {
				SetError("Could not open joystick2");
				return false;
			}
		}

		// Go through the joystick list
		for(n=0;n<sizeof(Joysticks) / sizeof(joystick_t);n++) {
			if(stricmp(Joysticks[n].text,string) == 0) {
				Data = Joysticks[n].value;
				Extra = Joysticks[n].extra;
				return true;
			}
		}
	}


	// Must be a keyboard character
	Type = INP_KEYBOARD;
	Data = 0;

	// Go through the key list checking with piece of text it was
	for(n=0;n<sizeof(Keys) / sizeof(keys_t);n++) {
		if(stricmp(Keys[n].text,string) == 0) {
			Data = Keys[n].value;
			return true;
		}
	}

	return false;
}


///////////////////
// Returns if the input has just been released
int CInput::isUp(void)
{
	keyboard_t	*Keyb = GetKeyboard();
	mouse_t		*Mouse = GetMouse();
	

	switch(Type) {
		
		// Keyboard
		case INP_KEYBOARD:	
			if(Keyb->KeyUp[Data])
				return true;
			break;
		
		// Mouse
		case INP_MOUSE:	
			// TOOD: Calculate mouse_up for ALL the buttons
			if(Mouse->Up)
				return true;
			break;
		
		// Joystick
		// TODO:

	}

	return false;
}


///////////////////
// Returns if the input is down
int CInput::isDown(void)
{
	keyboard_t	*Keyb = GetKeyboard();
	mouse_t		*Mouse = GetMouse();
	SDL_Joystick *joy;
	int val;
	
	switch(Type) {

		// Keyboard
		case INP_KEYBOARD:
			if(Keyb->keys[Data])
				return true;
			break;

		// Mouse
		case INP_MOUSE:
			if(Mouse->Button & SDL_BUTTON(Data))
				return true;
			break;

		// Joystick
		case INP_JOYSTICK1:
		case INP_JOYSTICK2:
			joy = joy1;
			if(Type == INP_JOYSTICK2)
				joy = joy2;
	
			SDL_JoystickUpdate();

			switch(Data) {
				case JOY_UP:
					val = SDL_JoystickGetAxis(joy,1);
					if(val < -3200)
						return true;
					break;
				case JOY_DOWN:
					val = SDL_JoystickGetAxis(joy,1);
					if(val > 3200)
						return true;
					break;
				case JOY_LEFT:
					val = SDL_JoystickGetAxis(joy,0);
					if(val < -3200)
						return true;
					break;
				case JOY_RIGHT:
					val = SDL_JoystickGetAxis(joy,0);
					if(val > 3200)
						return true;
					break;
				case JOY_BUTTON:
					if(SDL_JoystickGetButton(joy,Extra))
						return true;
					break;
			}
			break;

	}

	return false;
}


///////////////////
// Returns if the input was pushed down once
int CInput::isDownOnce(void)
{
	int d = isDown();	

	if(isDown()) {
		if(!Down) {		
			Down = true;
			return true;
		}
	}
	else
		Down = false;

	return false;
}


///////////////////
// Clear the up state of the input device
void CInput::ClearUpState(void)
{
	keyboard_t	*Keyb = GetKeyboard();
	mouse_t		*Mouse = GetMouse();
	

	switch(Type) {
		
		// Keyboard
		case INP_KEYBOARD:	
			Keyb->KeyUp[Data] = false;
			break;
		
		// Mouse
		case INP_MOUSE:	
			// TOOD: Calculate mouse_up for ALL the buttons
			Mouse->Up = false;
			break;
		
		// Joystick
		// TODO:
	}
}