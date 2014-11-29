#strict 2

local pCommandObj;
local pBar1, pBar2, pBar3, pBar4, pBar5, pBar6, pBar7, pBar8, pBar9, pBar10, pBar11, pBar12, pBar13, pBar14;
local pBarOffset;
local aBars;
local pEquip, pEquipOffset;

local iButtonBarIndex, iButtonIndex;

/* -------------------- globale Funktionen -----------------------*/

global func CreateCharMenu(object obj )
{
	if( !obj ) return( 0 );
	if( !( obj->~CanUseCharMenu() ) ) return 0;

	// Altes Menu evtl. schließen

	var existing_menu = LocalN("pCharMenu",obj);

	if( existing_menu ) return 0;

	// Neues Menu öffnen

	var menu = CreateObject( ID_Menu_Char,30,20,-1);

	SetPosition( 0, 0, menu);

	LocalN("pCharMenu",obj) = menu;
	menu->~AttachCharMenu( obj );

	return( 1 );
}

global func CloseCharMenu( obj )
{
	if( !obj ) return( 0 );
	if( !( obj->~CanUseCharMenu() ) ) return 0;

	var existing_menu = LocalN("pCharMenu",obj);

	if( existing_menu )
	{
		existing_menu->~CloseMainMenu();

		return 1;
	}

	return;
}


public func AttachCharMenu( object pObj )
{
	if(!pObj) return RemoveObject();
	SetOwner(GetOwner(pObj));

	pCommandObj = pObj;

	CharMenuCreateBars();
	CharMenuCreateTitleBar();
	CharMenuCreateEquipment();
}

protected func Destruction()
{
	for( bar in aBars )
		if( bar )
			RemoveObject( bar );
}


public func ComboClose()
{
	var existing_menu = LocalN("pComboMenu",pCommandObj);

	if( existing_menu == this ) LocalN("pComboMenu",pCommandObj) = 0;

	//aBars[iButtonBarIndex]->~ButtonSelected(-1);

	for( var i=2; i < 14; i++ )
		aBars[i]->~ButtonSelected(-1);
}

public func ComboInput( key )
{
	var query;

	if( pCommandObj->~MenuQueryCancel() ) query = 1;

	if( szMenuStatus == "main" ) NavigateMainMenu( key );
	if( szMenuStatus == "inventory" ) NavigateInventory( key );
	if( szMenuStatus == "equipment" ) NavigateEquipment( key );

	return 1;
} 



//////////////////////////////////////////////////////////////////////////////////
//
// Die Auswahl-Leiste

protected func CharMenuCreateTitleBar()
{
	if(!aBars[1]) return;

	(aBars[1]->GetButton(0))->~SetButton(CNKT,"OpenInventory",this);
	(aBars[1]->GetButton(1))->~SetButton(FLNT,"OpenEquipment",this);
	(aBars[1]->GetButton(2))->~SetButton(SCRG,"OpenQuestLog",this);
}

protected func CharMenuCreateBars()
{
	pBar1 = CreateObject( ID_Menu_CharMenuBar,0,0,-1);
	pBar2 = CreateObject( ID_Menu_CharMenuBar,0,0,-1);
	pBar3 = CreateObject( ID_Menu_CharMenuBar,0,0,-1);
	pBar4 = CreateObject( ID_Menu_CharMenuBar,0,0,-1);
	pBar5 = CreateObject( ID_Menu_CharMenuBar,0,0,-1);
	pBar6 = CreateObject( ID_Menu_CharMenuBar,0,0,-1);
	pBar7 = CreateObject( ID_Menu_CharMenuBar,0,0,-1);
	pBar8 = CreateObject( ID_Menu_CharMenuBar,0,0,-1);
	pBar9 = CreateObject( ID_Menu_CharMenuBar,0,0,-1);
	pBar10 = CreateObject( ID_Menu_CharMenuBar,0,0,-1);
	pBar11 = CreateObject( ID_Menu_CharMenuBar,0,0,-1);
	pBar12 = CreateObject( ID_Menu_CharMenuBar,0,0,-1);
	pBar13 = CreateObject( ID_Menu_CharMenuBar,0,0,-1);
	pBar14 = CreateObject( ID_Menu_CharMenuBar,0,0,-1);

	aBars = [];

	aBars[0]=pBar1;
	aBars[1]=pBar2;
	aBars[2]=pBar3;
	aBars[3]=pBar4;
	aBars[4]=pBar5;
	aBars[5]=pBar6;
	aBars[6]=pBar7;
	aBars[7]=pBar8;
	aBars[8]=pBar9;
	aBars[9]=pBar10;
	aBars[10]=pBar11;
	aBars[11]=pBar12;
	aBars[12]=pBar13;
	aBars[13]=pBar14;

	pBarOffset = 0;

	pBar1->~AttachToHUD(this,-288,-44,0);
	pBar2->~AttachToHUD(this,-288,0,"2");
	pBar3->~AttachToHUD(this,-288,44,"2");
	pBar4->~AttachToHUD(this,-288,88,"2");
	pBar5->~AttachToHUD(this,-288,132,"2");
	pBar6->~AttachToHUD(this,-288,176,"2");
	pBar7->~AttachToHUD(this,-288,220,"2");
	pBar8->~AttachToHUD(this,-288,264,"2");
	pBar9->~AttachToHUD(this,-288,308,"2");
	pBar10->~AttachToHUD(this,-288,352,"2");
	pBar11->~AttachToHUD(this,-288,396,"2");
	pBar12->~AttachToHUD(this,-288,440,"2");
	pBar13->~AttachToHUD(this,-288,484,"2");
	pBar14->~AttachToHUD(this,-288,528,"2");
}

