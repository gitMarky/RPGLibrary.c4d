/* Waypoint */

#strict 2

/* Global */

static WAYP_visibleWaypoints;

global func CreateWP(int x, int y) {
  var WP = CreateObject(WAYP,AbsX(x),AbsY(y+8),-1);
  return WP;
}

// Flags sind nicht kombinierbar
static const Path_MoveTo = 0; // ;)
static const Path_Jump = 1;
static const Path_Backflip = 2;
static const Path_Lift = 3;
static const Path_Door = 4;
static const Path_Stairs = 5;
static const Path_Fly = 6;
static const Path_Remove = 100;
//... weitere denkbar

static const WAYP_PathErr = "Path %d does not exist.";

// macht alle Wegpunkte sichtbar/unsichtbar
global func WaypointsVisible(bool visu) {
  for(var obj in FindObjects(Find_ID(WAYP)))
    obj->SetVisible(visu);
  WAYP_visibleWaypoints = visu;
	if(visu == 1)
		if(!FindObject2(Find_ID(WPED))) CreateObject(WPED);
	if(visu == 0)
		RemoveAll(WPED);
}

// alle Wegpunkte testen (deren Pfade)
global func WaypointsTest() {
  var time = 0;
  for(var obj in FindObjects(Find_ID(WAYP))) {
    ScheduleCall(obj,"Test",time);
    time += 80;
  }
}

// Schreibt ein Wegpunktscript in den Enginelog
global func LogWaypoints() {
  var aWaypoints = FindObjects(Find_ID(WAYP));
  
  var i = 0;
  for(var pWp in aWaypoints)
    Log("  var wp%d = CreateWP(%d,%d);", ++i, pWp->GetX(), pWp->GetY());
  
  i = 0;
  for(pWp in aWaypoints)
  {
    i++;
    for(var j = 0,eo ; j < pWp->WAYP::GetPathCount() ; j++)
    {
      eo = Waypoints_GetArrayPosition(aWaypoints, pWp->WAYP::GetPathTarget(j));
      if(i < 0)
      {
        Log("// Path error: no target!");
        continue; // Pfad kann man nicht auswerten
      }

      var flag = pWp->WAYP::GetPathFlag(j);
      var strFlag = Format("%d",flag);
      if(flag == 0) strFlag = "Path_MoveTo";
      if(flag == 1) strFlag = "Path_Jump";
      if(flag == 2) strFlag = "Path_Backflip";
      if(flag == 3) strFlag = "Path_Lift";
      if(flag == 4) strFlag = "Path_Door";
      if(flag == 5) strFlag = "Path_Stairs";

      Log("  wp%d -> AddPath(wp%d, %s, %d);", i,eo, strFlag, pWp->WAYP::GetPathDir(j));
      
      var cmd;
      for(var z = 0; cmd = pWp->WAYP::GetArriveCommand(j,z,0,true);z++)
      {
        Log("  wp%d -> SetArriveCommand(%d, %d, \"%s\", Hier Objekt einfügen [%s], %d, %d);",i, j,z,cmd, GetName(pWp->WAYP::GetArriveCommand(j,z,1,true)), pWp->WAYP::GetArriveCommand(j,z,2,true), pWp->WAYP::GetArriveCommand(j,z,3,true));
      }
    }
  }
}

/* Serialisierung */

