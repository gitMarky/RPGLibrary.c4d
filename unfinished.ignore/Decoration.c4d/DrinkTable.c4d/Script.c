/*-- Bett --*/

#strict 2

local aPlaces;
local aDoubles;

/* Initialisierung */
protected func DoInitialize()
{
	if(!Contained())
	{
		aPlaces =  [0,0,0];
		aDoubles = [CreateContents(GetID()), CreateContents(GetID()), CreateContents(GetID())];
		UpdateState();
	}
}

func IsAssassinable() { return 1; }
func OnAssassination() { return UpdateState(); }

private func IsOccupied()
{
	var obj;
	var count = 0;
  if (obj = FindObject(0, 0,0,0,0, OCF_Alive, 0,0, this))
	{
    count++;
	}
  return count;
}

private func UpdateState()
{
	for(var i = 0; i < 3; i++)
	{
		if(aPlaces[i])
		{
			SetGraphics(0, this, 0, i+1, GFXOV_MODE_Object, 0, 0, aDoubles[i]);
		}
		else
			SetGraphics(0, this, 0, i+1, 0);
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
  if (IsOccupied() >= 3) return(CheckChallenge(pClonk));
  // Betreten
  Enter(this(),pClonk);
	var i = 0;
	for(;i<3;i++)
	{
		if(aPlaces[i]==0) break;
	}
	aPlaces[i] = pClonk;
	SetGraphics(Format("%i", GetID(pClonk)), aDoubles[i]);
	SetColorDw(pClonk->GetColorDw(), aDoubles[i]);
	aDoubles[i]->SetAction(Format("Wait%d", i+1));
  UpdateState();
  return(1);
}

private func OnExit(object pClonk)
{
  Exit(pClonk,0,+15);
	var i = 0;
	for(;i<3;i++)
	{
		if(aPlaces[i]==pClonk) break;
	}
	aPlaces[i] = 0;
  UpdateState();
}

private func CheckChallenge(object pClonk)
{
  // Ein feindlicher Clonk von außen fordert den Insassen
  var pEnemy = FindObject(0,0,0,0,0,OCF_Living,0,0,this);
  if(pEnemy)
	{
    if(Hostile(GetOwner(pClonk), GetOwner(pEnemy)) || !GetAlive(pEnemy))
      OnExit(pEnemy);
	}
  return(0);
}