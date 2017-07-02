/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Combo box
// Created 3/9/02
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"
#include "Menu.h"


///////////////////
// Draw the combo box
void CCombobox::Draw(SDL_Surface *bmpDest)
{
	mouse_t *tMouse = GetMouse();

    Menu_redrawBufferRect( iX,iY, iWidth+1,17);
    if( !iDropped && iLastDropped ) {
        Menu_redrawBufferRect( iX,iY+17, iWidth+1,117);
        iLastDropped = false;
    }    

	// Draw the background bit
	Menu_DrawBoxInset(bmpDest, iX, iY, iX+iWidth, iY+17);	

	if(iDropped) {
		// Dropped down
		if(tSelected)
			tLX->cFont.Draw(bmpDest, iX+3, iY+2, MakeColour(128,128,128), tSelected->sName);

        iLastDropped = true;

		// Change the widget's height
		iHeight = 116;
		Menu_DrawBox(bmpDest, iX, iY+18, iX+iWidth, iY+iHeight);
		DrawRectFill(bmpDest, iX+2,iY+20,iX+iWidth-1, iY+iHeight-1,0);

		// Draw the items
		int count=0;
		int y = iY+20;
		int w = iX+iWidth-1;
		if(iGotScrollbar)
			w-=16;
		cb_item_t *item = tItems;
		for(;item;item=item->tNext,count++) {
			if(count < cScrollbar.getValue())
				continue;

            bool selected = false;

			if(tMouse->X > iX && tMouse->X < w)
				if(tMouse->Y >= y && tMouse->Y < y+16)
                    selected = true;

            if(selected)
                DrawRectFill(bmpDest, iX+2, y, w, y+15, MakeColour(0,66,102));

			tLX->cFont.Draw(bmpDest, iX+3, y, 0xffff, item->sName);
			y+=16;
			if(y >= iY+iHeight)
				break;
		}

		if(iGotScrollbar)
			cScrollbar.Draw(bmpDest);

	} else {
		// Normal
		if(tSelected)
			tLX->cFont.Draw(bmpDest, iX+3, iY+2, 0xffff, tSelected->sName);

		iHeight = 17;
	}

	// Button
	int x=0;
	if(iArrowDown)
		x=15;
	DrawImageAdv(bmpDest, gfxGUI.bmpScrollbar, x,14, iX+iWidth-16,iY+2, 15,14);

	if(!iFocused)
		iDropped = false;

	iArrowDown = false;
}


///////////////////
// Create the combo box
void CCombobox::Create(void)
{
	tItems = NULL;
	tSelected = NULL;
	iItemCount = 0;
	iGotScrollbar = false;
	iDropped = false;
	iArrowDown = false;

	cScrollbar.Create();
	cScrollbar.Setup(0, iX+iWidth-16, iY+20, 14, 95);
	cScrollbar.setMin(0);
	cScrollbar.setMax(1);
	cScrollbar.setValue(0);
	cScrollbar.setItemsperbox(7);
}


///////////////////
// Destroy the combo box
void CCombobox::Destroy(void)
{
	// Free the items
	cb_item_t *i,*item;
	for(i=tItems;i;i=item) {
		item = i->tNext;

		// Free the item
		delete i;
	}

	tItems = NULL;
	tSelected = NULL;

	cScrollbar.Destroy();
}


///////////////////
// Mouse over event
int CCombobox::MouseOver(mouse_t *tMouse)
{
	if(tMouse->X >= iX+iWidth-16 && iGotScrollbar && iDropped)
		cScrollbar.MouseOver(tMouse);

	return CMB_NONE;
}


///////////////////
// Mouse down event
int CCombobox::MouseDown(mouse_t *tMouse, int nDown)
{
	int x = iX+iWidth-16;
	iArrowDown = false;

	if(tMouse->X >= iX+iWidth-16 && iGotScrollbar && iDropped) {
		cScrollbar.MouseDown(tMouse, nDown);
		return CMB_NONE;
	}

	if(tMouse->X >= x && tMouse->X <= x+15)
		if(tMouse->Y >= iY+2 && tMouse->Y < iY+16) {

            //
            // If we aren't dropped, shift the scroll bar
            //
            if(!iDropped) {
                cb_item_t *i = tItems;
                int count = 0;
                for(; i; i=i->tNext, count++) {
                    if(i->iSelected) {
                        // Setup the scroll bar so it shows this item in the middle
                        cScrollbar.setValue( count - cScrollbar.getItemsperbox() / 2 );
                        break;
                    }
                }
            }


            // Drop it
			iArrowDown = true;
			iDropped = true;
		}

	return CMB_NONE;
}


