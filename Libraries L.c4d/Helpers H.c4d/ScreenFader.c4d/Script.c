#strict 2

static const gScreenModulation_RampCount = 10;
static const gScreenModulation_RampBuilding		= 0;
static const gScreenModulation_RampDamage		= 1;
static const gScreenModulation_RampLensflare 	= 9;


func Initialize()
{
	Local(0) = 0;
	Local(1) = 0;
	SetVisibility(VIS_Owner);

	var width = Max(LandscapeWidth(),4000);
	var height = Max(LandscapeHeight(),3000);

	SetObjDrawTransform(1000*width/GetDefWidth(GetID()),0,750*width -1000*GetDefWidth(GetID()),0,1000*height/GetDefHeight(GetID()),750*height -1000*GetDefHeight(GetID()));

	SetPosition(1,1);

    SetClrModulation(RGBa(0, 0, 0, 255));
}

public func FxIntCheckCursorTimer(object target, int effect, int time)
{
	var cursor = GetCursor(GetOwner(target));
	var isFocused = false;
	if(cursor)
	{
		if(cursor == target)
			isFocused = true;
	}

	if(isFocused)
		SetVisibility(VIS_Owner);
	else
		SetVisibility(VIS_None);
}

global func SetObjScreenModulation(object pObj, int iRGBa, int iBlitMode, int iRamp, int foreground)
{
	if (!pObj && !(pObj = this)) return false;
	if (GetOwner(pObj) == -1) return false;

	var effectName = "IntScreenModulation";
	var effect = GetScreenModulationEffect(pObj, effectName);
	if(!effect) return false;

	var pRamp = SetScreenModulationRamp(pObj, iRGBa, iBlitMode, iRamp, effect, foreground);

	if(pRamp)
	{
		pRamp->SetOwner(GetOwner(pObj));
		AddEffect("IntCheckCursor",pObj,1,1,pRamp);
	}
//	// Sichtbarkeit prüfen
//	if (GetCursor(GetOwner(pObj)) != pObj)
//		SetVisibility(VIS_None, pRamp);

	return true;
}

global func SetPlrScreenModulation(int player, int iRGBa, int iBlitMode, int iRamp, int foreground)
{
	if(player == -1) return false;

	var effectName = "IntPlrScreenModulation";
	var effect = GetScreenModulationEffect(0, effectName);
	if(!effect) return false;

	var pRamp = SetScreenModulationRamp(0, iRGBa, iBlitMode, gScreenModulation_RampCount * player + iRamp, effect, foreground);

	if(pRamp)
	{
		pRamp->~SetOwner(player);
	}

	CleanUpScreenModulationEffect(0, effect, effectName);

	return true;
}

global func GetScreenModulationEffect(object pObj, string effectName)
{
	var effect = GetEffect(effectName, pObj);

	if(!effect) effect = AddEffect(effectName, pObj, 1);
	//if(!effect) return -1;
	return effect;
}

global func CleanUpScreenModulationEffect(object pObj, int effect, string effectName)
{
	// nicht benötigte Effekte wieder entfernen
	var removeEffect = true;
	for(var i = 0; i < gScreenModulation_RampCount; i++)
	{
		removeEffect &= !EffectVar(i,pObj,effect);
	}

	if(removeEffect)
	{
		Log("Remove changed effect");
		RemoveEffect(effectName, pObj);
	}
}

global func SetScreenModulationRamp(object pObj, int iRGBa, int iBlitMode, int iRamp, int effect, int foreground)
{
	var pRamp = EffectVar(iRamp, pObj, effect);

	// keine Farbe
	var r, g, b, a;
	SplitRGBaValue(iRGBa, r, g, b, a);
	if (!(r + g + b + a) || a == 255)
	{
		if (pRamp) RemoveObject(pRamp);
		return false;
	}

	if(!pRamp) (pRamp = EffectVar(iRamp, pObj, effect) = CreateObject(_SCF,0,0,-1));
	if(pRamp)
	{
		// Modulation...
		SetObjectBlitMode(iBlitMode, pRamp);
		SetClrModulation(iRGBa, pRamp);

		SetVisibility(VIS_Owner, pRamp);

		if (foreground == 1)
			pRamp->SetCategory(GetDefCategory(GetID(pRamp)) | C4D_Foreground);
		else if (foreground == 2)
			pRamp->SetCategory(GetDefCategory(GetID(pRamp)) | C4D_Foreground | C4D_Object);
		else if (foreground == -1)
			pRamp->SetCategory(GetDefCategory(GetID(pRamp)) | C4D_Background);

	}

	return pRamp;
}

