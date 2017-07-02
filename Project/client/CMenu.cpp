/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Menu
// Created 28/6/03
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"
#include "Menu.h"


///////////////////
// CMenu constructor
CMenu::CMenu(int nPosX, int nPosY)
{
    m_nPosX = nPosX;
    m_nPosY = nPosY;

    m_nHeight = 0;
    m_nWidth = 0;

    m_psItemList = NULL;
}


///////////////////
// Handle a menu message
int CMenu::SendMessage(int iMsg, DWORD Param1, DWORD Param2)
{
    switch(iMsg) {

        // Add an item
        case MNM_ADDITEM:
            addItem(Param1, (char *)Param2);
            return 0;

        // Redraw the area to the buffer
        case MNM_REDRAWBUFFER:
            Menu_redrawBufferRect(m_nPosX, m_nPosY, m_nWidth+1,m_nHeight+1);
            return 0;
    }

    return 0;
}


///////////////////
// Create the menu
void CMenu::Create(void)
{
    m_nHeight = 0;
    m_nWidth = 0;

    m_psItemList = NULL;
}


///////////////////
// Destroy the menu
void CMenu::Destroy(void)
{
    mnu_item_t *it = m_psItemList;
    mnu_item_t *n;
    for(; it; it=n) {
        n=it->psNext;

        delete it;
    }

    m_psItemList = NULL;
}


///////////////////
// Add an item to the menu
void CMenu::addItem(int nID, char *szName)
{
    mnu_item_t *i = new mnu_item_t;
    if( !i )
        return;

    i->nID = nID;
    i->psNext = NULL;
    i->nSelected = false;
    strcpy(i->szName, szName);
    m_nHeight += 20;
    m_nHeight = MAX(m_nHeight, 23);

    m_nWidth = MAX(m_nWidth, tLX->cFont.GetWidth(szName)+10 );

    // Link it in at the end
    mnu_item_t *it = m_psItemList;
    for(; it; it=it->psNext) {
        if( it->psNext == NULL ) {
            it->psNext = i;
            return;
        }
    }

    // First item if we got here
    m_psItemList = i;
}


///////////////////
// Draw the menu
void CMenu::Draw(SDL_Surface *bmpDest)
{
    Menu_redrawBufferRect(m_nPosX+1, m_nPosY+1, m_nWidth-1,m_nHeight-1);

    DrawRectFill(bmpDest, m_nPosX+1, m_nPosY+1, m_nPosX+m_nWidth-1, m_nPosY+m_nHeight-1, 0);
    Menu_DrawBox(bmpDest, m_nPosX, m_nPosY, m_nPosX+m_nWidth, m_nPosY+m_nHeight);

    mnu_item_t *it = m_psItemList;
    int y = m_nPosY+2;
    for(; it; it=it->psNext) {

        if( it->nSelected )
            DrawRectFill(bmpDest, m_nPosX+2,y,  m_nPosX+m_nWidth-1, y+20, MakeColour(0,66,102));            
        tLX->cFont.Draw(bmpDest, m_nPosX+5, y+2, 0xffff, it->szName);

        it->nSelected = false;

        y+=20;
    }
}


///////////////////
// Move over event
int CMenu::MouseOver(mouse_t *tMouse)
{
    if( !MouseInRect(m_nPosX, m_nPosY, m_nWidth, m_nHeight) )
        return MNU_NONE;

    int y = m_nPosY + 2;
    mnu_item_t *it = m_psItemList;
    for(; it; it=it->psNext) {

        if( tMouse->Y > y && tMouse->Y < y+20 ) {
            it->nSelected = true;
            break;
        }

        y+=18;
    }

    return MNU_NONE;
}


///////////////////
// Mouse Up event
int CMenu::MouseUp(mouse_t *tMouse, int nDown)
{
    // Lose focus?
    if( !MouseInRect(m_nPosX, m_nPosY, m_nWidth, m_nHeight) )
        return MNU_LOSTFOCUS;

    int y = m_nPosY + 2;
    mnu_item_t *it = m_psItemList;
    for(; it; it=it->psNext) {

        if( tMouse->Y > y && tMouse->Y < y+20 )
            return MNU_USER + it->nID;

        y+=18;
    }

    return MNU_NONE;
}


///////////////////
// Mouse down event
int CMenu::MouseDown(mouse_t *tMouse, int nDown)
{
    // Lose focus?
    if( !MouseInRect(m_nPosX, m_nPosY, m_nWidth, m_nHeight) )
        return MNU_LOSTFOCUS;

    return MNU_NONE;
}