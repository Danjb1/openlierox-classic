/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Chat Box class
// Created 26/8/03
// Jason Boettcher


#include "defs.h"
#include "LieroX.h"


///////////////////
// Clear the chatbox
void CChatBox::Clear(void)
{
    nWidth = 500;
    for(int n=0;n<MAX_CLINES;n++) {
        Lines[n].strLine[0] = '\0';
        Lines[n].iUsed = false;
        Lines[n].iColour = 0xffff;
        Lines[n].fTime = 0;
    }
}


///////////////////
// Add a line of text to the chat box
void CChatBox::AddText(char *txt, int colour, float time)
{
    int     n;
    char    c;
    int     l=-1;
    char    buf[128], buf2[128];

    // If this line is too long, break it up
    if(tLX->cFont.GetWidth(txt) > nWidth) {
        int nLength = strlen(txt);
        int nBreakPos = -1;
        int nPos = 0;
        
        for(int i=0; i<nLength+1; i++) {
            c = txt[i];
            sprintf(buf,"%c",c);
            nPos += tLX->cFont.GetWidth(buf);
            
            if(c == ' ' || c == '\0') {
                if(nPos > nWidth && nBreakPos >= 0) {
                    // Line1
                    strcpy(buf,txt);
                    buf[nBreakPos] = '\0';

                    // Line2
                    strcpy(buf2,txt+nBreakPos+1);

                    // Recursively Add the first line & the second line
                    AddText(buf,colour,time);
                    AddText(buf2,colour,time);
                    return;
                }
                nBreakPos = i;
                continue;
            }
        }

        // If we're over the edge & found no spaces, do a hard break
        nPos = 0;
        for(i=0; i<nLength+1; i++) {
            sprintf(buf,"%c",txt[i]);
            nPos += tLX->cFont.GetWidth(buf);
            if(nPos > nWidth) {
                // Line 1
                strcpy(buf,txt);
                buf[i] = '\0';
                
                // Line2
                strcpy(buf2,txt+i);
                
                // Recursively Add the first line & the second line
                AddText(buf,colour,time);
                AddText(buf2,colour,time);
                return;
            }
        }
        return;
    }


    // Move up the lines?
    for(n=0;n<MAX_CLINES;n++) {
        if(!Lines[n].iUsed) {
            l=n;
            break;
        }
    }
    
    if(l<0) {
        MoveUp();
        l=MAX_CLINES-1;
    }
    
    strcpy(Lines[l].strLine,txt);
    Lines[l].iUsed = true;
    Lines[l].iColour = colour;
    Lines[l].fTime = time;
}



///////////////////
// Move up one line	
void CChatBox::MoveUp(void)
{
    for(int n=0;n<MAX_CLINES-1;n++) {
        strcpy(Lines[n].strLine,Lines[n+1].strLine);
        Lines[n].iColour = Lines[n+1].iColour;
        Lines[n].fTime = Lines[n+1].fTime;
    }
}
