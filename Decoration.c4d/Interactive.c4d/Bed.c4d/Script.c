/*-- Bed --*/

#strict 2

/* Initialization */
protected func Initialize()
{
	UpdateState();
}

private func IsOccupied()
{
	var obj;
	if (obj = FindObject(0, 0, 0, 0, 0, OCF_Living, 0, 0, this))
	{
		if (!obj->GetAlive())
			return -1;
		return 1;
	}
	return 0;
}

private func UpdateState()
{
	var occ = IsOccupied();
	if (occ == 1)
	{
		if (GetAction() != "Occupied")
			return SetAction("Occupied");
	}
	else if (occ == -1)
	{
		if (GetAction() != "OccupiedSilent")
			return SetAction("OccupiedSilent");
	}
	else
	{
		if (GetAction() != "Empty")
			return SetAction("Empty");
	}
}

protected func ActivateEntrance(object pClonk)
{
	// On entry
	if (!Contained(pClonk))
		if (GetOCF(pClonk) & OCF_CrewMember)
			return OnEntrance(pClonk);
	// On leaving
	if (Contained(pClonk) == this)
		return OnExit(pClonk);
	return 0;
}

private func OnEntrance(object pClonk)
{
	if (IsOccupied())
		return CheckChallenge(pClonk);

	Enter(this, pClonk);
	UpdateState();
	return 1;
}

private func OnExit(object pClonk)
{
	Exit(pClonk, 0, +15);
	UpdateState();
}

public func Zzz()
{
	var iTime = GetActTime();
	CreateParticle("Zzz", 8, -3, 2 + (iTime % 50) * 3 / 25 + RandomX(-1, +1), -5, 60, RGBa(255, 255, 255, 128));
}
  
private func CheckChallenge(object pClonk)
{
	// An enemy clonk requests the sleeper
	var pEnemy = FindObject(0, 0, 0, 0, 0, OCF_Living, 0, 0, this);
	if (pEnemy)
	{
		if (Hostile(GetOwner(pClonk), GetOwner(pEnemy)) || !GetAlive(pEnemy))
			OnExit(pEnemy);
	}
	return 0;
}

