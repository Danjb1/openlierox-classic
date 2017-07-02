/////////////////////////////////////////
//
//             Liero Xtreme
//
//     Copyright Auxiliary Software 2002
//
//
/////////////////////////////////////////


// gui list view class
// Created 30/5/02
// Jason Boettcher


#ifndef __CLISTVIEW_H__
#define __CLISTVIEW_H__


// Event msg's for this control
enum {
	LV_NONE=-1,
	LV_CHANGED=0,
    LV_RIGHTCLK,
	LV_DOUBLECLK,
};


// Listview messages
enum {
	LVM_ADDCOLUMN=0,
	LVM_ADDITEM,
	LVM_ADDSUBITEM,
	LVM_REMOVEITEM,
	LVM_GETCURINDEX,
	LVM_GETCURSINDEX,
	LVM_GETITEMCOUNT,
	LVM_CLEAR,
	LVM_GETITEMINDEX,
	LVM_GETINDEX,
    LVM_GETCURITEM
};


// Sub-item types
enum {
	LVS_IMAGE=0,
	LVS_TEXT
};



// Column structure
typedef struct lv_column_s {
	char		sText[32];
	int			iWidth;

	struct		lv_column_s *tNext;

} lv_column_t;


// Sub item structure
typedef struct lv_subitem_s {
	int			iType;
	char		sText[128];
	SDL_Surface	*bmpImage;
	int			iVisible;
	int			iExtra;

	struct		lv_subitem_s *tNext;

} lv_subitem_t;


// Item structure
typedef struct lv_item_s {
	char		sIndex[128];
	int			iIndex;
    int         _iID;
	int			iSelected;
	int			iHeight;
	int			iColour;

	lv_subitem_t *tSubitems;

	struct	lv_item_s	*tNext;

} lv_item_t;


// Button control class
class CListview: public CWidget {
public:
	// Constructors
	CListview() {
		tColumns = NULL;
		tItems = NULL;
		tLastItem = NULL;
		tSelected = NULL;
		iItemCount=0;
		iGotScrollbar=false;
		iType = wid_Listview;
		fLastMouseUp = -99999;
		iContentHeight = 0;
        iItemID = 0;
        bShowSelect = true;
	}


private:
	// Attributes

	// Columns
	lv_column_t		*tColumns;

	// Items
	lv_item_t		*tItems;
	lv_item_t		*tLastItem;
	lv_item_t		*tSelected;
	int				iItemCount;
    int             iItemID;
	int				iGotScrollbar;
	int				iContentHeight;

	float			fLastMouseUp;
	int				iClickedSub;

	// Scrollbar
	CScrollbar		cScrollbar;

    bool            bShowSelect;

	
public:
	// Methods
	
	void	Create(void);
	void	Destroy(void);

	//These events return an event id, otherwise they return -1
	int		MouseOver(mouse_t *tMouse);
	int		MouseUp(mouse_t *tMouse, int nDown);
	int		MouseDown(mouse_t *tMouse, int nDown);
	int		KeyDown(char c)					{ return LV_NONE; }
	int		KeyUp(char c)					{ return LV_NONE; }

	void	Draw(SDL_Surface *bmpDest);

	int		SendMessage(int iMsg, DWORD Param1, DWORD Param2);

	void	ReadjustScrollbar(void);

	void	Clear(void);

	void	AddColumn(char *sText, int iWidth);
	void	AddItem(char *sIndex, int iIndex, int iColour);
	void	AddSubitem(int iType, char *sText, SDL_Surface *img);
	void	RemoveItem(int iIndex);
	int		getIndex(int count);

    int     getSelectedID(void);
    void    setSelectedID(int id);

    int     getNumItems(void);

    void    scrollLast(void);    

	int		getCurIndex(void)		{ if(tSelected) return tSelected->iIndex; else return -1; }
	char	*getCurSIndex(void)		{ if(tSelected) return tSelected->sIndex; else return NULL; }
    

	lv_subitem_t	*getCurSub(void);	

	int			getItemCount(void)		{ return iItemCount; }
	lv_item_t	*getItems(void)			{ return tItems; }
	lv_item_t	*getLastItem(void)	{ return tLastItem; }

	int		getClickedSub(void)		{ return iClickedSub; }

    void    setShowSelect(bool s)   { bShowSelect = s; }

};



#endif  //  __CLISTVIEW_H__