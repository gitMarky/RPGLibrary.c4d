#strict 2
#appendto _CLN

local szMoveOverride;
local fCombatMode, fCombatEnds;
local szWeapon1Drawn, szWeapon2Drawn, szWeapon3Drawn;
local fWeapon1Sheathe, fWeapon2Sheathe, fWeapon3Sheathe;
local pWeaponP, pWeaponS; // Hauptwaffe, Zweitwaffe
local pWeaponDraw; // Waffe, die mit Verzögerung gezogen werden soll
local fWeaponDraw; // soll bei der Verzögerung autokombiniert werden, oder nicht?
//local iWeaponPPos, iWeaponSPos; // Position: 0 = linke Hand, 1 = rechte Hand
//local szWpPKey, szWpSKey; // Mapping: mit welcher Taste wird welche Waffe bedient?

/* Kampfstile

szStyle		Bit		Beschreibung
SpSh		1		Speer & Schild, oder Schwert & Buckler: Schild weit vorne, Hauptwaffe hinten
SwSh		2		Schwert & Schild: kann für alles benutzt werden, auch Handkampf
TwAn		4		Schwerter, die im Anderhalthandstil geführt werden können
TwSp		8		Zweihandspeer
TwBl		16		Zweihandhiebwaffen
TwSt		32		Zweihandstab

manche Waffen haben mehrere Stile, für das Autokombinieren bei Kampfstart

*/



//////////////////////////////////////////////////////////////////////////////////
//
// Bewegungs-Modus

protected func CheckWalk()
{
	CheckForceMove( "Walk" );
}

protected func CheckJump()
{
	CheckForceMove( "Jump" );
}

// Mit gezogener Waffe kann man nur kleine Stufen raufklettern
protected func Scaling()
{
	if( szMoveOverride && GetActTime())
		return CheckForceMove("Jump" );   

	_inherited(...);
}

protected func CheckForceMove( string szAction, bool fAlways )
{
	if( szMoveOverride )
	{
		var szNew = Format("%s_%s",szMoveOverride, szAction );
		if(GetAction() != szNew ) SetAction( szNew );
	}
	else if( fAlways )
	{
		if(GetAction() != szAction ) SetAction( szAction );
	}
}



//////////////////////////////////////////////////////////////////////////////////
//
// Waffe ziehen und wegstecken

/* Spezialtaste: Waffe ziehen */ 
protected func ControlSpecial() 
{ 
	[$CtrlInventoryDesc$|Image=CXIV]

	// Kampfmodus starten

	if( fCombatMode )
	{
		if( pComboMenu )
			ComboCloseMenu(this);
		else
		{
			ComboCreateMenu( this );

			if(pWeapon1)
			{
				if( CombatIsWeaponDrawn( pWeapon1 ) )
					ComboAddMenuItem( this, "P", pWeapon1->~GetID(), pWeapon1->GetName(), "CombatSheatheWeapon",pWeapon1,szMoveOverride,CombatGetWeaponPos( pWeapon1 ));
				else
					ComboAddMenuItem( this, "P", pWeapon1->~GetID(), pWeapon1->GetName(), "CombatDrawWeaponDelayed",pWeapon1);
			}
			if(pWeapon2)
			{
				if( CombatIsWeaponDrawn( pWeapon2 ) )
					ComboAddMenuItem( this, "U", pWeapon2->~GetID(), pWeapon2->GetName(), "CombatSheatheWeapon",pWeapon2,szMoveOverride,CombatGetWeaponPos( pWeapon2 ));
				else
					ComboAddMenuItem( this, "U", pWeapon2->~GetID(), pWeapon2->GetName(), "CombatDrawWeaponDelayed",pWeapon2);
			}
			if(pWeapon3)
			{
				if( CombatIsWeaponDrawn( pWeapon3 ) )
					ComboAddMenuItem( this, "S", pWeapon3->~GetID(), pWeapon3->GetName(), "CombatSheatheWeapon",pWeapon3,szMoveOverride,CombatGetWeaponPos( pWeapon3 ));
				else
					ComboAddMenuItem( this, "S", pWeapon3->~GetID(), pWeapon3->GetName(), "CombatDrawWeaponDelayed",pWeapon3);
			}
			ComboAddMenuItem( this, "D", MCMX, "$CombatSheatheAll$", "CombatLeaveCombat");

		}
		//fCombatMode = false;
		//CombatLeaveCombat();
	}
	else
	{
		if(CombatEnterCombat()) fCombatMode = true;
	}

	return 1; 
} 


