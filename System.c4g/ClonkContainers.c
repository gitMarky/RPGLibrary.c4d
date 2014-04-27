/*-- Container-Script:
 * Clonk kann aus Containern Dinge holen, ohne anfassen --*/

#strict 2
#appendto _CLN


// Objekte im falschen Layer können nie aufgenommen werden
// ansonsten Funktion überladen!!
global func IgnoreInventory( object pCollector)
{
	if(GetEffect("IgnoreInventory",this)) return true; // TODO: Effekt einbauen
	return pCollector->GetObjectLayer() != this->GetObjectLayer();
}

public func RejectCollect( id idObj, object pObj )
{

	if(GetCommand() == "Get" && GetCommand(0, 1) == pObj)
	{
		//MenuInvInteractWith( pObj);
		ScheduleCall(this,"MenuInvInteractWith",1,0,pObj);
		return 1;
	}

	return _inherited( idObj, pObj);
}

public func MenuInvOpen( object pInObj )
{
	RingCreateMenu( this );

	var loot, chests, i;
	i=0;
	//loot=FindObjects( Find_Distance(ClonkViewRange()),Find_And( Find_Container(pInObj), Find_OCF(OCF_Collectible)), Find_Not( Find_Func("IsHidden",this()) ));

	if( !pInObj )
		loot=FindObjects( Find_Distance(ClonkViewRange()),Find_OCF(OCF_NotContained), Find_OCF(OCF_Collectible), Find_Not(Find_Func("IsHidden",this)), Find_Not(Find_Func("IgnoreInventory",this)) );
	else
		loot=FindObjects( Find_Distance(ClonkViewRange()),Find_And(Find_Container(pInObj), Find_OCF(OCF_Collectible)), Find_Not(Find_Func("IsHidden",this)), Find_Not(Find_Func("IgnoreInventory",this)) );

	//loot=FindObjects( Find_Distance(ClonkViewRange()),Find_OCF(OCF_NotContained), Find_OCF(OCF_Collectible), Find_Not(Find_Func("IsHidden",this)) );
	chests=FindObjects( Find_Distance(ClonkViewRange()),Find_OCF(OCF_NotContained), Find_Or(Find_OCF(OCF_Container), Find_Func("IsContainer") ), Find_Not(Find_Func("IsHidden",this)), Find_Not(Find_Func("IgnoreInventory",this)), Find_Exclude(pInObj) );

	if(GetType(loot) == C4V_Array)
	for( obj in loot )
	{
		RingAddMenuItem( this, obj->GetID(), obj->GetGraphics(), obj->GetName(), "MenuInvCollect",obj,0,0,0,obj);
		i++;
	}
	if(GetType(chests) == C4V_Array)
	for( obj in chests )
	{
		var button = RingAddMenuItem( this, obj->GetID(), obj->GetGraphics(), Format("$MenuContainerSearch$",obj->GetName()), "MenuInvContainer",obj,0,0,0,obj);
		if(button)
		{
			var overlayID = 3;
			SetGraphics("",button,CXIN,overlayID,GFXOV_MODE_IngamePicture);
			SetObjDrawTransform(400,0,5000,0,400,5000,button,overlayID,0);
			//SetObjDrawTransform(600,0,0,0,600,0,button,overlayID);
		}
		i++;
	}



	RingSetSelectCall( this, "MenuInvHighlight" );
	if( pInObj ) RingSetCloseCall(this, "MenuInvOpen" );

	if(!i)
	{
		//ClonkMessage("$MenuInvNoTargets$",this);
		MessageEx("$MenuInvNoTargets$", this);
		ComboCloseMenu(this);
	}
}

public func MenuInvOpenRemote( object pCaller, object pInObj)
{
	MenuInvOpen( pInObj );
}

public func MenuInvHighlight( aButton )
{
	//[ button,call,par1,par2,par3,par4,0,0,0]

	if( aButton[1] == "MenuInvCollect" || aButton[1] == "MenuInvContainer" )
	{
		PlayerMessage(GetOwner(),"!",aButton[2]);
	}
}

public func MenuInvCollect( object pObj )
{
	if(!pObj) return;

	//AddCommand(this,"MoveTo",pObj,0,0,0,100,0,1);
	AddCommand(this,"Get",pObj,0,0,0,100,0,1);
	//AppendCommand(this,"Call",this,pObj,0,0,5,"MenuInvShowContents");

}

