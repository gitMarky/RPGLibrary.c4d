/* Objekte, denen die KI ausweichen sollte */

#strict 2

#appendto ELEC

protected func UpdateTransferZone()
{
  return SetTransferZone();
}

private func FindWaitingClonk()
{
  var clnk, best, proc;
  while(clnk = FindObject(0, -30, RangeTop - GetY(), 60, 6000, OCF_CrewMember, 0, 0, NoContainer(), clnk) )
  {
		if(GetPlayerType(GetOwner(clnk)) == C4PT_Script) continue; // Scriptplayer fordern es sich selber an! (somit blockieren sie sich auch nicht gegenseitig)
    proc = GetProcedure(clnk);
    if(GetComDir(clnk) == COMD_Stop || (proc == "SWIM" && Inside(GetXDir(clnk), -5, 5)) )
    {
      if(proc == "WALK" || proc == "PUSH" || proc == "SCALE" || proc == "HANGLE" || proc == "SWIM")
        if(!Hostile(GetController(clnk), GetOwner()) )
        {
          // Nicht erreichbar?
          if(GetY(clnk) < GetY() - 7) if(!PathFree(GetX(), GetY(), GetX(), GetY(clnk)) ) continue;
          if(GetY(clnk) > GetY() + 7) if(!PathFree(GetX(), GetY() + 16, GetX(), GetY(clnk)) ) continue;
          // Priorität prüfen (Cursor hat Priorität, sonst nächster Clonk)
          if(!best)
            best = clnk;
          else if(GetCursor(GetController(clnk)) == clnk)
            best = clnk;
          else if(GetCursor(GetController(best)) != best)
            if(ObjectDistance(clnk) < ObjectDistance(best))
              best = clnk;
        }
    }
  }
  return best;
}
