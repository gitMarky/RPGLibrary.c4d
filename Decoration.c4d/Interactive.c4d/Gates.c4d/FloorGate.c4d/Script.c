/*-- 
A gate that slides in a certain direction.
@title Floor gate
@author Unknown
@version 0.1.0
--*/

#strict 2

local iSwitchDir;
local iManualDuration;

protected func Initialize()
{
	SetAction("Gate");
	SetComDir(iSwitchDir = COMD_Down);
	iManualDuration = 30;
}

/**
 * Starts upward movement.
 *
 * @param pController The object that ordered the command.
 *        Input from switches moves the object permanently.
 *        Input from other objects moves the object for a
 *        certain time, then reverts the movement.
 * @return int The new ComDir value of the block.
 */
public func ControlUp(object pController)
{
	[$DescUp$]
	
	return ControlSwitch(pController, COMD_Up);
}

/**
 * Starts downward movement.
 *
 * @param pController The object that ordered the command.
 *        Input from switches moves the object permanently.
 *        Input from other objects moves the object for a
 *        certain time, then reverts the movement.
 * @return int The new ComDir value of the block.
 */
public func ControlDown(object pController)
{
	[$DescDown$]
	
	return ControlSwitch(pController, COMD_Down);
}

/**
 * Starts leftward movement.
 *
 * @param pController The object that ordered the command.
 *        Input from switches moves the object permanently.
 *        Input from other objects moves the object for a
 *        certain time, then reverts the movement.
 * @return int The new ComDir value of the block.
 */
public func ControlLeft(object pController)
{
	[$DescUp$]
	
	return ControlSwitch(pController, COMD_Left);
}

/**
 * Starts rightward movement.
 *
 * @param pController The object that ordered the command.
 *        Input from switches moves the object permanently.
 *        Input from other objects moves the object for a
 *        certain time, then reverts the movement.
 * @return int The new ComDir value of the block.
 */
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
		Schedule("SetComDir(iSwitchDir)", iManualDuration);
	return SetComDir(comdir);
}
