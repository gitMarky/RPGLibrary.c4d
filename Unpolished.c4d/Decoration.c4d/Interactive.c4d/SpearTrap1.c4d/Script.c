/*--
@title Spear trap 1
@version 0.2.0
@author Stephan Breer (Die Pyramide von Clucatan)
--*/

#strict 2

local active;

private func Initialize()
{
	SetAction("Down");
}

private func Activate()
{
	active = true;
	
	if (GetAction() == "Down") SetAction("Rise");
}

public func Deactivate()
{
	active = false;
}

public func ShouldRise()
{
	if (active) SetAction("Rise");
}

private func HurtHero()
{
	var target;
	if (target = FindObject(0, -3, -13, 6, 13, OCF_Alive | OCF_CrewMember))
		DoEnergy(-5, target);
	if (GetAction() == "Up") Sound("Click");
}
