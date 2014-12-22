/*-- Can be used outside of arguments, too!
@title Strawman
@author Randrian ({@link http://clonkforge.net/pr.php?pr=1415})
@version 0.2.0
--*/

#strict 2

protected func Initialize()
{
  SetAction("Stand");
  return true;
}

protected func QueryCatchBlow(pObj) 
{ 
  SetAction("GetPunched");
  return true;
} 

protected func CatchBlow(damage, obj) 
{ 
  // No energy loss
  DoEnergy(-damage); 
  SetAction("GetPunched");
  return true;
} 

private func Fighting()
{
  return true;
}

public func IsTrainingPuppet() { return true; }