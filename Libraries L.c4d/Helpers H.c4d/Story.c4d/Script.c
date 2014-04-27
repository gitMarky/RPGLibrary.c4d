/*-- Story --*/

#strict 2

func Initialize() { FramesLeft = -1; }

func Start()
{
	ScheduleCall(this, "DoStart", 1, 1);
	//ScheduleCall(this, "CreateQuests", 1, 1);
	//this->~CreateQuests();
}

func DoStart()
{
	// Starte in einer anderen Section
	  SaveObjects();
	  LoadScenarioSection("Main", 3);
	  // Alles wiederherstellen
	  RestoreSectObjs();

	StartSzen();
}

func CreateQuests()
{
}

local NextFunc;
local FramesLeft;

func Timer()
{
	if(!FramesLeft)
	{
		FramesLeft = -1;
		Call(NextFunc);
	}
	if(FramesLeft > 0) FramesLeft--;
}

func SetNext(next, frames) { NextFunc = next; FramesLeft = frames; }

global func GetStory(){ return FindObject(_STY); }