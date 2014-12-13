/*--
A diagonal edge for temples.
@title Edge
@author Sven2
@version 0.1.0
--*/

#strict 2
#include LF_D

protected func Initialize()
{
	Snap();
	return true;
}

public func Snap(int offsetX, int phase)
{
	SetPosition((GetX() + 5) / 10 * 10 - 5 + offsetX, (GetY() + 5) / 10 * 10 - 5 + phase);
}