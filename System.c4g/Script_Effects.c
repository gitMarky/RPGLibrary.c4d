/* Effects helpers */

#strict 2

global func RemoveAllEffects(object pTarget, string name)
{
	var i, iEffect;
	
	// Von Effektzahl abwärts zählen, da Effekte entfernt werden
	i = GetEffectCount(0, pTarget);
	while (i--)
		if (iEffect = GetEffect(name, pTarget, i))
			RemoveEffect(0, pTarget, iEffect);
}
