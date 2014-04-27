/*---- Wegfindung ----*/

#strict 2
#appendto CLNK
#appendto BIRD

// Logs an error message
global func ErrorLog(string msg) {
  var out = Format(msg,Par(1),Par(2),Par(3),Par(4),Par(5),Par(6),Par(7),Par(8),Par(9));
  var out = Format("ERROR: %s",out);
  Log(out);
  if(this)
    Log(" in: %s @(%d,%d)",GetName(),GetX(),GetY());
}

// Logs if debug mode is on
global func DebugLog(string msg, name) {
  if(Debug(name))
    Log(msg,Par(2),Par(3),Par(4),Par(5),Par(6),Par(7),Par(8),Par(9));
}

// Displays the message if the debug mode is on
// always per-object
global func DebugMessage(string msg,  name) {
  if(Debug(name))
    Message(msg,this,Par(2),Par(3),Par(4),Par(5),Par(6),Par(7),Par(8),Par(9));
}

// Checks wether the debug mode is on
global func Debug(name) {

  // this namespace on
  if(g_DebugMode == name) return true;

  // generally on
  if(g_DebugMode == "all") return true;

  return false;
}

static g_DebugMode;

global func DebugMode(bool switch, name) {

  if(!name) name = "all";

  // on
  if(switch) {
    g_DebugMode = name;
  }
  // off
  else {
    g_DebugMode = 0;
  }
}

local aMacroCommandList;
// Wegpunktearray
local aPath;
local fThrow;
local pMacroEnter;
local aNextPoint;

public func SetMacroCommand(object pCallback, string szCommand, object pTarget, int iX, int iY, int iDelay, int iAggro)
{
	if(!GetEffect("ExecMacroCommand", this))
		AddEffect("ExecMacroCommand", this, 300, 1, this);
	aMacroCommandList =  [ [szCommand, iX, iY, pTarget] ];
/*	RemoveEffect("ExecMacroCommand", this);
	AddEffect("ExecMacroCommand", this, 300, 1, this);
  // Befehl eintragen
  var iEffect = AddEffect("MacroCommand", this, 300, 0, this);
  aMacroCommandList[0] = iEffect;
  EffectVar(0, this, iEffect) = szCommand;
  EffectVar(1, this, iEffect) = pTarget;
  EffectVar(2, this, iEffect) = iX;
  EffectVar(3, this, iEffect) = iY;
  EffectVar(4, this, iEffect) = iDelay;
  EffectVar(5, this, iEffect) = pCallback;
  EffectVar(6, this, iEffect) = iAggro;
	if(szCommand != "None")
    ScheduleCall(this, Format("MacroCom%s", szCommand), 1);
  else
  {
    ClearMacroCommands();
  }*/
}

func FxExecMacroCommandTimer(pTarget, iNumber, iTime)
{
	var aMacroCommand = aMacroCommandList[0];
	if(aMacroCommand)
	{
		var res = 0;
		if(aMacroCommand[0] == "MacroMoveTo")
			res = MacroComMoveTo(aMacroCommand[1],aMacroCommand[2],aMacroCommand[3]);
		if(aMacroCommand[0] == "MoveToStep")
			res = MacroComMoveToStep(aMacroCommand[1],aMacroCommand[2],aMacroCommand[3]);
		if(aMacroCommand[0] == "MoveTo")
			res = MacroMoveTo(aMacroCommand[1],aMacroCommand[2],aMacroCommand[3]);
		if(res)
			FinishMacroCommand();
	}
}

func MacroMoveTo(iX, iY, pTarget)
{
	if(pTarget) { iX = GetX(pTarget); iY = GetY(pTarget); }
	if(iX < GetX()) SetComDir(COMD_Left);
	else if(iX > GetX()) SetComDir(COMD_Right);
	else
	{
		SetComDir(COMD_Stop);
		return 1;
	}
	if(GetAction() == "Scale")
		if(GetDir() != (GetComDir() == COMD_Right))
			SetComDir(COMD_Down);
}

