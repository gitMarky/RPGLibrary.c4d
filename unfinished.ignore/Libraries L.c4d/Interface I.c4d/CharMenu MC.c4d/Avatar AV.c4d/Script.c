/* HUD */

#strict 2

local iPosY, iPosX;

public func AttachToHUD(object pHUD, int iX, int iY)
{
	iPosX = iX;
	iPosY = iY;
	
	SetPosition(iX, iY); // Positionieren
	Local(0) = Local(1) = 0; // Parallax auf 0 Setzen
	
	SetCategory(C4D_Vehicle | C4D_Parallax | C4D_Foreground | C4D_MouseIgnore | C4D_IgnoreFoW);
	
	SetVisibility(VIS_Owner); // Nur für den Besitzer sichtbar
	SetAction("Exist"); // Anzeige-Action setzen
	
	SetOwner(GetOwner(pHUD));
	
	AddEffect("Display", this, 50, 1, this);
}

protected func FxDisplayTimer()
{
	if (GetX() >= 0)
		SetVisibility(VIS_None);
	else
		SetVisibility(VIS_Owner);
}

public func UpdatePosition(int iOff)
{
	SetPosition(iPosX + iOff, iPosY);
}