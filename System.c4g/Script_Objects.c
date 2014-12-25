#strict 2

global func FixVertically(object target)
{
	if (!target) if (!(target = this)) return false;

	var effect = GetEffect("IntFixVer", target);
	
	if (!effect)
	{
		effect = AddEffect("IntFixVer", target, 50, 1);
	}
	
	EffectVar(0, target, effect) = target->GetY();
	
	return true;
}

global func FxIntFixVerTimer(object target, int effect_nr, int time)
{
	var y = EffectVar(0, target, effect_nr);
	
	if (target->GetY() != y) target->SetPosition(target->GetX(), y);
}


global func FixHorizontally(object target)
{
	if (!target) if (!(target = this)) return false;

	var effect = GetEffect("IntFixHor", target);
	
	if (!effect)
	{
		effect = AddEffect("IntFixHor", target, 50, 1);
	}
	
	EffectVar(0, target, effect) = target->GetX();
	
	return true;
}

global func FxIntFixHorTimer(object target, int effect_nr, int time)
{
	var x = EffectVar(0, target, effect_nr);
	
	if (target->GetX() != x) target->SetPosition(x, target->GetY());
}