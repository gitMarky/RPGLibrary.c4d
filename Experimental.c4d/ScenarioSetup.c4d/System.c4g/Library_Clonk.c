/* Respawning */

#strict 2

#appendto CLNK


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