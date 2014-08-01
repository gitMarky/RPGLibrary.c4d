/*-- Ausrüstungs-Overlays --*/

#strict 2
#appendto _CLN

//local dwColorSkin, dwColorHair;
local szGraphicsHair,szGraphicsBeard,szGraphicsSkin;
local dwColorHair;

protected func Initialize()
{
	dwColorHair=RGB(68,34,0);
	szGraphicsHair="Hair";
	szGraphicsBeard="Beard";
	szGraphicsSkin="Skin";
	DrawEquipmentOverlays();
	_inherited(...);
}

public func DrawEquipmentOverlays()
{
	
	DrawEquipmentOverlayX( 22, pWeapon1, true );
	DrawEquipmentOverlayX( 21, pWeapon2, true );
	DrawEquipmentOverlayX( 20, pWeapon3, true );
	//DrawEquipmentOverlayX( 19, pHelmet );

	DrawEquipmentOverlay( 18, szGraphicsHair, dwColorHair );
	DrawEquipmentOverlay( 17, szGraphicsBeard, dwColorHair );
	//DrawEquipmentOverlay( 16, "Tools", RGB(255,255,255));
/*
	DrawEquipmentOverlayX( 15, pBackpack );
	DrawEquipmentOverlayX( 14, pCloak );
	DrawEquipmentOverlayX( 13, pAmulet );*/
	/*DrawEquipmentOverlayX( 12, aBeltItems[0] );
	DrawEquipmentOverlayX( 11, aBeltItems[1] );
	DrawEquipmentOverlayX( 10, aBeltItems[2] );
	DrawEquipmentOverlayX( 9, aBeltItems[3] );
	DrawEquipmentOverlayX( 8, aBeltItems[4] );*/
/*
	DrawEquipmentOverlayX( 7, pArmor );
	DrawEquipmentOverlayX( 6, pGloves );
	DrawEquipmentOverlayX( 5, pShirt );
	DrawEquipmentOverlayX( 4, pGreaves );
	DrawEquipmentOverlayX( 3, pBoots );
	DrawEquipmentOverlayX( 2, pPants );
*/
	DrawEquipmentOverlay( 0, szGraphicsSkin, RGB(255,255,255) );// skin
}

protected func DrawEquipmentOverlayX(int iIndex, object pObj, bool fUseObjGfx)
{
	if(!pObj) return DrawEquipmentOverlay(iIndex,0,RGB(255,255,255));

	var idDef;
	if( fUseObjGfx ) idDef = pObj->GetID();

	DrawEquipmentOverlay( iIndex, pObj->~GetOverlayGraphics(this), pObj->~GetOverlayColor(this), idDef );
}

protected func DrawEquipmentOverlay(int iIndex, string szGraphics, rgbColor, id idDef)
{
	var avatar;
	var gfxmode;

	if(!idDef) idDef = _CLN;
	if(!rgbColor) rgbColor = RGB(255,255,255);

	if( szGraphics ) gfxmode = GFXOV_MODE_ExtraGraphics;

	if(pCharMenu) avatar=pCharMenu->~GetAvatar();

	SetGraphics( szGraphics, this, idDef, iIndex, gfxmode );
	SetClrModulation( rgbColor, this, iIndex );
	if(avatar)
	{
		SetGraphics( szGraphics, avatar, ID_Menu_Avatar, iIndex, gfxmode );
		SetClrModulation( rgbColor, avatar, iIndex );
	}
}
