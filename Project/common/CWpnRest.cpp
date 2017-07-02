/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// Weapon Restrictions class
// Created 30/3/03
// Jason Boettcher


#include "defs.h"
#include "Lierox.h"


///////////////////
// WpnRest Constructor
CWpnRest::CWpnRest()
{
    m_psWeaponList = NULL;
    m_psSortedList = NULL;
    m_nCount = 0;
}


///////////////////
// Update the weapons list from a gamescript file
void CWpnRest::updateList(CGameScript *pcGameS)
{
    assert( pcGameS );

    // Go through the weapons in the gamescript
    // If any weapon is not in our list, add it to the list
    weapon_t *psWpn = pcGameS->GetWeapons();
    int count = pcGameS->GetNumWeapons();
    int i;


    for(i=0; i<count; i++, psWpn++) {

        wpnrest_t *w = findWeapon( psWpn->Name );
        if( !w ) {
            // No match, add it to the list
            addWeapon( psWpn->Name, wpr_enabled );
        }
    }

    // Sort the list
    sortList();
}


///////////////////
// Reset all the weapons to default (enabled)
void CWpnRest::reset(void)
{
    wpnrest_t *psWpn = m_psWeaponList;
    for(; psWpn; psWpn=psWpn->psNext) {
        psWpn->nState = wpr_enabled;
    }
}


///////////////////
// Reset all the weapons in the current game script to default (enabled)
void CWpnRest::resetVisible(CGameScript *pcGameS)
{
    assert(pcGameS);

    wpnrest_t *psWpn = m_psWeaponList;
    for(; psWpn; psWpn=psWpn->psNext) {
        if(pcGameS->weaponExists(psWpn->szName))
            psWpn->nState = wpr_enabled;
    }
}


///////////////////
// Randomize all the weapons in the current game script
void CWpnRest::randomizeVisible(CGameScript *pcGameS)
{
    assert(pcGameS);

    wpnrest_t *psWpn = m_psWeaponList;
    for(; psWpn; psWpn=psWpn->psNext) {
        if(pcGameS->weaponExists(psWpn->szName))
            psWpn->nState = GetRandomInt(2);
    }
}


///////////////////
// Find a weapon in the list
wpnrest_t *CWpnRest::findWeapon(char *szName)
{
    assert( szName );

    char name[256];
    lx_strncpy(name, szName, 255);

    wpnrest_t *psWpn = m_psWeaponList;

    for(; psWpn; psWpn=psWpn->psNext) {

        // We need to be a bit lenient here in case some simple mistakes in different game scripts occur
        // Like case & leading/trailing spaces
        strcpy(name, TrimSpaces( name ));
        if( stricmp(psWpn->szName, name) == 0 )
            return psWpn;
    }

    // No match
    return NULL;
}


///////////////////
// Add a weapon to the list
void CWpnRest::addWeapon(char *szName, int nState)
{
    assert( szName );

    wpnrest_t *psWpn = new wpnrest_t;

    if( !psWpn )
        return;

    psWpn->szName = new char[ strlen(szName)+1 ];
    strcpy(psWpn->szName, szName);
    psWpn->nState = nState;
    
    // Link it in
    psWpn->psNext = m_psWeaponList;
    m_psWeaponList = psWpn;

    // Sort the list
    sortList();
}


///////////////////
// Save the weapons restrictions list
void CWpnRest::saveList(char *szFilename)
{
    // Save it as plain text
    FILE *fp = fopen(szFilename, "wt");
    if( !fp )
        return;

    wpnrest_t *psWpn = m_psWeaponList;
    for(; psWpn; psWpn=psWpn->psNext) {

        fprintf(fp, "%s,%d\n", psWpn->szName, psWpn->nState);
    }

    fclose(fp);
}


///////////////////
// Load the weapons restrictions list
void CWpnRest::loadList(char *szFilename)
{
    // Shutdown the list first
    Shutdown();

    FILE *fp = fopen(szFilename, "rt");
    if( !fp )
        return;

    char line[256];

    while( !feof(fp) ) {
        fscanf(fp, "%[^\n]\n",line);
        char *tok = strtok(line,",");
        if( tok )
            addWeapon( tok, atoi(strtok(NULL,",")) );
    }

    fclose(fp);

    // Sort the list
    sortList();
}


///////////////////
// Checks if the weapon is enabled or not
bool CWpnRest::isEnabled(char *szName)
{
    wpnrest_t *psWpn = findWeapon(szName);

    // If we can't find the weapon, then assume it is banned
    if( !psWpn )
        return false;
    
    return (psWpn->nState == wpr_enabled);
}


