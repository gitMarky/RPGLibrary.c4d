/*-- Table where clonks can have a drink, for free ;)
@title Drinking Table
@author Randrian ({@link http://clonkforge.net/pr.php?pr=1415}), Marky (additional graphics for King, Mage, Sorcerer)
@version 0.2.0 --*/

#strict 2

local seat;
local graphics_dummy;

static const gDrinkTable_MaxPlaces = 3;

/* Initialisierung */
protected func DoInitialize()
{
	if (!Contained())
	{
		seat = [];
		graphics_dummy = [];
		
		for (var i = 0; i < gDrinkTable_MaxPlaces; i++)
		{
			seat[i] = 0;
			graphics_dummy[i] = CreateContents(GetID());
		}

		UpdateState();
	}
}

private func IsOccupied()
{
	var obj;
	var count = 0;
	if (obj = FindObject(0, 0, 0, 0, 0, OCF_Alive, 0, 0, this))
	{
		count++;
	}
	return count;
}

private func UpdateState()
{
	for (var i = 0; i < gDrinkTable_MaxPlaces; i++)
	{
		if (seat[i])
		{
			SetGraphics(0, this, 0, i + 1, GFXOV_MODE_Object, 0, 0, graphics_dummy[i]);
		}
		else
			SetGraphics(0, this, 0, i + 1, 0);
	}
}

protected func ActivateEntrance(object clonk)
{
	// entry request
	if (!Contained(clonk))
		if (GetOCF(clonk) & OCF_CrewMember)
			return OnEntrance(clonk);
			
	// exit request
	if (Contained(clonk) == this)
		return OnExit(clonk);
		
	return 0;
}

private func OnEntrance(object clonk)
{
	// already occupied?
	if (IsOccupied() >= gDrinkTable_MaxPlaces)
		return CheckChallenge(clonk);
		
	// enter
	Enter(this, clonk);
	var i = 0;
	for (; i < gDrinkTable_MaxPlaces; i++)
	{
		if (seat[i] == 0)
			break;
	}
	
	seat[i] = clonk;
	SetGraphics(Format("%i", GetID(clonk)), graphics_dummy[i]);
	SetColorDw(clonk->GetColorDw(), graphics_dummy[i]);
	graphics_dummy[i]->SetAction(Format("Wait%d", i + 1));
	UpdateState();
	
	return true;
}

private func OnExit(object clonk)
{
	Exit(clonk, 0, +15);
	var i = 0;
	for (; i < gDrinkTable_MaxPlaces; i++)
	{
		if (seat[i] == clonk)
			break;
	}
	seat[i] = 0;
	UpdateState();
}

private func CheckChallenge(object clonk)
{
	// a hostile clonk requests one of the occupants to exit
	var enemy = FindObject(0, 0, 0, 0, 0, OCF_Living, 0, 0, this);
	if (enemy)
	{
		if (Hostile(GetOwner(clonk), GetOwner(enemy)) || !GetAlive(enemy))
			OnExit(enemy);
	}
	
	return false;
}