protected func CombatDrawWeapon( object pWeapon, string szStyle, array szPos, object pCompare )
{

		szMoveOverride = szStyle;
/*
	if( pWeapon->~IsTwoHanded() || !fCombatMode )
		CheckForceMove( "Draw" );
	else
	{
		var szMyPos = CombatGetWeaponSlot( pWeapon );
		var szCpPos, szDrPos;

		if( pCompare )
			szCpPos = CombatGetWeaponSlot( pCompare );
		else
			szCpPos = "0";

		if( szPos[1] == "R" )
			szDrPos = Format("Draw%s%s",szMyPos, szCpPos );
		else
			szDrPos = Format("Draw%s%s",szCpPos, szMyPos ); 

		Log("Drawing %s", szDrPos);
		CheckForceMove( szDrPos );
	}
*/
	var szMyPos = CombatGetWeaponSlot( pWeapon );
	var szCpPos, szDrPos;

	if( pWeapon->~IsTwoHanded() || fCombatMode )
			szCpPos = "0";
	else
	{
		if( pCompare )
			szCpPos = CombatGetWeaponSlot( pCompare );
		else
			szCpPos = "0";
	}
	
		if( szPos[1] == "R" )
			szDrPos = Format("Draw%s%s",szMyPos, szCpPos );
		else
			szDrPos = Format("Draw%s%s",szCpPos, szMyPos ); 

		Log("Drawing %s", szDrPos);
		CheckForceMove( szDrPos );

		//fCombatMode = true;

		LocalN(Format("pWeapon%s",szPos[0]))=pWeapon;

		if( pWeapon == pWeapon1 )
		{
			szWeapon1Drawn = szPos[1];
		}
		if( pWeapon == pWeapon2 )
		{
			szWeapon2Drawn = szPos[1];
		}
		if( pWeapon == pWeapon3 )
		{
			szWeapon3Drawn = szPos[1];
		}
		DrawEquipmentOverlays();
	//}
}

public func CombatSheatheWeapon( object pWeapon, string szStyle, string szPos, object pCompare )
{
	Log("Sheathe %s", szPos);
	if( !fCombatEnds )
	{
		//SetAction(  );
		var szMyPos = CombatGetWeaponSlot( pWeapon );
		var szCpPos, szDrPos;

			szCpPos = "0";

		if( szStyle == "SpSh" )
		{
			if( szPos == "S" )
				szDrPos = Format("Sheathe%s%s",szMyPos, szCpPos );
			else
				szDrPos = Format("Sheathe%s%s",szCpPos, szMyPos );
		}
		else
		{
			if( szPos == "P" )
				szDrPos = Format("Sheathe%s%s",szMyPos, szCpPos );
			else
				szDrPos = Format("Sheathe%s%s",szCpPos, szMyPos ); 
		}

		Log("Sheathe %s", szDrPos);

		CheckForceMove( szDrPos ); //Format("Sheathe%s",szPos));
		if(szPos == "P" || szPos == "S" )
		{
			Log("%s = 0", szPos);
			LocalN(Format("pWeapon%s",szPos))=0;
		}
		else // beide wegstecken
		{
			LocalN("pWeaponP")=0;
			LocalN("pWeaponS")=0;
		}
	}
	else
	{
		var szMyPos = CombatGetWeaponSlot( pWeapon );
		var szCpPos, szDrPos;

		if( pCompare )
			szCpPos = CombatGetWeaponSlot( pCompare );
		else
			szCpPos = "0";

		if( szStyle == "SpSh" )
		{
			if( szPos == "S" )
				szDrPos = Format("Sheathe%s%s",szMyPos, szCpPos );
			else
				szDrPos = Format("Sheathe%s%s",szCpPos, szMyPos );
		}
		else
		{
			if( szPos == "P" )
				szDrPos = Format("Sheathe%s%s",szMyPos, szCpPos );
			else
				szDrPos = Format("Sheathe%s%s",szCpPos, szMyPos ); 
		}

		Log("SheatheFinal %s", szDrPos);

		// noch nicht im Kampfmodus? D.h. wir brauchen eine Animation zum ziehen
		CheckForceMove( szDrPos );//"Sheathe");
		szMoveOverride = 0;
		//fCombatMode = true;

		LocalN(Format("pWeapon%s",szPos))=0;
		if(pCompare) LocalN(Format("pWeapon%s",CombatGetWeaponPos( pCompare )))=0;
		//DrawEquipmentOverlays();
	}

	if( pWeapon == pWeapon1 || pCompare == pWeapon1 )
	{
		//Schedule("LocalN(\"szWeapon1Drawn\",this)=0",48);
		fWeapon1Sheathe = true;
	}
	if( pWeapon == pWeapon2 || pCompare == pWeapon2 )
	{
		//szWeapon2Drawn = 0;
		//Schedule("LocalN(\"szWeapon2Drawn\",this)=0",48);
		fWeapon2Sheathe = true;
	}
	if( pWeapon == pWeapon3 || pCompare == pWeapon3 )
	{
		//szWeapon3Drawn = 0;
		//Schedule("LocalN(\"szWeapon3Drawn\",this)=0",48);
		fWeapon3Sheathe = true;
	}
}

