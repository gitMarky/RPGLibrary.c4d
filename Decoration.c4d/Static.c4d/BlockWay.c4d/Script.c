/*--
Blocks the way, indestructible.
@title Road Block
@author Sven2
@version 0.1.0
--*/

#strict 2

protected func SetDirection(int dir)
{
	SetAction("Standard");
	SetPhase(dir);
	SetSolidMask(dir * 32, 0, 32, 32);
}