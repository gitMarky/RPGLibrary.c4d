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
	// create Marvin the sorcerer
	var sorcerer = CreateObject(SCLK, 200, 390, NO_OWNER);
	sorcerer->SetName("Marvin");
	AttachDialogue(sorcerer, "Marvin");
	
	// create Marvin's tower
	CreateObject(WTWR, 215, 400, NO_OWNER);
	
	// create the castle
	CreateObject(CST2, 310, 400, NO_OWNER);
	
	// create Arthur the knight
	var knight = CreateObject(KNIG, 345, 390, NO_OWNER);
	knight->SetName("Arthur");
	AttachDialogue(knight, "Arthur");
	
	// create the black armour
	// CreateContents(BRMR, tower);

	CreateQuest(marvins_quest);
	
	CreateObject(TIME);
	AttachDayNightCycle(sorcerer,"Marvin");
	AttachDayNightCycle(knight, "Arthur");
}

protected func InitializePlayer(int player, int tx, int ty, object pBase, int iTeam)
{
	DoInitPlayer(player);
}

// information for the script goal object
public func IsGoalFulfilled() { return false; }
public func GetGoalDesc() { return "This scenario cannot be fulfilled until it has valid script goals"; }
