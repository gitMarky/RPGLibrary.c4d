/*-- Wasserdings --*/

#strict 2

protected func Initialize() 
{ 
  // Zuf�llige Form  
  SetAction("Random");
  SetPhase(Random(2));
  if (!Random(20)) SetPhase(2);
 }
