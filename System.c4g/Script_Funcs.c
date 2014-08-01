#strict 2

global func Chance(int chance) { // chance from 0 to 100%
	return Random(100) < chance;
}

global func Normalize(int iAngle, int iStart, int iPrecision) {
  if(!iPrecision) iPrecision = 1;
  var iEnd = iPrecision*360+iStart;
  
  while(iAngle<iStart) iAngle += iPrecision*360;
  while(iAngle>=iEnd) iAngle -= iPrecision*360;

  return iAngle;
}

global func CheckVisibility(object Obj1, object Obj2) {	// if Owner of Obj2 can see Obj1
	if(!Obj2)
		Obj2 = this;
		
	var vis = GetVisibility(Obj1);
	if(vis != VIS_All) {
		var own1 = GetOwner(Obj1);
		var own2 = GetOwner(Obj2);
	
		// VIS Owner
		if(vis & VIS_Owner)
			if(own1 == own2)
				return true;
		// VIS Enemies
		if(vis & VIS_Enemies)
			if(Hostile(own1,own2))
				return true;
		// VIS Allies
		if(vis & VIS_Allies)
			if(!Hostile(own1,own2))
				return true;
		// VIS Local
		if(vis & VIS_Local) {
			var lo = own2/32;
			var bit = 1 << own2%32;
			// man, kompliziert...
			if(bit & Local(lo,Obj1))
				return true;
		}
		// sonst gibt es keine Modi
		return false;
	}
	return true;
}

global func IsUserCrewMember(object pObj)
{
	return (GetOCF(pObj) & OCF_CrewMember && GetPlayerType(GetOwner(pObj)) == C4PT_User);
}

global func RemoveAllEffects(object pTarget, string name)
{
	var i, iEffect;

	  // Von Effektzahl abwärts zählen, da Effekte entfernt werden
	  i = GetEffectCount(0, pTarget);
	  while (i--)
	    if (iEffect = GetEffect(name, pTarget, i))
	      RemoveEffect(0, pTarget, iEffect);
}

global func HasCommand(object pTarget)
{
	return GetCommand(pTarget) && GetCommand(pTarget) != "Wait";
}