//global func FadeSightModulation(int iRGBa, int iBlitMode, int iRamp, int player, int fBack, int fForeground)
//{
//	// Manager und Rampe
//	var pClrObj, pRamp;
//	if (pClrObj = FindObjectOwner(_SCF, player, 0, 0, 0, 0, 0, "PlrMng"))
//		pRamp = Local(iRamp, pClrObj);
//
//	// ansonsten ggf. Manager und Rampe erzeugen
//	if(!pClrObj) (pClrObj = CreateObject(_SCF,AbsX(2),AbsY(2),player))->_SCF::PlrManage(player);
//	if(!pRamp) (pRamp = Local(iRamp,pClrObj) = CreateObject(_SCF,AbsX(2),AbsY(2),player))->_SCF::Ramp();
//
//	if (fBack)
//		pRamp->SetCategory(C4D_Foreground);
//	if (fForeground)
//	{
//		pRamp->SetCategory(C4D_Foreground | C4D_Object);
//		pRamp->SetAction("View");
//		pRamp->Message("@!", pRamp);
//	}
//	AddEffect("FadeSightModulation", 0, 1, 1, 0, 0, iRGBa, iRamp, player, [fBack, fForeground]);
//}
//
//global func FxFadeSightModulationStart( pTarget, iNumber, fTmp, iRGBa, iRamp, player, a)
//{
//	if (fTmp) return;
//
//	var r, g, b, a;
//	var fBack = a[0];
//	var fForeground = a[1];
//	SplitRGBaValue(iRGBa, r, g, b, a);
//	EffectVar(0, pTarget, iNumber) = 0;
//	EffectVar(gScreenFader_EffectVar_RGBa_R, pTarget, iNumber) = r;
//	EffectVar(gScreenFader_EffectVar_RGBa_G, pTarget, iNumber) = g;
//	EffectVar(gScreenFader_EffectVar_RGBa_B, pTarget, iNumber) = b;
//
//	EffectVar(gScreenFader_EffectVar_Ramp, pTarget, iNumber) = iRamp;
//	EffectVar(gScreenFader_EffectVar_Owner, pTarget, iNumber) = player;
//	EffectVar(gScreenFader_EffectVar_Background, pTarget, iNumber) = fBack;
//
//	EffectVar(gScreenFader_EffectVar_Foreground, pTarget, iNumber) = fForeground;
//}
//
//global func FxFadeSightModulationTimer( pTarget, iNumber)
//{
//	var a = 255 - EffectVar(0, pTarget, iNumber);
//	if (EffectVar(gScreenFader_EffectVar_Background, pTarget, iNumber)) a = EffectVar(0, pTarget, iNumber);
//
//	SetPlrSightModulation( RGBa(EffectVar(gScreenFader_EffectVar_RGBa_R, pTarget, iNumber),
//								EffectVar(gScreenFader_EffectVar_RGBa_G, pTarget, iNumber),
//								EffectVar(gScreenFader_EffectVar_RGBa_B, pTarget, iNumber), a),
//						EffectVar(4, pTarget, iNumber),
//						EffectVar(gScreenFader_EffectVar_Ramp, pTarget, iNumber),
//						EffectVar(gScreenFader_EffectVar_Owner, pTarget, iNumber),
//						EffectVar(gScreenFader_EffectVar_Foreground, pTarget, iNumber));
//
//	EffectVar(0, pTarget, iNumber) += 26;
//	if (EffectVar(0, pTarget, iNumber) >= 255)
//	{
//		if (EffectVar(gScreenFader_EffectVar_Background, pTarget, iNumber))
//			SetPlrSightModulation(0,
//					EffectVar(4, pTarget, iNumber),
//					EffectVar(gScreenFader_EffectVar_Ramp, pTarget, iNumber),
//					EffectVar(gScreenFader_EffectVar_Owner, pTarget, iNumber));
//		else
//			SetPlrSightModulation( RGBa(EffectVar(gScreenFader_EffectVar_RGBa_R, pTarget, iNumber),
//										EffectVar(gScreenFader_EffectVar_RGBa_G, pTarget, iNumber),
//										EffectVar(gScreenFader_EffectVar_RGBa_B, pTarget, iNumber), 0),
//					EffectVar(4, pTarget, iNumber), EffectVar(gScreenFader_EffectVar_Ramp, pTarget, iNumber),
//					EffectVar(gScreenFader_EffectVar_Owner, pTarget, iNumber));
//		return -1;
//	}
//}
//
//func OnFadeDone()
//{
//	// keine Farbe
//	var r, g, b, a, pClrObj;
//	SplitRGBaValue(GetClrModulation(), r, g, b, a);
//	if (!(r + g + b + a) || a == 255)
//		RemoveObject();
//}

