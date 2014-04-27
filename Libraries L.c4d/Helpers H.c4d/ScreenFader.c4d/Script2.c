#strict 2

static const gScreenFader_EffectVar_RGBa_R = 1;
static const gScreenFader_EffectVar_RGBa_G = 2;
static const gScreenFader_EffectVar_RGBa_B = 3;
static const gScreenFader_EffectVar_Ramp = 5;
static const gScreenFader_EffectVar_Owner = 6;
static const gScreenFader_EffectVar_Background = 7;
static const gScreenFader_EffectVar_Foreground = 8;

func DoAttach()
{
	var iOffsetX = 0, iOffsetY = 0;
	var pTarget = GetActionTarget();
	if (!pTarget) return RemoveObject();

	SetClrModulation(RGBa(255, 255, 255, 255));
	var pLayer;
	if (pLayer = pTarget->GetObjectLayer())
	{
		SetPosition(pTarget->GetX() + pLayer->GetLayerOffset(), pTarget->GetY() + pLayer->GetLayerOffset(1));
		if (GetCursor(GetOwner(pTarget)) == pTarget)
			SetPlrView(GetOwner(pTarget), this);
	}
	else
		SetPosition(pTarget->GetX(), pTarget->GetY());
}

func Manage(object pTarget)
{
	SetAction("Mng", pTarget);
	SetVisibility(VIS_None);
}

func PlrManage(int player)
{
	SetAction("PlrMng");
	SetOwner(player);
	SetVisibility(VIS_Owner);
}

func Ramp()
{
	Local(0) = 0;
	Local(1) = 0;
	SetVisibility(VIS_Owner);
	SightOrder();
	// Leute: Nicht mit Auflösungen über 2000x2000 spielen. k?
	SetObjDrawTransform(1000 * 2000, 0, GetDefWidth(GetID()) * Abs(1000 - 2000 * 1000) / 2, 0, 1000 * 2000, GetDefHeight(GetID()) * Abs(1000 - 2000 * 1000) / 2);
	SetObjDrawTransform(1000 * 1024, 0, 1000 * 1024 - 2000, 0, 1000 * 768, 1000 * 768 - 2000);
}

func SightOrder()
{
	// tut des nach vorne
}

// Aufrüfe an Manager
func Enable()
{
	for (var i = 0; i < MaxRampCount(); ++i)
		if (Local(i))
			SetVisibility(VIS_Owner, Local(i));
}
func Disable()
{
	for (var i = 0; i < MaxRampCount(); ++i)
		if (Local(i))
			SetVisibility(VIS_None, Local(i));
}

global func MaxRampCount()
{
	return 10;
}

global func SetSightModulation(int iRGBa, int iBlitMode, int iRamp, object pObj)
{

	if (!pObj)
		if (!(pObj = this()))
			return false;
	if (GetOwner(pObj) == -1)
		return false;

	// Manager und Rampe
	var pClrObj, pRamp;
	if (pClrObj = FindObjectOwner(_VIW, GetOwner(pObj), 0, 0, 0, 0, 0, "Mng", pObj))
		pRamp = Local(iRamp, pClrObj);

	// keine Farbe
	var r, g, b, a;
	SplitRGBaValue(iRGBa, r, g, b, a);
	if (!(r + g + b + a) || a == 255)
	{
		if (pRamp)
			RemoveObject(pRamp);
		return false;
	}

	// ansonsten ggf. Manager und Rampe erzeugen
	if(!pClrObj) (pClrObj = CreateObject(_VIW,AbsX(2),AbsY(2),GetOwner(pObj)))->_VIW::Manage(pObj);
	if(!pRamp) (pRamp = Local(iRamp,pClrObj) = CreateObject(_VIW,AbsX(2),AbsY(2),GetOwner(pObj)))->_VIW::Ramp();

	// Modulation...
	SetObjectBlitMode(iBlitMode, pRamp);
	SetClrModulation(iRGBa, pRamp);

	// Sichtbarkeit prüfen
	if (GetCursor(GetOwner(pObj)) != pObj)
		SetVisibility(VIS_None, pRamp);

	return true;
}

global func SetPlrSightModulation(int iRGBa, int iBlitMode, int iRamp, int player, int fForeground)
{

	// Manager und Rampe
	var pClrObj, pRamp;
	if (pClrObj = FindObjectOwner(_VIW, player, 0, 0, 0, 0, 0, "PlrMng"))
		pRamp = Local(iRamp, pClrObj);

	// keine Farbe
	var r, g, b, a;
	SplitRGBaValue(iRGBa, r, g, b, a);
	if (!(r + g + b + a) || a == 255)
	{
		if (pRamp)
			RemoveObject(pRamp);
		return false;
	}

	// ansonsten ggf. Manager und Rampe erzeugen
	if(!pClrObj) (pClrObj = CreateObject(_VIW,AbsX(2),AbsY(2),player))->_VIW::PlrManage(player);
	if(!pRamp) (pRamp = Local(iRamp,pClrObj) = CreateObject(_VIW,AbsX(2),AbsY(2),player))->_VIW::Ramp();

	if (fForeground)
	{
		pRamp->SetCategory(2097153 | C4D_Foreground | C4D_Object);
		pRamp->SetAction("View");
		pRamp->Message("@!", pRamp);
	}
	else
		pRamp->SetCategory(2097153);

	// Modulation...
	SetObjectBlitMode(iBlitMode, pRamp);
	SetClrModulation(iRGBa, pRamp);

	return true;
}