func AddMacroCommand(szCommand, iX, iY, pTarget)
{Log("AddMacroCommand %v %v %v %v", szCommand, iX, iY, pTarget);
	if(!aMacroCommandList) aMacroCommandList = CreateArray();
	PushFront([szCommand, iX, iY, pTarget], aMacroCommandList);
}

func FinishMacroCommand(iIndex)
{
	DeleteArrayItem(iIndex, aMacroCommandList);
}

// MoveTo
protected func MacroComMoveTo(iX, iY, pTarget)
{
  // Erstes Kommando ist gar nicht MoveTo?
//  if(GetMacroCommand(0,0) != "MoveTo") return;
  // Kein Ziel? (bewirkt, dass der Clonk nicht nach 0,0 laufen kann!)
  if(!iX && !iY)
    if(!pTarget)
      return 0;//MacroComSuccessFailed(0, Macro_MoveToNoTarget);
  // Pfad suchen
  aPath = CreateArray();
  var start, end;
  start = FindWaypoint(GetX(), GetY());
  end = FindWaypoint(iX, iY);
	if(start == end)
	{
		if(pMacroEnter) return SetCommand(this, "Enter", pMacroEnter);
		return SetCommand(this, "MoveTo", 0, iX, iY);
	}
  if(!start || !end)
	{
		//MacroComSuccessFailed(0, Macro_NoPath);
		SetCommand(this, "MoveTo", 0, iX, iY);
		return;
	}
	var breaktime = 0;
	if(aNextPoint && start->GetNextWaypoint(end) == aNextPoint[0])
	{
		start = aNextPoint[0];
		if(aNextPoint[1])
			breaktime = aNextPoint[1];
	}
  aPath = FindPath(start, end, true);
  // Kein Pfad konnte gefunden werden?
  if(!aPath) return 0;//MacroComSuccessFailed(0, Macro_NoPath);

	AddMacroCommand("MoveToStep", 1);
  // In Bewegung setzen (der erste Wegpunkt sollte durch MoveTo erreichbar sein, wenn nicht -> doof
  // Spezialhack: reitet!
//  if(Call("IsRiding")) SetAction("Walk");
/*	if(Abs(GetX()-GetX(aPath[0])) > 5)
	{
		aNextPoint = [aPath[0], 0];
		SetCommand(this, "MoveTo", aPath[0]);
		if(GetHorse()) SetCommand(GetHorse(), "MoveTo", aPath[0]);
	}
  AppendCommand(this, "Call", this, 1,0,0,0, "MacroComMoveToStep");
  // Automatischer Abbruch
  ScheduleCall(this, "MacroComSuccessFailed", 500, 0, 0, Macro_PathImpossible*0-10);
  // Falls Startwegpunkt = Jumppadpunkt
  if(GetLength(aPath) > 1 && aPath[0]->GetPathCount() == 1)
    ScheduleCall(this, "JumppadCheck", 5, 0, aPath[1], 2);
	if(breaktime)
		ScheduleCall(this, "MacroComSuccessFailed", breaktime, 0, 0, Macro_PathImpossible);*/
  //Log("%s #%d: MacroComMoveTo", GetName(), ObjectNumber());
}
local fDoLog;
local fBreak;
global func SetCommand(pTarget) { if(LocalN("fBreak", pTarget)) DoBreak(); return _inherited(pTarget, ...); }

global func DoBreak() { Call("a"); }

