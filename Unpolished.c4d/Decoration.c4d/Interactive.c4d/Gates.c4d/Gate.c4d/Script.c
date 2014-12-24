/*--
A gate that moves up or down.
@title Stone gate
@author Sven2
@version 0.1.0
--*/

#strict 2

local com_dir_by_switch;

protected func Initialize()
{
	SetAction("Gate");
	SetComDir(com_dir_by_switch = COMD_Down);
}

public func ControlLeft(object controller)
{
	[$DescUp$]
	return ControlUp(controller);
}

/**
 * Starts upward movement.
 *
 * @par controller The object that ordered the command. Only switches can switch gates.
 * @return int The new ComDir value of the block.
 */
public func ControlUp(object controller)
{
	[$DescUp$]

	return ControlSwitch(controller, COMD_Up);
}

public func ControlRight(object controller)
{
	[$DescDown$]
	return ControlDown(controller);
}

/**
 * Starts downward movement.
 *
 * @par pController The object that ordered the command. Only switches can switch gates.
 * @return int The new ComDir value of the block.
 */
public func ControlDown(object controller)
{
	[$DescDown$]

	return ControlSwitch(controller, COMD_Down);
}

protected func Hit()
{
	Sound("Discharge");
}

/**
 * Inverts the current direction of movement.
 * @par pController The object that ordered the command. Only switches can switch gates.
 */
public func Activate(object controller)
{
	if (GetComDir() == COMD_Up)
		ControlDown(controller);
	else
		ControlUp(controller);
}


public func RecheckTransfer(object clonk)
{
	// Open gate
	SetComDir(COMD_Up);
	SetAction("GateTempOpen");
	
	// Not open yet? Wait a little
	if (!GetContact())
	{
		AddCommand(clonk, "Wait", this, 0, 0, 0, 0, 10);
		return true;
	}
	// Go on
}

private func ControlSwitch(object controller, int com_dir)
{
	// User cannot switch gates
	if (!controller)
		return;
		
	if (!controller->~IsSwitch())
	{
		PlrMessage("$MsgNoDirect$", GetController(controller));
		Sound("Error", false, this, 100, GetController(controller) + 1);
		return;
	}
	else
	{
		com_dir_by_switch = com_dir;
		SetYDir();
	}
	
	if (GetComDir() != com_dir)
		Sound("Elevator");
	
	return SetComDir(com_dir);
}