global func FadeSightModulation(int iRGBa, int iBlitMode, int iRamp, int player, int fBack, int fForeground)
{
	// Manager und Rampe
	var pClrObj, pRamp;
	if (pClrObj = FindObjectOwner(_VIW, player, 0, 0, 0, 0, 0, "PlrMng"))
		pRamp = Local(iRamp, pClrObj);

	// ansonsten ggf. Manager und Rampe erzeugen
	if(!pClrObj) (pClrObj = CreateObject(_VIW,AbsX(2),AbsY(2),player))->_VIW::PlrManage(player);
	if(!pRamp) (pRamp = Local(iRamp,pClrObj) = CreateObject(_VIW,AbsX(2),AbsY(2),player))->_VIW::Ramp();

	if (fBack)
		pRamp->SetCategory(C4D_Foreground);
	if (fForeground)
	{
		pRamp->SetCategory(C4D_Foreground | C4D_Object);
		pRamp->SetAction("View");
		pRamp->Message("@!", pRamp);
	}
	AddEffect("FadeSightModulation", 0, 1, 1, 0, 0, iRGBa, iRamp, player, [fBack, fForeground]);
}

global func FxFadeSightModulationStart( pTarget, iNumber, fTmp, iRGBa, iRamp, player, a)
{
	if (fTmp) return;

	var r, g, b, a;
	var fBack = a[0];
	var fForeground = a[1];
	SplitRGBaValue(iRGBa, r, g, b, a);
	EffectVar(0, pTarget, iNumber) = 0;
	EffectVar(gScreenFader_EffectVar_RGBa_R, pTarget, iNumber) = r;
	EffectVar(gScreenFader_EffectVar_RGBa_G, pTarget, iNumber) = g;
	EffectVar(gScreenFader_EffectVar_RGBa_B, pTarget, iNumber) = b;

	EffectVar(gScreenFader_EffectVar_Ramp, pTarget, iNumber) = iRamp;
	EffectVar(gScreenFader_EffectVar_Owner, pTarget, iNumber) = player;
	EffectVar(gScreenFader_EffectVar_Background, pTarget, iNumber) = fBack;

	EffectVar(gScreenFader_EffectVar_Foreground, pTarget, iNumber) = fForeground;
}

global func FxFadeSightModulationTimer( pTarget, iNumber)
{
	var a = 255 - EffectVar(0, pTarget, iNumber);
	if (EffectVar(gScreenFader_EffectVar_Background, pTarget, iNumber)) a = EffectVar(0, pTarget, iNumber);

	SetPlrSightModulation( RGBa(EffectVar(gScreenFader_EffectVar_RGBa_R, pTarget, iNumber), EffectVar(gScreenFader_EffectVar_RGBa_G, pTarget, iNumber), EffectVar(gScreenFader_EffectVar_RGBa_B, pTarget, iNumber), a), EffectVar(4, pTarget, iNumber), EffectVar(gScreenFader_EffectVar_Ramp, pTarget, iNumber), EffectVar(gScreenFader_EffectVar_Owner, pTarget, iNumber), EffectVar(gScreenFader_EffectVar_Foreground, pTarget, iNumber));
	EffectVar(0, pTarget, iNumber) += 26;
	if (EffectVar(0, pTarget, iNumber) >= 255)
	{
		if (EffectVar(gScreenFader_EffectVar_Background, pTarget, iNumber))
			SetPlrSightModulation(0, EffectVar(4, pTarget, iNumber), EffectVar(gScreenFader_EffectVar_Ramp, pTarget, iNumber), EffectVar(gScreenFader_EffectVar_Owner, pTarget, iNumber));
		else
			SetPlrSightModulation( RGBa(EffectVar(gScreenFader_EffectVar_RGBa_R, pTarget, iNumber), EffectVar(gScreenFader_EffectVar_RGBa_G, pTarget, iNumber), EffectVar(gScreenFader_EffectVar_RGBa_B, pTarget, iNumber), 0), EffectVar(4, pTarget, iNumber), EffectVar(gScreenFader_EffectVar_Ramp, pTarget, iNumber), EffectVar(gScreenFader_EffectVar_Owner, pTarget, iNumber));
		return -1;
	}
}

func OnFadeDone()
{
	// keine Farbe
	var r, g, b, a, pClrObj;
	SplitRGBaValue(GetClrModulation(), r, g, b, a);
	if (!(r + g + b + a) || a == 255)
		RemoveObject();
}