protected func MacroComMoveToStep(int iStep) // MoveTo-Schritt
{
  // Erstes Kommando ist gar nicht MoveTo?
//  if(GetMacroCommand(0,0) != "MoveTo") return;
  // Wird haben das Ende des Pfades erreicht?
  if(GetLength(aPath) == iStep)
  {
		//if(fDoLog) Log("%s #%d: MacroComMoveTo finished", GetName(), ObjectNumber());
    // Zum Ziel laufen und beenden
/*		aNextPoint = 0;
    var x = GetMacroCommand(0,2), y = GetMacroCommand(0,3);
		ClearScheduleCall(this, "MacroComMoveTo");
		if(pMacroEnter)
			SetCommand(this, "Enter", pMacroEnter);
    else
			SetCommand(this, "MoveTo", 0, x,y);
    AppendCommand(this, "Call", this, 0,0,0,0, "MacroComSuccess");
		if(GetHorse())
		{
			SetCommand(GetHorse(), "MoveTo", 0, x,y);
			AppendCommand(GetHorse(), "Call", this, 0,0,0,0, "MacroComSuccess");
		}*/
    return 1;
  }
  // Aus irgendeinem Grund ist kein Wegpunkt da?
  if(!aPath[iStep]) return Log("1!") && 0;//MacroComSuccessFailed(0, Macro_PathBroken);
  if(!aPath[iStep-1]) return Log("2!") && 0;//MacroComSuccessFailed(0, Macro_PathBroken);
	// Rausfinden, welchen Weg wir gehen müssen
  for(var i = 0, path = -1 ; i < aPath[iStep-1]->GetPathCount() ; i++)
    if(aPath[iStep-1]->GetPathTarget(i) == aPath[iStep])
    {
      path = i;
      break;
    }
  // Kein Weg? -> Fehler
  if(path < 0) return Log("3!") && 0;//MacroComSuccessFailed(0, Macro_PathBroken);
  // Nächsten Wegpunkt begehen
  FinishMacroCommand();
	AddMacroCommand("MoveToStep", iStep+1);
  MoveAlongPath(aPath[iStep-1], aPath[iStep], iStep+1, path);
//  AppendCommand(this, "Call", this, iStep+1,0,0,0, "MacroComMoveToStep");
  // Automatischer Abbruch
//  ClearScheduleCall(this, "MacroComSuccessFailed");
  // 200 Frames pro 100 Pixel Abstand sind angesetzt
/*  var breaktime = Max(ObjectDistance(aPath[iStep-1], aPath[iStep]) / 100 * 200 * 5, 100);
	var breaktime = aPath[iStep-1]->GetPathTime(path) * 110 / 100; // Normale Zeit plus 10% gratis!
	if(aPath[iStep-1]->GetPathFlag(path) == Path_Lift) breaktime *= 2; // Wenn der Aufzug gerade nicht da ist kann es schon mal doppelt so lange dauern
	if(breaktime)
	{
		// TODO: also for taken up follow command
		ScheduleCall(this, "MacroComSuccessFailed", breaktime, 0, 0, Macro_PathImpossible);
	}
	aNextPoint = [aPath[iStep], breaktime];
	DebugLog(Format("%s #%d: breaktime: %d (%d)", GetName(), ObjectNumber(), breaktime), "AI_ExecMovement");*/
  //Log("%s #%d: MacroComMoveToStep", GetName(), ObjectNumber());
  return 0;
}