public func Serialize(array& extra)
{
	var cmd;
	for (var i=0;i<GetLength(aPath);++i) {
		if(GetPathFlag(i) == 0) cmd = "Path_MoveTo";
		else if(GetPathFlag(i) == 1) cmd = "Path_Jump";
		else if(GetPathFlag(i) == 2) cmd = "Path_Backflip";
		else if(GetPathFlag(i) == 3) cmd = "Path_Lift";
		else if(GetPathFlag(i) == 4) cmd = "Path_Door";
		else if(GetPathFlag(i) == 5) cmd = "Path_Stairs";
		extra[GetLength(extra)] = Format("AddPath(Object(%%d),%s,%d)", GetPathTarget(i), cmd, GetPathDir(i));
		for (var j=0;cmd=GetArriveCommand(i,j,0);++j) {
			if (GetArriveCommand(i,j,4)) {
				// TODO: any Data für AddCommand in der KI kann u.U. auch Objektzeiger beinhalten.. auch serialisieren. :D
				extra[GetLength(extra)]  = Format("SetArriveCommand(%d,%d,\"%s\",Object(%%d),%d,%d,Object(%%d),%v)", i, j, cmd, GetArriveCommand(i,j,1), GetArriveCommand(i,j,2), GetArriveCommand(i,j,3), GetArriveCommand(i,j,4), GetArriveCommand(i,j,5));
			} else {
				extra[GetLength(extra)]  = Format("SetArriveCommand(%d,%d,\"%s\",Object(%%d),%d,%d,0,%v)", i, j, cmd, GetArriveCommand(i,j,1), GetArriveCommand(i,j,2), GetArriveCommand(i,j,3), GetArriveCommand(i,j,5));
			}
		}
	}
}

global func Waypoints_GetArrayPosition(array aWps, object pFind)
{
  var i=0, gotit = false;
  for(var pWp in aWps)
  {
    if(pWp == pFind)
    {
      gotit = true;
      break;
    }
    i++;
  }
  if(!gotit) i=-1;
  else i += 1;
  return i;
}

/* Effekt */
// Der Effekt speichert Pfaddaten (Kanteneigenschaften)

// EffectVars
// 0: nächster Knoten
// 1: Flags
// 2: X-Richtung des Pfades
// 3: Jetpack-Zündungs-Delay
// 4: Jetpack-Zündungsrichtung
// ab 5 bis ...: Kommandos

/* Lokal */

local aPath;

local aConnectionList;

local fUseCommand;

public func IsUsingCommands() { return fUseCommand; }

public func GetNextWaypoint(pEnd)
{
	for(var item in aConnectionList)
		if(item[0] == pEnd)
			return item[1];
	return false;
}

public func GetLengthToTarget(pEnd)
{
	for(var item in aConnectionList)
		if(item[0] == pEnd)
			return item[2];
	return false;
}

global func WaypointsMakeList()
{
  var iterationLimit = ObjectCount(WAYP);
	for(var obj in FindObjects(Find_ID(WAYP)))
	{
		obj->ResetList();
		obj->CheckPaths();
		obj->AddNeighboursToList();
	}
	for(var i = 0; i < iterationLimit; i++)
		for(var obj in FindObjects(Find_ID(WAYP)))
			obj->AddNeighboursList();
}

public func ResetList() { aConnectionList = []; }
public func CheckPaths()
{
	for(var i = 0; i < GetLength(aPath); i++)
	{
		var obj = GetPathTarget(i);
		if(obj) continue;
		RemovePath(i);
		return 0;
	}
}

public func AddNeighboursToList()
{
	var obj;
	for(var i = 0; i < GetLength(aPath); i++)
	{
		obj = EffectVar(0,this(),aPath[i]);
		var distance = GetPathTime(i);
		if(!distance) distance = ObjectDistance(obj);
		aConnectionList[i] = [obj, obj, distance];
	}
}

public func AddNeighboursList()
{
	var newList, obj, dist, pEnd;
	for(var i = 0; i < GetLength(aPath); i++)
	{
		obj = EffectVar(0,this(),aPath[i]);
		dist = ObjectDistance(obj);
		newList = obj->GetList();
		for(var j = 0; j < GetLength(newList); j++)
		{
			pEnd = newList[j][0];
			if(pEnd == this)
				continue;
			for(var k = 0; k < GetLength(aConnectionList); k++)
				if(aConnectionList[k][0] == pEnd)
					break;
			// Haben wir schon und ist so näher
			if(k < GetLength(aConnectionList) && aConnectionList[k][2] < newList[j][2]+dist)
				continue;
			aConnectionList[k] = [pEnd, obj, newList[j][2]+dist];
		}
	}
}

public func GetList() { return aConnectionList; }

// Setters

