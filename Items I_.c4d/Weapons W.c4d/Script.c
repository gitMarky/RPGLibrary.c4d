/*--- Waffe ---*/

#strict 2
#include LF_I

/* Ist eine Handwaffe */
public func IsWeapon(){ return true;}
public func IsShield(){ return false;}

public func CanSwing( string szStyle ){ return true;}
public func CanThrust( string szStyle ){ return true;}
public func CanStrike( string szStyle ){ return true;}
public func CanLow( string szStyle ){ return true;}


/* Kann geschmiedet werden */
public func IsAnvilProduct() { return true; }

public func GetOverlayGraphics( object pObj )
{
	var gfx = "Walk";

	if( pObj )

	if( pObj )
	{
		var pos = pObj->~CombatIsWeaponDrawn( this );
		if( pos )
			gfx = Format("Fight%s",pos);
		else
		{
			pos = pObj->~CombatGetWeaponSlot( this );
			gfx = Format("Walk%s",pos);
		}
	}
	return gfx;
}

public func GetStrikeAction( string szStyle, bool fPrimary )
{
	var szAct = "Cancel", iRan, szSuffix;

	if( szStyle == "SpSh" && (GetStrikeStyles() & 1) )
	{
		if( !fPrimary ) 
		{
			if(!IsShield())
				return "Cancel"; // Schwert und Buckler, d.h. ich bin immer Primary
			else
				szSuffix = "Sh";
		}
		else
		{
			if(IsShield())
				return "Cancel";
			else
				szSuffix = "Sw";
		}
	}
	if( szStyle == "SwSh" && (GetStrikeStyles() & 2) )
	{
		if( !fPrimary ) 
			szSuffix = "S";
		else
			szSuffix = "P";
	}

	var aTypes = [];

	if( CanStrike( szStyle ) ) PushBack( "Strike", aTypes );
	if( CanThrust( szStyle ) ) PushBack( "Thrust", aTypes );
	if( CanSwing( szStyle ) ) PushBack("Swing",aTypes);
	if( CanLow( szStyle ) ) PushBack("Low",aTypes);


	szAct = Format("%s%s",aTypes[Random(GetLength(aTypes))],szSuffix);

	return Format("%s_%s",szStyle,szAct);
}
/*
public func GetBlockAction( string szStyle, bool fPrimary )
{
	var szAct, iRan;
	if( szStyle == "SwSh" )
	{
		if( !fPrimary ) return "Cancel"; // Schwert und Buckler, d.h. ich bin immer Primary
		szAct = "ParrySw";
	}

	return Format("%s_%s",szStyle,szAct);
}
*/
public func GetBlockAction( string szStyle, bool fPrimary )
{
	var szAct = "Parry", iRan, szSuffix;
	/*if( szStyle == "SwSh" )
	{
		if( !fPrimary ) return "Cancel"; // Schwert und Buckler, d.h. ich bin immer Primary
		szAct = "ParrySw";
	}*/

	if( szStyle == "SpSh" )
	{
		if( fPrimary )
			szSuffix = "Sw";
		else
			szSuffix = "Sh";
	}
	else
	{
		if( fPrimary )
			szSuffix = "P";
		else
			szSuffix =  "S";
	}

	return Format("%s_%s%s",szStyle,szAct,szSuffix);
}


public func GetStrikePos( string szStyle, bool fPrimary )
{
	if( szStyle == "SpSh" ) return [ "P", "L" ];

	if( fPrimary )
		return ["P","R"];
	else
		return ["S","L"];
}


public func GetStrikeStyles(){ return 1|2; }
public func GetStrikeDistance(){ return -2;}
public func GetStrikeDamage(){	return 8;}
