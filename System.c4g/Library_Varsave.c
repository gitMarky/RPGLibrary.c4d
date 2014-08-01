/*-- Variablenspeicher - lässt dynamisches Generieren von LocalN zu --*/

#strict 2


global func &CreateLocalN2(string szName , object pTarget, vValue)
{
	/*return*/ AddEffect("IntVarsave",pTarget,50,0,0,0, szName, vValue);
	return LocalN2(szName, pTarget);
}

global func &LocalN2(string szName , object pTarget)
{
		var iEffectNr = GetEffect("IntVarsave",pTarget);
		if(!iEffectNr) return 0;
		var aNames = EffectVar(0,pTarget,iEffectNr);
		var aValues = EffectVar(1,pTarget,iEffectNr);
		if(GetType(aNames) != C4V_Array && GetType(aValues) != C4V_Array) return 0;

		var index = GetArrayItemPosition( szName, aNames );
		if(index < 0) return 0;
		return EffectVar(1,pTarget,iEffectNr)[index];
}


global func FxIntVarsaveStart(object pTarget, int iEffectNumber, int iTemp, string szVarName, vValue)
{
	if(!EffectVar(0,pTarget,iEffectNumber)) EffectVar(0,pTarget,iEffectNumber) = []; // speichert die Namen
	if(!EffectVar(1,pTarget,iEffectNumber)) EffectVar(1,pTarget,iEffectNumber) = []; // speichert den Wert

	PushBack(szVarName,EffectVar(0,pTarget,iEffectNumber));
	PushBack(vValue,EffectVar(1,pTarget,iEffectNumber));
}

global func FxIntVarsaveEffect(string szNewEffectName)
{
  // gleichen Effekt zusammenfassen
  if(szNewEffectName == "IntVarsave") return -2;
}

global func FxIntVarsaveAdd(object pTarget, int iEffectNumber, string szNewEffectName, int iNewEffectTimer, string szVarName, vValue)
{
  // Effekt übernehmen
	if(!EffectVar(0,pTarget,iEffectNumber)) EffectVar(0,pTarget,iEffectNumber) = []; // speichert die Namen
	if(!EffectVar(1,pTarget,iEffectNumber)) EffectVar(1,pTarget,iEffectNumber) = []; // speichert den Wert

	var index = GetArrayItemPosition(szVarName,EffectVar(0,pTarget,iEffectNumber));
	if(index > -1)
	{
		EffectVar(0,pTarget,iEffectNumber)[index] = szVarName;
		EffectVar(1,pTarget,iEffectNumber)[index] = vValue;
	}
	else
	{
		PushBack(szVarName,EffectVar(0,pTarget,iEffectNumber));
		PushBack(vValue,EffectVar(1,pTarget,iEffectNumber));
	}
}  