//////////////////////////////////////////////////////////////////////////////////
//
// Haupt-Funktion

local szMenuStatus;


public func OpenMainMenu()
{
	if( szMenuStatus )
	{
		if(szMenuStatus == "main")
			CloseMainMenu();
		else
			ComboInput("S");

		return;
	}

	if(GetEffect()) return;
	AddEffect("OpenInventory",this,50,1,this,0,0);
}

protected func CloseMainMenu()
{
	if(GetEffect()) return;
	//AddEffect("CloseMainMenu",this,50,1,this);
	AddEffect("CloseInventory",this,50,1,this,0,-1);
	ComboClose();

}


protected func OnOpenedMainMenu( int iButton )
{
	if( !pCommandObj ) return RemoveObject();

	szMenuStatus = "main";

	// Altes Menu evtl. schließen

	var existing_menu = LocalN("pComboMenu",pCommandObj);

	if( existing_menu ) existing_menu->~ComboClose();

	LocalN("pComboMenu",pCommandObj) = this;

	// Button auswählen
	iButtonBarIndex = 1;
	iButtonIndex = iButton;
	aBars[iButtonBarIndex]->~ButtonSelected(iButtonIndex);

}

protected func NavigateMainMenu( key )
{
	// links auswählen
	if( key == "L" || key == "LL" )
	{
		iButtonIndex -= Min(1,iButtonIndex);
		aBars[iButtonBarIndex]->~ButtonSelected(iButtonIndex);
	}

	// rechts auswählen
	if( key == "R" || key == "RR" )
	{
		iButtonIndex += Min(1,4-iButtonIndex);
		aBars[iButtonBarIndex]->~ButtonSelected(iButtonIndex);
	}


	// Auswahl bestätigen
	if( key == "P" )
	{
		aBars[iButtonBarIndex]->~ButtonPressed(iButtonIndex);
	}

	// schließen
	if( key == "S" )
	{
		//aBars[iButtonBarIndex]->~ButtonSelected(-1);
		//ComboClose();
		CloseMainMenu();
	}
}


//////////////////////////////////////////////////////////////////////////////////
//
// Inventar-Funktion


public func OpenInventory()
{
	if( szMenuStatus == "inventory" )
	{
		CloseMainMenu();
		return;
	}

	if(GetEffect()) return;
	if( szMenuStatus!="main" && szMenuStatus ) return;

	var backpack = pCommandObj->~GetBackpack();

	if(!backpack) return;

	AddEffect("OpenInventory",this,50,1,this,0,backpack->~IsBackpack());

	UpdateInventoryButtons();
}

protected func CloseInventory()
{
	if(GetEffect()) return;
	AddEffect("CloseInventory",this,50,1,this,0,0);
	ComboClose();

}

protected func OnOpenedInventory()
{
	if( !pCommandObj ) return RemoveObject();

	szMenuStatus = "inventory";

	// Altes Menu evtl. schließen

	var existing_menu = LocalN("pComboMenu",pCommandObj);

	if( existing_menu ) existing_menu->~ComboClose();

	LocalN("pComboMenu",pCommandObj) = this;

	// Button auswählen
	iButtonBarIndex = 2;
	iButtonIndex = 0;
	aBars[iButtonBarIndex]->~ButtonSelected(iButtonIndex);

}

