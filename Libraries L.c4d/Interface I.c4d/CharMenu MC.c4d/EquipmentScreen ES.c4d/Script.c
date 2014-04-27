/* HUD */

#strict 2

local iPosY, iPosX;
local aButtons;
local pAvatar;

public func AttachToHUD( object pHUD, int iX, int iY )
{
	iPosX = iX;
	iPosY = iY;

	SetPosition(iX,iY); 		// Positionieren
	Local(0)=Local(1)=0; 		// Parallax auf 0 Setzen

	SetCategory(C4D_StaticBack|C4D_Parallax|C4D_Foreground|C4D_MouseIgnore|C4D_IgnoreFoW);

	SetVisibility(VIS_Owner); 	// Nur für den Besitzer sichtbar
	SetAction("Exist"); 		// Anzeige-Action setzen

	SetOwner(GetOwner(pHUD));

	AddEffect("Display",this,50,1,this);

	CreateButtons( pHUD );
}

protected func FxDisplayTimer()
{
	if(GetX() >= 0) SetVisibility(VIS_None);
	else SetVisibility(VIS_Owner);
}

public func UpdatePosition(int iOff)
{
	SetPosition( iPosX + iOff, iPosY );

	if( aButtons ) for( button in aButtons )
		button->~UpdatePosition( iOff,0 );

	pAvatar->~UpdatePosition( iOff );
}

protected func CreateButtons( object pHUD )
{
	aButtons = [];
	for( var i = 0; i <= 20; i++ )
	{
		aButtons[i] = CreateObject( ID_Menu_CharButton,0,0,-1 );

		var x, y;

		if(  0 <= i && i <  5) { x = 150      +i*44; y =  20; }
		if(  5 <= i && i < 10) { x = 150 +(i- 5)*44; y =  65; }
		if( 10 <= i && i < 15) { x = 150 +(i-10)*44; y = 110; }
		if( 15 <= i && i < 20) { x = 150 +(i-15)*44; y = 160; }

		aButtons[i]->~AttachToHUD( pHUD, iPosX + x, iPosY +y, LIGH );
	}

	pAvatar = CreateObject( ID_Menu_Avatar,0,0,-1 );

	pAvatar->~AttachToHUD( pHUD, iPosX + 20, iPosY + 20);
}

public func GetButton( int iIndex )
{
	return aButtons[iIndex];
}

public func ButtonSelected( int iIndex)
{
	for( var i = 0; i <= 20; i++ )
	{
		if( i == iIndex ) aButtons[i]->~OnSelect();
		else aButtons[i]->~OnDeSelect();
	}
}

public func ButtonPressed( int iIndex)
{
	if(aButtons[iIndex]) aButtons[iIndex]->~OnPressed();
}

