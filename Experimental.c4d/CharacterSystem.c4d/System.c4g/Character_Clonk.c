#strict 2
#appendto _CLN

local pComboMenu;
local pCharMenu;
local pBackpack;
local pSaveBuffer;
local pQuestLog;
local pWeapon1, pWeapon2, pWeapon3;
local aBeltItems;

local pHelmet, pArmor, pGreaves;
local pAmulet, pRing1, pRing2;
local pGloves, pBoots, pCloak;
local pShirt, pPants;

public func CanUseCharMenu(){ return 1; }
public func CanUseComboMenu(){ return 1; }
public func CanUseRingMenu(){ return 1; }

public func GetBackpack(){return pBackpack;}
public func GetQuestLogEx(){return pQuestLog;}


public func MaxContentsCount() { return 0; } // kann zwei Gegenstände tragen


public func ClonkViewRange()
{
	return(120 + BoundBy( daylight+daylight_override, 6, 100 )*2);
}

private func FxIntViewRangeTimer()
{
	SetPlrViewRange(ClonkViewRange());
}

public func HasItem( id idItem, int iCount )
{
	var obj;
	if(iCount)
	{
		var objects = FindObjects( Find_ID(idItem), Find_Or(Find_Container(pBackpack),Find_Container(this)) );

		if( GetLength(objects) >= iCount ) obj = objects;
	}
	else
	{
		if(pBackpack) obj = FindContents(idItem,pBackpack);
		if(!obj) obj = FindContents(idItem);
	}

	return obj;
}

//////////////////////////////////////////////////////////////////////////////////
//
// Engine-Aufrufe


public func Initialize()
{
	aBeltItems = [];
	AddEffect("IntViewRange",this,50,5,this);
	_inherited(...);
}


/* Bei Hinzufügen zu der Crew eines Spielers */
protected func Recruitment(int iPlr)
{
	// Charmenu erstellen
	if(!pCharMenu)
	{
		CreateCharMenu(this);
	}

	if( !pSaveBuffer )
	{
		pSaveBuffer = CreateObject(TIM1,0,0,-1);
		pSaveBuffer ->~ SetPosition(1,1);

		pQuestLog = CreateObject( LI_L,0,0,-1);
		Enter(pSaveBuffer,pQuestLog);
	}
		//pCharMenu = CreateObject( ID_CharMenu, 0, 0, -1 );
	// Alchemieregel: Jeder Clonk kriegt einen angelegten Beutel spendiert
	//if(ObjectCount(ALCO)) CreateObject(ALC_,0,0,-1)->~BelongTo(this());
	// Broadcast für Crew
	GameCallEx("OnClonkRecruitment", this(), iPlr);
}


/* Die Control-Steuerung */
private func Control2Effect(string szControl)
{

	// Combomenu benutzen
	if( pComboMenu )
	{
		var szInput = "";
		if( szControl == "ControlLeft" ) szInput = "L"; 
		if( szControl == "ControlLeftDouble" ) szInput = "LL"; 
		if( szControl == "ControlRight" ) szInput = "R"; 
		if( szControl == "ControlRightDouble" ) szInput = "RR"; 
		if( szControl == "ControlUp" ) szInput = "U"; 
		if( szControl == "ControlDown" ) szInput = "D"; 
		//if( szControl == "ControlDownSingle" ) szInput = "D"; 
		if( szControl == "ControlDownDouble" ) szInput = "DD"; 
		if( szControl == "ControlThrow" ) szInput = "P"; 
		if( szControl == "ControlDig" ) szInput = "S"; 
		if( szControl == "ControlCrew" ) szInput = "T"; 

		pComboMenu->~ComboInput( szInput );
		return 1;
	}
	if( fCombatMode ) return Control2Combat( szControl );

	if( szControl == "ControlThrow" )
	{
		var pInObj;
		if(GetAction() == "Push") pInObj = GetActionTarget();
		if(Contained()) pInObj = Contained();

		MenuInvOpen(pInObj);
		return 1;
	}

	if (Control2Contents(szControl)) return 1; 
	return _inherited(szControl);
}

/* Leitet die Steuerung an das erste Inhaltsobjekt weiter.	
	 Rückgabewert 1 wenn erfolgreich */ 
private func Control2Contents(szControl)
{ 
	// Getragenes Objekt hat spezielle Steuerungsauswertung 
	if (ObjectCall(Contents(),szControl, this) ) 
		return(1); 
	// Keine Inhaltssteuerung 
	return(0); 
} 


protected func ControlDigSingle()
{
	return 1;
}

		 
/* Leitet die Steuerung an ein ggf. gesteuertes Fahrzeug weiter. 
	 Rückgabewert 1 wenn erfolgreich */ 
 
private func Control2Vehicle(szControl) 
{ 	 
	// Keine Fahrzeugsteuerung 
	return 0; 
} 

/* Spezialtaste: Charmenu */
protected func ControlSpecial2()
{
	if( pCharMenu )
	{
		pCharMenu ->~ OpenMainMenu();
	}
}

protected func ControlSpecial2Double()
{
	if( pCharMenu )
	{
		pCharMenu ->~ OpenInventory();
	}
}


public func CrewSelection( bool fDeselect, bool fCursorOnly )
{
	if( fCursorOnly )
	{
		Control2Effect("ControlCrew");
	}
}


//////////////////////////////////////////////////////////////////////////////////
//
// Kontextmenu
