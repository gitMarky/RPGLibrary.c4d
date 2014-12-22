/*-- Strohmann --*/

#strict 2

protected func Initialize()
{
  SetAction("Stand");
  return 1;
}

protected func QueryCatchBlow(pObj) 
{ 
  SetAction("GetPunched");
  return 1;
} 

protected func CatchBlow(iLevel, pObj) 
{ 
  // Kein Energieverlusst 
  DoEnergy(-iLevel); 
  SetAction("GetPunched");
  return 1;
} 

private func Fighting()
{
//  if (!Random(2)) SetAction("GetPunched");
  return(1);
}

public func IsTrainingPuppet() { return 1; }