protected func CombatWeaponSheathed()
{

	if( fWeapon1Sheathe )
	{
		szWeapon1Drawn = 0;
		fWeapon1Sheathe = false;
	}
	if( fWeapon2Sheathe )
	{
		szWeapon2Drawn = 0;
		fWeapon2Sheathe = false;
	}
	if( fWeapon3Sheathe )
	{
		szWeapon3Drawn = 0;
		fWeapon3Sheathe = false;
	}

	if( fCombatEnds )
	{
		fCombatMode = false;
		fCombatEnds = false;
	}
	else
	{/*
		if( pWeaponDraw )
		{
			if( pWeaponDraw == pWeapon1 ) CombatDrawWeaponCombined( pWeapon1, pWeapon2, pWeapon3, fWeaponDraw );
			if( pWeaponDraw == pWeapon2 ) CombatDrawWeaponCombined( pWeapon2, pWeapon3, pWeapon1, fWeaponDraw );
			if( pWeaponDraw == pWeapon3 ) CombatDrawWeaponCombined( pWeapon3, pWeapon1, pWeapon2, fWeaponDraw );

			pWeaponDraw = 0;
			fWeaponDraw = false;

		}*/

		if( pWeaponDraw ) CombatDrawWeaponDelayed( pWeaponDraw );
	}

	DrawEquipmentOverlays();
}

protected func CombatEnterCombat()
{
	if( pWeapon1 ) return CombatDrawWeaponCombined( pWeapon1, pWeapon2, pWeapon3, true );
	if( pWeapon2 ) return CombatDrawWeaponCombined( pWeapon2, pWeapon3, pWeapon1, true );
	if( pWeapon3 ) return CombatDrawWeaponCombined( pWeapon3, pWeapon1, pWeapon2, true );

	// waffenlosen Kampf gibt es noch nicht
	return false;
}

public func CombatLeaveCombat()
{
	fCombatEnds = true;
	if( pWeaponP ) CombatSheatheWeapon( pWeaponP, szMoveOverride, "P", pWeaponS );
	if( pWeaponS ) CombatSheatheWeapon( pWeaponS, szMoveOverride, "S", pWeaponP );

	// wenn er nix wegstecken kann (waffenloser Kampf)
	if(GetProcedure() == "WALK" || GetProcedure() == "FLIGHT")
	{
		fCombatEnds = false;
		fCombatMode = false;
		szMoveOverride = 0;
	}
}