public func MenuInvContainer( object pObj )
{
	if(!pObj) return;

	if(GetOCF(pObj)&OCF_Grab)
	{
		AddCommand(this,"Grab",pObj,0,0,0,100,0,1);
		AppendCommand(this,"Call",this,pObj,0,0,5,"MenuInvOpenRemote");
	}
	else if(GetOCF(pObj)&OCF_Entrance)
	{
		AddCommand(this,"Enter",pObj,0,0,0,100,0,1);
		AppendCommand(this,"Call",this,pObj,0,0,5,"MenuInvOpenRemote");
	}
	else
	{
		AddCommand(this,"MoveTo",pObj,0,0,0,100,0,1);
		AppendCommand(this,"Call",this,pObj,0,0,5,"MenuInvOpenRemote");
	}
}



public func MenuInvInteractWith( object pObj, int iIsInventory )
{
	Message("",this);

	if(!pObj) return;

	var sz_info,i;
	var caption=Format("$MenuInvInteractWith$",GetName(pObj));

	var itemvolume;

	if( pObj->~IsItem() )
		itemvolume = pObj->~ItemVolume();
	else
		itemvolume = pObj->~GetMass();

	var menuIconItem,menuIconPack;
	if(pObj) menuIconItem = pObj->~GetMenuInvIcon();
	if(pBackpack) menuIconPack = pBackpack->~GetMenuInvIcon();

	if(!menuIconItem) menuIconItem = Format("{{%i}}",GetID(pObj));
	if(!menuIconPack) menuIconPack = Format("{{%i}}",GetID(pBackpack));

	if(iIsInventory==1 && pBackpack)
	{
		var available,free,volume;

		volume=pBackpack->~PackVolume(); free=pBackpack->~ContentsVolume();


		caption=Format("%s (%s %d %s %d/%d)", GetName(pBackpack),menuIconItem,itemvolume,menuIconPack,free,volume);

	}

	//CreateMenu( CXIV, this, this, 0, caption, 0, 131 );
	CreateMenu( MCCR, this, this, 0, caption, 0, 131 );

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	//AddMenuItem( "","",pObj->GetID(),this);
    //var szPortrait = Format("Portrait:%i::%x::%s", pObj->GetID(), pObj->GetClrModulation(), "");
    //AddMenuItem(szPortrait, "", NONE, this, 0, 0, "", C4MN_Add_ImgTextSpec, 0, 0);
	AddMenuItem("","",NONE,this,0,0,"",4,pObj);
	if( pObj->~IsItem() )
	{
		for(i = 0; pObj->~GetInfoText(this(),i) ; i++) AddMenuItem( pObj->~GetInfoText(this,i),"",0,this);
	}
	else
	{
		AddMenuItem( pObj->~GetDesc(),"",0,this);
	}

	//AddMenuItem( "","",0,this); // Leerzeile
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// benutzen
	var UseInfo = pObj->~GetInventoryUseInfo();
	if(GetType(UseInfo) == C4V_Array && GetLength(UseInfo)>0)
	for(var info in UseInfo)
	{

		var szCaption = info[0], szCommand = info[1], idItem = info[2], iCount = info[3], iParameter = info[4], szInfoCaption = info[5], iExtra = info[6], XPar1 = info[7], XPar2 = info[8], conditions = info[9];

		szCommand = ReplaceAll(szCommand,"pTarget",Format("Object(%d)",ObjectNumber(this)));
		szCommand = ReplaceAll(szCommand,"pItem", Format("Object(%d)",ObjectNumber(pObj)));

		if(CheckConditions(conditions,"pTarget",this,"pItem",pObj))
			AddMenuItem (szCaption, szCommand, idItem, this, iCount, iParameter, szInfoCaption, iExtra, XPar1, XPar2);
		//AddMenuItem( "$MenuInvUse$", Format("MenuInvUseItem(Object(%d))",ObjectNumber(pObj)), RMMG,this);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// In den Rucksack packen
	if(pBackpack)
	{
		if(iIsInventory != 1)
		{
			var available,free,volume;

			volume=pBackpack->~PackVolume(); free=pBackpack->~ContentsVolume();

			if( pObj->~ItemVolume() <= volume - free )
			{
				//if(pObj != pBackpack) AddMenuItem( Format( "$MenuInvPackTo$", GetName(pBackpack),menuIconItem,itemvolume,menuIconPack,free,volume), Format("PackToBackpack(Object(%d),Object(%d))",ObjectNumber(pObj),ObjectNumber(pBackpack)), GetID(pBackpack),this);//, 0, 0, 0, 4, pObj );
				//if(pObj != pBackpack) AddMenuItem( Format( "$MenuInvPackTo$",menuIconItem,itemvolume,menuIconPack,free,volume), Format("PackToBackpack(Object(%d),Object(%d))",ObjectNumber(pObj),ObjectNumber(pBackpack)), GetID(pBackpack),this);//, 0, 0, 0, 4, pObj );
				if(pObj != pBackpack) AddMenuItem( Format( "$MenuInvPackTo$",menuIconPack,free,ColorizeString(Format("+%d",itemvolume),RGB(150,150,150)),volume), Format("PackToBackpack(Object(%d),Object(%d))",ObjectNumber(pObj),ObjectNumber(pBackpack)), GetID(pBackpack),this, 0,0,0,4,pObj);//, 0, 0, 0, 4, pObj );
			}
			else
			{
				//if(pObj != pBackpack) AddMenuItem( ColorizeString(Format( "$MenuInvPackTo$", GetName(pBackpack),menuIconItem,itemvolume,menuIconPack,free,volume),RGB(255,0,0)), "",GetID(pBackpack),this);//, 0, 0, 0, 4, pObj );
				//if(pObj != pBackpack) AddMenuItem( ColorizeString(Format( "$MenuInvPackTo$", menuIconItem,itemvolume,menuIconPack,free,volume),RGB(255,0,0)), "",GetID(pBackpack),this);//, 0, 0, 0, 4, pObj );
				if(pObj != pBackpack) AddMenuItem( ColorizeString(Format( "$MenuInvPackTo$", menuIconItem,free,Format("+%d",itemvolume),volume),RGB(255,0,0)), "",GetID(pBackpack),this);//, 0, 0, 0, 4, pObj );
			}
		}
	}

	// Rucksack anlegen oder austauschen
	if( pObj->~IsBackpack() )
	{
			//if( pObj != pBackpack ) AddMenuItem( Format("$MenuInvUseBackpack$", pObj->GetName(), pObj->GetID(), pObj->~ContentsVolume(), pObj->~PackVolume(), ROCK, pObj->~ContentsCount(), (pObj->~IsBackpack())*5 ), "AddBackpack", LIGH, this, 0, pObj);//, 0, 4, pObj  );
			//if( pObj != pBackpack ) AddMenuItem( Format("$MenuInvUseBackpack$", pObj->GetID(), pObj->~ContentsVolume(), pObj->~PackVolume(), ROCK, pObj->~ContentsCount(), (pObj->~IsBackpack())*5 ), "AddBackpack", MCMC, this, 0, pObj);//, 0, 4, pObj  );
			//if( pObj != pBackpack ) AddMenuItem( Format("$MenuInvUseBackpack$", pObj->GetID(), pObj->~ContentsVolume(), pObj->~PackVolume(), ROCK, pObj->~ContentsCount(), (pObj->~IsBackpack())*5 ), "AddBackpack", MCMC, this, 0, pObj);//, 0, 4, pObj  );
			if( pObj != pBackpack ) AddMenuItem( Format("$MenuInvUseBackpack$", pObj->GetID(), pObj->~ContentsVolume(), pObj->~PackVolume(), "{{MCCR:6}}", pObj->~ContentsCount(), (pObj->~IsBackpack())*5 ), "AddBackpack", MCCR, this, 0, pObj, 0, 2, 15);//, 0, 4, pObj  );
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Waffe anlegen oder tauschen
	if( pObj->~IsWeapon() )
	{
		var str;
		if( pObj != pWeapon1 )
		{
			str = "$MenuInvEquipWeapon1$";
			if(pWeapon1) str = ColorizeString( str, RGB(70,70,238) );
			AddMenuItem( str ,Format("MenuInvEquipWeapon(Object(%d),1)",ObjectNumber(pObj)),pObj->~GetID(),this,0,0,0,4,pObj);
		}
		if( pObj != pWeapon2 )
		{
			str = "$MenuInvEquipWeapon2$";
			if(pWeapon2) str = ColorizeString( str, RGB(70,70,238) );
			AddMenuItem( str ,Format("MenuInvEquipWeapon(Object(%d),2)",ObjectNumber(pObj)),pObj->~GetID(),this,0,0,0,4,pObj);
		}
		if( pObj != pWeapon3 )
		{
			str = "$MenuInvEquipWeapon3$";
			if(pWeapon3) str = ColorizeString( str, RGB(70,70,238) );
			AddMenuItem( str ,Format("MenuInvEquipWeapon(Object(%d),3)",ObjectNumber(pObj)),pObj->~GetID(),this,0,0,0,4,pObj);
		}
	}

	// Items aus dem Sammeln heraus equippen
	var aItemList = [ "Helmet", "Armor", "Greaves", "Amulet", "Gloves", "Boots", "Cloak", "Shirt", "Pants" ];
	var szItem;

	for( szItem in aItemList )
	{
		if( ObjectCall( pObj, Format("Is%s",szItem)) && LocalN(Format("p%s",szItem))!=pObj )
			AddMenuItem(Format("$MenuInvChoose%s$",szItem),Format("MenuInvEquipItem(Object(%d),\"%s\")",ObjectNumber(pObj),szItem),pObj->~GetID(),this,0,0,0,4,pObj);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(iIsInventory)
	{
		var chests=FindObjects( Find_Distance(ClonkViewRange()), Find_OCF(OCF_Container) );
		var obj;

		AddMenuItem("$Drop$",Format("DropFromBackpack(%d)",ObjectNumber(pObj)),MCCR,this,0,0,0,2,14);
		for( obj in chests ) AddMenuItem(Format("$MenuInvDropIn$",GetName(obj)),Format("PlaceFromBackpack(%d,%d)",ObjectNumber(pObj),ObjectNumber(obj)),GetID(obj),this);
		AddMenuItem("$Cancel$","querycancel=0",MCMX,this);
	}
	else
	{
		 AddMenuItem("$Cancel$","MenuInvCollectCancel",MCMX,this);
	}
}






//////////////////////////////////////////////////////////////////////////////////
//
// Rucksack-Funktionen


public func PackToBackpack(object pObj, object pPack)
{
	if( pObj && pPack) Enter(pPack,pObj);

	MenuInvEquipmentDropCheck( pObj );

	if(!LocalN("szMenuStatus",pCharMenu))
		MenuInvOpen();
}

public func AddBackpack( par0, object pPack )
{
	if( pBackpack )
	{
		//Enter(this,pBackpack);
		//SetCommand(this,"Drop");
		MenuInvUnequipItem( pBackpack );

		if( pCharMenu )
		{
			if(LocalN( "szMenuStatus", pCharMenu ))
				pCharMenu->~CloseMainMenu();
		}
	}

	pBackpack = pPack;
	Enter( pSaveBuffer, pBackpack );
	if(pCharMenu) pCharMenu->~UpdateEquipmentButtons();

	if(LocalN("szMenuStatus",pCharMenu) == "inventory" )
		pCharMenu->~UpdateInventoryButtons();
	else if(!LocalN("szMenuStatus",pCharMenu) && par0 != "NoMenu")
		MenuInvOpen();

		DrawEquipmentOverlays();
}

public func GetBackpack(){ return pBackpack; }

public func DropFromBackpack(int iObj)
{
	var pObj = Object(iObj);

	Enter(this,pObj);
	SetCommand(this,"Drop");

	MenuInvEquipmentDropCheck( pObj );

	pCharMenu->~UpdateInventoryButtons();
}

public func PlaceFromBackpack(int iObj, int iPack)
{
}


//////////////////////////////////////////////////////////////////////////////////
//
// Ausrüstungs-Funktionen

// entfernt abgelegte Ausrüstung aus dem Speicher
public func MenuInvEquipmentDropCheck( pObj )
{
	if( pObj == pWeapon1 ) pWeapon1 = 0;
	if( pObj == pWeapon2 ) pWeapon2 = 0;
	if( pObj == pWeapon3 ) pWeapon3 = 0;
	if( pObj == pBackpack ) pBackpack = 0;
	if( pObj == pHelmet ) pHelmet = 0;
	if( pObj == pArmor ) pArmor = 0;
	if( pObj == pGreaves ) pGreaves = 0;
	if( pObj == pAmulet ) pAmulet = 0;
	if( pObj == pRing1 ) pRing1 = 0;
	if( pObj == pRing2 ) pRing2 = 0;
	if( pObj == pGloves ) pGloves = 0;
	if( pObj == pBoots ) pBoots = 0;
	if( pObj == pCloak ) pCloak = 0;
	if( pObj == pShirt ) pShirt = 0;
	if( pObj == pPants ) pPants = 0;

	if( pCharMenu )
	{
		pCharMenu->~UpdateEquipmentButtons();
	}

	DrawEquipmentOverlays();
}

public func MenuInvChooseWeapon( int iIndex, int iDummy )
{
	CreateMenu( ROCK, this, this, 0, "$MenuInvChooseWeapon$", 0, 1 );

	if( pBackpack )
	{
		var weapons, item;
		
		weapons = FindObjects( Find_Container( pBackpack ), Find_Func("IsWeapon") );

		for( item in weapons )
		{
			AddMenuItem("$MenuInvEquip$",Format("MenuInvEquipWeapon(Object(%d),%d)",ObjectNumber(item),iIndex),item->~GetID(),this,0,0,0,4,item);
		}
	}

	AddMenuItem("$Cancel$","GetX()",MCMX,this);
}

public func MenuInvEquipWeapon( object pObj, int iIndex )
{
	if(!pSaveBuffer) return;

	Enter(pSaveBuffer,pObj);

	// alte Waffe ablegen
	if( LocalN(Format("pWeapon%d",iIndex)) )
	{
		MenuInvUnequipItem( LocalN(Format("pWeapon%d",iIndex)) );
	}

	// falls es schon eine andere Waffe ist, diese verwerfen
	if( pObj == pWeapon1 ) pWeapon1 = 0;
	if( pObj == pWeapon2 ) pWeapon2 = 0;
	if( pObj == pWeapon3 ) pWeapon3 = 0;


	LocalN(Format("pWeapon%d",iIndex))=pObj;

	if(pCharMenu) pCharMenu->~UpdateEquipmentButtons();
	DrawEquipmentOverlays();

	if(!LocalN("szMenuStatus",pCharMenu))
		MenuInvOpen();
}

public func MenuInvChooseItem( string szItem, id idSymbol )
{
	CreateMenu( idSymbol, this, this, 0, Format("$MenuInvChoose%s$",szItem), 0, 1 );
	if( pBackpack )
	{
		var list, item;
		
		list = FindObjects( Find_Container( pBackpack ), Find_Func(Format("Is%s",szItem)) );

		for( item in list )
		{
			AddMenuItem("$MenuInvEquip$",Format("MenuInvEquipItem(Object(%d),\"%s\")",ObjectNumber(item),szItem),item->~GetID(),this,0,0,0,4,item);
		}
	}
	AddMenuItem("$Cancel$","GetX()",MCMX,this);
}

public func MenuInvEquipItem( object pObj, string szItem)
{
	if(!pSaveBuffer) return;

	if( LocalN(Format("p%s",szItem)) )
	{
		MenuInvUnequipItem( LocalN(Format("p%s",szItem)) );
		//Enter(this, LocalN(Format("p%s",szItem))  );
		//SetCommand(this,"Drop");
	}

	Enter(pSaveBuffer,pObj);

	LocalN(Format("p%s",szItem))=pObj;

	if(pCharMenu) pCharMenu->~UpdateEquipmentButtons();
	DrawEquipmentOverlays();

	if(!LocalN("szMenuStatus",pCharMenu))
		MenuInvOpen();

	CheckForceMove(pObj->~GetPutOnAction(),true);
}

public func MenuInvChooseBackpack()
{
	CreateMenu( ROCK, this, this, 0, "$MenuInvChooseBackpack$", 0, 1 );

	if( pBackpack )
	{
		var packs, pObj;
		
		packs = FindObjects( Find_Container( pBackpack ), Find_Func("IsBackpack") );

		for( pObj in packs )
		{
			AddMenuItem( Format("$MenuInvUseBackpack$", pObj->GetName(), pObj->GetID(), pObj->~ContentsVolume(), pObj->~PackVolume(), ROCK, pObj->~ContentsCount(), (pObj->~IsBackpack())*5 ), "AddBackpack", pObj->~GetID(), this, 0, pObj);//, 0, 4, pObj  );
		}
	}

	AddMenuItem("$Cancel$","GetX()",MCMX,this);
}

public func MenuInvUnequipItem( object pObj )
{
	
	var available,free,volume;

	if(pBackpack)
	if( pObj != pBackpack )
	{
		volume=pBackpack->~PackVolume();
		free=pBackpack->~ContentsVolume();
		available=(pBackpack->~IsBackpack())*5 - ContentsCount(0,pBackpack);
	}

	if( pObj->~ItemVolume() <= (volume - free) && available )
		Enter( pBackpack, pObj );
	else
	{
		Enter(this, pObj );
		SetCommand(this,"Drop");
	}
}

public func MenuInvUseItem(object pObj)
{
	pObj->~Activate(this);
}
