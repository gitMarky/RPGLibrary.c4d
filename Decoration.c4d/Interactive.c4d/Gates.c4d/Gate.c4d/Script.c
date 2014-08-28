/*-- Steintor - by Sven 2 --*/

#strict 2

local iSwitchDir;

protected func Initialize()
{
	SetComDir(iSwitchDir = COMD_Down);
}

public func ControlLeft(object pController)
{
	[$DescUp$]
	return ControlUp(pController);
}

public func ControlUp(object pController)
{
	[$DescUp$]
	// User cannot switch gates
	if (!pController)
		return;
	if (!pController->~IsSwitch())
	{
		PlrMessage("$MsgNoDirect$", GetController(pController));
		Sound("Error", false, this, 100, GetController(pController) + 1);
		return;
	}
	else
	{
		iSwitchDir = COMD_Up;
		SetYDir();
	}
	
	if (GetComDir() != COMD_Up)
		Sound("Elevator");
		
	return SetComDir(COMD_Up);
}

public func ControlRight(object pController)
{
	[$DescDown$]
	return ControlDown(pController);
}

public func ControlDown(object pController)
{
	[$DescDown$]
	// Benutzer kann Steintore nicht schalten
	if (!pController)
		return;
	if (!pController->~IsSwitch())
	{
		PlrMessage("$MsgNoDirect$", GetController(pController));
		Sound("Error", false, this, 100, GetController(pController) + 1);
		return;
	}
	else
	{
		iSwitchDir = COMD_Down;
		SetYDir();
	}
	// 2do: sound
	if (GetComDir() != COMD_Up)
		Sound("Elevator");
	return SetComDir(COMD_Down);
}

protected func Hit()
{
	Sound("Discharge");
}

public func Activate(object pController)
{
	if (GetComDir() == COMD_Up)
		ControlDown(pController);
	else
		ControlUp(pController);
}

public func RecheckTransfer(object pClonk)
{
	// Open gate
	SetComDir(COMD_Up);
	SetAction("GateTempOpen");
	
	// Not open yet? Wait a little
	if (!GetContact())
	{
		AddCommand(pClonk, "Wait", this, 0, 0, 0, 0, 10);
		return true;
	}
	// Go on
}