protected func CombatDrawWeaponCombined( object pW1, object pW2, object pW3, bool fAutoCombine )
{
	var style = "SwSh";
	if( szMoveOverride ) style = szMoveOverride;
	//var pos = ["P", "R"];

	var pos0a = "P";
	var pos0b = "R";
	if( pWeaponP )
	{
		pos0a = "S";
		if(CombatIsWeaponDrawn( pWeaponP ) == "R" ) pos0b = "L";
	}

	var pos = [ pos0a, pos0b ];

	if( pW1 ->~ IsTwoHanded() )
	{
		CombatDrawWeapon( pW1, style, pos, 0 );
	}
	else
	{
		if((!pW2 && !pW3) || !fAutoCombine )
		{
			CombatDrawWeapon( pW1, style, pos, 0 );
			Log("DWCombine Standard");
		}
		else
		{
			var rating2, rating3;
			var style2, pos2a = [], pos2b = [];
			var style3, pos3a = [], pos3b = [];

			if( pW2 ) rating2 = CombatWeaponRating( pW1, pW2, style2, pos2a, pos2b );
			if( pW3 ) rating3 = CombatWeaponRating( pW1, pW3, style3, pos3a, pos3b );

			//Log("Found more than 1 weapon!");
			//if(pW2) Log("Weapon2: %s - Rating: %d, Style: %s", GetName(pW2),rating2);

			if( rating2 >= rating3 )
			{
				if( rating2 )
				{
					if(!CombatIsWeaponDrawn(pW1)) CombatDrawWeapon( pW1, style2, pos2a, pW2 );
					if(!CombatIsWeaponDrawn(pW2)) CombatDrawWeapon( pW2, style2, pos2b, pW1 );
				}
				else
				{
					CombatDrawWeapon( pW1, style, pos, 0 );
				}
			}
			else
			{
				if( rating3 )
				{
					if(!CombatIsWeaponDrawn(pW1)) CombatDrawWeapon( pW1, style3, pos3a, pW2 );
					if(!CombatIsWeaponDrawn(pW2)) CombatDrawWeapon( pW2, style3, pos3b, pW1 );
				}
				else
				{
					CombatDrawWeapon( pW1, style, pos, 0 );
				}
			}
		}
	}

	return true;
}

public func CombatDrawWeaponDelayed( object pWeapon )
{
	Log("DrawDelayed");
	if( pWeapon->~IsTwoHanded() )
	{
		// erst alles wegstecken
		if(CombatIsWeaponDrawn( pWeapon1 ) || CombatIsWeaponDrawn(pWeapon2) || CombatIsWeaponDrawn(pWeapon3) )
		{
			CombatSheatheWeapon( pWeapon1, szMoveOverride, "" );
			CombatSheatheWeapon( pWeapon2, szMoveOverride, "" );
			CombatSheatheWeapon( pWeapon3, szMoveOverride, "" );
			pWeaponDraw = pWeapon;
		}
		else
		{
			CombatDrawWeaponCombined( pWeapon, 0, 0, false );
			pWeaponDraw = 0;
		}
	}
	else
	{
		if( pWeaponP && !pWeaponS )
		{
			var rating, style, pos1 = [], pos2 = [];

			rating = CombatWeaponRating( pWeaponP, pWeapon, style, pos1, pos2 );

			if( rating && style == szMoveOverride )
			{
				CombatDrawWeaponCombined( pWeaponP, pWeapon, 0, true);
				pWeaponDraw = 0;
				return;
			}
			else
			{
				if( CombatIsWeaponDrawn( pWeaponP ) )
				{
					CombatSheatheWeapon( pWeaponP, szMoveOverride, "P" );
					pWeaponDraw = pWeapon;
					return;
				}
				else
				{
					CombatDrawWeaponCombined( pWeapon, 0, 0, false );
					pWeaponDraw = 0;
					return;
				}
			}
		}
		if( pWeaponS && !pWeaponP )
		{
			var rating, style, pos1 = [], pos2 = [];

			rating = CombatWeaponRating( pWeapon, pWeaponS, style, pos1, pos2 );

			if( rating && style == szMoveOverride )
			{
				CombatDrawWeaponCombined( pWeapon, pWeaponS, 0, true);
				pWeaponDraw = 0;
				return;
			}
			else
			{
				if( CombatIsWeaponDrawn( pWeaponS ) )
				{
					CombatSheatheWeapon( pWeaponS, szMoveOverride, "S" );
					pWeaponDraw = pWeapon;
					return;
				}
				else
				{
					CombatDrawWeaponCombined( pWeapon, 0, 0, false );
					pWeaponDraw = 0;
					return;
				}
			}
		}
		if( !pWeaponP && !pWeaponS )
		{
			Log("NoWeaponsWhatsoever");
			CombatDrawWeaponCombined( pWeapon, 0, 0, false );
			pWeaponDraw = 0;
			return;
		}
		if( pWeaponP && pWeaponS )
		{
			Log("both weapons drawn");
			var rating2, rating3;
			var style2, pos2a = [], pos2b = [];
			var style3, pos3a = [], pos3b = [];

			rating2 = CombatWeaponRating( pWeaponP, pWeapon, style2, pos2a, pos2b );
			rating3 = CombatWeaponRating( pWeapon, pWeaponS, style3, pos3a, pos3b );

			if( rating2 && style2 == szMoveOverride ) rating2++;
			if( rating3 && style3 == szMoveOverride ) rating3++;

			// das bessere auswählen
			if( rating3 >= rating2 ) rating2 = 0;
			else rating3 = 0;

			Log("rating P %d, S %d", rating2, rating3);

			if( rating3 )
			{
				if( CombatIsWeaponDrawn( pWeaponP ) )
				{
					CombatSheatheWeapon( pWeaponP, szMoveOverride, "P" );
					pWeaponDraw = pWeapon;
					return;
				}
				else
				{
					Log("this should not apply!");
					CombatDrawWeaponCombined( pWeapon, pWeaponS, 0, false );
					pWeaponDraw = 0;
					return;
				}
			}
			if( rating2 )
			{
				if( CombatIsWeaponDrawn( pWeaponS ) )
				{
					CombatSheatheWeapon( pWeaponS, szMoveOverride, "S" );
					pWeaponDraw = pWeapon;
					return;
				}
				else
				{
					Log("this should not apply, too!");
					CombatDrawWeaponCombined( pWeaponP, pWeapon, 0, false );
					pWeaponDraw = 0;
					return;
				}
			}
			if( !rating2 && !rating3 )
			{
				// erst alles wegstecken
				if(CombatIsWeaponDrawn( pWeapon1 ) || CombatIsWeaponDrawn(pWeapon2) || CombatIsWeaponDrawn(pWeapon3) )
				{
					CombatSheatheWeapon( pWeapon1, szMoveOverride, "" );
					CombatSheatheWeapon( pWeapon2, szMoveOverride, "" );
					CombatSheatheWeapon( pWeapon3, szMoveOverride, "" );
					pWeaponDraw = pWeapon;
				}
				else
				{
					CombatDrawWeaponCombined( pWeapon, 0, 0, false );
					pWeaponDraw = 0;
				}
			}
		}
	}
}