///////////////////
// Finds a weapon that is enabled and returns the name
char *CWpnRest::findEnabledWeapon(CGameScript *pcGameS)
{
    assert(pcGameS);

    // Go from the start of the list looking for an enabled weapon
    // The weapon must also be in the gamescript
    wpnrest_t *psWpn = m_psWeaponList;
    for(; psWpn; psWpn=psWpn->psNext) {

        if( psWpn->nState != wpr_enabled )
            continue;

        // Is the weapon in the gamescript?
        if( !pcGameS->FindWeapon(psWpn->szName) )
            continue;

        // Must be good
        return psWpn->szName;
    }

    // No enabled weapons found
    return NULL;
}


///////////////////
// Get the state of a weapon
int CWpnRest::getWeaponState(char *szName)
{
    wpnrest_t *psWpn = findWeapon(szName);

    // Default to disabled if we can't find the weapon
    if( !psWpn )
        return wpr_banned;

    return psWpn->nState;
}


///////////////////
// Return the sorted weapon list
wpnrest_t *CWpnRest::getList(void)
{
    return m_psSortedList;
}


///////////////////
// Return the number of weapons
int CWpnRest::getNumWeapons(void)
{
    return m_nCount;
}


///////////////////
// Create a sorted list
void CWpnRest::sortList(void)
{
    int i, j;

    // Free any previous list
    if( m_psSortedList )
        delete[] m_psSortedList;

    // Count the number of weapons
    m_nCount = 0;
    wpnrest_t *psWpn = m_psWeaponList;
    for(; psWpn; psWpn=psWpn->psNext)
        m_nCount++;

    // Allocate the sorted list
    m_psSortedList = new wpnrest_t[m_nCount];
    if( !m_psSortedList )
        return;

    // Fill in the links
    psWpn = m_psWeaponList;
    for( i=0; i<m_nCount; i++, psWpn=psWpn->psNext)
        m_psSortedList[i].psLink = psWpn;
    
    if( m_nCount < 2 )
        return;

    // Sort the list using a simple bubble sort
    wpnrest_t temp;
   	for(i=0; i<m_nCount; i++) {
		for(j=0; j<m_nCount-1-i; j++) {

            if( strcmp(m_psSortedList[j].psLink->szName, m_psSortedList[j+1].psLink->szName) > 0 ) {

                // Swap the 2 items
                temp = m_psSortedList[j];
                m_psSortedList[j] = m_psSortedList[j+1];
                m_psSortedList[j+1] = temp;
            }
        }
    }
}


///////////////////
// Send the list
void CWpnRest::sendList(CBytestream *psByteS)
{
    wpnrest_t *psWpn = NULL;

    // Count the number of weapons that are _not_ enabled
    int nCount = 0;
    psWpn = m_psWeaponList;
    for(; psWpn; psWpn = psWpn->psNext ) {
        if( psWpn->nState != wpr_enabled )
            nCount++;
    }


    // Write the header
    psByteS->writeInt(nCount,2);

    // Go through the list writing out weapons that are _not_ enabled
    psWpn = m_psWeaponList;
    for(; psWpn; psWpn = psWpn->psNext ) {
        if( psWpn->nState != wpr_enabled ) {
            
            psByteS->writeString(psWpn->szName);
            psByteS->writeByte(psWpn->nState);
        }
    }
}


///////////////////
// Receive the list
void CWpnRest::readList(CBytestream *psByteS)
{
    char szName[256];
    int nState;
    wpnrest_t *psWpn = NULL;

    // Initialize all the weapons to a default of 'enabled'
    psWpn = m_psWeaponList;
    for(; psWpn; psWpn = psWpn->psNext ) {
        psWpn->nState = wpr_enabled;
    }


    int nCount = psByteS->readInt(2);

    // Go through the list reading weapons
    for( int i=0; i<nCount; i++ ) {
        psByteS->readString(szName);
        nState = psByteS->readByte();

        // Try and find the weapon
        psWpn = findWeapon(szName);
        if( psWpn )
            psWpn->nState = nState;
        else {
            // If the weapon doesn't exist, thats ok
            // just add it to the list
            addWeapon(szName,nState);
        }
    }
}


///////////////////
// Shutdown the weapons restrictions list
void CWpnRest::Shutdown(void)
{
     wpnrest_t *psWpn = m_psWeaponList;
     wpnrest_t *psNext = NULL;

     for(; psWpn; psWpn=psNext) {
         psNext = psWpn->psNext;

         if( psWpn->szName )
             delete[] psWpn->szName;

         delete psWpn;
     }

     m_psWeaponList = NULL;

    // Free any sorted list
    if( m_psSortedList )
        delete[] m_psSortedList;
    m_psSortedList = NULL;
}