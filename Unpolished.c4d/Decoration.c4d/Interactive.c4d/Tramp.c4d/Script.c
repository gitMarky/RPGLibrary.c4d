/*--
Bounces things into the air.
@note Bouncy things are all things alive, collectible, or returning {@c true} in IsTrampolinObject().  
@title Trampolin
@author Sven2
@version 0.1.0
--*/

#strict 2

static const gTrampolin_Identifier_Function = "IsTrampolinObject";

local high_frequency_check;


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
	var target, effect_nr;
	for (target in FindObjects
	    (
			Find_InRect(-15, -25, 30, 28),
			Find_Or(Find_OCF(OCF_Alive | OCF_Collectible), Find_Func(gTrampolin_Identifier_Function)),
			Find_NoContainer()
		)) 
	{
		if ((effect_nr = GetEffect("IntTramp", target)) || target->GetYDir() >= 0)
		if (!target->GetContact(0, -1, 8))
		if (target->GetDefBottom() >= GetY() - 12)
		{
			if (!effect_nr && target->GetAction() == "Tumble" && !Random(4)) continue; // skip CrewMember (ala)
			
			var y_dir;
			if (effect_nr)
			{
				y_dir = EffectVar(0, target, effect_nr);
				RemoveEffect(0, target, effect_nr);
			}
			else
				y_dir = target->GetYDir();
			if (!GetAlive(target))
			{
				y_dir = Min(y_dir, 30);
			}
			if (!target->Fling(0, 0, y_dir * -3 / 20 - 2))
				target->SetYDir(y_dir * -3 / 2 - 20);
			else
				target->SetXDir(target->GetXDir() * 3 / 2);
		}
	}
}

protected func CheckTime()
{
	if (GetR() || Contained()) return;
	// activate trampolin
	var target;
	for (target in FindObjects
		(
			Find_InRect(-15, -25, 30, 28),
			Find_Or(Find_OCF(OCF_Alive | OCF_Collectible), Find_Func(gTrampolin_Identifier_Function)),
			Find_NoContainer()
		)) 
		if (target->GetYDir() > 0)
		if (!target->GetContact(0, -1, 8))
		if (target->GetDefBottom() >= GetY() - 12)
		{
			if (target->GetAction() == "Tumble" && !Random(4))
				return SetAction("Fail");
			TrampDowning();
			return SetAction("TrampDown");
		}
}

protected func TrampDowning()
{
	// bind objects
	var target;
	for (target in FindObjects
		(
			Find_InRect(-15, -25, 30, 28),
			Find_Or(Find_OCF(OCF_Alive | OCF_Collectible), Find_Func(gTrampolin_Identifier_Function)),
			Find_NoContainer()
		)) 
		if (target->GetYDir() > 0)
		if (!target->GetContact(0, -1, 8))
		if (target->GetDefBottom() >= GetY() - 12)
			if (!GetEffect("IntTramp", target))
				AddEffect("IntTramp", target, 1, 1, this);
}

protected func FxIntTrampStart(object target, int effect_nr, int temp)
{
	// save velocity
	if (temp) return;
	EffectVar(0, target, effect_nr) = target->GetYDir();
	// tumble the clonk
	target->SetAction("Tumble");
	// ...fix velocity
	FxIntTrampTimer(target, effect_nr);
	return FX_OK;
}

protected func FxIntTrampTimer(object target, int effect_nr, int time)
{
	// animation done?
	if (GetAction() == "Ready") return FX_Execute_Kill;
	// moved out of the area of effect?
	if (Abs(GetX(target) - GetX()) > 16) return FX_Execute_Kill;
	
	// bind object
	var ydirfact = 5;
	if (target->GetAlive())
	{
		target->SetXDir(BoundBy(target->GetXDir(), -10, 10));
	}
	else
	{
		target->SetXDir(BoundBy(target->GetXDir(), -15, 15));
	}
	target->SetYDir(BoundBy(GetY() - GetY(target) - 4, -5, target->GetYDir(0, ydirfact)), 0, ydirfact);
	return FX_OK;
}

protected func Failing()
{
	// Shit happens...
	Sound("Boing");
	SetYDir(-20);
}
