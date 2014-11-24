/* Wiederbelebung */

#strict 2

#appendto CLNK

static g_fIntroSkiped;

global func Nothing() { return; }

global func DoInitPlayer(int player)
{
	var index = 0;
	while(GetPlayerByIndex(index, C4PT_User) != player && index < GetPlayerCount(C4PT_User)) index++;
	var pClonk = GetHiRank(player);
	SetFoW(1, player);
	SetPlrViewRange(0, pClonk);
	pClonk->SetRelaunch();

	// Storyobject erzeugen
	if(!FindObject(_STY))
	{
		CreateObject(_STY)->~Start();
	}
}

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

global func StartSzen()
{
	for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
	{
		GetCursor(GetPlayerByIndex(i,C4PT_User))->~StopClonkEx();
		GameCall("PlayerStart", GetPlayerByIndex(i,C4PT_User));
	}
	GameCall("DoStartSzen", GetPlayerByIndex(i,C4PT_User));

	// Spielziel erzeugen
	if(!FindObject(SCRG)) CreateObject(SCRG);
	
	// Neu Initialisierung
	var story = FindObject(_STY);
	if(GetID() == _STY) story = this;
	if(!story) story = CreateObject(_STY);
	story->~DoStartSzen();

	// Gemeinschaftskonto
	if(!FindObject(TACC) && GetPlayerCount(C4PT_User) > 1) CreateObject(TACC);
}

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

public func SetRelaunch()
{
  if (GetEffect("IntRelaunch", this)) return;
  return AddEffect("IntRelaunch", this, 200, 0, this);
}

protected func FxIntRelaunchEffect(string szNewEffect)
  {
  if(szNewEffect == "IntRelaunch") return -1;
  if(szNewEffect == "ReincarnationPSpell") Local(10)=1;
  }

protected func FxIntRelaunchStop(object pTarget, int iEffectNumber, int iReason, int fTmp)
  {
  // Temporäres entfernen: nichts machen
  if(fTmp) return;
  // Noch am Leben? Nichts machen
  if(GetAlive(pTarget)) return -1;
  // Nur wenn noch Besitzer da ist
  if(GetOwner(pTarget)<0) return 1;
  // Nicht wenn es eine Kopie ist
  if(GetEffect("ReplicationSpell", pTarget)) return 1;
  if(GetEffect("AI", pTarget)) return 1;
  if(GetPlayerType(GetOwner(pTarget)) == C4PT_Script) return 1;

  // Wenn er entfernt wurde, neu erzeugen
  if(iReason == 3 || GetY(pTarget)>LandscapeHeight())
  {
    var pOld = pTarget;
    var pSymbol = CreateObject(CXCP,  GetX(pTarget)-GetX(), LandscapeHeight()-GetY()-10, GetOwner());
    pTarget = CreateObject(GetID(pTarget), GetX(pTarget)-GetX(), LandscapeHeight()-GetY()-10, GetOwner());
    Enter(pSymbol, pTarget);
//    MakeCrewMember(pTarget, GetOwner(pTarget));
    GrabObjectInfo(this, pTarget);
    if(!GetCursor(GetOwner())) SetCursor(GetOwner(), pTarget);
    SetPlrViewRange(150, pTarget);
    while(Contents(0,pOld)) Enter(pTarget,Contents(0,pOld));
    // Attachte Objekte sichern (Schilde, Amulette, ect...)
    for(var obj in FindObjects(Find_ActionTarget(pOld)))
    {
      // Nur wirklich attachte Objekte (z.B. keine kämpfenden)
      if(GetProcedure(obj)=="ATTACH")
        obj->~SetActionTagets(pTarget);
    }
    // Spawnkoordinaten übernehmen
    for(var i = 0; i < 8; i++)
      pTarget->Local(10+i) = Local(10+i, pOld);
    ScheduleCall(pTarget, "OpenRelauchMenu", 1, 1, "Du bist gestorben.");
  }
  else
  {
    // Wenn nicht tot, nichts machen
//    if(GetAlive(pTarget)) return -1;
    // Wiederbeleben
    DoEnergy(-GetEnergy(pTarget), pTarget);
    Extinguish(pTarget);
    SetAlive(1, pTarget);
    SetAction("Dead", pTarget);
    DeathAnnounce ();
    Sound("Die");
    pTarget->OpenRelauchMenu("Du bist gestorben.");
		Schedule("SetAlive(0)",1);
  }
//  pTarget->~UnHide();
  
  return -1;
}

