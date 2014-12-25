/*-- 
@title Pit
@author Stephan Breer (Die Pyramide von Clucatan)
@version 0.2.0
 --*/

#strict 2


protected func Initialize()
{
	SetAction("Closed");
	SetSolidMask(0, 0, 40, 20);
}

private func CheckHero()
{
	if ((GetAction() == "Open") || (GetAction() == "Opened"))
		return;
	if (Var() = FindObject(0, -20, -30, 40, 20, OCF_Alive | OCF_CrewMember))
		SetAction("Open");
}

private func PutSolidMask()
{
	SetSolidMask(Max(GetPhase(), 3) * 40, 0, 40, 20);
}