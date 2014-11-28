/* Helper functions */

#strict 2

// Richtung anpassen, so dass das Objekt zum anderen Objekt schaut
global func SetDirTo(object target, object looker)
{
	if (GetX(target) > GetX(looker))
		SetDir(DIR_Right, looker);
	else if (GetX(target) < GetX(looker))
		SetDir(DIR_Left, looker);
}

// Clonk anhalten
global func StopClonk(object target)
{
	if (!target)
		if (!this)
			return false;
	SetComDir(COMD_Stop, target);
	SetCommand(target, "None");
	Call("~StopAiming");
	return true;
}
  
// Clonk für Dialog vorbereiten: Anhalten, Menüs schließen, etc.
// Aber nicht aufhören zu reiten!
global func StopClonkEx(object target, bool keep_action)
{
	if (!target)
		if (!this)
			return;
	if (!keep_action)
	{
		if (!WildcardMatch(GetAction(target), "Ride*"))
		{
			if (target)
				ObjectSetAction(target, "Walk");
			else
				SetAction("Walk");
		}
	}
	StopClonk(target);
	CloseMenu(target);
	//ComboCloseMenu(target);
	//CloseCharMenu(target);
}