// Lässt den Clonk von einem Wegpunkt zum anderen laufen, wobei er die jeweilige Pfadeigenschaft beachten muss
private func MoveAlongPath(object pCurrentWp, object pNextWp, int iNextStep, int path)
{
  // Flag rausfinden und entsprechend agieren
  var flag = pCurrentWp->GetPathFlag(path);
  if(flag == Path_MoveTo)
  {
    // Hinlaufen
    AddMacroCommand("MoveTo", 0, 0, pNextWp);
    return 1;
  }
  if(flag == Path_Jump)
  {
    // Springen und dann laufen
    var dir = pCurrentWp->GetPathDir(path);
    var vx = GetPhysical("Walk")*dir*7/250, vy = -GetPhysical("Jump")/10;
		if(dir < 0) dir = 0;
    SetDir(dir);
/*		SetAction("Walk");
    Jump();
		SetXDir(vx, this, 1000);SetYDir(vy, this, 1000);*/
    SetCommand(this, "MoveTo", pNextWp);
		AddCommand(this, "Jump");
		AddCommand(this, "Call", this, dir, 0, 0, 0, "ForceDir");
		AddCommand(this, "Wait", 0, 0, 0, 0, 0, 1);
    return 0;//AddSpecialCommands(pCurrentWp, path);
  }
/*  if(flag == Path_Backflip)
  {
    // Backflippen und dann laufen
    var dir = pCurrentWp->GetPathDir(path);
    if(dir < 0) dir = 0;
    SetDir(!dir);
    var iEff = AddEffect("ControlStack", this, 110, 5, this);
		if(GetDir() == DIR_Left)
		  EffectVar(0, this, iEff) = COMD_Right;
		else
		  EffectVar(0, this, iEff) = COMD_Left;
    Jump();
    ScheduleCall(0, "JumpStart", 1, 1, true);

    SetCommand(this, "MoveTo", pNextWp);
    return AddSpecialCommands(pCurrentWp, path);
  }
  if(flag == Path_Lift)
  {
    // Wir suchen den Lift und warten oder betreten ihn
    LiftControl(0, ObjectNumber(pCurrentWp), ObjectNumber(pNextWp));
    return AddSpecialCommands(pCurrentWp, path);
  }
  if(flag == Path_Door)
  {
    // Wir suchen die Tür und öffnen sie
    DoorControl(0, ObjectNumber(pCurrentWp), ObjectNumber(pNextWp));
    return AddSpecialCommands(pCurrentWp, path);
  }
  if(flag == Path_Stairs)
	{
		// Wir suchen Treppenaufgänge und gehen rein
    StairsControl(0, ObjectNumber(pCurrentWp), ObjectNumber(pNextWp));
    return AddSpecialCommands(pCurrentWp, path);
	}*/
  // Unbekanntes flag, MoveTo versuchen
  SetCommand(this, "MoveTo", pNextWp);
  return 0;//AddSpecialCommands(pCurrentWp, path);
}

func ForceDir(pDummy, iDir) { SetDir(iDir); }