// iNo ist die entsprechende Nummer (wer hätte das gedacht?) - beginnend bei 0! :o
public func SetArriveCommand(int pId, int iNo, string command, object pTarget, int iX, int iY, object pTarget2, Data) {
  EffectVar(iNo + 5, this(), aPath[pId]) = [command,pTarget,iX,iY,pTarget2,Data];
}

public func AddPathX(target, int flag, int dir, bool fReconnect)
{
	if(Object(target) == this) return Log("ERROR: No self reference allowed!");
	return AddPath(Object(target), flag, dir, fReconnect);
}

// wenn fReconnect true ist, wird die Strecke zurückverbunden
public func AddPath(object target, int flag, int dir, bool fReconnect) {
  // ein Pfad ohne Target ist nix
  if(!target) return false;
	for(var i = 0; i < GetPathCount(); i++)
	{
		if(GetPathTarget(i) == target)
		{
			if(Path_Remove)
				RemovePath(i);
			else
				SetPathFlag(i, flag);
			if(fReconnect)
				target->AddPath(this(), flag, dir*-1, 0, 0);
			return i;
		}
	}
	if (flag == Path_Remove) return;
  // Pfad eintragen
  aPath[GetLength(aPath)] = AddEffect("PathData", this(), 1, 0, this());

  var pId = GetLength(aPath)-1;

  SetPathTarget(pId, target);
  SetPathDir(pId, dir);
  SetPathFlag(pId,flag);

  if(WAYP_visibleWaypoints)
     CreateObject(VIPU,0,0,-1)->Set(this(),target,flag,0,pId);

  if(fReconnect)
    target->AddPath(this(), flag, dir*-1, 0, 0);

  return pId;
}

public func RemovePath(int pId) {
  RemoveEffect(0,this(),aPath[pId]);
  var aNewArray = CreateArray(), i;
  for(var path in aPath)
  {
    if(i != pId) aNewArray[GetLength(aNewArray)] = path;
    i++;
  }
  aPath = aNewArray;
}

public func SetPathTarget(int pId, object target) {
  if(GetPathCount() <= pId) return ErrorLog(WAYP_PathErr, pId);

  // Vorsicht: wenn auf 0 gesetzt wird, zeigt der Pfad ins nix
  EffectVar(0,this(),aPath[pId]) = target;
}

public func SetPathDir(int pId, int dir) {
  if(GetPathCount() <= pId) return ErrorLog(WAYP_PathErr, pId);

  // dir nicht angegeben: automatisch aus Position von target ermitteln
  if(!dir) {
    var target = GetPathTarget(pId);
    if(!target) return;
    var diff = GetX(target)-GetX();
    dir = diff/Abs(diff);
  }
  EffectVar(2,this(),aPath[pId]) = dir;
}

public func SetPathFlag(int pId, int flag) {
  if(GetPathCount() <= pId) return ErrorLog(WAYP_PathErr, pId);

  EffectVar(1,this(),aPath[pId]) = flag;
}

public func SetPathTime(int pId, int time) {
  if(GetPathCount() <= pId) return ErrorLog(WAYP_PathErr, pId);

  EffectVar(3,this(),aPath[pId]) = time;
}

// Getters

// iElement
// 0: szCommand
// 1: pTarget
// 2: iX
// 3: iY
// 4: pTarget2
// 5: Data
public func GetArriveCommand(int pId, int iNo, int iElement, bool NoLog) {
  if(GetPathCount() <= pId)
  { 
    if(NoLog)
      return;
    return ErrorLog(WAYP_PathErr, pId);
  }
  if(iElement < 0 || iElement > 5) return;
  if(!EffectVar(iNo + 5, this(), aPath[pId])) return;
  return EffectVar(iNo + 5,this(),aPath[pId])[iElement];
}

public func GetArriveCommandCount(int pId) {
  var i = 0;
  while(EffectVar(i + 5,this,aPath[pId]))
    i++;
  return i;
}

