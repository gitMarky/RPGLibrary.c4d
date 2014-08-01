#strict 2

global func FxNoBurningEffect(szNewEffect, iEffectTarget, iEffectNumber, iNewEffectNumber, var1, var2, var3)
  {
  // Feuer abblocken
  if (WildcardMatch(szNewEffect, "*Fire*")) return -1;
  // Alles andere ist OK
  return;
  }

global func FxNoBurningDamage()
  {
  // Kein Schaden
  return;
  }

protected func FxNoBurningEffect(string szNewEffect)
  {
  if(szNewEffect == "NoBurning") return -1; // Einer reicht
  }
