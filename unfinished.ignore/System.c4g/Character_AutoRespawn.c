/* Wiederbelebung */

#strict 2

#appendto CLNK

static g_fIntroSkiped;

global func Nothing() { return; }

func GetPortrait (object pObj, bool fGetID, bool fGetPermanent)
{
	var result = _inherited(pObj, fGetID, fGetPermanent);
	if(fGetID)
	{
		if(idPortrait) return idPortrait;
		if(result) return result;
		return GetID();
	}
	if(szPortrait) return szPortrait;
	if(result) return result;
	return "1";
}

local szPortrait;
local idPortrait;

func SetPortrait (string szNewPortrait, object pTarget, id idSrcDef, bool fPermanent, bool fCopyGfx)
{
	szPortrait = szNewPortrait;
	idPortrait = idSrcDef;
	return _inherited(szNewPortrait, pTarget, idSrcDef, fPermanent, fCopyGfx);
}

//C4SECT_SaveLandscape, C4SECT_SaveObjects und C4SECT_KeepEffects

public func ContextLoad(pCaller)
{
  [Laden|Image=_SAV:1]
  ReloadGame();
  return 1;
}

public func ContextSave(pCaller) 
{
  [Speichern|Image=_SAV]
  SaveGame();
  return 1;
}

static g_szSectionName;

global func LoadScenarioSection(szName, iFlags)
{
	g_szSectionName = szName;
	return _inherited(szName, iFlags, ...);
}

global func GetScenarioSection()
{
	if(!g_szSectionName) return "main";
	return g_szSectionName;
}

protected func MenuQueryCancel()
{
 // Die Relaunchmessage verschwindet nach dem Schließen des Relaunchmenüs
 if (GetMenu() == WIPF) return(1);
}

func RestartGame()
{
	ReloadGame();
}

func StopGame()
{
  EliminatePlayer(GetOwner());
}

func FxIntLaunchStop(pTarget)
{
  DoLaunch(pTarget);
}

//SetObjDrawTransform(-800, 0, 1000, 0, 1000, 1000, this, 1)
global func ReloadGame()
{
	StopFilm();
  SaveObjects();
  // Auf in die nächste Section! 
  LoadScenarioSection("main", 0);
  // Auf in die nächste Section! 
  //LoadScenarioSection("Map", 0);
  // Alles wiederherstellen
  RestoreSectObjs(!g_GameSaved);

  if(!g_GameSaved)
  {
    GameCall("Initialize");
  }
}

static g_GameSaved;

global func SaveGame()
{
  SaveObjects(1);
  // Auf in die nächste Section! 
  LoadScenarioSection("main", 3);
  // Auf in die nächste Section! 
  //LoadScenarioSection("Map", 0);
  g_GameSaved = 1;
  // Alles wiederherstellen
  RestoreSectObjs();
}

global func CallOnRestore()
{
	for(var obj in FindObjects(Find_InRect(-GetX(),-GetY(),LandscapeWidth(),LandscapeHeight())))
		obj->~OnRestore();
	GameCall("OnRestore");
}

/*-- In order for RPG Library to function correctly, the scenario has to be set up in a certain way.
This should not be too limiting, though.
{@section Initializing a player}
This is where the fun begins. The most convenient way to get things to works is calling DoInitPlayer(),
which enables fog of war and relaunches for the player and creates a story object.
So the minimal player initialization script is:
{@code
protected func InitializePlayer(int player, int tx, int ty, object pBase, int iTeam)
{
	DoInitPlayer(player);
}
}
Once the first player joins the game, and a story object has created (either by DoInitPlayer() or 
by a custom script) the story object does its part. It calls DoStart(), which loads the scenario section
"Main" and does the necessary stuff for changeing sections. Then the story object calls StartSzen(), which
issues the GameCall("DoStartSzen") and also calls DoStartSzen() in the story object (this function is not
defined so that it can be used for further customization) and creates a script goal for the scenario.
In short:
{@code
Scenario:InitializePlayer() -> Global:DoInitPlayer() -> Story:DoStart() -> Global:StartSzen() -> Game, Story:DoStartSzen()
}
{@section Scenario Script Stub}
So our basic scenario script should look like this:
{@code
/bc
Scenario script
bc/
@br
#strict 2
@br
	func Initialize()
	{
		// this should be empty
	}
@br	
	func DoStartSzen()
	{
		// do the scenario setup, empty for now
	}
@br	
	protected func InitializePlayer(int player, int tx, int ty, object pBase, int iTeam)
	{
		DoInitPlayer(player);
	}
@br
	// information for the script goal object
	public func IsGoalFulfilled() { return false; }
	public func GetGoalDesc() { return "This scenario cannot be fulfilled until it has valid script goals"; }
}
{@b Alternative Architectures}
Differing from this approach should pose no problem. In fact, calling {@code
Scenario:Initialize() -> Global:StartSzen() -> Game, Story:DoStartSzen()
Scenario:InitializePlayer() -> Global:DoInitPlayer() //( -> Story:DoStart() is not called, if the story object already exists)
}
is a valid approach. Initializing everything from InitializePlayer() solves the problem of stuff happening before players
exist, though.
@title Scenario Basics: Script Architecture
@category #Tutorials
@file tutorialScenarioBasicsStructure.html
@author Marky
--*/