public func CombatIsWeaponDrawn( object pObj )
{
	if( pObj == pWeapon1 ) return szWeapon1Drawn;
	if( pObj == pWeapon2 ) return szWeapon2Drawn;
	if( pObj == pWeapon3 ) return szWeapon3Drawn;
	return 0;
	//return ( pObj == pWeapon1 && fWeapon1Drawn ) || ( pObj == pWeapon2 && fWeapon2Drawn ) || ( pObj == pWeapon3 && fWeapon3Drawn );
}

public func CombatGetWeaponSlot( object pObj )
{
	if( pObj->~IsShield() )
	if( pObj == pWeapon1 || pObj == pWeapon2 || pObj == pWeapon3 )
		return "S";
	if( pObj == pWeapon1 ) return "1";
	if( pObj == pWeapon2 ) return "2";
	if( pObj == pWeapon3 ) return "3";
	return "0";
	//return ( pObj == pWeapon1 && fWeapon1Drawn ) || ( pObj == pWeapon2 && fWeapon2Drawn ) || ( pObj == pWeapon3 && fWeapon3Drawn );
}

public func CombatGetWeaponPos( object pObj )
{
	if( pObj == pWeaponP ) return "P";
	if( pObj == pWeaponS ) return "S";
	return 0;
}

public func CombatWeaponRating( object pW1, object pW2, string &szStyle, array &szPosA, array &szPosB)
{
	var rating, styles;

	styles = pW1->~GetStrikeStyles() & pW2->~GetStrikeStyles();
	if( styles ) // sie sind kompatibel
	{
		rating = 1;

		// den gemeinsamen Stil prüfen
		if( styles == 1 )  szStyle = "SpSh";
		if( styles == 2 )  szStyle = "SwSh";
		if( styles == 4 )  szStyle = "TwAn";
		if( styles == 8 )  szStyle = "TwSp";
		if( styles == 16 ) szStyle = "TwBl";
		if( styles == 32 ) szStyle = "TwSt";

		// TODO: falls mehr als ein Stil möglich ist: sinnvollen Stil raussuchen
		if(!szStyle)
		{
			szStyle = "SwSh";
		}

		szPosA = pW1->~GetStrikePos( szStyle, true );
		szPosB = pW2->~GetStrikePos( szStyle, false );

		if( szPosA[0] == szPosB[0] ) rating = 0;
	}

	return rating;
}



//////////////////////////////////////////////////////////////////////////////////
//
// Kampfsteuerung

