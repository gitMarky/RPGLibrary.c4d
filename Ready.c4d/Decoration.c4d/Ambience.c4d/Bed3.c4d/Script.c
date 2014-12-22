/*-- Military bed.
@author Marky, Hazard Team (graphics)
@version 0.2.0
--*/

#strict 2

#include BED1

local seat;
local graphics_dummy;

static const gBed3_MaxPlaces = 2;

/* Initialisierung */
protected func Initialize()
{
	if (!Contained())
	{
		seat = [];
		graphics_dummy = [];
		
		for (var i = 0; i < gBed3_MaxPlaces; i++)
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
	for (var i = 0; i < gBed3_MaxPlaces; i++)
	{
		if (seat[i])
		{
			SetGraphics(0, this, 0, i + 1, GFXOV_MODE_Object, 0, 0, graphics_dummy[i]);
			
			if (!seat[i]->GetAlive())
			{
				graphics_dummy[i]->SetAction("OccupiedSilent");
			}
			else
			{
				graphics_dummy[i]->SetAction("Occupied");
			}
		}
		else
			SetGraphics(0, this, 0, i + 1, 0);
	}
}

private func OnEntrance(object clonk)
{
	// already occupied?
	if (IsOccupied() >= gBed3_MaxPlaces)
		return CheckChallenge(clonk);
		
	// enter
	Enter(this, clonk);
	var i = 0;
	for (; i < gBed3_MaxPlaces; i++)
	{
		if (seat[i] == 0)
			break;
	}
	
	seat[i] = clonk;
	SetGraphics(Format("Sleep%d", i + 1), graphics_dummy[i]);
	SetColorDw(clonk->GetColorDw(), graphics_dummy[i]);
	graphics_dummy[i]->SetAction(Format("Occupied"));
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

func Zzz()
{
	var iTime = GetActTime();
	
	if (seat[0])
	{
		CreateParticle("Zzz", 8, -3, 2 + (iTime % 50) * 3 / 25 + RandomX(-1, +1), -5, 60, RGBa(255, 255, 255, 128));
	}
	if (seat[1])
	{
		CreateParticle("Zzz", 8, -23, 2 + ((iTime + 25) % 50) * 3 / 25 + RandomX(-1, +1), -5, 60, RGBa(255, 255, 255, 128));
	}
}