///////////////////
// Mouse up event
int CCombobox::MouseUp(mouse_t *tMouse, int nDown)
{
	iArrowDown = false;

	if(tMouse->X >= iX+iWidth-16 && iGotScrollbar && iDropped) {
		cScrollbar.MouseUp(tMouse, nDown);
		return CMB_NONE;
	}


	// Go through the items checking for a mouse click
	int count=0;
	int y = iY+20;
	int w = iX+iWidth-1;
	if(iGotScrollbar)
		w-=16;

	cb_item_t *item = tItems;
	for(;item;item=item->tNext,count++) {
		if(count < cScrollbar.getValue())
			continue;

		if(tMouse->X > iX && tMouse->X < w)
			if(tMouse->Y >= y && tMouse->Y < y+16)
				if(tMouse->Up & SDL_BUTTON(1)) {
                    if(tSelected)
                        tSelected->iSelected = false;

					tSelected = item;
                    tSelected->iSelected = true;
					iDropped = false;
					return CMB_CHANGED;
				}

				
		y+=16;
		if(y > iY+iHeight)
			break;
	}
	

	return CMB_NONE;
}


///////////////////
// Process a message sent to this widget
int CCombobox::SendMessage(int iMsg, DWORD Param1, DWORD Param2)
{

	switch(iMsg) {

		// Add item message
		case CBM_ADDITEM:
			addItem(Param1, "", (char *)Param2);
			return 0;

		// Add item message (string index)
		case CBM_ADDSITEM:
			addItem(0, (char *)Param1, (char *)Param2);
			return 0;

		// Get the current item's index
		case CBM_GETCURINDEX:
			if(tSelected)
				return tSelected->iIndex;
			break;

		// Get the current item's string index
		case CBM_GETCURSINDEX:
			if(tSelected) {
				strncpy((char *)Param1, tSelected->sIndex, Param2);
				char *p = (char *)Param1;
				p[Param2-1] = '\0';
			}
			break;

        // Get the current item's name
        case CBM_GETCURNAME:
            if(tSelected) {
				strncpy((char *)Param1, tSelected->sName, Param2);
				char *p = (char *)Param1;
				p[Param2-1] = '\0';
			}
			break;


		// Get the current item
		case CBM_GETCURITEM:
			return (int)tSelected;

		// Set the current item
		case CBM_SETCURSEL:
			setCurItem(Param1);
			break;

        // Set the current item based on the string index
        case CBM_SETCURSINDEX:
            setCurSIndexItem((char *)Param1);
            break;

        // Set the current item based on the int index
        case CBM_SETCURINDEX:
            setCurIndexItem(Param1);
            break;
	}


	return 0;
}


///////////////////
// Add an item to the combo box
void CCombobox::addItem(int index, char *sindex, char *name)
{
	cb_item_t *item;

	item = new cb_item_t;
	if(item == NULL)
		return;

	// Fill in the info
	item->iIndex = index;
	strcpy(item->sIndex, sindex);
	strcpy(item->sName,name);
	item->tNext = NULL;
	item->iSelected = false;

	if(!tSelected) {
		// Set the first item to selected
		tSelected = item;
		item->iSelected = true;
	}


	// Add it to the list
	if(tItems) {
		cb_item_t *i = tItems;
		for(;i;i = i->tNext) {
			if(i->tNext == NULL) {
				i->tNext = item;
				break;
			}
		}
	}
	else
		tItems = item;

		
	iItemCount++;
    cScrollbar.setMax( iItemCount );

	iGotScrollbar = false;
	if(iItemCount*16 >= 100)
		iGotScrollbar = true;
}


///////////////////
// Set the current item based on count
void CCombobox::setCurItem(int index)
{
	cb_item_t *i = tItems;
	int count=0;

	for(; i; i=i->tNext, count++) {
		if(count == index) {
			if(tSelected)
				tSelected->iSelected = false;

			tSelected = i;
			tSelected->iSelected = true;
			return;
		}
	}
}


///////////////////
// Set the current item based on string index
void CCombobox::setCurSIndexItem(char *szString)
{
    cb_item_t *i = tItems;
	for(; i; i=i->tNext) {

        if( stricmp(i->sIndex,szString) == 0 ) {
            if(tSelected)
                tSelected->iSelected = false;

            tSelected = i;
            tSelected->iSelected = true;
            return;
        }
    }
}


///////////////////
// Set the current item based on numerical index
void CCombobox::setCurIndexItem(int nIndex)
{
    cb_item_t *i = tItems;
	for(; i; i=i->tNext) {
        if( i->iIndex == nIndex ) {
            if(tSelected)
                tSelected->iSelected = false;

            tSelected = i;
            tSelected->iSelected = true;
            return;
        }
    }
}


///////////////////
// Clear the data
void CCombobox::clear(void)
{
    Destroy();
    Create();
}