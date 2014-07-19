/*-- Fading --*/

#strict 2

global func FxFadeOutStart(target, no, temp, level, amount)
{
  if(!level)
    EffectVar(1, target, no) = 255;
  else
    EffectVar(1, target, no) = level;

  EffectVar(2, target, no) = Max(1,amount);
}

global func FxFadeOutTimer(target, no)
{
  EffectVar(0, target, no)+= EffectVar(2, target, no);
  SetObjAlpha(EffectVar(0, target, no),target);
  //SetClrModulation(RGBa(255,255,255, EffectVar(0, target, no)), target);
  if(EffectVar(0, target, no) >= EffectVar(1, target, no)) { if(EffectVar(1, target, no) >= 255) RemoveObject(target); return(-1); }
}

global func FadeOutNew(object pObject, int level, bool fFast, int iAmount)
{
  if(!pObject)
    pObject = this;

  if(GetEffect("*FadeOut*", pObject)) return;
  if( fFast ) return AddEffect("FadeOut", pObject, 101, 1,0,0,level, iAmount);
  return(AddEffect("FadeOut", pObject, 101, 2,0,0,level, iAmount));
}

global func FxFadeInStart(target, no, temp, level, amount)
{
  EffectVar(0, target, no) = 255;
  EffectVar(1, target, no) = level;
  EffectVar(2, target, no) = Max(1,amount);
}

global func FxFadeInTimer(target, no)
{
  EffectVar(0, target, no)-=EffectVar(2, target, no);
  SetObjAlpha(EffectVar(0, target, no),target);
  //SetClrModulation(RGBa(255,255,255, EffectVar(0, target, no)), target);
  if(EffectVar(0, target, no) <= EffectVar(1, target, no)) { return(-1); }
}

global func FadeInNew(object pObject, int level, bool fFast, int iAmount)
{
  if(!pObject)
    pObject = this;

  if(GetEffect("*FadeIn*", pObject)) return;
  if( fFast ) return AddEffect("FadeIn", pObject, 101, 1,0,0,level, iAmount);
  return(AddEffect("FadeIn", pObject, 101, 2,0,0,level, iAmount));
}

global func StopFading(int color, object pObject) {
  if(!pObject)
    pObject = this();

  var eff = GetEffectCount("*Fade*",pObject);
  for(eff; eff; eff--)
    RemoveEffect("*Fade*",pObject);

  SetClrModulation(color,pObject);
}

global func StopFadingNew(object pObject)
{
  if(!pObject)
    pObject = this();

  var eff = GetEffectCount("*Fade*",pObject);
  for(eff; eff; eff--)
    RemoveEffect("*Fade*",pObject);
  }


global func SetObjAlpha(int byAlpha,object pObj)
{
	var dwClrMod=GetClrModulation(pObj);
	if(dwClrMod == 0) dwClrMod = RGBa(255,255,255,0);
//	if (!dwClrMod)
//		dwClrMod = byAlpha<<24;
//	else dwClrMod = dwClrMod & 16777215 | byAlpha<<24;

	dwClrMod = SetRGBaValue(dwClrMod,BoundBy(byAlpha,0,255),0);
 	return(SetClrModulation(dwClrMod, pObj));
}
