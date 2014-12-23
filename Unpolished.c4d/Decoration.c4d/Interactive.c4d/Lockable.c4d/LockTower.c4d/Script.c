/*-- Lockable tower part with steel gate.
@author Sven2
@version 0.2.0
--*/

#strict 2

/* Initialisierung */

protected func Initialize()
{
	SetAction("Be");
}

public func SetDir(direction)
{
	if (direction == 1)
		SetSolidMask(15, 74, 15, 74);
	else
		SetSolidMask(15, 0, 15, 74);
	return _inherited(direction);
}