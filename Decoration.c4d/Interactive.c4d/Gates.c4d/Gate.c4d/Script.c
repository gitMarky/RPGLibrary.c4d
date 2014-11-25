/*--
A gate that moves up or down.
@title Stone gate
@author Sven2
@version 0.1.0
--*/

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

/**
 * Starts upward movement.
 *
 * @param pController The object that ordered the command. Only switches can switch gates.
 * @return int The new ComDir value of the block.
 */
public func ControlUp(object pController)
{
	[$DescUp$]

	return ControlSwitch(pController, COMD_Up);
}

public func ControlRight(object pController)
{
	[$DescDown$]
	return ControlDown(pController);
}

/**
 * Starts downward movement.
 *
 * @param pController The object that ordered the command. Only switches can switch gates.
 * @return int The new ComDir value of the block.
 */
public func ControlDown(object pController)
{
	[$DescDown$]

	return ControlSwitch(pController, COMD_Down);
}

protected func Hit()
{
	Sound("Discharge");
}

/**
 * Inverts the current direction of movement.
 * @param pController The object that ordered the command. Only switches can switch gates.
 */
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

private func ControlSwitch(object pController, int iComDir)
{
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
		iSwitchDir = iComDir;
		SetYDir();
	}
	
	if (GetComDir() != iComDir)
		Sound("Elevator");
	
	return SetComDir(iComDir);
}