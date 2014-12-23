/*-- 
A gate that slides in a certain direction.
@title Floor gate
@author Unknown
@version 0.1.0
--*/

#strict 2

local com_dir_by_switch;
local manual_use_reset_timer;

protected func Initialize()
{
	SetAction("Gate");
	SetComDir(com_dir_by_switch = COMD_Down);
	manual_use_reset_timer = 30;
}

/**
 * Starts upward movement.
 *
 * @par pController The object that ordered the command.
 *        Input from switches moves the object permanently.
 *        Input from other objects moves the object for a
 *        certain time, then reverts the movement.
 * @return int The new ComDir value of the block.
 */
public func ControlUp(object controller)
{
	[$DescUp$]
	
	return ControlSwitch(controller, COMD_Up);
}

/**
 * Starts downward movement.
 *
 * @par controller The object that ordered the command.
 *        Input from switches moves the object permanently.
 *        Input from other objects moves the object for a
 *        certain time, then reverts the movement.
 * @return int The new ComDir value of the block.
 */
public func ControlDown(object controller)
{
	[$DescDown$]
	
	return ControlSwitch(controller, COMD_Down);
}

/**
 * Starts leftward movement.
 *
 * @par controller The object that ordered the command.
 *        Input from switches moves the object permanently.
 *        Input from other objects moves the object for a
 *        certain time, then reverts the movement.
 * @return int The new ComDir value of the block.
 */
public func ControlLeft(object controller)
{
	[$DescUp$]
	
	return ControlSwitch(controller, COMD_Left);
}

/**
 * Starts rightward movement.
 *
 * @par controller The object that ordered the command.
 *        Input from switches moves the object permanently.
 *        Input from other objects moves the object for a
 *        certain time, then reverts the movement.
 * @return int The new ComDir value of the block.
 */
public func ControlRight(object controller)
{
	[$DescDown$]
	
	return ControlSwitch(controller, COMD_Right);
}

protected func Hit()
{
	Sound("Discharge");
}

protected func ControlSwitch(object controller, com_dir)
{
	if (GetComDir() != com_dir)
		Sound("Elevator");
	
	// Switches switch status permanently, a user for 30 frames only
	if (controller && controller->~IsSwitch())
		com_dir_by_switch = com_dir;
	else
		Schedule("SetComDir(iSwitchDir)", manual_use_reset_timer);
	return SetComDir(com_dir);
}
