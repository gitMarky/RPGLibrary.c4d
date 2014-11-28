/*-- Base script for items
@title Item
@author Marky
@version 0.1.0
 --*/

#strict 2

/**
 * Identifies the object as an item.
 * @return bool Default value: true.
 */
public func IsItem(){ return true; }

// Item attributes

/**
 * The item occupies this much volume in a container.
 * @return int Default value: 1.
 */
public func ItemVolume(){ return 1; }
/**
 * Identifies the object as 'hidden' for the purpose of finding it in a container.
 * @return bool Default value: false
 * @note Overload this function to make a hidden object detectable.
 * @example TODO
 */
public func ItemHidden(){ return false; }

/**
 * TODO
 * @return The volume of all objects that this item contains.
 */
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

// Basic info text in Menus
/**
 * This is used to build the info text for this object.
 * @param pObj TODO
 * @param iLine A line parameter used by the object that parses the info text. 
 * @param pComp An object that this item will be compared to.
 * @param fSkipDesc Defines whether the item description should be skipped.
 * @return string As long as this function returns values other than 0 the parser will keep parsing. Verify, that there are no infinite loops caused by your code!
 */
public func GetInfoText( object pObj, int iLine, object pComp, bool fSkipDesc )
{
	if( iLine == 0 ) return GetDesc(this);
	return 0;
}

// basic functions for using the item with Activate() in the RPG-Inventory, if usable
/**
 * Tells the inventory what kind of interactions are possible with this object.
 * @return array An array of possible inventory interactions, see below. The default option reads "Use", see GetInventoryActivateText() TODO, and calls Activate() in the item, if the condition CanBeActivated() TODO is true. 
 * @note An inventory interaction is an array that consists of the following entries:<br>
 * [string szCaption, string szCommand, id idItem, int iCount, int iParameter, string szInfoCaption, int iExtra, any XPar1, any XPar2, any conditions]<br>
 * The parameters work exactly like AddMenuItem(), except that there is no pMenuObject.<br>
 * <br>
 * Conditions, any:<br>
 * A string or array of strings. This will be parsed by the Condition Library TODO. The interaction will be displayed in the inventory menu only if the condition is true.<br>
 * The string "pTarget" can be used a reference to the user of the item, "pItem" can be used as a reference to the item itself.
 * @example: TODO.
 */
public func GetInventoryUseInfo()
{
	// TODO: translate
	// Array von Arrays vom Format
	// [string szCaption, string szCommand, id idItem, int iCount, int iParameter, string szInfoCaption, int iExtra, any XPar1, any XPar2, string szConditions oder array ["Condition1","Condition2",...]]
	// Funktion wie in AddMenuItem(), nur ohne pMenuObject
	// Strings "pTarget" werden ersetzt durch den Benutzer des Objekts, "pItem" durch das Item
	//return[[GetInventoryActivateText(),"Activate(pTarget)",TALK,0,0,0,2,10,0,CanBeActivated()]];
	return[[GetInventoryActivateText(),"ObjectCall(pItem,\"Activate\",pTarget)",TALK,0,0,0,2,10,0,CanBeActivated()]];
}

/**
 * Standard string for interactions with the object.
 * @return string Returns a localized string $MenuInvUse$, per default this is "Use".
 * @note Gets passed to GetInventoryUseInfo() as the default interaction info. 
 */
public func GetInventoryActivateText(){ return "$MenuInvUse$"; }
/**
 * Determines whether the inventory options menu shows a "Use" option for this item.
 * @return bool Default value is false. Override this function if you want to be able to activate the item in the inventory menu. 
 */
public func CanBeActivated(){ return "false"; }

/**
 * Legacy, no idea what I had in mind there.
 * @ignore Well, better ignore it :)
 */
public func IsInteractiveItem(){ return false;}
