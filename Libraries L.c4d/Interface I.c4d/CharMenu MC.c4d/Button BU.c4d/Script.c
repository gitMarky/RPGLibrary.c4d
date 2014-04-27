/* HUD */

#strict 2

local iPosY, iPosX;
local bEnabled;
local szMenuCall, pMenuObj, anyMenuPar, anyMenuPar2;

public func AttachToHUD( object pHUD, int iX, int iY, idSymbol )
{
	iPosX = iX;
	iPosY = iY;

	SetPosition(iX,iY); 		// Positionieren
	Local(0)=Local(1)=0; 		// Parallax auf 0 Setzen

	SetCategory(C4D_Vehicle|C4D_Parallax|C4D_Foreground|C4D_MouseIgnore|C4D_IgnoreFoW);

	SetVisibility(VIS_Owner); 	// Nur für den Besitzer sichtbar
	SetAction("Exist"); 		// Anzeige-Action setzen

	SetOwner(GetOwner(pHUD));

	AddEffect("Display",this,50,1,this);

	UpdatePicture( idSymbol );

	ButtonEnable(false);
}

public func SetButton( idSymbol, szCall, pObj, anyPar, anyPar2, pTarget )
{
	UpdatePicture( idSymbol, pTarget );

	szMenuCall = szCall;
	pMenuObj = pObj;
	anyMenuPar = anyPar;
	anyMenuPar2 = anyPar2;
}

protected func FxDisplayTimer()
{
	if(GetY() >= 0 || GetX() >=0 ) SetVisibility(VIS_None);
	else SetVisibility(VIS_Owner);
}

public func UpdatePosition(int iOffX, int iOffY)
{
	SetPosition( iPosX + iOffX, iPosY + iOffY );
}

public func UpdatePicture( idSymbol, object pObj )
{
	var offset = 500*Max(GetDefCoreVal("Picture", "DefCore", idSymbol, 2),GetDefCoreVal("Picture", "DefCore", idSymbol, 3));
	// setzt die Picture-Grafik richtig
	var gfx = "";
	if(pObj) gfx = pObj->GetGraphics();
	SetGraphics(gfx,this,idSymbol,2,GFXOV_MODE_IngamePicture);
	SetObjDrawTransform(900,0,offset,0,900,offset,this,2);

	SetClrModulation(RGB(255,255,255),this,2);
	if(pObj) if(pObj != this) if(GetClrModulation(pObj)) SetClrModulation(GetClrModulation(pObj),this,2);
}

public func ButtonEnable( bValue )
{
	return bEnabled = bValue;
}

public func OnDeSelect()
{
	SetClrModulation(RGB(255,255,255));
}

public func OnSelect()
{
	SetClrModulation(RGB(255,0,0));
}

public func OnPressed()
{
	//Message("SUCKKKK!!!",this);
	ObjectCall(pMenuObj,szMenuCall,anyMenuPar,anyMenuPar2);
}
