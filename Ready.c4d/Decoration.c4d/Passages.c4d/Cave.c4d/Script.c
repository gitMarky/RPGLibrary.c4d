/*-- Cave
@author Unknown
@version 0.2.0
--*/

#strict 2

local link;

protected func Initialize()
{
	SetEntrance(1);
}

private func CheckContents()
{
	var exit = link;
	if (!exit) exit = this;
	var obj;
	while (obj = Contents())
	{
		Exit(obj, GetX(exit) - GetX(), GetY(exit) - GetY() + 15, 0, 0);
	}
}