/* Sonderfunktionen */
/*
protected func JumppadCheck(object pTargetWp, int iNextStep)
{
  // Pfad hat sich irgendwie geändert
  if(GetLength(aPath) <= iNextStep) return;
  var pNextWp = aPath[iNextStep];
  // Feststellen, ob pTargetWp immernoch unser Bewegungsziel ist
  if(aPath[iNextStep-1] != pTargetWp)
    return;
  // Schauen, ob wir näher an pNextWp dran sind
  if(ObjectDistance(pTargetWp) > ObjectDistance(pNextWp))
    // Wegpunkt wird übersprungen
    return MacroComMoveToStep(0, iNextStep);
  // Weiterlaufen
  ScheduleCall(this, "JumppadCheck", 5, 0, pTargetWp, iNextStep);
}

protected func LiftControl(object dummy, int pCurrentWp, int pNextWp)
{//CPCD
  // Fässt den Lift schon an?
  if(GetAction() == "Push")
  {
    var lift = GetActionTarget();
    if(!lift) return;
    // Nah genug am Ziel? Absteigen
    if(Abs(GetY()-GetY(Object(pNextWp))) < 5)
		{//ObjectDistance(Object(pNextWp)) <= 50)
//			SetAction("Walk");
      return AddCommand(this, "MoveTo", Object(pNextWp));
		}
    // Liftplatten befehligen
		if(!lift->GetCommand())
			lift->~SetMoveTo(Object(pNextWp)->GetY());
//    lift->~ControlCommand("MoveTo",0, Object(pNextWp)->GetX());
    // Warten
    AddCommand(this, "Call", this, pCurrentWp, pNextWp, 0,0, "LiftControl");
    AddCommand(this, "Wait", 0,0,0,0,0, 15);
    return 1;
  }
  // Liftplatte suchen
  var y1 = Object(pCurrentWp)->GetY();
  var y2 = Object(pNextWp)->GetY();
  if(y1 > y2) { y1 = y2; y2 = Object(pCurrentWp)->GetY(); }
//  Log("%d %d %d %d", -25, AbsX(y1), 50, y2-y1);
  var lift = FindObject2(Find_Or(Find_Func("IsLift"), Find_ID(ELEC)), Sort_Distance());//Find_AtRect(-50, AbsY(y1)-20-500, 100, y2-y1+20+500));
  var pOtherControler = FindObject2(Find_OCF(OCF_CrewMember), Find_Action("Push"), Find_ActionTarget(lift), Find_Exclude(this));
  // Kein Lift? -> Fehlschlag
  if(!lift) return MacroComSuccessFailed(0, Macro_PathBroken);
  // Lift nah genug? -> Einsteigen
  if(Abs(GetY()-GetY(lift)) < 5)
  {
    AddCommand(this, "Call", this, pCurrentWp, pNextWp, 0,0, "LiftControl");
    AddCommand(this, "Grab", lift);
    return 1;
  }
  else if(!lift->GetCommand() && !pOtherControler)
		lift->~SetMoveTo(Object(pCurrentWp)->GetY());
  // Warten
  AddCommand(this, "Call", this, pCurrentWp, pNextWp, 0,0, "LiftControl");

  AddCommand(this, "Wait", 0,0,0,0,0, 15);
  return 1;
}

protected func DoorControl(object dummy, int pCurrentWp, int pNextWp)
{
  // Liftplatte suchen
  var x1 = Object(pCurrentWp)->GetX();
  var x2 = Object(pNextWp)->GetX();
  if(x1 > x2) { x1 = x2; x2 = Object(pCurrentWp)->GetX(); }
//  Log("%d %d %d %d", x1-GetX(), -25, x2-x1, 50);
  var door = FindObject2(Find_ID(CPDP), Find_AtRect(x1-GetX(), -25, x2-x1, 50));
	if(door)
	{
		if(door->IsOpen()) AddCommand(this, "MoveTo", Object(pNextWp));
		else
		{
			AddCommand(this, "Call", door, 0, 0, 0,0, "DoClose");
			AddCommand(this, "MoveTo", Object(pNextWp));
			AddCommand(this, "Call", door, 0, 0, 0,0, "DoOpen");
		}
	}
	door = FindObject2(Find_Or(Find_ID(CPGL), Find_ID(CPGR)), Find_AtRect(x1-GetX(), -25, x2-x1, 50));
	if(door)
	{
		AddCommand(this, "Call", door, 0, 0, 0,0, "Close");
    AddCommand(this, "MoveTo", Object(pNextWp));
		AddCommand(this, "Call", door, 0, 0, 0,0, "Open");
		return; 
	}
	door = FindObject2(Find_Or(Find_ID(CPBL), Find_ID(CPGR)), Find_AtRect(x1-GetX(), -50, x2-x1, 100));
	if(door)
	{
		AddCommand(this, "Call", door, 0, 0, 0,0, "ControlUp");
		AddCommand(this, "MoveTo", Object(pNextWp));
		AddCommand(this, "Wait", 0,0,0,0,0, 65);
		AddCommand(this, "Call", door, 0, 0, 0,0, "ControlDown");
		return 1;
	}
	door = Object(pCurrentWp)->FindObject2(Find_ID(WGTW), Find_AtRect(-5, -50, 10, 100));
	if(door && GetY(Object(pCurrentWp))>GetY(door) && Abs(GetX(Object(pCurrentWp))-GetX(door)) < 5)
	{
		var szCommand;
		if(GetX(Object(pCurrentWp)) < GetX(Object(pNextWp)))
		{
			szCommand = "ControlRight";
			if(door->RightGateOpen()) szCommand = 0;
		}
		else
		{
			szCommand = "ControlLeft";
			if(door->LeftGateOpen()) szCommand = 0;
		}Log(".%v %v", door, szCommand);
		if(szCommand) AddCommand(this, "Call", door, 0, 0, 0,0, szCommand);
		AddCommand(this, "MoveTo", Object(pNextWp));
		if(szCommand) AddCommand(this, "Call", door, 0, 0, 0,0, szCommand);
		return 1;
	}
	door = Object(pNextWp)->FindObject2(Find_ID(WGTW), Object(pNextWp)->Find_AtRect(-5, -50, 10, 100));
	if(door && GetY(Object(pNextWp))>GetY(door) && Abs(GetX(Object(pNextWp))-GetX(door)) < 5)
	{
		var szCommand;
		if(GetX(Object(pCurrentWp)) > GetX(Object(pNextWp)))
		{
			szCommand = "ControlRight";
			if(door->RightGateOpen()) szCommand = 0;
		}
		else
		{
			szCommand = "ControlLeft";
			if(door->LeftGateOpen()) szCommand = 0;
		}
		if(szCommand) AddCommand(this, "Call", door, 0, 0, 0,0, szCommand);
		AddCommand(this, "MoveTo", Object(pNextWp));
		if(szCommand) AddCommand(this, "Call", door, 0, 0, 0,0, szCommand);
		return 1;
	}
  return 1;
}

protected func StairsControl(object dummy, int pCurrentWp, int pNextWp)
{
  // Treppenhaus suchen
	var direction;
	if(GetY(Object(pNextWp)) < GetY()) direction = "ContainedUp";
	else direction = "ContainedDown";
	var wait = 0;
  var stairs = Object(pCurrentWp)->FindObject2(Find_Func("IsStaircase"), Find_AtRect(-25, -25, 50, 50));
	if(!stairs) { direction = 0; stairs = Object(pCurrentWp)->FindObject2(Find_ID(CSTE),Find_AtRect(-25, -25, 50, 50)); }
	if(!stairs) { wait = 35; direction = 0; stairs = Object(pCurrentWp)->FindObject2(Find_ID(WGTW),Find_AtRect(-25, -25, 50, 50)); }
	if(!stairs) AddCommand(this, "MoveTo", Object(pNextWp));
	else
	{
    AddCommand(this, "MoveTo", Object(pNextWp));
		if(direction) AddCommand(this, "Call", stairs, 0, 0, 0,0, direction);
		if(wait) AddCommand(this, "Wait", 0,0,0,0,0, wait);
		AddCommand(this, "Enter", stairs);
	}
  return 1;
}
*/

