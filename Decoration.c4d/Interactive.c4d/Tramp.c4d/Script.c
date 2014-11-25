/*--
Bounces things into the air
@title Trampolin
@author Sven2
@version 0.1.0
--*/

#strict 2

local high_frequency_check;

/* Initialisierung */

protected func Initialize()
{
	high_frequency_check = true;
	DoReady();
}

protected func DoReady()
{
	if (high_frequency_check)
		SetAction("ReadyEx");
	else
		SetAction("Ready");
	return true;
}

protected func TrampUpStart()
{
	Sound("Boing");
	SetYDir(-5);
	var obj, eff;
	for (obj in FindObjects
		(
			Find_InRect(-15, -25, 30, 28),
			Find_Or(Find_OCF(OCF_Alive | OCF_Collectible), Find_Func("IsTrampolinObject")),
			Find_NoContainer()
		)) 
	{
		if ((eff = GetEffect("IntTramp", obj)) || obj->GetYDir() >= 0)
			if (!obj->GetContact(0, -1, 8))
				if (obj->GetDefBottom() >= GetY() - 12)
				{
					if (!eff && obj->GetAction() == "Tumble" && !Random(4))
						continue; // CrewMember rauslassen (ala)
					var iYDir;
					if (eff)
					{
						iYDir = EffectVar(0, obj, eff);
						RemoveEffect(0, obj, eff);
					}
					else
						iYDir = obj->GetYDir();
					if (!GetAlive(obj))
					{
						iYDir = Min(iYDir, 30);
					}
					if (!obj->Fling(0, 0, iYDir * -3 / 20 - 2))
						obj->SetYDir(iYDir * -3 / 2 - 20);
					else
						obj->SetXDir(obj->GetXDir() * 3 / 2);
				}
	}
}

protected func CheckTime()
{
	if (GetR() || Contained())
		return;
	// Trampolin auslösen
	var obj;
	for (obj in FindObjects
		(
			Find_InRect(-15, -25, 30, 28),
			Find_Or(Find_OCF(OCF_Alive | OCF_Collectible), Find_Func("IsTrampolinObject")),
			Find_NoContainer()
		)) 
		if (obj->GetYDir() > 0)
			if (!obj->GetContact(0, -1, 8))
				if (obj->GetDefBottom() >= GetY() - 12)
				{
					if (obj->GetAction() == "Tumble" && !Random(4))
						return SetAction("Fail");
					TrampDowning();
					return SetAction("TrampDown");
				}
}

protected func TrampDowning()
{
	// Objekte halten
	var obj;
	for (obj in FindObjects
		(
			Find_InRect(-15, -25, 30, 28),
			Find_Or(Find_OCF(OCF_Alive | OCF_Collectible), Find_Func("IsTrampolinObject")),
			Find_NoContainer()
		)) 
		if (obj->GetYDir() > 0)
			if (!obj->GetContact(0, -1, 8))
				if (obj->GetDefBottom() >= GetY() - 12)
					if (!GetEffect("IntTramp", obj))
						AddEffect("IntTramp", obj, 1, 1, this);
}

protected func FxIntTrampStart(object pTarget, int iEffectNumber, int iTemp)
{
	// Geschwindigkeit sichern...
	if (iTemp)
		return;
	EffectVar(0, pTarget, iEffectNumber) = pTarget->GetYDir();
	// Clonk knien
	pTarget->SetAction("Tumble");
	// ...und Geschwindigkeit richten
	FxIntTrampTimer(pTarget, iEffectNumber);
	return FX_OK;
}

protected func FxIntTrampTimer(object pTarget, int iEffectNumber, int iEffectTime)
{
	// Trampolin-Action vorbei?
	if (GetAction() == "Ready")
		return FX_Execute_Kill;
	// Aus dem Trampolin-Bereich?
	if (Abs(GetX(pTarget) - GetX()) > 16)
		return FX_Execute_Kill;
	// Objekt halten
	var ydirfact = 5;
	if (pTarget->GetAlive())
	{
		pTarget->SetXDir(BoundBy(pTarget->GetXDir(), -10, 10));
	}
	else
	{
		pTarget->SetXDir(BoundBy(pTarget->GetXDir(), -15, 15));
	}
	pTarget->SetYDir(BoundBy(GetY() - GetY(pTarget) - 4, -5, pTarget->GetYDir(0, ydirfact)), 0, ydirfact);
	return FX_OK;
}

protected func Failing()
{
	// Shit happens...
	Sound("Boing");
	SetYDir(-20);
}