local aRespawnMsg;

public func OpenRelauchMenu(szMessage)
{
	if(!aRespawnMsg) aRespawnMsg = [szMessage];
	else
	{
		aRespawnMsg[GetLength(aRespawnMsg)] = szMessage;
		CloseMenu();
	}
  var pTarget = this;
	var pPicture = this;
	if(g_pCamera) pTarget = g_pCamera;
  var portrait = Format("Portrait:%i::%x::%s", pPicture->GetPortrait(0, 1), GetColorDw(pTarget), pPicture->GetPortrait(0, 0));
  CreateMenu(WIPF,pTarget,this,0,Format("<c %x>%s:</c>", pPicture->GetColorDw(), pPicture->GetName()),0,C4MN_Style_Dialog);
  AddMenuItem(portrait,0,NONE,pTarget,0,0,0,5);
	for(var text in aRespawnMsg)
		AddMenuItem(text,0,NONE,pTarget);
  AddMenuItem("Neustarten", "ReloadGame()", LMM2, pTarget, 0, 0, 0, 2, 4);
//  AddMenuItem("Beenden", "StopGame", MCMX, pTarget);
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

func DoLaunch(pTarget)
{
  SetRelaunch();
  CloseMenu();
  aRespawnMsg = 0;
  SetPlrViewRange(150);
  ObjectSetAction(this, "Walk", 0, 0, 1);
  Message(" ", this);
  if(Local(12, pTarget))
    DoEnergy(Local(12, pTarget)-GetEnergy());
  else DoEnergy(100);

  if(Local(14))
  {
   SetAction(Local(14), Object(Local(20)));
   SetPhase(Local(15));
   SetDir(Local(16));
  }
  if(Local(17))
    Enter(Object(Local(17)));
  SetComDir(COMD_Stop);

	if(Local(19))
		this->~SetDisguised(Local(19));
	else
		this->~SetNotDisguised();
	
	if(Local(18))
		this->~SetHideValue(Local(18));

	if(Local(21))
	{
		Object(Local(21)[0])->SetAction("GettingCarried", this);
		this->~UnHide();
		if(!GetEffect("CarryControlCorps", this))
			AddEffect("CarryControlCorps", this, 1, 1, this, 0, Object(Local(21)[0])->Contents(), Object(Local(21)[0]));
		var iEff = GetEffect("CarryControlCorps", this);
		EffectVar(0, this, iEff) = Local(21)[0];
		EffectVar(1, this, iEff) = Local(21)[1];
		EffectVar(2, this, iEff) = Local(21)[2];
		SetGraphics(0, this,0, 11, GFXOV_MODE_Object, 0, 0, Object(EffectVar(1, this, iEff)));
	}
	else
		RemoveEffect("CarryControlCorps", this);
	
  DoWealth(GetOwner(), Local(13)-GetWealth(GetOwner()));

  Extinguish(pTarget);
  // An die Stelle setzen
  SetPosition(pTarget->Local(10), pTarget->Local(11), pTarget);
  SetXDir(0, pTarget);
  SetYDir(0, pTarget);
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

global func SaveObjects(fAdjustRespawn)
{
	for(var obj in FindObjects(Find_InRect(-GetX(),-GetY(),LandscapeWidth(),LandscapeHeight())))
		obj->~OnSave();
	GameCall("OnSave");
	
  // Spieler sichern
  var iCount, obj, container;
  for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
  {
    iCount = GetCrewCount(GetPlayerByIndex(i,C4PT_User));
    while (iCount--) {
      obj = GetCrew(GetPlayerByIndex(i,C4PT_User), iCount);
			if(!obj->GetAlive())
				obj->SetAlive(1);
      if( fAdjustRespawn)
      {
        Local(10, obj) = GetX(obj);
        Local(11, obj) = GetY(obj);
				Local(12, obj) = GetEnergy(obj);
				Local(13, obj) = GetWealth(GetOwner(obj));
				Local(14, obj) = GetAction(obj);
				Local(15, obj) = GetPhase(obj);
				Local(16, obj) = GetDir(obj);
				Local(20, obj) = ObjectNumber(obj->GetActionTarget());
				Local(18, obj) = obj->~GetHideValue();
				Local(19, obj) = obj->~IsDisguised();
				if(GetEffect("CarryControlCorps", obj))
				{
					var iEff = GetEffect("CarryControlCorps", obj);
					Local(21, obj) = [EffectVar(0, obj, iEff), EffectVar(1, obj, iEff), EffectVar(2, obj, iEff)];
				}
				else
					Local(21, obj) = 0;
				if(Contained(obj))
					Local(17, obj) = ObjectNumber(Contained(obj));
				else Local(17, obj) = 0;
				container = obj->CreateObject(CXCN, 0, 0, -1);
				while(Contents(0, obj)) Enter(container, Contents(, obj));
				SetName(GetName(obj), container);
      }
      else
			{
				if(Contained(obj)) obj->Exit();
				obj->~UnHide();
				while(Contents(0, obj)) Exit(Contents(, obj));
			}
      Exit(obj);
//      ObjectSetAction(obj, "Walk");
      SaveSectObj(obj);
      SetPosition(100, 100, obj);
    }
  }
  if(this) SaveSectObj(this);
}

static g_iSaveSectIndex;

global func SaveSectObj(object obj)
  {
  // Kein Objekt?
  if(!obj) return;
  if(GetID(obj) == WCHR) return; // Bleibt da!
	if(GetID(obj) == _CYH) return;
  // Hatten wir das Objekt schon? (Endlosrekursionen sollen vermieden werden)
  var pTest, i;
  while(pTest = Global(20+(i++)*3)) if(pTest == obj) return;
  // Inhalt sichern
  var o2,i=obj->ContentsCount(); while (o2=obj->Contents(--i)) SaveSectObj(o2);
  // Attachte Objekte sichern (Schilde, Amulette, ect...)
  o2 = 0;
  for(o2 in FindObjects(Find_ActionTarget(obj)))
  {
    // Nur wirklich attachte Objekte (z.B. keine kämpfenden)
    if(GetProcedure(o2)=="ATTACH")
      SaveSectObj(o2);
  }
  // Objekt sichern
	Global(20+g_iSaveSectIndex) = [];
	Global(20+g_iSaveSectIndex)[1] = GetX(obj);
  Global(20+g_iSaveSectIndex)[2] = GetY(obj);
  (Global(20+(g_iSaveSectIndex++))[0] = obj)->SetObjectStatus(2);
  }

global func RestoreSectObjs(fReInitPlayer)
  {
	for(var obj in FindObjects(Find_InRect(-GetX(),-GetY(),LandscapeWidth(),LandscapeHeight())))
		obj->~OnRestore();
	GameCall("OnRestore");

  var obj;
  // Alles gespeicherte wieder herstellen
	var i = g_iSaveSectIndex;
  if (g_iSaveSectIndex)
	{
    while (i--)
      if (obj=Global(20+i)[0])
        obj->SetObjectStatus(1);
		i = g_iSaveSectIndex;
		while (i--)
		{
      if (obj=Global(20+i)[0])
			{
				SetPosition(Global(20+i)[1], Global(20+i)[2], obj);
				Global(20+i)=0;
			}
		}
		g_iSaveSectIndex=0;
	}

  // Spielerinventar wiederherstellen
  var iCount, obj, container;
  for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
  {
		var iPlr = GetPlayerByIndex(i,C4PT_User);
		ClearLastPlrCom(iPlr);
    iCount = GetCrewCount(iPlr);
    while (iCount--) {
      obj = GetCrew(GetPlayerByIndex(i,C4PT_User), iCount);
      container = FindCrewByName(CXCN, GetName(obj));
      if(container)
			{
				while(Contents(0, container)) Enter(obj, Contents(, container));
				RemoveObject(container);
			}
			obj->DoLaunch(obj);
    }
//    if(fReInitPlayer) GameCall("PlayerStart", GetPlayerByIndex(i,C4PT_User));
  }
  if(fReInitPlayer) StartSzen();
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