public func Control2Combat( string szControl )
{
		if( szControl == "ControlLeft" )
		{
		}
		if( szControl == "ControlLeftDouble" )
		{
		}
		if( szControl == "ControlRight" )
		{
		}
		if( szControl == "ControlRightDouble" )
		{
		}
		if( szControl == "ControlUp" )
		{
		}
		if( szControl == "ControlDown" )
		{
		}
		if( szControl == "ControlDownDouble" )
		{
		}
		if( szControl == "ControlThrow" )
		{
			if( pWeaponP ) return CombatStartAttack( pWeaponP, true );
		}
		if( szControl == "ControlDig" )
		{
			if( pWeaponS ) return CombatStartAttack( pWeaponS, false );
		}
		if( szControl == "ControlCrew" )
		{
		}
}

protected func CombatStartAttack( object pWeapon, bool fPrimary )
{
	if(!pWeapon) return false;
	if(!CombatCanAct()) return false;

	var szAct = pWeapon->~GetStrikeAction( szMoveOverride, fPrimary );
	var szSpeed = 2;

	if( szAct == "Cancel" ) return false;

	if( GetXDir() && GetProcedure() == "WALK" )
		SetAction( Format("%s_Walk",szAct) );
	else
		SetAction( szAct );
	//AddEffect( "CombatHitDetection",this, 300, 1, this, 0, pWeapon, 7*szSpeed, szAct );
	AddEffect( "CombatHitDetection",this, 300, 1, this, 0, pWeapon, 5*szSpeed, szAct );
	return true;
}

protected func CombatCanAct()
{
	if( GetProcedure() != "WALK" && GetProcedure() != "FLIGHT" ) return false;
	if( GetProcedure() == "WALK" && 
		GetAction() != "Walk" &&
		GetAction() != "SpSh_Walk" &&
		GetAction() != "SwSh_Walk" ) return false;
	if( GetAction() == "Tumble" ) return false;
	return true;
}



//////////////////////////////////////////////////////////////////////////////////
//
// Trefferhandling