protected func NavigateInventory( key )
{
	// links auswählen
	if( key == "L" || key == "LL" )
	{
		iButtonIndex -= Min(1,iButtonIndex);
		aBars[iButtonBarIndex]->~ButtonSelected(iButtonIndex);
	}

	// rechts auswählen
	if( key == "R" || key == "RR" )
	{
		iButtonIndex += Min(1,4-iButtonIndex);
		aBars[iButtonBarIndex]->~ButtonSelected(iButtonIndex);
	}

	if( key == "U" || key == "UU" )
	{
		aBars[iButtonBarIndex]->~ButtonSelected(-1);

		iButtonBarIndex -= Min(1,iButtonBarIndex-2);

		aBars[iButtonBarIndex]->~ButtonSelected(iButtonIndex);
	}

	if( key == "D" || key == "DD" )
	{
		var maxindex = pBarOffset / (-44);

		aBars[iButtonBarIndex]->~ButtonSelected(-1);

		iButtonBarIndex += Min(1,maxindex-iButtonBarIndex);

		aBars[iButtonBarIndex]->~ButtonSelected(iButtonIndex);
	}


	// Auswahl bestätigen
	if( key == "P" )
	{
		aBars[iButtonBarIndex]->~ButtonPressed(iButtonIndex);
	}

	// schließen
	if( key == "S" )
	{
		CloseInventory();
	}
}

protected func UpdateInventoryButtons()
{
	var backpack = pCommandObj->~GetBackpack();
	var inventory = FindObjects( Find_Container( backpack ) );
	var bar_index=2, button_index=0;
	var item;

	for( var i=2; i < 14; i++ )
	{
		for( var j=0; j<5; j++ )
		{
			(aBars[i]->~GetButton(j))->~SetButton( ID_MenuIcon_StandardButton, "", pCommandObj );
		}
	}

	for( item in inventory )
	{
		(aBars[bar_index]->~GetButton(button_index))->~SetButton( item->GetID(), "MenuInvInteractWith", pCommandObj, item, 1, item ); // item ist im Inventory 

		button_index += 1;
		if( button_index > 4 )
		{
			button_index = 0;
			bar_index += 1;
		}

		if( bar_index > 13 ) break;
	}


}

//////////////////////////////////////////////////////////////////////////////////
//
// Equipment-Funktion


protected func CharMenuCreateEquipment()
{
	pEquip = CreateObject( ID_Menu_Equipment,0,0,-1);

	pEquipOffset = 0;

	pEquip->~AttachToHUD(this,0,-280);
}


public func OpenEquipment()
{
	if( szMenuStatus == "equipment" )
	{
		CloseEquipment();
		return;
	}

	if(GetEffect()) return;

	AddEffect("OpenEquipment",this,50,1,this);

	UpdateEquipmentButtons();

	pCommandObj->~DrawEquipmentOverlays();
}

protected func CloseEquipment()
{
	if(GetEffect()) return;
	AddEffect("CloseEquipment",this,50,1,this,0,0);
	ComboClose();
}

protected func OnOpenedEquipment()
{
	if( !pCommandObj ) return RemoveObject();

	szMenuStatus = "equipment";

	// Altes Menu evtl. schließen

	var existing_menu = LocalN("pComboMenu",pCommandObj);

	if( existing_menu ) existing_menu->~ComboClose();

	LocalN("pComboMenu",pCommandObj) = this;

	// Button auswählen
	iButtonBarIndex = 0;
	iButtonIndex = 0;
	pEquip->~ButtonSelected(iButtonIndex + 5*iButtonBarIndex);

}

protected func NavigateEquipment( key )
{
	// links auswählen
	if( key == "L" || key == "LL" )
	{
		iButtonIndex -= Min(1,iButtonIndex);
		pEquip->~ButtonSelected(iButtonIndex + 5*iButtonBarIndex);
	}

	// rechts auswählen
	if( key == "R" || key == "RR" )
	{
		iButtonIndex += Min(1,4-iButtonIndex);
		pEquip->~ButtonSelected(iButtonIndex + 5*iButtonBarIndex);
	}

	if( key == "U" || key == "UU" )
	{
		iButtonBarIndex -= Min(1,iButtonBarIndex);
		pEquip->~ButtonSelected(iButtonIndex + 5*iButtonBarIndex);
	}

	if( key == "D" || key == "DD" )
	{
		iButtonBarIndex += Min(1,3-iButtonBarIndex);
		pEquip->~ButtonSelected(iButtonIndex + 5*iButtonBarIndex);
	}


	// Auswahl bestätigen
	if( key == "P" )
	{
		pEquip->~ButtonPressed(iButtonIndex + 5*iButtonBarIndex);
	}

	// schließen
	if( key == "S" )
	{
		CloseEquipment();
	}
}


