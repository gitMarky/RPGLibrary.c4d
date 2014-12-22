/*-- Wasserdings --*/

#strict 2

protected func Initialize() 
{ 
  // Zufällige Form  
  SetAction("Random");
  SetPhase(Random(2));
  if (!Random(20)) SetPhase(2);
 }
