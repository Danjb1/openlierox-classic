/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// GUI Layout class
// Created 5/6/02
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"
#include "Menu.h"


///////////////////
// Initialize the layout
void CGuiLayout::Initialize(void)
{
	Shutdown();

	cWidgets = NULL;
	tEvent = new gui_event_t;
	cFocused = NULL;

	// Reset mouse repeats
	nMouseButtons = 0;
	for(int i=0; i<3; i++)
		fMouseNext[i] = -9999;

}


///////////////////
// Add a widget to the gui layout
void CGuiLayout::Add(CWidget *widget, int id, int x, int y, int w, int h)
{
	widget->Setup(id, x, y, w, h);
	widget->Create();

	// Link the widget in
	widget->setPrev(NULL);
	widget->setNext(cWidgets);

	if(cWidgets)
		cWidgets->setPrev(widget);

	cWidgets = widget;
}


///////////////////
// Remove a widget
void CGuiLayout::removeWidget(int id)
{
    CWidget *w = getWidget(id);
    if( !w )
        return;

    // If this is the focused widget, set focused to null
    if(cFocused) {
        if(w->getID() == cFocused->getID())
            cFocused = NULL;
    }

    // Unlink the widget
    if( w->getPrev() )
        w->getPrev()->setNext( w->getNext() );
    else
        cWidgets = w->getNext();

    if( w->getNext() )
        w->getNext()->setPrev( w->getPrev() );

    // Free it
    w->Destroy();
    delete w;
}


///////////////////
// Shutdown the gui layout
void CGuiLayout::Shutdown(void)
{
	CWidget *w,*wid;

	for(w=cWidgets ; w ; w=wid) {		
		wid = w->getNext();

		w->Destroy();

		if(w)
			delete w;
	}
	cWidgets = NULL;

	if(tEvent) {
		delete tEvent;
		tEvent = NULL;
	}
}


///////////////////
// Draw the widgets
void CGuiLayout::Draw(SDL_Surface *bmpDest)
{
	CWidget *w, *end;

	// Draw the widgets in reverse order
	end = NULL;
	for(w=cWidgets ; w ; w=w->getNext()) {
		if(w->getNext() == NULL) {
			end = w;
			break;
		}
	}


	for(w=end ; w ; w=w->getPrev()) {
		if(w->getEnabled())
			w->Draw(bmpDest);
	}
}


///////////////////
// Process all the widgets
gui_event_t *CGuiLayout::Process(void)
{
	CWidget *w;
	mouse_t *tMouse = GetMouse();
	SDL_Event *Event = GetEvent();
	int ev=-1;
	int i;
	int widget = false;


	// Parse keyboard events to the focused widget
	if(cFocused) {
		
		// Make sure a key event happened
		if(Event->type == SDL_KEYUP || Event->type == SDL_KEYDOWN) {

			// Check the characters
			if(Event->key.state == SDL_PRESSED || Event->key.state == SDL_RELEASED) {
				tEvent->cWidget = cFocused;
				tEvent->iControlID = cFocused->getID();

				char input = (char)(Event->key.keysym.unicode & 0x007F);

				if(Event->type == SDL_KEYUP || Event->key.state == SDL_RELEASED)
					ev = cFocused->KeyUp(input);
				
				if(Event->type == SDL_KEYDOWN)
					ev = cFocused->KeyDown(input);

				if(ev >= 0) {
					tEvent->iEventMsg = ev;
					return tEvent;
				}
			}
		}
	}


	// Go through all the widgets
	for(w=cWidgets ; w ; w=w->getNext()) {
		tEvent->cWidget = w;
		tEvent->iControlID = w->getID();

		// Don't process disabled widgets
		if(!w->getEnabled())
			continue;


		if(w->InBox(tMouse->X,tMouse->Y)) {

			// Mouse button event first
			if(tMouse->Down) {

				widget = true;
				if(cFocused)
					cFocused->setFocused(false);
				w->setFocused(true);
				cFocused = w;

				if( (ev=w->MouseDown(tMouse, tMouse->Down)) >= 0) {
					tEvent->iEventMsg = ev;
					return tEvent;
				}
			}

			// Mouse up event
			if(tMouse->Up) {
				widget = true;
				if(cFocused)
					cFocused->setFocused(false);
				w->setFocused(true);
				cFocused = w;

				if( (ev=w->MouseUp(tMouse, tMouse->Up)) >= 0) {
					tEvent->iEventMsg = ev;
					return tEvent;
				}				
			}


			// Mouse over
			if( (ev=w->MouseOver(tMouse)) >= 0) {
				tEvent->iEventMsg = ev;
				return tEvent;
			}

			return NULL;
		}
	}

	// If the mouse is clicked on empty space, take the focus of off the widget
	if(!widget && (tMouse->Down || tMouse->Up)) {
		if(cFocused)
			cFocused->setFocused(false);
		cFocused = NULL;
	}

	return NULL;
}


///////////////////
// Return a widget based on id
CWidget *CGuiLayout::getWidget(int id)
{
	CWidget *w;

	for(w=cWidgets ; w ; w=w->getNext()) {
		if(w->getID() == id)
			return w;
	}

	return NULL;
}


///////////////////
// Send a message to a widget
int CGuiLayout::SendMessage(int iControl, int iMsg, DWORD Param1, DWORD Param2)
{
	CWidget *w = getWidget(iControl);

	// Couldn't find widget
	if(w == NULL)
		return 0;

	// Check if it's a widget message
	if(iMsg < 0) {
		switch( iMsg ) {
			
			// Set the enabled state of the widget
			case WDM_SETENABLE:
				w->setEnabled(Param1);
				break;
		}
		return 0;
	}

	return w->SendMessage(iMsg, Param1, Param2);
}