public func GetPathToTarget(object pTarget)
{
	for(var i = 0; i < GetPathCount(); i++)
	{
		if(GetPathTarget(i) == pTarget)
			return i;
	}
	return -1;
}

public func GetPathCount() {
  return GetLength(aPath);
}

public func GetPathTarget(int pId) {
  if(GetPathCount() <= pId) return ErrorLog(WAYP_PathErr, pId);
  return EffectVar(0,this(),aPath[pId]);
}

public func GetPathDir(int pId) {
  if(GetPathCount() <= pId) return ErrorLog(WAYP_PathErr, pId);
  return EffectVar(2,this(),aPath[pId]);
}

public func GetPathFlag(int pId) {
  if(GetPathCount() <= pId) return ErrorLog(WAYP_PathErr, pId);
  return EffectVar(1,this(),aPath[pId]);
}

public func GetPathTime(int pId) {
  if(GetPathCount() <= pId) return ErrorLog(WAYP_PathErr, pId);
  return EffectVar(3,this(),aPath[pId]);
}

// Visualization

public func SetVisible(bool vis) {
  if(!vis) {
    SetAction("Idle");
    Message("", this());
    // alle Pfad-Anzeige-Objekte löschen
    RemovePathDisplays();
		SetCategory(C4D_StaticBack);
  }
  else {
    SetAction("Vis");
		SetCategory(4194312);
    Message("@%d", this(), ObjectNumber());
    // alle Pfad-Anzeige-Objekte erstellen
    // (von Pfaden die vom Knoten wegführen)
    CreatePathDisplays();
  }
}

local iTime;

public func Test() {
  // alle Pfade
  for(var i; i<GetLength(aPath); ++i) {
	SetPathTime(i, 0);
	var hzck;
	if(GetPathFlag() == Path_Fly)
		hzck = CreateObject(BIRD,0,10,-1);
	else hzck = CreateObject(_CLK,0,10,-1);
	hzck->SetLocal(9,GetPathTarget(i));
	hzck->SetLocal(8,i);
	//hzck->SetMacroCommand(this(),"Wait",0,0,0,40);
	hzck->SetMacroCommand("MoveTo",GetPathTarget(i));
	hzck->LocalN("fMacroTest") = 1;
	hzck->AppendMacroCommand("Call", this, "OnMacroCommandSuccess", hzck);
  }
  iTime = 0;
  ScheduleCall(this, "Timer", 1, 1);
}

public func Timer()
{
	if(iTime < 35*60)
	{
		iTime++;
		ScheduleCall(this, "Timer", 1, 1);
	}
}

public func OnMacroCommandFailed(clonk, iReason) {
  Log("Failed for <c ff0000>%d</c>(%d,%d)-><c ff0000>%d</c>(%d,%d). Reason: %d",ObjectNumber(),GetX(),GetY(),ObjectNumber(Local(9,clonk)),GetX(Local(9,clonk)),GetY(Local(9,clonk)),iReason);
}

public func OnMacroCommandSuccess(clonk) {
  if(clonk) 
  {
		SetPathTime(clonk->Local(8), iTime);
    RemoveObject(clonk);
  }
}

/* Intern */

protected func Initialize()
{
  aPath = CreateArray();
  SetObjectBlitMode(GFX_BLIT_Additive);
  if(WAYP_visibleWaypoints) { Message("@%d", this(), ObjectNumber()); SetAction("Vis");SetCategory(4194312|C4D_Foreground); }
}

func MouseSelection()
{
	FindObject(WPED)->Selection(this);
}

private func CreatePathDisplays() {
  // alle Pfade
  for(var i; i<GetLength(aPath); ++i) {
		if(!GetPathTarget(i)) continue;
     var visu = CreateObject(VIPU,0,0,-1);
     visu->Set(this(),GetPathTarget(i),GetPathFlag(i),0,i);
  }
}

private func RemovePathDisplays() {
  for(var obj in FindObjects(Find_ID(VIPU),Find_ActionTarget(this())))
    if(obj)
      RemoveObject(obj);
}


// visu Set(anfangsknoten, endknoten, flag, jetpack)