protected func UpdateEquipmentButtons()
{
	var item;

	item = LocalN("pWeapon1",pCommandObj);
		UpdateEquipmentButton(0,item,I_W_,"MenuInvChooseWeapon",1,0,GetID(item),"MenuInvInteractWith",item,2);
	item = LocalN("pWeapon2",pCommandObj);
		UpdateEquipmentButton(5,item,I_W_,"MenuInvChooseWeapon",2,0,GetID(item),"MenuInvInteractWith",item,2);
	item = LocalN("pWeapon3",pCommandObj);
		UpdateEquipmentButton(10,item,I_W_,"MenuInvChooseWeapon",3,0,GetID(item),"MenuInvInteractWith",item,2);

	item = LocalN("pBackpack",pCommandObj);
		UpdateEquipmentButton(2,item,I_W_,"MenuInvChooseBackpack",0,0,GetID(item),"MenuInvInteractWith",item,2);

	item = LocalN("pHelmet",pCommandObj);
			UpdateEquipmentButton(6,item,I_W_,"MenuInvChooseItem","Helmet",ROCK,GetID(item),"MenuInvInteractWith",item,2);
	item = LocalN("pArmor",pCommandObj);
			UpdateEquipmentButton(7,item,I_W_,"MenuInvChooseItem","Armor",ROCK,GetID(item),"MenuInvInteractWith",item,2);
	item = LocalN("pGreaves",pCommandObj);
			UpdateEquipmentButton(8,item,I_W_,"MenuInvChooseItem","Greaves",ROCK,GetID(item),"MenuInvInteractWith",item,2);
	item = LocalN("pAmulet",pCommandObj);
			UpdateEquipmentButton(1,item,I_W_,"MenuInvChooseItem","Amulet",ROCK,GetID(item),"MenuInvInteractWith",item,2);

	item = LocalN("pRing1",pCommandObj); // 9
	item = LocalN("pRing2",pCommandObj); // 14

	item = LocalN("pGloves",pCommandObj);
			UpdateEquipmentButton(4,item,I_W_,"MenuInvChooseItem","Gloves",ROCK,GetID(item),"MenuInvInteractWith",item,2);
	item = LocalN("pBoots",pCommandObj);
			UpdateEquipmentButton(3,item,I_W_,"MenuInvChooseItem","Boots",ROCK,GetID(item),"MenuInvInteractWith",item,2);
	item = LocalN("pCloak",pCommandObj);
			UpdateEquipmentButton(11,item,I_W_,"MenuInvChooseItem","Cloak",ROCK,GetID(item),"MenuInvInteractWith",item,2);
	item = LocalN("pShirt",pCommandObj);
			UpdateEquipmentButton(12,item,I_W_,"MenuInvChooseItem","Shirt",ROCK,GetID(item),"MenuInvInteractWith",item,2);
	item = LocalN("pPants",pCommandObj);
			UpdateEquipmentButton(13,item,I_W_,"MenuInvChooseItem","Pants",ROCK,GetID(item),"MenuInvInteractWith",item,2);

}

protected func UpdateEquipmentButton(int iIndex, object pItem, id idSymbolF, string szCallF, aPar1F, aPar2F, id idSymbolT, string szCallT, aPar1T, aPar2T  )
{
	if( pItem )
	{
		(pEquip->~GetButton(iIndex))->~SetButton( idSymbolT, szCallT, pCommandObj, aPar1T, aPar2T, pItem );
	}
	else
	{
		(pEquip->~GetButton(iIndex))->~SetButton( idSymbolF, szCallF, pCommandObj, aPar1F, aPar2F );
	}
}

public func GetAvatar()
{
	if(!pEquip) return 0;

	return LocalN("pAvatar",pEquip);
}

