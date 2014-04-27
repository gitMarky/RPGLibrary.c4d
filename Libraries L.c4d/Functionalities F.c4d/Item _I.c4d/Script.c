/*-- Grundscript für alle Items --*/

#strict 2

// Ist ein Gegenstand

public func IsItem(){ return true; }

// Eigenschaften des Gegenstandes

public func ItemVolume(){ return 1; }
public func ItemHidden(){ return false; }

public func ContentsVolume()
{
	var contents,obj,i;

	contents=FindObjects(Find_Container(this));

	for( obj in contents )
	{

		if(obj->~IsItem())
			i+=(obj->~ItemVolume());
		else
			i+=GetMass(obj);
	}

	return i;
}

// Basis Info-Text bei Menus
public func GetInfoText( object pObj,int iLine, object pComp, bool fSkipDesc )
{
	if( iLine == 0 ) return GetDesc(this);
	return 0;
}

// Basis Benutzungs-Funktion für Activate() im RPG-Inventar, falls benutzbar
public func GetInventoryUseInfo()
{
	// Array von Arrays vom Format
	// [string szCaption, string szCommand, id idItem, int iCount, int iParameter, string szInfoCaption, int iExtra, any XPar1, any XPar2, string szConditions oder array ["Condition1","Condition2",...]]
	// Funktion wie in AddMenuItem(), nur ohne pMenuObject
	// Strings "pTarget" werden ersetzt durch den Benutzer des Objekts, "pItem" durch das Item
	//return[[GetInventoryActivateText(),"Activate(pTarget)",TALK,0,0,0,2,10,0,CanBeActivated()]];
	return[[GetInventoryActivateText(),"ObjectCall(pItem,\"Activate\",pTarget)",TALK,0,0,0,2,10,0,CanBeActivated()]];
}

public func GetInventoryActivateText(){ return "$MenuInvUse$"; }
public func CanBeActivated(){ return "false"; }
public func IsInteractiveItem(){ return false;} // not used
