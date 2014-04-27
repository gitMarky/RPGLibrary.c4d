/* HUD */

#strict 2

local iPosY, iPosX;
local aButtons;

public func AttachToHUD( object pHUD, int iX, int iY, string szGraphics )
{
	iPosX = iX;
	iPosY = iY;

	SetPosition(iX,iY); 		// Positionieren
	Local(0)=Local(1)=0; 		// Parallax auf 0 Setzen

	SetCategory(C4D_StaticBack|C4D_Parallax|C4D_Foreground|C4D_MouseIgnore|C4D_IgnoreFoW);

	SetVisibility(VIS_Owner); 	// Nur für den Besitzer sichtbar
	SetAction("Exist"); 		// Anzeige-Action setzen

	SetOwner(GetOwner(pHUD));

	SetGraphics(szGraphics);

	AddEffect("Display",this,50,1,this);

	if( szGraphics ) CreateButtons( pHUD );
}

protected func FxDisplayTimer()
{
	if(GetY() >= 0) SetVisibility(VIS_None);
	else SetVisibility(VIS_Owner);
}

public func UpdatePosition(int iOff)
{
	SetPosition( iPosX, iPosY + iOff );

	if( aButtons ) for( button in aButtons )
		button->~UpdatePosition( 0, iOff );
}

protected func CreateButtons( object pHUD )
{
	aButtons = [];
	for( var i = 0; i <= 4; i++ )
	{
		aButtons[i] = CreateObject( ID_Menu_CharButton,0,0,-1 );
		aButtons[i]->~AttachToHUD( pHUD, iPosX + 20 +44*i, iPosY, LIGH );
	}
}

public func GetButton( int iIndex )
{
	return aButtons[iIndex];
}

public func ButtonSelected( int iIndex)
{
	for( var i = 0; i <= 4; i++ )
	{
		if( i == iIndex ) aButtons[i]->~OnSelect();
		else aButtons[i]->~OnDeSelect();
	}
}

public func ButtonPressed( int iIndex)
{
	if(aButtons[iIndex]) aButtons[iIndex]->~OnPressed();
}

