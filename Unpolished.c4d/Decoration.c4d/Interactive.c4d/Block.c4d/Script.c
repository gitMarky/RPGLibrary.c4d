/*--
A block than can move in all four directions. Usually the movement is started by a switch.

@author Sven2
@version 0.1.0
--*/

#strict 2

local iXDir, iYDir;

static const gBlock_MoveSound = "Elevator",
             gBlock_HitSound  = "Discharge";

protected func Initialize()
{
	SetAction("Block");
}

/**
 * Starts upward movement.
 *
 * @par pController The object that ordered the command.
 * @return int The new ComDir value of the block.
 */
public func ControlUp(object pController)
{
	if (GetXDir())
		return;
	
	if (iYDir >= 0)
		Sound(gBlock_MoveSound);
	
	iYDir = -1;
	iXDir = 0;
	
	return UpdateComDir();
}

/**
 * Starts downward movement.
 *
 * @par pController The object that ordered the command.
 * @return int The new ComDir value of the block.
 */
public func ControlDown(object pController)
{
	if (GetXDir())
		return;
	
	if (iYDir <= 0)
		Sound(gBlock_MoveSound);
	
	iYDir = 1;
	iXDir = 0;
	
	return UpdateComDir();
}

/**
 * Starts leftward movement.
 *
 * @par pController The object that ordered the command.
 * @return int The new ComDir value of the block.
 */
public func ControlLeft(object pController)
{
	if (Abs(GetYDir()) > 2)
		return;
	
	if (iXDir >= 0)
		Sound(gBlock_MoveSound);
	SetYDir();
	
	iXDir = -1;
	iYDir = 0;
	
	return UpdateComDir();
}

/**
 * Starts rightward movement.
 *
 * @par pController The object that ordered the command.
 * @return int The new ComDir value of the block.
 */
public func ControlRight(object pController)
{
	if (Abs(GetYDir()) > 2)
		return;
	
	if (iXDir <= 0)
		Sound(gBlock_MoveSound);
	SetYDir();
	
	iXDir = 1;
	iYDir = 0;
	
	return UpdateComDir();
}

protected func ContactLeft()
{
	if (iXDir >= 0)
		return;
		
	Sound(gBlock_HitSound);
	return iXDir = 0;
}
protected func ContactRight()
{
	if (iXDir <= 0)
		return;
		
	Sound(gBlock_HitSound);
	return iXDir = 0;
}

protected func Hit()
{
	Sound(gBlock_HitSound);
	
	SetXDir();
	SetYDir();
	
	iXDir = iYDir = 0;
	SetComDir();
}

private func UpdateComDir() // only for iXDir|iYDir != 0
{
	// Puzzling, isn't it?
	return SetComDir(iXDir * (iYDir - 2) + !iXDir * (iYDir - 1) * 2 + 5);
}
