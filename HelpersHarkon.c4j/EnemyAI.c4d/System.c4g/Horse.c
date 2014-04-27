/* Objekte, denen die KI ausweichen sollte */

#strict

#appendto HORS

private func Activity()
{
  // Locals übertragen (für alte Savegames)
  if(!iGait && Local(0)) { iGait = (Local(0)+1)%5; if(GetComDir() == COMD_None()) iGait = 0; Local(0) = 0; }  
  if(!idSaddle && Local(1)) { idSaddle = Local(1); Local(1) = 0; }
  // Angehalten: Stehen
  if (GetComDir() == COMD_Stop())
    if (!GetXDir())
      if (GetAction() eq "Walk" || GetAction() eq "Trot" || GetAction() eq "Gallop")
			{  SetAction("Stand"); iGait = 0; }
  // Bewegungskommando: Aktion anpassen
  if (GetCommand() eq "MoveTo")
    MoveToAdjustAction();
  // Ggf. Reiterposition anpassen
  if(GetAction() ne "Jump")
    ResetRiderPos();   
  // Pferd steht?
  if(GetAction() eq "Stand") SetGait(iGait);
  // Anhalten, wenn ohne Reiter
  return(StopRunning());
}