/*-- Floor gate --*/

#strict 2

local iSwitchDir;

protected func Initialize()
{
	SetAction("Gate");
	SetComDir(iSwitchDir = COMD_Down);
}

public func ControlUp(object pController)
{
	[$DescUp$]
	
	return ControlSwitch(pController, COMD_Up);
}

public func ControlDown(object pController)
{
	[$DescDown$]
	
	return ControlSwitch(pController, COMD_Down);
}

public func ControlLeft(object pController)
{
	[$DescUp$]
	
	return ControlSwitch(pController, COMD_Left);
}

public func ControlRight(object pController)
{
	[$DescDown$]
	
	return ControlSwitch(pController, COMD_Right);
}

protected func Hit()
{
	Sound("Discharge");
}

protected func ControlSwitch(object pController, comdir)
{
	if (GetComDir() != comdir)
		Sound("Elevator");
	
	// Switches switch status permanently, a user for 30 frames only
	if (pController && pController->~IsSwitch())
		iSwitchDir = comdir;
	else
		Schedule("SetComDir(iSwitchDir)", 30);
	return SetComDir(comdir);
}
