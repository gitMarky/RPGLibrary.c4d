/*-- Bett --*/

#strict 2

/* Initialisierung */
protected func Initialize()
{
  UpdateState();
}

func IsAssassinable() { return 1; }
func OnAssassination() { return UpdateState(); }

private func IsOccupied()
{
	var obj;
  if (obj = FindObject2(Find_OCF(OCF_Living), Find_Container(this), Find_Func("IsClonk")))
	{
		if(!obj->GetAlive())
			return -1;
    return 1;
	}
  return 0;
}

public func IsFree() { return !IsOccupied(); }

private func UpdateState()
{
	var occ = IsOccupied();
  if (occ == 1)
  {
    if(GetAction() != "Occupied")
      return SetAction("Occupied");
  }
  else if (occ == -1)
  {
    if(GetAction() != "OccupiedSilent")
      return SetAction("OccupiedSilent");
  }
  else   
  {
    if(GetAction() != "Empty")
      return SetAction("Empty");
  }
}

protected func ActivateEntrance(object pClonk)
{
  // Eingangsanfrage
  if(!Contained(pClonk) )
    if(GetOCF(pClonk) & OCF_CrewMember)
      return(OnEntrance(pClonk));
  // Ausgangsanfrage
  if(Contained(pClonk) == this() )
    return(OnExit(pClonk));
  return(0); 
}

private func OnEntrance(object pClonk)
{
  // Schon besetzt
  if (IsOccupied()) return(CheckChallenge(pClonk));
	// Leiche reinlegen
	if(pClonk->~IsCarryingCorps())
	{
		Enter(this, pClonk->~GetCorps(1));
		UpdateState();
		FinishCommand(pClonk, 1);
		return 1;
	}
  // Betreten
  Enter(this(),pClonk);
  UpdateState();
  return(1);
}

private func OnExit(object pClonk)
{
  Exit(pClonk,0,+15);
  UpdateState();
}

func Zzz()
{
	var iTime = GetActTime();
	CreateParticle("Zzz",8*(1-2*GetDir()),-3,2+(iTime%50)*3/25+RandomX(-1,+1),-5,60,
                 RGBa(255,255,255,128));
}
  
private func CheckChallenge(object pClonk)
{
  // Ein feindlicher Clonk von außen fordert den Insassen
  var pEnemy = FindObject(0,0,0,0,0,OCF_Living,0,0,this);
  if(pEnemy)
	{
    if(!GetAlive(pEnemy))
    {
      OnExit(pEnemy);
      if(!Hostile(GetOwner(pClonk), GetOwner(pEnemy)))
        pClonk->AI_LookForFriend(pEnemy);
    }
    else if(Hostile(GetOwner(pClonk), GetOwner(pEnemy)))
		{
      OnExit(pEnemy);
			pEnemy->~DoShow(200);
			pClonk->~DoShow(200);
			if (pEnemy->~IsAI())
			  pEnemy->AITimer(pClonk);
      if (pClonk->~IsAI())
			  pClonk->AITimer(pEnemy);
//			pEnemy->FightWith(pClonk);
		}
	}
  return(0);
}

/* Bett lädt Energie auf */  
  
private func RefillEnergy()
{
  var clonk = FindObject(0, 0,0,0,0, OCF_CrewMember, 0,0, this);
  if(clonk) DoEnergy(1, clonk);
}