public func FindPath(object pStart, object pEnd)
{
  // naive Wegfindung. Wir gehen davon aus, dass wenn der WP links von einem ist,
  // nach links gegangen werden muss und gehen einfach Schritt fuer Schritt in diese
  // Richtung.

  // maximale Iterationen fuer Wegfindung (abhaengig von Anzahl Wegpunkte)
  var iterationLimit = ObjectCount(WAYP);

  var ex = GetX(pEnd);
  var ey = GetY(pEnd);

  var pCurrent = pStart;

  var aWaypoints = CreateArray();

  for(var j=0; j< iterationLimit; ++j) {

    aWaypoints[j] = pCurrent;

    // fertig!
    if(pCurrent == pEnd)
      return aWaypoints;

		var pNext = pCurrent->GetNextWaypoint(pEnd);
		if(!pNext)
		{
      DebugLog("Failure: Selected waypoint leads into a dead end.","waypoints");
      return false;
    }

    pCurrent = pNext;
  }
  DebugLog("Failure: No path found after %d iterations.","waypoints",iterationLimit);
  return false;
}

// nächsten Wegpunkt von einer Position aus suchen (geht nur, wenn PathFree!)
private func FindWaypoint(int iX, int iY)
{
  var wp;
  while(wp = FindObject(WAYP, AbsX(iX), AbsY(iY), -1, -1, 0,0,0,0, wp)) {
    if(PathFree(iX, iY, wp->GetX(), wp->GetY()) || GBackSolid(wp->GetX()-GetX(), wp->GetY()-GetY()))
      return wp;
  }
  // Nix gefunden
  return;
}