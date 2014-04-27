/*--- Waffe ---*/

#strict 2
#include LF_I

/* Ist eine Handwaffe */
public func IsWeapon(){ return true;}

/* Kann geschmiedet werden */
public func IsAnvilProduct() { return(1); }

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
	var szAct, iRan;
	if( szStyle == "SwSh" )
	{
		if( !fPrimary ) return "Cancel"; // Schwert und Buckler, d.h. ich bin immer Primary
		iRan = Random(4);
		if( iRan == 0 ) szAct = "StrikeSw";
		if( iRan == 1 ) szAct = "SwingSw";
		if( iRan == 2 ) szAct = "LowSw";
		if( iRan == 3 ) szAct = "ThrustSw";
	}

	return Format("%s_%s",szStyle,szAct);
}

public func GetStrikeAction( string szStyle, bool fPrimary )
{
	var szAct, iRan;
	if( szStyle == "SwSh" )
	{
		if( !fPrimary ) return "Cancel"; // Schwert und Buckler, d.h. ich bin immer Primary
		iRan = Random(4);
		if( iRan == 0 ) szAct = "StrikeSw";
		if( iRan == 1 ) szAct = "SwingSw";
		if( iRan == 2 ) szAct = "LowSw";
		if( iRan == 3 ) szAct = "ThrustSw";
	}

	return Format("%s_%s",szStyle,szAct);
}

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


public func GetStrikePos( string szStyle )
{
	if( szStyle == "SpSh" ) return [ "P", "L" ];
	return ["P","R"];
}


public func GetStrikeStyles(){ return 1|2; }
public func GetStrikeDistance(){ return -2;}
public func GetStrikeDamage(){	return 8;}
