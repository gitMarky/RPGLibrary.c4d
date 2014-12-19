/*
Scenario script
*/

#strict 2

func Initialize()
{
	// this should be empty
}

func DoStartSzen()
{
	var switch = CreateObject(_SWT, 0, 0, NO_OWNER);
	switch->SetPosition(162, 181);
	switch->TwoStatesSwitch();
	switch->StartInactive();
	
	var waterfall = PlaceWaterfall(420, 135, 230, 370, 0, 10);
	var fall = waterfall[0];
	var drain = waterfall[1];
	
	fall->SwitchOff();
	drain->SwitchOff();
	
	var states = [
	SwitchDef(0)->TargetCall(fall, "SwitchOff")->TargetCall(drain, "SwitchOn"),
	SwitchDef(1)->TargetCall(fall, "SwitchOn")->TargetCall(drain, "SwitchOff")
	];
	
	switch->SetSwitchStates(states);
}

protected func InitializePlayer(int player, int tx, int ty, object pBase, int iTeam)
{
	DoInitPlayer(player);
	return true;
}

protected func PlayerStart(int player, int tx, int ty, object pBase, int iTeam)
{
	var pClonk = GetCursor(player);
	pClonk->SetPosition(145, 160);
	return true;
}



// information for the script goal object
public func IsGoalFulfilled() { return false; }
public func GetGoalDesc() { return "This scenario cannot be fulfilled until it has valid script goals"; }