protected func FxCombatHitDetectionStart( object pTarget, int iEffectNumber, int iTemp, var1, var2, var3, var4)
{
	EffectVar(0,pTarget,iEffectNumber) = var1; // weapon
	EffectVar(1,pTarget,iEffectNumber) = var2; // strike time
	EffectVar(2,pTarget,iEffectNumber) = var3; // action
	EffectVar(3,pTarget,iEffectNumber) = []; // Array bereits getroffener Gegner

	var distance = 24+EffectVar(0,pTarget,iEffectNumber)->~GetStrikeDistance();
	var targets = FindObjects( Find_InRect(-distance+(distance+8)*GetDir(),-10,distance-8,20), Find_Exclude(pTarget), Find_OCF(OCF_Living)  );
	var guy;
	//Log("Rect = %d,%d",-distance+(distance+8)*GetDir(),distance-8);

	for( guy in targets )
	{
		// Defense Check
		guy->~CombatDefenseCheck( pTarget, var1, var2 +4 ); 
	}

}
/*
protected func FxCombatHitDetectionTimer( object pTarget, int iEffectNumber, int iEffectTime )
{
	if(GetAction() != EffectVar(2,pTarget,iEffectNumber) && GetAction() != Format("%s_Walk",EffectVar(2,pTarget,iEffectNumber)) ) return -1;

	// damit er weiterschlägt, wenn er stehenbleibt
	if( GetAction() == Format("%s_Walk",EffectVar(2,pTarget,iEffectNumber)) && !(iEffectTime%2) && !GetXDir() )
	{
		var phase = GetPhase();
		SetAction( EffectVar(2,pTarget,iEffectNumber) );
		SetPhase(phase);
	}

	if(iEffectTime > EffectVar(1,pTarget,iEffectNumber)*2+2) return-1;
	if(iEffectTime > EffectVar(1,pTarget,iEffectNumber)+2) return;
	if(iEffectTime < EffectVar(1,pTarget,iEffectNumber)) return;

	var guy;
	var distance = 24+EffectVar(0,pTarget,iEffectNumber)->~GetStrikeDistance();
	//var targets = FindObjects( Find_Distance(24+distance), Find_Not(Find_Allied()), Find_Exclude(pTarget),  );
	var targets = FindObjects( Find_InRect(-distance+(distance+8)*GetDir(),-10,distance-8,20), Find_Exclude(pTarget), Find_OCF(OCF_Living)  );

	for( guy in targets )
	{
		if( GetArrayItemPosition(guy,EffectVar(3,pTarget,iEffectNumber)) < 0)
		{
			PushBack(guy,EffectVar(3,pTarget,iEffectNumber));

			if( guy->~CombatCanBlock( EffectVar(0,pTarget,iEffectNumber) ) )
			{
				Log("Blocked");
			}
			else
				DoDmg(EffectVar(0,pTarget,iEffectNumber)->~GetStrikeDamage(), DMG_Melee, guy, 0, GetOwner()+1);
		}
	}
}
*/
protected func FxCombatHitDetectionTimer( object pTarget, int iEffectNumber, int iEffectTime )
{
	if(GetAction() != EffectVar(2,pTarget,iEffectNumber) && GetAction() != Format("%s_Walk",EffectVar(2,pTarget,iEffectNumber)) ) return -1;

	// damit er weiterschlägt, wenn er stehenbleibt
	if( GetAction() == Format("%s_Walk",EffectVar(2,pTarget,iEffectNumber)) && !(iEffectTime%2) && !GetXDir() )
	{
		var phase = GetPhase();
		SetAction( EffectVar(2,pTarget,iEffectNumber) );
		SetPhase(phase);
	}

	if(iEffectTime > EffectVar(1,pTarget,iEffectNumber)*2+2) return-1;
	//if(iEffectTime > EffectVar(1,pTarget,iEffectNumber)+2) return;
	//if(iEffectTime < EffectVar(1,pTarget,iEffectNumber)) return;

	var guy;
	var distance = 24+EffectVar(0,pTarget,iEffectNumber)->~GetStrikeDistance();
	//var targets = FindObjects( Find_Distance(24+distance), Find_Not(Find_Allied()), Find_Exclude(pTarget),  );
	var targets = FindObjects( Find_InRect(-distance+(distance+8)*GetDir(),-10,distance-8,20), Find_Exclude(pTarget), Find_OCF(OCF_Living)  );

	for( guy in targets )
	{
		if( iEffectTime >= EffectVar(1,pTarget,iEffectNumber) && iEffectTime <= EffectVar(1,pTarget,iEffectNumber)+2 )
		{
			if( GetArrayItemPosition(guy,EffectVar(3,pTarget,iEffectNumber)) < 0)
			{
				PushBack(guy,EffectVar(3,pTarget,iEffectNumber));

				if( guy->~CombatCanBlock( EffectVar(0,pTarget,iEffectNumber) ) )
				{
					Log("Blocked");
				}
				else
					DoDmg(EffectVar(0,pTarget,iEffectNumber)->~GetStrikeDamage(), DMG_Melee, guy, 0, GetOwner()+1);
			}
		}
		else
		{
			if( !GetDir() && !GBackSolid(+10,0) && Max(0,GetX()-GetX(guy)) <= distance-4 )
			{
				SetYDir(-3);
				SetXDir(+6);
				SetDir();
			}
			if( GetDir() && !GBackSolid(-10,0) && Max(0,GetX(guy)-GetX()) <= distance-4 )
			{
				SetYDir(-3);
				SetXDir(-6);
				SetDir(1);
			}
		}


	}
}



protected func FxCombatHitDetectionStop()
{
}



//////////////////////////////////////////////////////////////////////////////////
//
// Verteidigung gegen Schaden

public func CombatDefenseCheck( object pTarget, object pWeapon, int iDuration )
{
	if(!pTarget || !pWeapon || iDuration <= 0 ) return;
	// keine Chance, sich zu verteidigen, wenn der Gegner in den Rücken schlägt
	var failed = false;
	if(( GetX(pTarget) < GetX() && GetDir()) || ( GetX(pTarget)>GetX() && !GetDir() ) )
	{
		failed = true;
		Log("failed block direction");
	}

	if(!failed)
	{
		if(!CombatCanAct())
		{
			return ScheduleCall(this,"CombatDefenseCheck",2,0,pTarget,pWeapon,iDuration);
		}
		else
		{

			// eine einfache Waffenauswahl
			var szAct;
			if( pWeaponP ) szAct = pWeaponP->~GetBlockAction(szMoveOverride,true);
			if( pWeaponS ) szAct = pWeaponS->~GetBlockAction(szMoveOverride,false);
			//CheckForceMove();

			SetAction(szAct);

			// eine einfache Fehlerberechnung
			if(!Random(4)) failed = true;
		}
	}

	if(!failed)
	{
		Log("Block for %d",iDuration);
		AddEffect( "CombatBlock", this, 300, iDuration, this, 0, pWeapon );
	}
}

