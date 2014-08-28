#strict 2

public func Initialize()
{
	SetAction("Check");
	
	var i = Random(15);
	var clonk = FindObject(KNIG, -150, -150, 300, 300);
	
	if (clonk)
	{
		if (i == 0)
		{
			Message("<%s>|Jetzt wird's gefährlich!", clonk, GetName(clonk));
			Sound("Massaker_01", clonk);
		}
		else if (i == 1)
		{
			Message("<%s>|Ich mach dich platt!", clonk, GetName(clonk));
			Sound("Massaker_02", clonk);
		}
		else if (i == 2)
		{
			Message("<%s>|Oh Gott!", clonk, GetName(clonk));
			Sound("Massaker_03", clonk);
		}
		else if (i == 3)
		{
			Message("<%s>|Stirb!", clonk, GetName(clonk));
			Sound("Massaker_04", clonk);
		}
		else if (i == 4)
		{
			Message("<%s>|Der Kampf kann beginnen!", clonk, GetName(clonk));
			Sound("Massaker_05", clonk);
		}
		else if (i == 5)
		{
			Message("<%s>|Nieder mit ihnen!", clonk, GetName(clonk));
			Sound("Massaker_06", clonk);
		}
	}
}

public func Timer()
{
	var pEnemy;
	
	if (!FindObject(KNIG, -150, -150, 300, 300))
		return RemoveObject();
	while (pEnemy = FindObject(0, -150, -150, 300, 300, OCF_Edible, 0, 0, 0, pEnemy))
		if (pEnemy->~IsGegner())
			if (GetAlive(pEnemy))
				return 0;
	RemoveObject();
}