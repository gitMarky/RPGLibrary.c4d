/*-- Story --*/

#strict 2

public func Initialize()
{
	FramesLeft = -1;
}

public func Start()
{
	ScheduleCall(this, "DoStart", 1, 1);
	//ScheduleCall(this, "CreateQuests", 1, 1);
	//this->~CreateQuests();
}

public func DoStart()
{
	// Start in a different section
	SaveObjects();
	LoadScenarioSection("Main", 3);
	
	RestoreSectObjs();
	
	StartSzen();
}

public func CreateQuests()
{
}

local NextFunc;
local FramesLeft;

public func Timer()
{
	if (!FramesLeft)
	{
		FramesLeft = -1;
		Call(NextFunc);
	}
	if (FramesLeft > 0)
		FramesLeft--;
}

public func SetNext(next, frames)
{
	NextFunc = next;
	FramesLeft = frames;
}

global func GetStory()
{
	return FindObject(_STY);
}