public func CombatCanBlock( object pWeapon )
{
	Log("CombatCheckBLock %d", ObjectNumber(pWeapon) );
	  // Von Effektzahl abwärts zählen, da Effekte entfernt werden
	var i = GetEffectCount(0, this), iEffect;
	while (i--)
    if (iEffect = GetEffect("CombatBlock", this, i))
      if( EffectVar(0,this,iEffect) == pWeapon )
	  {
		  return true;
	  }
	return false;
}

public func FxCombatBlockStart( object pTarget, int iEffectNumber, int iTemp, var1, var2, var3, var4)
{
	EffectVar(0,pTarget,iEffectNumber) = var1; // weapon
	Log("Immune agains %d",ObjectNumber(var1));
}


//////////////////////////////////////////////////////////////////////////////////
//
// Schadens-Aufrufe

// Schadensreduktion
public func OnDmg( int iDmg, int iType)
{
}

// Schadenseffekte
public func OnHit(int iDmg, int iType, object pFrom, int iPrecDmg)
{
	_inherited(iDmg, iType, pFrom);
  
	if(!GetAlive()) return;

	CombatBloodSpray( iDmg, Random(6), pFrom );
	CheckForceMove( "GetPunched", true );
	Sound("Hurt*",0,this,BoundBy((iPrecDmg*500)/GetPhysical("Energy"),0,100));
}

protected func CombatBloodSpray( int iAmount, int iBodyPart, object pFrom )
{
	// Blut sprühen

	var start_x, start_y, vertex, amount, rot, min_xd, min_yd, max_xd, max_yd, i, x, c,d;

	vertex = 0;

	if( iBodyPart == 1 ) vertex = 1;
	if( iBodyPart == 2 )
	{
		vertex = 3;
		if(GetDir()) vertex = 4;
	}
	if( iBodyPart == 3 )
	{
		vertex = 4;
		if(GetDir()) vertex = 3;
	}
	if( iBodyPart == 4 )
	{
		vertex = 5;
		if(GetDir()) vertex = 6;
	}
	if( iBodyPart == 5 )
	{
		vertex = 6;
		if(GetDir()) vertex = 5;
	}

	start_x = GetVertex( vertex, 0 )-2+Random(5);
	start_y = GetVertex( vertex, 1 )-2+Random(5);

	if(GetX( pFrom ) > GetX())
		rot = -80 + Random( iAmount * 2 );
	else
		rot = +80 - Random( iAmount * 2 );

	min_xd = +Sin( rot, 14 );
	max_xd = +Sin( rot, 30 );

	min_yd = -Cos( rot, 14 );
	max_yd = -Cos( rot, 30 );
/*
	if( DrWodka_GoreMode )
	{
		iAmount = iAmount *2;
		min_size = min_size *2;
		max_size = max_size *2;

		CastObjects(_HEB,BoundBy( dam/5 ,1,8),60);
	}
*/
	for( i = 0; i <= (iAmount / 2); i++)
	{
		c = 255 - Random( 80 );
		x = Random(15);
		CreateParticle( "Blood", start_x, start_y, Sin( rot -7 +x, 14/2 ), -Cos( rot -7 +x, 14/2 ), 20 + Random( 10 ), RGBa( c,c,c, 130 + Random(60)) );
	}

	for( i = 0; i <= iAmount ; i++)
	{
		c = 255 - Random( 60 );
		x = Random(11);
		d = 14 + Random(18);
		CreateParticle( "Blood", start_x, start_y, Sin( rot -6 +x, d/2 ), -Cos( rot -6 +x, d/2 ), 20 + Random( 10 ), RGBa( c,c,c, 130 + Random(60)) );
	}

	for( i = 0; i <= (iAmount / 2); i++)
	{
		c = 255 - Random( 40 );
		x = Random(7);
		CreateParticle( "Blood", start_x, start_y, Sin( rot -3 +x, 32/2 ), -Cos( rot -3 +x, 32/2 ), 20 + Random( 10 ), RGBa( c,c,c, 130 + Random(60)) );
	}
}