//////////////////////////////////////////////////////////////////////////////////
//
// Effekte-Timer
/*
protected func FxOpenMainMenuTimer( object pTarget, int iEffectNumber, int iEffectTime)
{
	var i = Min(iEffectTime,11);
	pBarOffset = -4*i;

	for( bar in aBars )
		bar->~UpdatePosition( pBarOffset );

	//if(pBar1) pBar1->~SetPosition( -288, -44 +pBarOffset );
	//if(pBar2) pBar2->~SetPosition( -288,   0 +pBarOffset );

	if( i > 10 )
	{
		OnOpenedMainMenu();
		return -1;
	}
}

protected func FxCloseMainMenuStart( object pTarget, int iEffectNumber, int iTemp, var1, var2, var3, var4 )
{
	EffectVar(0, pTarget, iEffectNumber) = pBarOffset / 11;
}

protected func FxCloseMainMenuTimer( object pTarget, int iEffectNumber, int iEffectTime)
{
	//var i = Min(iEffectTime,pBarOffset / (-4));
	//var dist = 4*i;
	pBarOffset -= EffectVar(0,pTarget,iEffectNumber);

	for( bar in aBars )
		bar->~UpdatePosition( pBarOffset );//+ dist );

	//if(pBar1) pBar1->~SetPosition( -288, -44 +pBarOffset +dist );
	//if(pBar2) pBar2->~SetPosition( -288,   0 +pBarOffset +dist );

	if( pBarOffset >= 0 )
	//if( dist >= -pBarOffset )
	//{
	{
		szMenuStatus = 0;
		pBarOffset = 0;
		for( bar in aBars ) bar->~UpdatePosition( pBarOffset );
		return -1;
	}
}
*/
protected func FxOpenInventoryStart( object pTarget, int iEffectNumber, int iTemp, var1, var2, var3, var4 )
{
	EffectVar(0, pTarget, iEffectNumber) = var1+1;
}


protected func FxOpenInventoryTimer( object pTarget, int iEffectNumber, int iEffectTime)
{
	var i = -44*EffectVar(0, pTarget, iEffectNumber);
	pBarOffset += -4*EffectVar(0, pTarget, iEffectNumber);

	for( bar in aBars )
		bar->~UpdatePosition( pBarOffset );

	if( pBarOffset <= i )
	{
		pBarOffset = i;
		for( bar in aBars ) bar->~UpdatePosition( pBarOffset );

		if( pBarOffset == -44 )
			OnOpenedMainMenu();
		else
			OnOpenedInventory();
		return -1;
	}
}

protected func FxCloseInventoryStart( object pTarget, int iEffectNumber, int iTemp, var1, var2, var3, var4 )
{
	EffectVar(0, pTarget, iEffectNumber) = (pBarOffset / (-44)) - (var1+1);
	EffectVar(1, pTarget, iEffectNumber) = 0;
}


protected func FxCloseInventoryTimer( object pTarget, int iEffectNumber, int iEffectTime)
{
	var i = 44*EffectVar(0, pTarget, iEffectNumber);
	EffectVar(1, pTarget, iEffectNumber)  += 4*EffectVar(0, pTarget, iEffectNumber);

	for( bar in aBars )
		bar->~UpdatePosition( pBarOffset + EffectVar(1, pTarget, iEffectNumber) );

	if( EffectVar(1, pTarget, iEffectNumber) >= i )
	{
		pBarOffset += i;
		for( bar in aBars ) bar->~UpdatePosition( pBarOffset );
		
		if( pBarOffset == -44 )
		{
			szMenuStatus = "main";
			OnOpenedMainMenu();
		}
		else
			szMenuStatus = 0;

		return -1;
	}
}

protected func FxOpenEquipmentTimer( object pTarget, int iEffectNumber, int iEffectTime)
{
	pEquipOffset -= 35;

	pEquip->~UpdatePosition( Max(pEquipOffset,-386) );

	if( pEquipOffset <= -386 )
	{
		pEquipOffset = -386;
		OnOpenedEquipment();
		return -1;
	}
}

protected func FxCloseEquipmentTimer( object pTarget, int iEffectNumber, int iEffectTime)
{
	pEquipOffset += 35;

	pEquip->~UpdatePosition( Min(pEquipOffset,0) );

	if( pEquipOffset >= 0 )
	{
		pEquipOffset = 0;
		OnOpenedMainMenu(1);
		return -1;
	}
}

//////////////////////////////////////////////////////////////////////////////////
//
// QuestLog-Funktion

public func OpenQuestLog()
{
	var QuestLog = pCommandObj->~GetQuestLogEx();
	if(!QuestLog) return;

	QuestLog->~Activate(pCommandObj);

}
