/*-- Gegner KI --*/

// 2do: MoveTo-ins-Wasser-Check: Stacked MoveTo-commands?
// Unterstützte Zauber - in ai_aSpells festlegen, damit sie gezaubert werden:
// Heimkehr: EH69 (Homewarp)
// Ausweichen: CMFG, MCIB (Clonk in Flasche), MFFW, ABLA, GVTY (Schweben), MDFL (Zurückwerfen), MSSH (Steinschild), MBRG, MPXC (Positionstausch, nur wenn NTMG)
// Werfen: FGOB (Schleudern), MINC (Anzünden)
// Löschen: EXTG
// Wärmen: MWTH
// Panic: CMFG
// Heilung: ELX1, ELX2, MGHL
// Bewegung: WOLI
// Angriff: MCLT (M&M-Blitz), MLGT, ETFL, FRFS, ABLA, MBOT, MFRB, MDBT (Drachenatem), MFBL, MFWL, GZ9Z, LGCN, RUND, MBLS, CAHE, CPAN, CFAL, CCNF, MFSK
//          MGBW, MICS, MMED
//          MTSK (Teraschock), AFST (Luftklaue), PRFT (Druckfaust), AUEQ (Erdbebenaura), MARK (Steinschlag), SHFT (Schattenfaust), AUHE (Hitzeaura)
//          MFCS (Feuercluster), FLFT (Flammenfaust), MFWV (Frostwelle), ICFT (Eisfaust), MGSW (Schneeball), MFBZ (Frosthauch)
// Schwimmend: MCLD (Flüssigkeiten erstarren)
// Endboss: MGRP
// Ally-Hilfe: ELX2, ELX1, MCIB (Clonk in Flasche), MHGA (M&M-Gruppenheilung), GGHG
// Ally-Nahkampfunterstützung: GZ9Z, MDFL (Zurückwerfen), MSSH (Steinschild), MHTB (Heißes Blut), FLFT (Flammenfaust)
//
// Unterstützte Wunder - in ai_aSpells festlegen, damit sie gewirkt werden:
// MPRT (Schutz), MGPH (Energieangriff), MLDL (Lebensentzug), MRVL (Wiederbelebung), MSMN (Skelett), MSPD (Geschwindigkeit), MHEL (Heilung)
//
// Cheats:
// -Zauberenergie-Regeneration
// -Pfeilnachlieferung für Ritter
// -Komboobjekte werden automatisch im Inventar erzeugt
// -Homewarp-Zauber nur für KI-Magier
//
// Schwierigkeitsgrad-Auwirkungen:
// -Physicals
// -Zauberenergie-Regeneration

#strict 2

#appendto CLNK // Alle Clonks
//#appendto WL69 // Wolf

static const EA69_EnemySearchRange       = 400, // * Pixel nach d2
              EA69_RuleID                 = EA69;
              
static const EA69_RespectStaff          = true; // Wenn wahr, werden Stabzauber nur gezaubert, wenn der entsprechende Stab vorhanden ist und ein Stab ermöglicht alle diese Zauber
static const EA69_CheatMagicEnergy      = true; // Wenn wahr, regenerieren KI-Zauberclonks automatisch Zauberenergie
static const EA69_CheatCombo            = true; // Wenn wahr, werden Komboobjekte für Zauber erzeugt (KI-Cheat)
static const EA69_CheatAmmo             = true; // Wenn wahr, werden PFeile und Munitionspakete erzeugt (KI-Cheat)
static const EA69_RemoveItemsOnDeath	= true; // Wenn wahr, werden Ausrüstungsgegenstände beim Tod entfernt

static g_EA69_iDifficulty; // Gegner-KI-Schwierigkeit:

static g_EA69_aNoThrow; // Array von Gegenständen, die nicht geworfen werden sollten
static g_EA69_aRemoveOnDeath; // Array von Gegenständen, die beim Tod entfernt werden

static g_EA69_fAssassinModeOff;

static const EA69_Diff_Easy   = 0,
              EA69_Diff_Medium = 1,
              EA69_Diff_Hard   = 2;

local ai_iNoSpecial; // Sucht nicht nach Münzen, Leichen
							
local ai_iRange;                         // Suchweite für Gegner
local ai_iHomeX, ai_iHomeY, ai_iHomeDir, ai_pHome; // Eingenommene Position, wenn kein Gegner gefunden - ai_iHomeX==0 für einfach stehen bleiben.
local ai_pTarget;                        // Verfolgter Gegner
local ai_pActionTarget;			 // Aktonsziel
local ai_iActionIndex;
local ai_iActionType;
local ai_idFirstEncounterCB;             // Callback im Szenarienscript beim ersten Feindkontakt (AI_Encounter%i) - Par0 = this
local ai_fCallHome;
local ai_isEndboss;                      // Endgegner-Magier: Hält etwas länger
local ai_iStrength;                      // Gegnerstärke (Energie, Mana, etc. - 100=Normal)
local ai_pSpellTarget;                   // Ziel für Zauber (für Hilfszauber)
local ai_iRefOffCount, ai_iRefOffRange;  // Referenzzähler fürs Ausschalten
local ai_iMagicRegen;                    // Gesetzt für Magiclonks: Automatische Zauberenergie-Regeneration
local ai_aSpells;                        // Feld erlaubter Zauber/Wunder. 0 für alle Zauber/Wunder
local ai_iCastChance;                    // Chance, jeweils einen bestimmten Zauber auszuführen
local ai_pBoss;                          // ==this für Clonks, die andere KI-Clonks anschalten (Nur bei EncounterID!). Ansonsten Zeiger auf den Clonk, der sie aktiviert.
local ai_pWeapon;                        // Benutzes Kataplut, Armbrust oder Kanone

local ai_aPatrolPoints;			 // Punkte zum Patrollieren
local ai_iPatrolIndex;			 // Index für das Patrollieren

local ai_iDisableTime;                   // Verhindert KI-Aufrufe für die vorgegebene Zeit
local ai_dwFlags;
local ai_fFindAssassins;		 // Findet auch getarnte Meuchelmörder
local ai_fRunner;	// Rennt einfach durch und versucht so wenig wie möglich sich mit Kämpfen auf zu halten
static const EA69_ForceMelee  = 0,          // KI geht auch als Fernkämpfer in den Nahkampf
              EA69_ForceRanged = 1,          // KI bewegt sich nicht
              EA69_NoItemGet   = 2;          // Wenn gesetzt, versucht die KI nicht, sich Waffen oder Heilungsgegenstände zu holen

local ai_iLastX;			 // Speicherung der Koordinate, zum verhindern von nicht auszuführenden Kommandos
local ai_iLastY;

local ai_iAlarmedCounter;
local ai_iHeardAlarm;

public func OnLayerChanged()
{
	var iXOffset = 0, iYOffset = 0, pLayer;
	if(pLayer = GetObjectLayer())
	{
		iXOffset = pLayer->~GetLayerOffset(0);
		iYOffset = pLayer->~GetLayerOffset(1);
	}
	ai_iHomeX -= iXOffset; ai_iHomeY -= iYOffset;
	if(ai_aPatrolPoints)
	for(var i = 0; i < GetLength(ai_aPatrolPoints); i++)
	{
		if(GetType(ai_aPatrolPoints[i]) != C4V_C4Object)
		{
			ai_aPatrolPoints[i][0] -= iXOffset;
			ai_aPatrolPoints[i][1] -= iYOffset;
		}
	}
	return _inherited();
}

/* Initialisierung */

public func SetAI(object pATarget, int iARange)
  {
  var iScriptPlr, dwOldClr;
  if (pATarget) return pATarget->SetAI(0, iARange);
  if (GetCategory() & C4D_StaticBack) return; // Keine Speedclonks
  if (!GetAlive()) return; // Keine totgestellten Clonks
  if (ai_iRange) return; // Hat schon?
  if (!iARange) iARange = EA69_EnemySearchRange;
  if (!GetAlive()) return;
  // Gehört der Clonk keinem Spieler? Dann besser an einen Scriptspieler übergeben
  if (GetOwner() == NO_OWNER)
    {
    iScriptPlr = GetPlayerByIndex(0, C4PT_Script);
    if (iScriptPlr == NO_OWNER)
      {
 //     return FatalError(Format("Error setting AI for object %v: No script player defined!", this));
      }
    dwOldClr = GetColorDw();
    SetOwner(iScriptPlr);
    MakeCrewMember(this, iScriptPlr);
    if (dwOldClr) SetColorDw(dwOldClr);
    }
  // Werte setzen
  ai_iRange = iARange;
  ai_pHome = Contained();
  ai_iHomeX = GetX(); ai_iHomeY = GetY(); ai_iHomeDir = GetDir();
  if (IsMagiclonk()) { ai_iMagicRegen = 10; ai_iCastChance = 100; }
  else if (IsPaladin()) { ai_iMagicRegen = 1; ai_iCastChance = 200; }
  ai_aSpells = 0; // Alle Zauber erlaubt
  // Timer erstellen
  if (GetEffect("AI", this)) return;
  return AddEffect("AI", this, 1, 3, this);
  }
  
public func AI_AddSpell(id idSpell)
  {
  // Zauber zur Liste erlaubter Zauber hinzufügen, wenn noch nicht drin
  if (!idSpell) return;
  if (!GetName(0, idSpell)) { Log("Spell doesn't exist!"); return; }
  if (GetIndexOf(idSpell, ai_aSpells) >= 0) return;
  if (!ai_aSpells) ai_aSpells = CreateArray();
  ai_aSpells[GetLength(ai_aSpells)] = idSpell;
  return true;
  }

public func AI_AddPatrolPoint(iX, iY)
{
  if(!ai_aPatrolPoints) ai_aPatrolPoints = [];
  if(!iX && !iY)
	{
		iX = GetX();
		iY = GetY();
	}
  ai_aPatrolPoints[GetLength(ai_aPatrolPoints)] = [iX, iY];
}

public func AI_AddPatrolObject(pObj)
{
  if(!ai_aPatrolPoints) ai_aPatrolPoints = [];
  ai_aPatrolPoints[GetLength(ai_aPatrolPoints)] = pObj;
}

public func AI_ResetPatrol()
{
  ai_aPatrolPoints = [];
}

public func AI_ResetHome()
  {
  if (!ai_iRange) return;
  ai_pHome = Contained();
  ai_iHomeX = GetX();
  ai_iHomeY = GetY();
  ai_iHomeDir = GetDir();
  return true;
  }
  
global func AI_InitSystem()
  {
  // Keine nützlichen Gegenstände wegwerfen
  g_EA69_aNoThrow = [SWOR, AXE1, BOW1, TSWD, BRED, COKI, PHEA, PMON, PWIP, PFIS, PFIR, CSNK, CBRD, MUSK, SM8W, SMAR, SMER, SMFR, SMWT, SBLP, PBLP, MUSK, BLTP, WBRL];
  // Bestimmte Gegenstände beim Tod entfernen
  g_EA69_aRemoveOnDeath = [SPER, ROCK, FLNT, SFLN, EFLN, STFN, FBMP, BOW1, ARWP, ARRW, FARP, FARW, XARP, XARW, AXE1, SWOR, MUSK, TSWD, BLTP, PBLP, SBLP, DNGR, KNFE, SABR, CRBW];
  return true;
  }

// Temporäres Deaktivieren der KI  
public func AI_Disable() { ai_iRange = 0; }
public func AI_Enable(int iARange) { if (!(ai_iRange = iARange)) ai_iRange = EA69_EnemySearchRange; }

// Referenzgezähltes, temporäres Deaktivieren
public func AI_RefDisable() { if (!ai_iRefOffCount++) { ai_iRefOffRange = ai_iRange; ai_iRange=0; } }
public func AI_RefEnable() { if (!(ai_iRefOffCount=Max(ai_iRefOffCount-1))) if (ai_iRefOffRange) ai_iRange = ai_iRefOffRange; }

// KI ausschalten
global func DisableAI()
{
  // Alle Physicals aktualisieren
  for (var obj in FindObjects(Find_OCF(OCF_Alive), Find_Func("IsAI")))
    obj->AI_Disable();
}

global func EnableAI()
{
  // Alle Physicals aktualisieren
  for (var obj in FindObjects(Find_OCF(OCF_Alive), Find_Func("IsDisabledAI")))
    obj->AI_Enable();
}

public func AI_SetFirstEncounterCB(id idToCB)
  {
  // CB setzen
  ai_idFirstEncounterCB = idToCB;
  return true;
  }

protected func Recruitment(int iPlr)
  {
  // KI aktiviert: Alle Clonks im Besitz des Scriptspielers steuern
  if (ObjectCount(EA69_RuleID) && GetPlayerType(iPlr) == C4PT_Script && !ai_iRange)
    ScheduleCall(this, "SetAI", 50, 1);
  return _inherited(iPlr,...);
  }



/* Timer */

protected func FxAIStart(object pTarget, int iEffectNumber, int iTemp)
  {
  // Anfangsausrüstung
  if (!iTemp)
  {
    if (!Contents())
      if (!GetPlayerCount(C4PT_User)) // Nur während der Entwicklungszeit
        AI_Equip();
    if(g_EA69_iDifficulty)
      AI_SetPhysicals();
  }
  }

func GetCommandTargetX()
{
	var target = GetCommand(this, 1);
	if(target) return target->GetX();
	return GetCommand(this, 2);
}

local ai_fogetTimer;

protected func FxAITimer()
  {
  var obj,dx,dy,vx,vy,dir,fMagi,i,fInLava,pClonk;
  
  // Nicht wenn deaktiviert
  if (!ai_iRange) return;

  // Während bestimmter Aktionen absolut gar nichts tun, damit sie nicht abgebrochen werden!
  if (ai_iDisableTime)
    {
    --ai_iDisableTime;
    //Log("%v", ai_iDisableTime);
    return;
    }

  // X-Koordinate speichern
  if(GetCommand())
  {
    if(GetX()==ai_iLastX && GetY()==ai_iLastY && GetCommand() != "Wait" && !Random(10))
		{DebugLog(Format("%s #%d: ResetCommand", GetName(), ObjectNumber()), "AI_ExecMovement");
      SetCommand(this, "None");
		}
    else 
    {
      ai_iLastX = GetX();
      ai_iLastY = GetY();
    }
  }

  // Gegner bestimmen:
  // Gegner aktualisieren
  var pNewTarget;
  if (!Random(4))
    pNewTarget = AI_FindNearestEnemy();
  else if (Contained(ai_pTarget))
    // Tiere folgen nicht in Gebäude
    if (IsAnimal()) ai_pTarget = 0;
  // Nur wenn ein neuer Gegner da ist!
  if(pNewTarget)
	{
		if(!g_EA69_fAssassinModeOff)
		{
			if(ai_iAlarmedCounter)
			{
				MessageEx(GetStringSeenMurder(), this);
				ai_iAlarmedCounter = 0;
			}
			if(ai_iHeardAlarm)
			{
				ai_iHeardAlarm = 0;
			}
			else if(ai_pTarget != pNewTarget)
			{
				AI_AlarmEnemy(pNewTarget, 150);
				MessageEx(GetStringAttacker(), this);
			}
			pNewTarget->~DoShow(250);
		}
		ai_pTarget = pNewTarget;
		ai_fogetTimer = 0;
	}
	else
	{
		ai_fogetTimer++;
		if(ai_fogetTimer > 35)
			ai_fogetTimer = 0;
	}
  if(!GetAlive(ai_pTarget) || ObjectDistance(ai_pTarget) > ai_iRange) ai_pTarget = 0;

  if(ai_fRunner && ai_pTarget && 0)
	{
		if( (GetCommandTargetX() < GetX()) && (GetX(ai_pTarget) > GetX()) ) ai_pTarget = 0;
		if( (GetCommandTargetX() > GetX()) && (GetX(ai_pTarget) < GetX()) ) ai_pTarget = 0;
	}

  // Cheatzeug:
  // Magiclonks kriegen ständig Mana
  if (!Random(10)) if (EA69_CheatMagicEnergy) DoMagicEnergy(ai_iMagicRegen * (1+g_EA69_iDifficulty));
  // Ritter mit Bogen kriegen Pfeile
  if (!Random(700)) if (EA69_CheatAmmo)
    {
    if (IsKnight() && FindContents(BOW1))
      if (!this->GetArrowCount())
        CreateContents([XARP, FARP][Random(3)]);
    if (IsPaladin() && FindContents(MUSK))
      if (!this->GetBulletCount())
        CreateContents([BLTP, PBLP, SBLP][Random(3)]);
    }

  // Projektile im Umkreis suchen
  var aProjectiles = FindObjects(Find_InRect(-50,-50,100,80), Find_Or(Find_Category(C4D_Object), Find_Func("IsDangerous4AI"), Find_And(Find_Func("IsArrow"), Find_Hostile(GetOwner()))), Find_OCF(OCF_HitSpeed2), Find_Layer(GetObjectLayer()), Find_NoContainer());

  // Contained: Erstmal raus, wenn nicht drinnen am Kämpfen
  var pContainer;
  if ( (pContainer = Contained()) && !GetCommand() && !GetMacroCommand())
    {
    if (pContainer != ai_pHome || ai_pTarget)
      {
      // Aus Stein und Flasche auslösen
      if (!Random(50))
        {
        if (GetID(pContainer) == MRCK)  if (!Contained(pContainer)) return pContainer->ContainedDigDouble(this);
        if (GetID(Contained()) == CITB)
          {
          ProtectedCall(pContainer, "Break"); // Falls sie den Funktionsnamen mal korrigieren
          return ProtectedCall(pContainer, "Brake");
          }
        }
      // Mit Gegner im Haus: OK für Clonks; nicht für Tiere!
      if (!IsAnimal()) if (ai_pTarget) if (pContainer == Contained(ai_pTarget)) return true;
      // Nicht, wenn noch Projektile draußen rumfliegen
      if (GetLength(aProjectiles)) return true;
      // Sonst raus
      return AI_SetCommand("Exit");
      }
    }

  // Sofortreaktionen: Tlw. adaptiert von Newtons TheTemple-KI
  // Projektilen ausweichen
  else for (obj in aProjectiles)
    {
    // Luftklaue, aber geschützt?
    if (GetID(obj) == AFTF) if (GetEffect("NoABLA" , this)) continue;
    // Berechnen, ob das Projektil treffen würde
    dx = GetX(obj)-GetX(); dy = GetY(obj)-GetY();
    vx = GetXDir(obj); vy = GetYDir(obj); dir = vx/Abs(vx);
    if (!dir) dir = Random(2)*2-1;
    if(ai_fRunner)
		{
			if(GetCommandTargetX() < GetX()) dir = -1;
			else dir = 1;
		}
    // VecAbstand * VecGeschwindigkeit < -Cos(Toleranz)*|VecAbstand|*|VecGeschwindigkeit|)
    if (dx*vx+dy*vy < 70*Sqrt(dx*dx+dy*dy)*Sqrt(vx*vx+vy*vy)/-100)
      {
      // Feuerbälle löschen
      if (GetID(obj) == FIRB) if (AI_DoMagic(EXTG)) return true;
      // Vor einem Eingang? Prima! Rein da!
      if (pContainer = FindObject2(Find_AtPoint(), Find_OCF(OCF_Entrance), Find_Layer(GetObjectLayer()), Find_Not(Find_Func("IsHorse")), Find_Not(Find_ID(CAVE))))
        return AI_SetCommand("Enter", pContainer);
      // Sonst ausweichen
      if (!AI_Evade(dir, obj))
        if (ai_fRunner || !AI_Evade(-dir, obj))
          // Nowhere to go :o
          break;//return AI_Panic();
      return true;
      }
    }

  // Brennend: Löschaktionen
  var dist;
  if (OnFire())
    {
    // Löschzauber? Nicht in Lava schwimmend!
    fInLava = GetMaterialVal("Incindiary", "Material", GetMaterial());
    if (!fInLava) if (Random(4)) if (AI_DoMagic(EXTG)) return true;
    // Wasserfass dabei?
    if (!fInLava) if (obj = FindContents(WBRL))
      return ProtectedCall(obj, "Activate", this);
    // Feuerpotion?
    if (obj = FindContents(PFIR))
      return ProtectedCall(obj, "Activate", this);
    // Schon auf dem Weg ins Wasser?
    i = AI_GetStackedCommand("MoveTo");
    if (i >= 0)
      if(GetMaterialVal("Extinguisher", "Material", GetMaterial(GetCommand(0, 2, i)-GetX(),GetCommand(0, 3, i)-GetY())))
        return true; // Weiter so
    // Irgendwer muss doch helfen können?
    for (pClonk in FindObjects(Find_InRect(-150,-150,300,300), Find_OCF(OCF_Living), Find_NoContainer(), Find_Layer(GetObjectLayer()), Find_Exclude(this)))
      if (pClonk->~IsAI())
        if (pClonk->~IsMagiclonk())
          if (pClonk->AI_HelpAlly(this))
            // Puh, danke!
            return AI_Thank(pClonk);
    // Wasser in der Nähe?
    i = 20;
    while (i--)
      {
      dx = Random(401) - 200;
      dy = Random(401) - 200;
      if(GetMaterialVal("Extinguisher", "Material", GetMaterial(dx,dy)))
        if (dist = GetPathLength(GetX(), GetY(), GetX()+dx, GetY()+dy))
          if (dist < 800)
            return AI_SetCommand("MoveTo", 0, GetX()+dx, GetY()+dy);
      }
    // Keine Löschmöglichkeit: Zum nächsten Gegner laufen
    obj = AI_FindNearestEnemy(); if (!obj) obj = ai_pTarget;
    if (obj) return AI_SetCommand("Follow", obj);
    // Aaaah!
    return AI_Panic();
    }

  // Kein Gegner: Homeposition
  if (!ai_pTarget) return AI_Home();

//DebugLog(Format("%s #%d: AI_ExecMovement will follow", GetName(), ObjectNumber()), "AI_ExecMovement");
  // Normale Bewegungsaktionen
  if (AI_ExecMovement()) return true;

  // Kanonier, Katapultist oder Ballistist
  if(ai_pWeapon) if(AI_ExecWeapon()) return true;

  // Mystiker: Zaubern
  if(IsMystic()) if(AI_ExecMystic()) return true;

  // Magier: Zaubern
  if (AI_CanCast()) if (AI_ExecMagician()) return true;

  // Werfen wenn nötig
  if (Contents()) if (AI_ExecThrow()) return true;

  // Paladin: Paladinwaffen und Wunder benutzen
  if (IsPaladin()) if (AI_ExecPaladin()) return true;

  // Meuchelmörder: meucheln!
  if (IsAssassin()) if(AI_ExecAssassin()) return true;

  // Ritter: Waffen benutzen
  if (IsKnight() || IsKanderian()) if (AI_ExecKnight()) return true;

  // Pirat: Waffen benutzen
  if (IsPirate()) if (AI_ExecPirate()) return true;

  // Nahkampfangriff
  if (!AI_IsRanged() || (ai_dwFlags & EA69_ForceMelee) || 1)
    {
    // Zum Gegner laufen
    if (!GetCommand() && !GetMacroCommand())// || (GetCommand() == "MoveTo" && !Random(8)))
		{
			DebugLog(Format("%s #%d: Attack", GetName(), ObjectNumber()), "AI_ExecMovement");
      return AI_SetCommand("Attack", ai_pTarget);
		}
    }
  else if (!GetCommand() && !GetMacroCommand())
    {
    // Fernkämpfer aus blöden Situationen
    if (GetProcedure() == "SWIM")
      return AI_Home();
    if (GetProcedure() == "SCALE")
      if (GetDir()) return(SetComDir(COMD_Left)); else return(SetComDir(COMD_Right));
    if (GetProcedure() == "HANGLE")
      return AI_SetCommand("MoveTo", 0, GetX(), GetY()+20);
    // Fernkämpfer hat nichts zu tun
    //Log("nada");
    }

  // OK; Angriffsbefehl ausführen
  return true;
  }

protected func FxAIStop(object pTarget, int iEffectNumber, int iReason, bool fTmp)
  {
  if(fTmp) return;
  if(iReason!=4) return;
	if(GetAlive()) return -1;
  // Sollen die Gegenstände entfernt werden?
  if(!EA69_RemoveItemsOnDeath) return;
  // Ausrüstung entfernen
  var obj, i;
  while(obj = Contents(i, pTarget))
  {
    if(GetEffect("IntDeathRemoveMarker", obj)) RemoveObject(obj);
    else i++;
  }
  if(obj = FindObject(SHIA, 0, 0, 0, 0, 0, 0,pTarget)) RemoveObject(obj);
  if(IsKnight()) { LocalN("idShield", pTarget) = 0; LocalN("fArmored") = 0; LocalN("idArmorType") = 0; }
//  if(GetID()==MAGE) { LocalN("idStaff") = 0; }
  }

/* Bewegung */

func TextSound(szString)
{
	Sound(szString, 0, this);
}

private func GetStringCoin()
{
  var aStrings = ["Oh eine Münze!", "Hier liegt ja Geld rum", "Da hat jemand Gold verloren", "So ein Glück, Geld!"];
	var rand = Random(GetLength(aStrings));
	TextSound(Format("AI_Coin%d.ogg", rand+1));
  return aStrings[rand];
}
private func GetStringHurt()
{
  var aStrings = ["Bist du verletzt?", "%s was ist los mit dir?", "Ist dir übel?", "%s, du bist so still?!", "Hast du Probleme?"];
	var rand = Random(GetLength(aStrings));
	TextSound(Format("AI_Hurt%d.ogg", rand+1));
  return aStrings[rand];
}
private func GetStringDead()
{
  var aStrings = ["Oh nein! %s ist tot!", "Verflucht, %s wurde ermordet!", "Diese Schweine, sie haben %s getötet!", "Es muss ein Mörder unter uns sein!"];
	var rand = Random(GetLength(aStrings));
	TextSound(Format("AI_Dead%d.ogg", rand+1));
  return aStrings[rand];
}
private func GetStringWakeUp()
{
  var aStrings = ["Wach auf!", "Steh auf du Schlafmütze", "Wieder an die Arbeit Faulpelz", "Aus den Federn mit dir!"];
	var rand = Random(GetLength(aStrings));
	TextSound(Format("AI_WakeUp%d.ogg", rand+1));
  return aStrings[rand];
}
private func GetStringMouse()
{
  var aStrings = ["Muss wohl eine Maus gewesen sein.", "Komisch, ich dachte wirklich, da wäre was gewesen.", "Hmm, da ist ja garnichts.", "Hallo?|Ist da wer?"];
	var rand = Random(GetLength(aStrings));
	TextSound(Format("AI_Mouse%d.ogg", rand+1));
  return aStrings[rand];
}
private func GetStringSwitch()
{
  var aStrings = ["Oh, der Schalter war aber anders!", "Hat die Maus den Schalter umgelegt?", "Warum steht der Schalter anders da?", "So war das aber nicht hier."];
	var rand = Random(GetLength(aStrings));
	TextSound(Format("AI_Switch%d.ogg", rand+1));
  return aStrings[rand];
}
private func GetStringSeenMurder()
{
  var aStrings = ["Da ist der Mörder!", "Dort ist der Assassine!", "Der war es!", "Dort ist er!"];
	var rand = Random(GetLength(aStrings));
	TextSound(Format("AI_SeenMurder%d.ogg", rand+1));
  return aStrings[rand];
}
private func GetStringAttacker()
{
  var aStrings = ["Alarm! Ein Angreifer.", "Ein Meuchler hat sich reingeschlichen! Alarm!", "Achtung, da ist ein Feind!", "Ich habe einen Gegner entdeckt."];
	var rand = Random(GetLength(aStrings));
	TextSound(Format("AI_Attacker%d.ogg", rand+1));
  return aStrings[rand];
}

private func AI_Home()
  {
	if(!ai_iRange) return;
  if(aListActivitys || GetLength(iCurrentActivity))
		if(ExecActivity()) return true;
  if(GetCommand() || GetMacroCommand()) return true;
  // X-Richtung reicht fürs Home
  if (Abs(GetX() - ai_iHomeX) > 10 || Abs(GetY() - ai_iHomeY) > 30) if (!ai_pHome && ai_iHomeX)
    {
    // Magier kann sich auch warpen
    if (AI_DoMagic(EH69)) { AI_SetCommand("None"); return true; }
    AI_SetCommand("MoveTo", 0, ai_iHomeX, ai_iHomeY);
    return AI_ExecMovement();
    }
  // Container
  if (Contained() != ai_pHome)
    {
    if (ai_pHome)
      {
      if (!(GetOCF(ai_pHome) & OCF_Entrance)) { ai_pHome=0; return true; /* Pferd tot */ } 
      if (!WildcardMatch(GetAction(), "Ride*") || GetActionTarget() != ai_pHome)
        {
        AI_SetCommand("Enter", ai_pHome);
        return AI_ExecMovement();
        }
      else
			{
				if(ai_pHome->GetCommand()) return true;
			  if (Abs(GetX() - ai_iHomeX) > 10)
					ai_pHome->SetCommand(0, "MoveTo", 0, ai_iHomeX, ai_iHomeY);
			}
      }
    else
      {
      AI_SetCommand("Exit");
      return true;
      }
    }
	if(ai_fCallHome) GameCall("AI_HomeReached", this);
  // @ Home
  if (GetProcedure() == "WALK") SetDir(ai_iHomeDir);
  // Wenig Energie: Heilung
  if (GetEnergy() < 30) if (AI_Heal()) return true;
  // KI-Clonks regenerieren auch so (laaangsam)
  if (!Random(50)) DoEnergy(1);
  // Erstmal alles stoppen
  SetCommand(0, "None"); SetComDir(COMD_Stop);
  // Haben wir eine Route?
  if( ai_aPatrolPoints && (!Random(20+40*!!(Contained()) ) || ai_fRunner) )
  {
    // nächsten Punkt finden
    ai_iPatrolIndex++;
    // oder wenn wir schon beim letzen sind wieder von vorne anfangen
    if(ai_iPatrolIndex >= GetLength(ai_aPatrolPoints)) ai_iPatrolIndex = 0;
    if(GetType(ai_aPatrolPoints[ai_iPatrolIndex])==C4V_C4Object)
		{
			if(ai_pHome && !ai_pHome->~IsHorse()) ai_pHome = ai_aPatrolPoints[ai_iPatrolIndex];
			ai_iHomeX = GetX(ai_aPatrolPoints[ai_iPatrolIndex]);
			ai_iHomeY = GetY(ai_aPatrolPoints[ai_iPatrolIndex]);
		}
		else
		{
			// Punkt als neues Ziel setzen
			if(ai_pHome && !ai_pHome->~IsHorse()) ai_pHome = 0;
			ai_iHomeX = ai_aPatrolPoints[ai_iPatrolIndex][0];
			ai_iHomeY = ai_aPatrolPoints[ai_iPatrolIndex][1];
		}
    // Zum neunen Ziel schauen
    SetDir(ai_iHomeX > GetX());
  }
  // Clonkspezifische Aktion
  if (OnAIIdle()) return true;
  // Tjo. Nix zu tun.
  return true;
  }

private func AI_Evade(int iDir, object pProjectile) // iDir in {-1,+1}
  {//DebugLog(Format("%s #%d: AI_Evade", GetName(), ObjectNumber()), "AI_ExecMovement");
  var iOldDir, iYDiff, pShooter;
  //Log("AI_Evade %d %v", iDir, pProjectile);
  // Magier-Ausweichen
  // Kraftfeld kommt ggf. zu spät, aber schützt dann vor zusätzlichen Pfeilen
  if (!Random(5))
    if (AI_DoMagic(CMFG, ARRW, 50)) return true; // Tarnung
  if (!Random(5)) if (ai_pTarget) if (ObjectCount(NTMG))
    if (AI_DoMagic(MPXC, 0,0, ai_pTarget)) return true; // Positionswechsel (M&M)
  if (!Random(5))
    if (AI_DoMagic(MCIB, 0,0, this)) return true; // Clonk in Flasche (M&M)
  if (pProjectile) if (GetMagicEnergy()>=9) if (AI_CanCast())
    {
    if (!Random(5)) if (pProjectile->~IsArrow())
      {
      iOldDir = GetDir();
      SetDir((1-iDir)/2);
      // Bogenschützen von unten mit Kraftfeldbrücke abwehren
      pShooter = GetCursor(GetController(pProjectile));
      if (pShooter) if (WildcardMatch(GetAction(pShooter), "*Aim*") || WildcardMatch(GetAction(pShooter), "Magic*"))
        {
        iYDiff = GetY(pShooter) - GetY();
        if (iYDiff > 20) if (iYDiff > Abs(GetX() - GetX(pShooter)))
          if (AI_DoMagic(MBRG, METL, 1)) return true;
        }
      // Ansonsten hilft zur Abwehr die Kraftfeldwand
      if (AI_DoMagic(MFFW)) return true;
      SetDir(iOldDir);
      }
    // Windstoß gegen Projektile
    if (!Random(4))
      {
      iOldDir = GetDir();
      SetDir((1-iDir)/2);
      if (AI_DoMagic(ABLA, SPHR, 70, pProjectile)) return true;
      SetDir(iOldDir);
      }
    // M&M: Schweben
    if (!Random(3))
      if (!FindObject2(Find_ID(GVTY), Find_InRect(-50,-50,100,100)))
        if (AI_DoMagic(GVTY)) return true;
    // M&M: Zurückwerfen
    if (!Random(2))
      if (!GetEffect("DeflectionPSpell", this))
        if (AI_DoMagic(MDFL, 0,0, this)) return true;
    // M&M: Steinschild
    if (!GetEffect("StoneShieldPSpell", this))
      if (AI_DoMagic(MSSH, ROCK, 50, this)) return true;
    }
  // Sprung frei?
  if (PathFree(GetX(), GetY(), GetX()+25*iDir, GetY()-20))
    {
		if(GetProcedure() == "FIGHT")
			SetAction("Walk");
    AddCommand(0, "Jump", 0, iDir);
    SetComDir(5-2*iDir);
    return AI_ExecMovement();
    }
  // Weg frei?
  if (PathFree(GetX(), GetY(), GetX()+30*iDir, GetY()))
    {
		if(GetProcedure() == "FIGHT")
			SetAction("Walk");
    AddCommand(0, "MoveTo", 0, GetX()+30*iDir,GetY());
    return AI_ExecMovement();
    }
  // Nix frei :(
  }

private func AI_Panic()
  {
  // Tarnung: Nicht wenn brennend (Besser Zauberenergie sparen)
  if (!OnFire())
    if (!Random(2)) if (AI_DoMagic(CMFG, ARRW, 50)) return true;
  // Warp - deaktiviert fürs Szenario
  //if (!Random(2)) if (AI_DoMagic(MGWP)) return true;
  // Aaah!11
  if (GetProcedure() != "WALK") return true;
  Jump(); SetXDir(0); SetYDir(GetYDir()/2);
  SetAction("Jump");
  Sound("Scream");
  return true;
  }

private func AI_JumpShort()
{
  SetPhysical("Jump",GetPhysical("Jump", 0, this)/2,PHYS_StackTemporary, this);
}

private func AI_JumpShortStop()
{
  ResetPhysical(this, "Jump");
}

func SetCommand(a,b,c,d,e,f,g,h)
{
	if(b == "MoveTo")
		if( !c && !d && !e)
			b = "None";
	DebugLog(Format("%v: SetCommand(%v,%v,%v,%v,%v,%v,%v,%v)", this, a,b,c,d,e,f,g,h), "AI_ExecMovement");
	return _inherited(a,b,c,d,e,f,g,h);
}

func AddCommand(a,b,c,d,e,f,g,h)
{
	DebugLog(Format("%v: AddCommand(%v,%v,%v,%v,%v,%v,%v,%v)", this, a,b,c,d,e,f,g,h), "AI_ExecMovement");
	return _inherited(a,b,c,d,e,f,g,h);
}

public func RejectCollect(id idObject, object pObject)
{
	if(Local(9)) return true;
	return _inherited(idObject, pObject);
}

private func AI_ExecMovement()
  {//DebugLog(Format("%s #%d: AI_ExecMovement", GetName(), ObjectNumber()), "AI_ExecMovement");
	return;
  // Bestimmte Aktionen abbrechen
  if(GetAction()=="SitDown") SetAction("StandUp");
  //Log("ExecMovement: %s %s", GetProcedure(), GetCommand());
  var obj;
  var szCommand = GetCommand();
  var pTarget = GetCommand(0, 1), iTargetX, iTargetY;
  var iTargetX = GetCommand(0, 2), iTargetY = GetCommand(0, 3);
  var fShouldJump, fAnySolid, cx, dx, dy, iJumpOutDir;
  var pAnimal;
  // Sprünge während der normalen Bewegung
  if (GetProcedure() == "WALK")
    {
    if (szCommand == "MoveTo" || szCommand == "Attack" || szCommand == "Follow" || (szCommand == "Throw" && iTargetX))
      {
      if (!iTargetX) iTargetX = GetX(GetCommand(0, 1));
      if (!iTargetY) iTargetY = GetY(GetCommand(0, 1));
      //Log("ExecMovement: %d %d", iTargetX, iTargetY);
      // Über Brennendes springen
      if(!OnFire())
        if(obj=FindObject2(Find_InRect(-40*!GetDir(),-40,40,80), Find_Exclude(this), Find_OCF(OCF_OnFire), Find_Layer(GetObjectLayer()), Find_NoContainer()))
          fShouldJump = true;
      // Über Lava/Säurebecken/Tiefe Löcher springen
      cx=40*GetDir()-20;
      if(GetAction() != "Jump" && !InLiquid())
        for(var j=1; j<50; j+=5)
          if(GetMaterialVal("Incindiary", "Material", GetMaterial(cx,j)))
            fShouldJump = true;
          else if (GBackSolid(cx, j))
            {
            fAnySolid = j;
            break;
            }
      // Sprung zum Gegner, bzw. Sprung einfach so
      if (GetY() - iTargetY > 16)
        if (!Random(8)) fShouldJump = true;
      // Sprung ausführen
      if (fShouldJump || (!fAnySolid && (iTargetY - GetY() < 20)))
        // Will/kann die KI überhaupt über dieses Loch?
        if (  Abs(GetX() - iTargetX) > 40 &&  AI_SimJumpCheck(cx/20))
          // Dann springen
          return AddCommand(0, "Jump");
        else
          {
						AI_JumpShort();
						if (  Abs(GetX() - iTargetX) > 20 &&  AI_SimJumpCheck(cx/20))
						{
							AddCommand(0, "Call", this, 0,0, 0, 0, "AI_JumpShortStop");
							// Dann springen
							return AddCommand(0, "Jump");
						}
						else
						{
							AI_JumpShortStop();
							// Ansonsten stehen bleiben
							AI_SetCommand("None");
							SetComDir(COMD_Stop);
						}
          }
      }
    }
  // Im Wasser: Forcieren, dass geschwommen wird
  else if (GetProcedure() == "SWIM")
    {
    // Auf Flüssigkeiten laufen ist auch immer nett
    if (!Random(10)) if (!GetEffect("WalkOnLiquidSpell", this))
      if (AI_DoMagic(WOLI)) return true;
    dx = iTargetX-GetX(); dy = iTargetY-GetY();
    //Log ("Swimming %d  %d", dx, dy);
    if (dy<0 && GBackLiquid(0,-2))
      SetComDir(COMD_Up);
    else
      {
      if (dx>0)
        {
        SetComDir(COMD_Right);
        if (GetContact(0, -1, CNAT_Right)) iJumpOutDir=1;
        }
      else
        {
        SetComDir(COMD_Left);
        if (GetContact(0, -1, CNAT_Left)) iJumpOutDir=-1;
        }
      // Längere Schwimmzeit, wenn das Ziel weit oberhalb liegt, damit die KI nicht ewig im Wasser hin und herschwimmt
      if (-dy-10 > Abs(dx))
        AddCommand(0, "Wait", 0, 0,0, 0, 0,20);
      // Hängt an der Seite im Wasser? Rausspringen.
      if (iJumpOutDir)
        {
        AddCommand(0, "Call", this, iJumpOutDir,0, 0, 0, "AI_WaterJumpOut");
        AddCommand(0, "MoveTo", 0, GetX()-iJumpOutDir*10,GetY());
        ai_iDisableTime += 15;
        }
      }
    return true;
    }
  // Reitend: Bewegungssteuerung ans Tier
  else if (WildcardMatch(GetAction(), "Ride*"))
    {
    if (pAnimal = GetActionTarget())
      {
      if (szCommand == "MoveTo") 
        {
        SetCommand(pAnimal, "MoveTo", pTarget, iTargetX, iTargetY);
        }
      else
        {
        SetCommand(pAnimal, "None");
        SetComDir(COMD_Stop, pAnimal);
        if (szCommand == "Enter")
          {
          // Enter-Command: Geht nicht. Verwerfen.
          SetCommand(0, "None");
          }
        else if (szCommand == "Throw")
          {
          // Werfen umsetzen
          ControlThrow();
          SetCommand(0, "None");
          }
        }
      }
    }
  // Hangelnd und will nach unten? Dann mal runter
  else if (GetProcedure() == "HANGLE")
    {
    SetAction("Jump");
    return AddCommand(0, "MoveTo", 0, GetX(), GetY()+20);
    }
  // Kein Kommando mehr: Erst einmal anhalten
  if (!GetCommand() && !GetMacroCommand()) SetComDir(COMD_Stop);
  // Nahkampf:
  // Schon am Kämpfen?
  if (GetProcedure() == "FIGHT") return AI_ExecFight();
  // Gegner in Reichweite für Nahkampf?
  if (ai_pTarget) if (ObjectDistance(ai_pTarget) <= 15) if (!Random(3))
    if (Contained() == Contained(ai_pTarget))
      if (IsAnimal())
        // Tiere beißen zu
        return Attack(ai_pTarget);
      else
        if (!WildcardMatch(GetAction(), "Ride*")) // Nicht ganz so leicht vom Pferd stoßen lassen
          // Clonks kämpfen
          return FightWith(ai_pTarget); // (eigentlich redundant; die Engine sollte den Kampf schon gestartet haben)
  // Im Sprung normalerweise keine weiteren Aktionen, damit der Sprung nicht abgebrochen wird
  if (GetProcedure() == "FLIGHT" || GetCommand() == "Jump")
    {
    // Außer magischer Brücke zum Auffangen des Sprunges für Magier
    if (!Random(20)) if (GetYDir()>5 && Abs(GetXDir()) > 5)
      if (AI_DoMagic(MBRG, METL, 10)) return true;
    return true;
    }
  // Normales Gehen: Zum Gegner lieber laufen, wenn es geht
  var iPhase;
  if (ai_pTarget) if (GetAction() == "Walk")
    {
    iPhase = GetPhase();
    if (SetAction("Run")) SetPhase(iPhase);
    }
  // Keine Spezialbehandlung
  }
  
protected func AI_WaterJumpOut(object pThis, int iJumpDir)
  {
  //Log("AI_WaterJumpOut %v %v", pThis, iJumpDir);
  if (GetProcedure() != "SWIM") return;
  if (iJumpDir>0)
    {
    SetComDir(COMD_Right);
    SetDir(DIR_Right);
    }
  else
    {
    SetComDir(COMD_Left);
    SetDir(DIR_Left);
    }
  SetXDir(BoundBy(GetPhysical("Swim")/2500,24,38) * iJumpDir);
  return Call("ControlUpDouble");
  }
  
private func AI_SimJumpCheck(int iDir) // iDir =-1 oder +1
  {
  // Sprungparameter ermitteln
  var x=GetX(), y=GetY();
  var vx = GetPhysical("Walk")*iDir*7/250, vy = -GetPhysical("Jump")/10;
  var vyo = vy;
  //Log("SimFlight %d %d   %d %d",x,y,vx,vy);
  if (!SimFlight(x, y, vx, vy, 25, 1000, -1, 1000)) return false; // Bodenlos: Kein Sprung
//  Log("->SimFlight %d %d   %d %d",x,y,vx,vy);
  // Nicht zu tief nach unten, oder gegen Wand/Decke
  if (!Inside(vy, 0, vyo*-2)) return false;
  // Nicht in Lava oder Säure
  var iTargetMat = GetMaterial(x,y);
  if(GetMaterialVal("Incindiary", "Material", iTargetMat) || GetMaterialVal("Corrosive", "Material", iTargetMat)) return false;
  // Ansonsten OK
  return true;
  }


/* Werfen */

private func AI_ExecThrow()
  {
  // Kann jetzt werfen?
  if (GetProcedure() != "WALK" && !IsAiming()) return;
  // Sofort werfen wenn Gegner in Schussbahn
  // Irgendwas zum Werfen dabei?
  var iThrowWdt = 55;
  var pProjectile, i;
  if (pProjectile = FindContents(SPER))
    iThrowWdt = 100;
  else if (!(pProjectile = FindContents(TSWD)))
    {
    // Brennend bevorzugen
    if (!(pProjectile = FindObject2(Find_Container(this), Find_OCF(OCF_OnFire))))
      {
      // Ansonsten alle Inhaltsobjekte durchgehen, und mit Blacklist (Heiltränke, etc) vergleichen
      for(;;)
        {
        if (!(pProjectile = Contents(i)))
          {
          // Kein Projektil gefunden
          return;
          }
        if (GetIndexOf(GetID(pProjectile), g_EA69_aNoThrow) < 0) break;
        ++i;
        }
      }
    }
  // Nächste Gegnerposition ermitteln
  var pEnemy = AI_FindNearestEnemy();
  var dx = GetX(pEnemy) - GetX(), dy = GetY(pEnemy) - GetY(), fThrow;
  if (!dx) return; // Fängt auch pEnemy=0 ab
  var dir = dx/Abs(dx); dx = Abs(dx);
  // Beim Zweihänder ist die Wurfbahn alles direkt nebenan
  if ((GetID(pProjectile) == TSWD) && IsPaladin())
    {
    if (Inside(dy, -20, 20)) fThrow = Inside(dx, -40, 40);
    }
  else
    {
    // "In etwa" in der Wurfbahn?
    if (Inside(dy, -30, 30))
      {
      if (!(fThrow=Inside(Angle(0,0,Abs(dx),dy), 20, 60)))
        {
        if (!Random(2))
          if (GetMass(pProjectile)<17 && GetID(pProjectile) != EFLN)
            {
            ShiftContents(this, false, GetID(pProjectile));
            if (AI_DoMagic(FGOB)) return true; // M&M: FlingObject
            }
        return;
        }
      }
    else
      fThrow = Inside(dx, iThrowWdt - 10, iThrowWdt + 10);
    }
  if (!fThrow) return;
  // Zu werfendes Objekt vorher anzünden?
  if (!Random(3))
    if (GetOCF(pProjectile) & OCF_Inflammable)
      if (!OnFire(pProjectile))
        {
        ShiftContents(this, false, GetID(pProjectile));
        if (AI_DoMagic(MINC)) return true;
        }
  // Gegner anschauen
  SetDir((dir+1)/2);
  // k, Wurf! Mit Spezialbehandlung beim Speer und Zweihänder
  if (pProjectile->~ControlThrow(this)) return true;
  // Normalerweise ungezielt werfen; ab und zu mal einen Wurf mit Zielposition für Bewegung
  if (!Random(5)) return AI_SetCommand("Throw", pProjectile, GetX(pEnemy), GetY(pEnemy));
  // Beim Reiten muss direkt geworfen werden, weil die engineinterne Steuerung es nicht packt
  if (WildcardMatch(GetAction(), "Ride*"))
    {
    ShiftContents(this, false, GetID(pProjectile));
    return ControlThrow();
    }
  return AI_SetCommand("Throw", pProjectile);
  }



/* Nahkampf */

private func AI_ExecFight()
  {
  // Wenig Energie: Heilung wenn möglich
  if (GetEnergy() < 15) if (AI_Heal()) return true;
  // Im Kampf? Supportzauber wären nett
  var pClonk, pOpp;
  if (GetProcedure() == "FIGHT")
    for (pClonk in FindObjects(Find_InRect(-150,-150,300,300), Find_OCF(OCF_Living), Find_Layer(GetObjectLayer()), Find_NoContainer(), Find_Exclude(this)))
      if (pClonk->~IsAI())
        if (pClonk->~IsMagiclonk())
          if (pClonk->AI_SupportAllyFight(this))
            // Puh, danke!
            return AI_Thank(pClonk);
  // Fernkämpfer wollen eigentlich gar nicht
  if (!Random(15))
    {
    pOpp = GetActionTarget();
    SetAction("Walk");
    if (GetX() < GetX(pOpp)) SetComDir(COMD_Left); else SetComDir(COMD_Right);
    AI_SetCommand("MoveTo", 0, GetX()+(GetX() >= GetX(pOpp))*50-25, GetY());
    return AI_ExecMovement();
    }
  return true;
  }

/* Kanonen bzw Katapult/Ballisten Bedienung */
private func AI_ExecWeapon()
{
  if(!ai_pWeapon) return;
  if(GetAction()!="Push" || GetActionTarget()!=ai_pWeapon)
    return AI_SetCommand("Grab", ai_pWeapon);
  if(GetEffect("IntShootDelay", this)) return 1;
  if(GetID(ai_pWeapon) == CATA)
  {
    if(GetAction(ai_pWeapon)!="Ready") return 1;
    if(!Contents(0, ai_pWeapon)) ai_pWeapon->CreateContents(FLNT);
    ai_pWeapon->FireAt(GetX(ai_pTarget)+RandomX(-50,50));
    AddEffect("IntShootDelay", this, 1, 35);
    return 1;
  }
  if(GetID(ai_pWeapon) == CANN)
  {
    if(GetAction(ai_pWeapon)!="Ready") return 1;
    if(!Contents(0, ai_pWeapon)) { ai_pWeapon->CreateContents(SFLN); ai_pWeapon->CreateContents(GUNP); }
    ai_pWeapon->FireAt(GetX(ai_pTarget)+RandomX(-50,50), GetY(ai_pTarget)+RandomX(-50,50), 1);
    AddEffect("IntShootDelay", this, 1, 45);
    return 1;
  }
  if(GetID(ai_pWeapon) == XBOW)
  {
    if(GetAction(ai_pWeapon)!="Ready") return 1;
    if(!Contents(0, ai_pWeapon)) ai_pWeapon->CreateContents(XARW);
    ai_pWeapon->FireAt(GetX(ai_pTarget)+RandomX(-50,50), GetY(ai_pTarget)+RandomX(-50,50), 1);
    AddEffect("IntShootDelay", this, 1, 25);
    return 1;
  }
}

/* Assassinenangriff */

private func AI_ExecAssassin()
  {
  // Nachladen: Immer OK (wird von ExecuteAttackCrossbow falsch behandelt)
  if (IsLoading()) return true;
  // Armbrust dabei?
  if (IsCrossbowEquipped()) if (ExecuteAttackCrossbow(ai_pTarget)) return true;
  // Wenig Energie: Heilung wenn möglich
//  if (GetEnergy() < 15) if (AI_Heal()) return true;
  // 2do
  }

/* Ritterangriff */

private func AI_ExecKnight()
  {
	if(!ai_pTarget) return false;
  // Nachladen: Immer OK (wird von ExecuteAttackBow falsch behandelt)
  if (IsLoading()) return true;
  // Bogen dabei?
  if (IsBowEquipped()) if (ExecuteAttackBowX(ai_pTarget)) return true;
  if(IsAiming()) SetAction("Walk");
  // Wenig Energie: Heilung wenn möglich
  if (GetEnergy() < 15) if (AI_Heal()) return true;
  // 2do
  }

private func ExecuteAttackBowX(pTarget)
  {
  // Zielposition ermitteln
  var iTx = GetX(pTarget), iTy = GetY(pTarget);
	if(!FireBowAtX(iTx, iTy, true))
		return 0;
  // Ziel in Reichweite?
  if (Abs(iTx-GetX())>300 || Abs(iTy-GetY())>300)
    // Nicht? Mögliche Abschußposition einnehmen
    return( SetCommand(this(),"MoveTo",0, BoundBy(GetX(),iTx-290,iTx+290),GetY()) );
  // Bogen auswählen
  ShiftContents(0, 0, BOW1);
  // Kann leider von hier aus nicht zielen
  if (!IsAiming())
    if (!ReadyToFire())
      return(0);
  // Ziel nicht im Freien
  if (Contained(pTarget))
    return(0);
  // Ziel von hier aus nicht in Sicht
  if (!PathFree( GetX(),GetY(), iTx, iTy ))
    if (!PathFree( GetX(),GetY()-20, iTy-20 ))
      return(0);
  // Schießen
	FireBowAtX(iTx,iTy, false);
  // Weitermachen
  return 1;
  } 

private func FireBowAtX(int iX, int iY, bool fTest)
{
	SetMacroCommand("None");
	var dx = iX-GetX();
	var dy = iY-GetY();
	var iXPos = GetX(), iYPos = GetY();

	var angle = Angle(0,0,dx,dy);

	var vx = Sin(angle, 60+RandomX(-5,5)*!fTest);//dx/faktor;
	var vy = dy*vx/dx - 105*GetGravity()*dx/vx/1000;//Height;

  // Zielt gar nicht: erst mal laden
  if (!IsAiming() && !fTest)
    {
    LoadBow(FindContents(BOW1), 1);
    return;
    }
	var time = dx*10/vx;
	var iXDir = vx, iYDir = vy;
	if(SimFlightX(iXPos, iYPos, iXDir, iYDir, ARRW, time, 10)) //SimFlight(iX, iY, vx, vy, 1, 100, time))
	{
		vx = Sin(angle, 90+RandomX(-5,5)*!fTest);//dx/faktor;
		vy = dy*vx/dx - 105*GetGravity()*dx/vx/1000;//Height;

		iXPos = GetX(); iYPos = GetY(); iXDir = vx; iYDir = vy;
		time = dx*10/vx;
		if(SimFlightX(iXPos, iYPos, iXDir, iYDir, ARRW, time, 10))//SimFlight(iX, iY, vx, vy, 1, 100, time))
			return 0;
	}
	if(fTest) return true;
  // Zielen
  AimBowToAngle(angle);
  // Feuern
  return(Contents()->~FireSpeed(vx, vy, angle));
} 

/* Paladinangriff */

private func AI_ExecPaladin()
  {
  var clnk, length;
  // Muskete dabei?
  if (IsMusketEquipped()) if (ExecuteAttackMusket(ai_pTarget)) return true;
  // Wunder wirken
  if (AI_CanConjure()) if (!Random(3))
    {
    // Viel Zauberenergie, wenig Leben; Schutz
    if (GetMagicEnergy() > GetEnergy()) if (!Random(10)) if (!GetEffect("ProtectionPSpell", this)) if (AI_DoConjure(MPRT)) return true;
    // Gegner in Sichtweite: Einige Fernattacken
    if (!Random(6)) if (PathFree(GetX(), GetY(), GetX(ai_pTarget), GetY(ai_pTarget)))
      {
      // EnergyAttack
      if (!Random(3)) if (AI_DoConjure(MGPH)) return true;
      // Horizontaler Angriff: LifeDepletion
      if (Abs(GetY() - GetY(ai_pTarget)) < 20) if (AI_DoConjure(MLDL)) return true;
      }
    // Tote Verbündete wiederbeleben
    if (!Random(10))
      {
      for(clnk in FindObjects(Find_Distance(200), Find_Action("Dead"), Find_Owner(GetOwner()), Find_Func("IsClonk"), Find_Layer(GetObjectLayer()), Find_Not(Find_Func("GetAlive"))))
        {
        if(GetEffect("SummonSkelletons",clnk) || GetEffect("Revival",clnk)) continue;
        length=GetPathLength(GetX(),GetY(),GetX(clnk),GetY(clnk));
        if(!length || length > 200 || (!PathFree(GetX(),GetY(),GetX(clnk),GetY(clnk)) && length > 100)) continue;
        break;
        }
      if (clnk)
        {
        // Wiederbelebung als Clonk
        if (!Random(2)) if (AI_DoConjure(MRVL)) return true;
        // Wiederbelebung als Skelett
        if (AI_DoConjure(MSMN)) return true;
        }
      }
    // Schneller laufen
    if (GetComDir()) if (!Random(5)) if (!GetEffect("SpeedPSpell", this))if (AI_DoConjure(MSPD)) return true;
    }
  }

/* Piratenangriff */

private func AI_ExecPirate()
  {
  // Nachladen: Immer OK
  if (IsLoading()) return true;
  // Muskete dabei?
  if (IsMusketEquipped()) if (ExecuteAttackMusket(ai_pTarget)) return true;
  // Fausbüchse dabei?
  if (IsGunEquipped()) if (ExecuteAttackGun(ai_pTarget)) return true;
  // Wenig Energie: Heilung wenn möglich
  if (GetEnergy() < 15) if (AI_Heal()) return true;
  }


/* Mystik */
private func AI_ExecMystic()
{
  // Gegner erreichbar: Projektilzauber
  if(PathFree(GetX(), GetY(), GetX(ai_pTarget), GetY(ai_pTarget)))
  {
    if (!Random(ai_iCastChance)) // Kristall: Blitz
      if (AI_DoMystic(CRYS)) return true;
    if (!Random(ai_iCastChance)) // Erz: Blutsauger
      if (AI_DoMystic(ORE1)) return true;
    if (!Random(ai_iCastChance)) // Stein: Steinschlag
      if (AI_DoMystic(ROCK)) return true;
    if (!Random(ai_iCastChance)) // Schneeball: Schneeball :-)
      if (AI_DoMystic(SNWB)) return true;
    if (!Random(ai_iCastChance)) // Holz: Magischer Bogen
      if (AI_DoMystic(WOOD)) return true;
    if (!Random(ai_iCastChance)) // Schießpulver: Meteorschlag
      if (AI_DoMystic(GUNP)) return true;
    if (!Random(ai_iCastChance)) // Flint: Flammenschlag
      if (AI_DoMystic(FLNT)) return true;
    if (!Random(ai_iCastChance)) // Superflint: Feuerbombemwurf
      if (AI_DoMystic(SFLN)) return true;
    if (!Random(ai_iCastChance)) // Terafilnt: Drachenatem
      if (AI_DoMystic(EFLN)) return true;
    if (!Random(ai_iCastChance)) // Schädel: Knochenwurf
      if (AI_DoMystic(SKUL)) return true;
  }
}

/* Zauberei */

private func AI_ExecMagician()
  {
  //Log("AI_ExecMagician");
  // Wenig Zauberenergie: Meditation
  if (GetMagicEnergy() < 15)
    {
    if (AI_DoMagic(MMED)) return true;
    // Keine Zauber ohne Zauberenergie
    return false;
    }
  // Wenig Energie: Heilung
  if (GetEnergy() < 15)
    {
    if (AI_Heal()) return true;
    }
  // Gefroren: Wärmen
  if (Frozen(this)) if (AI_DoMagic(MWTH)) return true;

  // Gegner erreichbar: Blitz
  if (!Random(ai_iCastChance))
    if(PathFree(GetX(), GetY(), GetX(ai_pTarget), GetY(ai_pTarget)))
      if (AI_DoMagic(MLGT)) return true;

  // Gegner an Flammenposition: Ewige Flammen
  var dx = Abs(GetX(ai_pTarget)-GetX());
  var dy = GetY(ai_pTarget)-GetY();
  if (!Random(ai_iCastChance))
    if(Inside(dx, 50, 70) && dy > 0)
      if (AI_DoMagic(ETFL)) return true;

  // Gegner direkt nebenan: Feuerfaust
  if (!Random(ai_iCastChance))
    if(Inside(dx, 15, 50) && dy > -5)
      if (InLiquid())
        {
        if (AI_DoMagic(FRFS, METL)) return true;
        }
      else
        if (AI_DoMagic(FRFS)) return true;

  // Gegner erreichbar: Airblast
  if (!Random(ai_iCastChance))
    if(PathFree(GetX(), GetY(), GetX(ai_pTarget), GetY(ai_pTarget)))
      if (AI_DoMagic(ABLA, SPHR, 60)) return true;

  // Gegner nahe und "müde": Schlaf
  if (!Random(ai_iCastChance))
    if(Distance(dx,dy) < 80 && GetEnergy(ai_pTarget) < GetPhysical("Energy",1,ai_pTarget)/2000)
      if (AI_DoMagic(MBOT, ROCK, 50)) return true;

  // Gegner in Reichweite: Feuerball/Eisschlag/Magischer Bogen
  var idFlint;
  if (!Random(ai_iCastChance))
    if(PathFree(GetX(), GetY(), GetX(ai_pTarget), GetY(ai_pTarget)))
      {
      if (!Random(3)) if (AI_DoMagic(MICS)) return true;
      if (!Random(2)) if (Distance(dx,dy) < 300) if (AI_DoMagic(MGBW)) return true;
      idFlint = [EFLN, SFLN, FLNT][Random(3)];
      if (AI_DoMagic(MFRB, idFlint, 50)) return true;
      }

  // Gegner in Reichweite und wenig Energie: Blutsauger
  if (!Random(ai_iCastChance)) if (GetEnergy() < 50)
    if(PathFree(GetX(), GetY(), GetX(ai_pTarget), GetY(ai_pTarget)))
      if (AI_DoMagic(MBLS)) return true;

  // Gegner steht voraus: Feueratem
  if (!Random(ai_iCastChance))
    if(Inside(dx, 10, 40) && Abs(dy) < 15)
      if (AI_DoMagic(MDBT)) return true;
      
  // Gegner unterhalb: Feuerklumpen
  if (!Random(ai_iCastChance))
    if (dy > 30 && Inside(dx, 30, 200))
      if (AI_DoMagic(MFBL, FLNT, 50)) return true;
 
  // Gegner auf einer Höhe in Gefahrenzone: Feuerwand
  if (!Random(ai_iCastChance))
    if (Inside(dx, 25, 55) && Inside(dy, -150, 15))
      if (AI_DoMagic(MFWL)) return true;
      
  // Verbündete Clonks mit Schaden im Umkreis: Gruppenheilung oder Wiederherstellung
  var pClonk;
  if (!Random(ai_iCastChance))
    for (pClonk in FindObjects(Find_InRect(-100,-100,200,200), Find_OCF(OCF_CrewMember), Find_NoContainer(), Find_Layer(GetObjectLayer()), Find_Allied(GetOwner()), Find_Exclude(this)))
      if (GetEnergy(pClonk) < GetPhysical("Energy",0,pClonk)/1500)
        {
        if (AI_HelpAlly(pClonk)) return true;
        }
            
  // Der Gegner ist nahe: Zaps
  if (!Random(ai_iCastChance))
    if (Distance(dx,dy) < 45)
      if (AI_DoMagic(GZ9Z, 0,0, this)) return true;
      
  // Der Gegner ist nahe: Verblendung
  if (!Random(ai_iCastChance*20))
    if (Distance(dx,dy) < 60)
      if (AI_DoMagic(LGCN)) return true;

  // Leichen da: Totenbelebung
  if (!Random(ai_iCastChance))
    for (pClonk in FindObjects(Find_InRect(-150,-150,300,300), Find_OCF(OCF_Living), Find_NoContainer(), Find_Layer(GetObjectLayer()), Find_Exclude(this)))
      if (pClonk->~IsClonk())
        if(!GetAlive(pClonk))
          if(!GetEffectCount("*",pClonk)) // Keine Leichen, die wieder aufstehen könnten
            if (AI_DoMagic(RUND)) return true;
            
  // Flüche, wenn Gegner noch nicht verflucht. Erhöhte Chance wenn selber verflucht
  if (!Random(ai_iCastChance) || GetEffect("*Curse*", this))
    if (!GetEffect("*Curse*", ai_pTarget))
      {
      if (!Random(4)) if (AI_DoMagic(CAHE, 0,0, ai_pTarget)) return true;
      if (!Random(3)) if (AI_DoMagic(CPAN, 0,0, ai_pTarget)) return true;
      if (!Random(2)) if (AI_DoMagic(CFAL, 0,0, ai_pTarget)) return true;
      if (AI_DoMagic(CCNF, 0,0, ai_pTarget)) return true;
      }
  
  // Clonk steht unterhalb: Feuerschlangen
  if (!Random(ai_iCastChance)) if (dy > dx)
    if (AI_DoMagic(MFSK)) return true;

  // Endboss: Magische Kopien, wenn sonst keine Kopien im Umkreis
  if (ai_isEndboss) if (!Random(2))
    if (!ObjectCount(MAGE, -60,-60,120,120, OCF_Alive, 0,0, 0, GetOwner()))
      if (AI_DoMagic(MGRP, WIPF)) return true;

  // "Nah"kampfzauber
  if (Distance(dx,dy) < 85)
    {
    // Frostwelle
    if (!Random(ai_iCastChance))
      if (AI_DoMagic(MFWV)) return true;

    // Steinschlag
    if (!Random(ai_iCastChance))
      if (AI_DoMagic(MARK)) return true;
      
    // Metall+Magie-Zauber
    // Blitz
    if (!Random(ai_iCastChance))
      if(Abs(dy) < 10)
        if (AI_DoMagic(MCLT)) return true;
    // Terashock
    if (!Random(ai_iCastChance))
      if (AI_DoMagic(MTSK)) return true;

    // Luftklaue
    if (!Random(ai_iCastChance))
      if (AI_DoMagic(AFST)) return true;

    // Druckfaust
    if (!Random(ai_iCastChance))
      if (!GetEffect("PressureFistPSpell", this))
        if (AI_DoMagic(PRFT)) return true;

    // Viel in der Nähe zum Umherschleudern: Erdbebenaura
    if (!Random(ai_iCastChance))
      if (ObjectCount2(Find_Category(C4D_Object), Find_NoContainer()))
        if (!GetEffect("EarthquakeAura", this))
          if (AI_DoMagic(AUEQ)) return true;

    // Schattenfaust
    if (!Random(ai_iCastChance))
      if (!GetEffect("ShadowFistPSpell", this))
        if (AI_DoMagic(SHFT)) return true;
        
    // Hitzeaura, wenn keine Verbündeten nahe
    if (!Random(ai_iCastChance))
      if (!FindObject2(Find_OCF(OCF_Alive), Find_Allied(GetOwner()), Find_Layer(GetObjectLayer())))
        if (!GetEffect("HeatAura", this))
          if (AI_DoMagic(AUHE)) return true;
    
    // Feuercluster
    if (!Random(ai_iCastChance))
      if (AI_DoMagic(MFCS)) return true;
    
    // Flammenfaust (M&M)
    if (!Random(ai_iCastChance))
      if (!GetEffect("FlameFistPSpell", this))
        if (AI_DoMagic(FLFT)) return true;
      
    // Eisfaust
    if (!Random(ai_iCastChance))
      if (!GetEffect("IceFistPSpell", this))
        if (AI_DoMagic(ICFT)) return true;
    
    // Schneeball
    if (!Random(ai_iCastChance))
      if (AI_DoMagic(MGSW)) return true;
      
    // Frosthauch
    if (!Random(ai_iCastChance))
      if (AI_DoMagic(MFBZ)) return true;
    }

  // Schwimmend: LiquidConvert, um schneller raus zu kommen
  if (!Random(ai_iCastChance))
    if (GetProcedure() == "SWIM")
      if (AI_DoMagic(MCLD)) return true;

  // Zufallszauber?
  //if (!Random(ai_iCastChance)) if (AI_DoMagic(RMMG)) return true;
  // Nix zaubern
  }

private func AI_CanCast(id idSpell)
  {
  // Nur Zauberer
  if (!IsMagiclonk()) return false;
  // Zauber angegeben: Zauberenergie checken
  var idStaff, fHasSpell;
  if (idSpell)
    {
    if (!GetName(0, idSpell)) return false; // Zauber nicht geladen
    if (!CheckMana(idSpell)) return false;
    if (ObjectCount(ALCO)) if(!CheckAlchem(idSpell, this)) return false;
    // Stabzauber?
    if (EA69_RespectStaff)
      if (idStaff = idSpell->~GetSpellClass(this))
        if (idStaff->~IsStaff())
          if (this->~HasStaff() != idStaff)
            {
            // Stabzauber werden auch bei uneingeschränkten Zaubern gesperrt
            // Es ist allerdings möglich, dass der Stabzauber trotz nicht-Besitz des Stabes über die Zauberliste verfügbar ist
            if (!ai_aSpells) return false;
            }
          else
            fHasSpell = true;
    // Zauber eingeschränkt? Außer, es ist ein Stabzauber und der Magus hat den Stab!
    if (ai_aSpells) if (!fHasSpell) if (GetIndexOf(idSpell, ai_aSpells) < 0) return false;
    }
  // Nicht zu schnell nacheinander
  if (WildcardMatch(GetAction(), "Magic*")) return false;
  // Bereit? (Definitionsabhängiger Actioncheck)
  return (Call("ReadyToMagic"));
  }

private func AI_CanConjure(id idWonder)
  {
  // Paladine zaubern mal wieder anders, und haben auch ganz andere Zauber
  if (!IsPaladin()) return;
  // Zauber angegeben: Zauberenergie checken
  if (idWonder)
    {
    if (!GetName(0, idWonder)) return; // Wunder nicht geladen
    if(!CheckMana(idWonder)) return;
    if(ObjectCount(ALCO)) if(!CheckAlchem(idWonder, this)) return;
    if (ai_aSpells) if (GetIndexOf(idWonder, ai_aSpells) < 0) return;
    }
  // Der Paladin könnte laut ReadyToConjure immer Wunder wirken
  // KI-Paladine sind hier etwas eingeschränkt, damit sie nicht zu stark sind
  return (GetProcedure() == "WALK" || GetProcedure() == "ATTACH");
  }

private func AI_DoMystic(id idObject)
{
  var pObj = FindContents(idObject);
  // Objekt passender ID suchen, das nicht gerade läd
  for(pObj in FindObjects(Find_Container(this), Find_ID(idObject)))
  {
    if(!GetEffect("MysticDelay", pObj)) break;
  }
  if(!pObj) return;
  // Das Ziel anschauen, für alle gerichteten Zauber
  if (ai_pTarget) SetDir(GetX(ai_pTarget) > GetX());
  // Keine Befehle, damit Magieaktion nicht abgebrochen wird
  AI_SetCommand("None");
  SetComDir(COMD_Stop);
  Call("SetMagicAction", CastC4ID(pObj));
  // Irgendwelche Nachrichten von Zaubern löschen
  Message(" ", this);
  return true;
}

private func AI_DoMagic(id idSpell, id idComboObj, int iComboChance, object pSpellTarget)
  {
  if (!AI_CanCast(idSpell)) return;
  // Das Ziel anschauen, für alle gerichteten Zauber
  if (ai_pSpellTarget = pSpellTarget)
    SetDir(GetX(ai_pSpellTarget) > GetX());
  else if (ai_pTarget)
    SetDir(GetX(ai_pTarget) > GetX());
  // Keine Befehle, damit Magieaktion nicht abgebrochen wird
  AI_SetCommand("None");
  SetComDir(COMD_Stop);
  //Log("AI_DoMagic %s (%i) at %v or %v", GetName(0, idSpell), idSpell, ai_pSpellTarget, ai_pTarget);
  // Komboobjekt erzeugen
  // - bleibt möglicherweise im Magier, wenn der Zauber durch Treffer oder ähnliches abgebrochen wird
  //   egal; ist halt ein Zauberer :)
  if (idComboObj) if (!FindContents(idComboObj)) if (EA69_CheatCombo)
    if (!iComboChance || Random(100) <= iComboChance)
      CreateContents(idComboObj);
  Call("DoMagic", idSpell);
  // Irgendwelche Nachrichten von Zaubern löschen
  Message(" ", this);
  // Erstmal nichts weiter tun, damit der Zauber nicht abgebrochen wird
  ai_iDisableTime += 5;
  //Message("AI_DoMagic %s (%i)", this, GetName(0, idSpell), idSpell);
  //Log("AI_DoMagic %s (%i)", GetName(0, idSpell), idSpell);
  // Erstmal Mana auf 0, damit nicht dauerhaft gezaubert wird
  // Geht aber nicht wegen verzögerter Zauber. Ugh.
  //DoMagicEnergy(-GetMagicEnergy());
  return true;
  }

private func AI_DoConjure(id idWonder)
  {
  if (!AI_CanConjure(idWonder)) return;
  // Das Ziel anschauen, für alle gerichteten Zauber
  if (ai_pTarget) SetDir(GetX(ai_pTarget) > GetX());
  // Keine Befehle, damit Magieaktion nicht abgebrochen wird
  AI_SetCommand("None");
  SetComDir(COMD_Stop);
  //Log("AI_DoConjure %s (%i)", GetName(0, idWonder), idWonder);
  if(Call("ExecMagic", idWonder))
    // Zauberenergie abziehen
    DoMagicEnergy(-Value(idWonder));
  // Irgendwelche Nachrichten von Zaubern löschen
  Message(" ", this);
  return true;
  }

public func AI_HelpAlly(object pClonk)
  {
  // Wird schon geheilt?
  if (GetEffect("Cure*PSpell", pClonk)) return;
  // Nur Zauber helfen hier
  if (!AI_CanCast()) return;
  var fBurning = OnFire(pClonk);
  if (Random(2) || ObjectDistance(pClonk) > 80 || fBurning)
    {
    // Heilung auf verbündete Clonks
    if (AI_DoMagic(ELX2, 0,0, pClonk)) return true;
    if (!fBurning)
      {
      if (AI_DoMagic(ELX1, 0,0, pClonk)) return true;
      if (AI_DoMagic(MCIB, 0,0, pClonk)) return true;
      }
    // Brennend: Löschen hat Priorität
    if (fBurning) if (ObjectDistance(pClonk) <= 60) if (AI_DoMagic(EXTG)) return true;
    }
  // Oder einfach Gruppenheilung
  if (!Random(2)) if (AI_DoMagic(MHGA)) return true;
  if (AI_DoMagic(GGHG)) return true;
  // Heilung nicht erolgreich
  }

public func AI_SupportAllyFight(object pClonk)
  {
  //Log("AI_SupportAllyFight(%v)", pClonk);
  // Unterstützungszauber für den Nahkampf
  if (!AI_CanCast()) return;
  //Log("..cancast!");
  // Auf Zielclonk zaubern
  if (!Random(12)) if (AI_DoMagic(GZ9Z, 0,0, pClonk)) return true;
  if (!Random(11)) if (!GetEffect("DeflectionPSpell", pClonk)) if (AI_DoMagic(MDFL, 0,0, pClonk)) return true;
  if (!Random(10)) if (!GetEffect("StoneShieldPSpell", pClonk)) if (AI_DoMagic(MSSH, 0,0, pClonk)) return true;
  if (!Random(9)) if (!GetEffect("HotBloodPSpell", pClonk)) if (AI_DoMagic(MHTB, 0,0, pClonk)) return true;
  if (!Random(8)) if (!GetEffect("FlameFistPSpell", pClonk)) if (AI_DoMagic(FLFT, 0,0, pClonk)) return true; // M&M: Flammenfaust
  // Kein Zauber ging/keiner ergab sich zufällig
  //Log("..nothing");
  }

public func AI_Thank(object pClonk)
  {
  // Dankesnachricht? Man kann es auch übertreiben :)
  return true;
  }

private func AI_Heal()
  {
  // Wird schon geheilt?
  if (GetEffect("Cure*PSpell", this)) return;
  // Versuchen, an Energie zu kommen
  // Brote? Heiltränke? Alles essbare ist gut!
  var pFood;
  if (pFood = FindObject2(Find_Container(this), Find_OCF(OCF_Edible)))
    {
    // Clonk kämpft? Brotzeit!
    if (GetProcedure() == "FIGHT")
      SetAction("Walk");
    else if (GetProcedure() != "WALK")
      // Ansonsten essen, wenn Zeit dazu ist
      return false;
    if (ProtectedCall(pFood, "Activate", this))
      return true;
    }
  if (AI_CanCast())
    {
    // Futter herbeizaubern
    if (!Random(3)) if (AI_DoMagic(MGFO)) return true;
    // Heilzauber
    if (AI_DoMagic(ELX2)) return true;
    if (AI_DoMagic(MGHL)) return true;
    if (AI_DoMagic(ELX1)) return true;
    }
  // Heilwunder
  if (AI_DoConjure(MHEL)) return true;
  // Verbündeten suchen, der heilen kann
  var pClonk;
  for (pClonk in FindObjects(Find_InRect(-150,-150,300,300), Find_OCF(OCF_Living), Find_NoContainer(), Find_Layer(GetObjectLayer()), Find_Exclude(this)))
    if (pClonk->~IsAI())
      if (pClonk->~IsMagiclonk())
        if (pClonk->AI_HelpAlly(this))
          // Puh, danke!
          return AI_Thank(pClonk);
  // Heilgegenstand in der Nähe suchen
  if (!(ai_dwFlags & EA69_NoItemGet))
    for (pFood in FindObjects(Find_InRect(-150,-20,300,70), Find_OCF(OCF_Edible), Find_Layer(GetObjectLayer())))
      if (!Contained(pFood) || !GetAlive(Contained(pFood))) if (GetOCF(pFood) & OCF_Fullcon)
        {
        ai_iDisableTime += 15;
        return AI_SetCommand("Get", pFood);
        }
  // Kein Erfolg: Nicht heilen
  }
  
/* Gegnersuche */

private func AI_FindNearestEnemy(fLookAround)
  {
  // Sucher
  if(fLookAround || ai_fFindAssassins || ai_iAlarmedCounter) Message("{{_EYE}}", this);
//  else Message("", this);
    
  var cont = Contained();
  if (cont) cont = Find_Container(cont); else cont = Find_NoContainer();
  var obj, iDistance;
	//if(ai_iAlarmedCounter) ai_iAlarmedCounter--;
	fLookAround += ai_iAlarmedCounter;
  // Objekte prüfen:
  // Muss leben und im Freien sein, wenn die KI auch im Freien ist...
  for (var obj in FindObjects(Find_Distance(ai_iRange), cont, Find_OCF(OCF_Alive), Sort_Distance(), Find_Hostile(GetOwner()), Find_Layer(GetObjectLayer())))
    if ((GetOwner()<0 && GetOwner(obj)>=0) || Hostile(GetOwner(obj), GetOwner())) // ...verfeindet...
      if ( GetDir() == (GetX(obj)>GetX())) // ...sichtbar... (2do: Richtiger Check)
       if( fLookAround || ((( GetRGBaValue(GetClrModulation(obj), 0)<((ai_iRange-ObjectDistance(obj))*200/ai_iRange) ) || ai_fFindAssassins)&& !obj->~IsDisguised() ))
			 if( fLookAround || Abs(GetX()-GetX(obj)) > Abs(GetY()-GetY(obj)) ) // ... im "Sichtkegel"
        if (GetOCF(obj) & OCF_CrewMember)          // ...gegnerischer CrewMember...
          {
          if (!PathFree(GetX(), GetY(), GetX(obj), GetY(obj))) // ...in Sicht...
            if (!PathFree(GetX()+(GetX(obj)>GetX())*40-20, GetY()-20, GetX(obj), GetY(obj))) // ...oder in "Sprung/Wurfsicht"....
              continue;
          // Durch Verblendung verdeckt?
          if (FindObject2(Find_ID(LGCN), Find_InRect(GetX(obj)-GetX()-120, GetY(obj)-GetY()-120, 240,240))) continue;
          // Kontakt ans Szenscript melden
          if (ai_idFirstEncounterCB) AI_DoEncounter(obj, 0);
          return obj; // OK, den nehmen!
          }

	if(g_EA69_fAssassinModeOff) return;
	if(ai_fRunner) return;
	
	if(!ai_iNoSpecial && GetActivityPriority() <= 1)	
	// Münzen suchen
	for (var obj in FindObjects(Find_Distance(ai_iRange/2), cont, Find_ID(_CIN), Find_Layer(GetObjectLayer()), Sort_Distance()))
//    if(!GetEffect("IntFoundMarker", obj) && GetOwner(obj) != -1)
//     if (GetDir() == (GetX(obj)>GetX())) // ...sichtbar... (2do: Richtiger Check)
          {
					if (!GetVisibility(obj) && (GetDir() == (GetX(obj)>GetX())))
					if( fLookAround || Abs(GetX()-GetX(obj)) > Abs(GetY()-GetY(obj)) ) // ... im "Sichtkegel"
					{
						if (!PathFree(GetX(), GetY(), GetX(obj), GetY(obj))) // ...in Sicht...
							if (!PathFree(GetX()+(GetX(obj)>GetX())*40-20, GetY()-20, GetX(obj), GetY(obj))) // ...oder in "Sprung/Wurfsicht"....
								continue;
						if(obj->~GetCounter() < 2)
						{
							SetActivity(obj);
							obj->~AddCounter();
						}
//						AI_LookForFriend(obj);
					}
          }
	
  if(!ai_iNoSpecial && GetActivityPriority() <= 2)
  // Tote Verbündete suchen
  for (var obj in FindObjects(Find_Distance(ai_iRange), cont, Find_OCF(OCF_Living), Find_Action("Dead"), Find_Allied(GetOwner()), Find_Layer(GetObjectLayer()), Sort_Distance()))
    if(!GetEffect("IntFoundMarker", obj) && GetOwner(obj) != -1)
//     if (GetDir() == (GetX(obj)>GetX())) // ...sichtbar... (2do: Richtiger Check)
          {
					if (!GetVisibility(obj) && (GetDir() == (GetX(obj)>GetX())))
					if( fLookAround || Abs(GetX()-GetX(obj)) > Abs(GetY()-GetY(obj)) ) // ... im "Sichtkegel"
					{
						if (!PathFree(GetX(), GetY(), GetX(obj), GetY(obj))) // ...in Sicht...
							if (!PathFree(GetX()+(GetX(obj)>GetX())*40-20, GetY()-20, GetX(obj), GetY(obj))) // ...oder in "Sprung/Wurfsicht"....
								continue;
						AI_LookForFriend(obj);
					}
          }
  }

public func AI_LookForFriend(pFriend)
{
  // Beschäftigt? nichts machen
  if(ai_pTarget) return;
	if(ai_iNoSpecial) return 0;
  // Oh, da fällt jemand um!
  MessageEx("Hey, was ist da los %s?", this, GetName(pFriend));
	TextSound("AI_Hey1.ogg");
	if(GetEffect("SleepNSpell", pFriend))
		SetActivity([WAKE, pFriend]);
	else
		SetActivity([CORP, pFriend]);
  AddEffect("IntFoundMarker", pFriend, 1, 0);
  return 1;
}

private func GetStringAlarm()
{
  var aStrings = ["War da was?", "Da hab ich doch was gehört.", "Ich höre Geräusche.", "Da muss ich mal nachschauen."];
	var rand = Random(GetLength(aStrings));
	TextSound(Format("AI_Alarm%d.ogg", rand+1));
  return aStrings[rand];
}

public func AI_LookForCurios(pSource)
{
	if(g_EA69_fAssassinModeOff) return 0;
	if(ai_pTarget == pSource) return 0;
	if(ai_iNoSpecial) return 0;
  // Beschäftigt? nichts machen
  if(ai_pTarget) return;
  MessageEx(GetStringAlarm(), this);
	SetActivity([CURY, pSource]);
  return 1;
}

global func AI_AlarmCurious(pSource, iRange)
{
  if(!pSource) pSource = this;
  if(!pSource) return;
	if(GetEffect("IntNoAlarmCurious", pSource)) return;
	AddEffect("IntNoAlarmCurious", pSource, 1, 35*5);
  if(!iRange) iRange = EA69_EnemySearchRange;
  for(var obj in FindObjects(Find_Distance(iRange, GetX(pSource)-GetX(), GetY(pSource)-GetY()), Find_OCF(OCF_Living), Find_Layer(GetObjectLayer()), Find_OCF(OCF_Alive), Sort_Distance()))
    if (obj->~IsAI())
      if(obj->~AI_LookForCurios(pSource)) return 1;
}

public func AI_LookForEnemy(pSource, fNoCry)
{
	if(g_EA69_fAssassinModeOff) return 1;
	if(ai_pTarget == pSource) return 1;
	if(ai_iNoSpecial) return 0;
  // Beschäftigt? nichts machen
  if(ai_pTarget) return;
//  MessageEx(GetStringAlarm(), this);
/*  AI_SetCommand("MoveTo", 0, GetX(pSource), GetY(pSource));
  ai_pActionTarget = pSource;
  ai_iActionIndex = 0;
  ai_iActionType = 2;*/
	if(fNoCry) ai_iHeardAlarm = 1;
	SetActivity([ALRM, pSource->GetX(), pSource->GetY()]);
  return 1;
}

global func AI_AlarmEnemy(pSource, iRange)
{
  if(!pSource) pSource = this;
  if(!pSource) return;
  if(!iRange) iRange = EA69_EnemySearchRange;
  for(var obj in FindObjects(Find_Distance(iRange, GetX(pSource)-GetX(), GetY(pSource)-GetY()), Find_OCF(OCF_Living), Find_OCF(OCF_Alive), Find_Layer(GetObjectLayer()), Sort_Distance()))
    if (obj->~IsAI() && obj != this)
      if(obj->~AI_LookForEnemy(pSource, 1)) continue;
}

/* Ausrüstung */

public func AI_Equip()
  {
  // Nur einmal
  if (Contents() || !GetAlive()) return;
  // Tiere hauen nur mit Klauen
  if (IsAnimal()) return;
  // Erzeugt manchmal etwas mehr, als ins Inventar passt. Merkt schon keiner... :)
  if(IsMystic())
  {
    var idWeps = [CRYS, ORE1, ROCK, SNWB, WOOD, GUNP, FLNT, SFLN, EFLN, SKUL];
    var idWep;
    var iCount = Random(4)+3;
    for(var i = 0; i < iCount; i++)
    {
      idWep = idWeps[Random(GetLength(idWeps))];
      CreateContents(idWep);
    }
    return;
  }
  else if (IsKanderian())
    {
    if (!Random(2)) { CreateContents(BOW1); CreateContents(FARP); CreateContents(XARP); }
    if (!Random(2)) CreateContents(SHI2)->Activate(this);
    if (!Random(3)) CreateContents(AXE1); else if(!Random(2)) CreateContents(SCMT); else CreateContents(SPER);
    }
  // Ritter: Bogen, Schwert oder Axt, Schild
  else if (IsKnight())
    {
    if (!Random(2)) { CreateContents(BOW1); CreateContents(FARP); CreateContents(XARP); }
    if (!Random(2)) CreateContents(SHIE)->Activate(this);
    if (!Random(2)) CreateContents(AXE1); else CreateContents(SWOR);
    }
  // Waffen für alle
  var idWeps = [SPER, ROCK, FLNT, SFLN, EFLN, STFN, FBMP, SPER];
  var idWep = idWeps[Random(GetLength(idWeps))];
  CreateContents(idWep);
  return true;
  }

func SetMacroCommand() { return _inherited(...); }

/* Commands */

private func AI_SetCommand(string szCom, object pTarget, int tx, int ty, object pTarget2, int iData, int iRetries)
  {
  // Zielaktion stoppen
  this->~StopAiming();
	// Als Macro mit Wegpunkten machen
	if(FindObject2(Find_ID(WAYP)))
	{
		if (szCom == "MoveTo" || szCom == "Follow" || szCom == "Attack" || szCom == "Grab" || szCom == "Enter" || (szCom == "Throw" && tx))
			return SetMacroCommand(szCom, pTarget, tx, ty, pTarget2,  iData, iRetries);
	}
  //Log("Setting %s %v %d %d %v %d %d", szCom, pTarget, tx, ty, pTarget2, iData, iRetries);
  // Mit Call-Command versehen, damit Fehlschlag kein Hmmm verursacht
  SetCommand(0, "Call", this, 0,0, 0, "AI_Command");
  var res = AddCommand(0, szCom, pTarget, tx, ty, pTarget2, iData, iRetries, C4CMD_SilentBase);
  // Bei Bewegungskommandos gleich die Bewegung checken
  if (szCom == "MoveTo" || szCom == "Follow" || szCom == "Attack" || (szCom == "Throw" && tx)) AI_ExecMovement();
  return res;
  }

public func AI_Command() { return true; }
public func AI_CommandFailed() { return true; }

private func AI_GetStackedCommand(string szCom)
  {
  // Alle Commands durchcheken
  var i=-1, szCommand;
  while (szCommand = GetCommand(0, 0, ++i))
    if (!szCom || szCommand == szCom)
      return i;
  // Nichts gefunden
  return -1;
  }
  

/* Gegenrkontakt auch bei Schaden */

public func AI_DoEncounter(object pEnemy, bool fDead)
  {
  // Erster Gegnerkontakt!
  var idEncounterCB = ai_idFirstEncounterCB;
  ai_idFirstEncounterCB = 0;
  GameCall(Format("AI_Encounter%i", idEncounterCB), this, pEnemy, fDead);
  // Clonks aufzuwecken?
  if (ai_pBoss == this)
    {
    ai_pBoss = 0;
    for (var obj2 in FindObjects(Find_OCF(OCF_Alive)))
      if (obj2->~AI_GetBoss() == this)
        {
        obj2->AI_RefEnable();
        obj2->AI_SetBoss(0);
        }
    }
  }

protected func Damage(int iChange, int iCausedBy, a,b,c,d,e)
  {
  if (iChange > 0)
    {
    // Schaden: Gilt als Encounter
    if (ai_idFirstEncounterCB) AI_DoEncounter();
    }
  return _inherited(iChange, iCausedBy, a,b,c,d,e);
  }


/* Hilfappends */

public func IsMagiclonk()
  {
  return (GetID() == MCLK || GetID() == SCLK || GetID() == MAGE || GetID() == KING || GetID() == _RBS || GetID() == DWMG);
  }
  
public func IsPaladin()
  {
  return (GetID() == PLDN);
  }

public func IsAnimal() { return _inherited(...); }

public func OnAIIdle() { return _inherited(...); }

public func Attack(object pEnemy) { return _inherited(pEnemy, ...); }

public func AI_WarpHome()
  {
  return SetPosition(ai_iHomeX, ai_iHomeY);
  }

// Ritterfunktionen: Definition der Basisfunktionen im Clonk
private func GetObject2Drop(object pCollObj) { return _inherited(pCollObj, ...); }
private func IsBowEquipped() { return _inherited(...); }
private func IsMusketEquipped() { return _inherited(...); }
private func IsGunEquipped() { return _inherited(...); }
private func IsCrossbowEquipped() { return _inherited(...); }
private func ExecuteAttackBow(object pTarget) { return _inherited(pTarget, ...); }
private func ExecuteAttackMusket(object pTarget) { return _inherited(pTarget, ...); }
private func ExecuteAttackGun(object pTarget) { return _inherited(pTarget, ...); }
private func ExecuteAttackCrossbow(object pTarget) { return _inherited(pTarget, ...); }
private func IsAiming() { return _inherited(...); }
private func ReadyToFire() { return _inherited(...); }
private func LoadBow() { return _inherited(...); }
private func AimBowToAngle() { return _inherited(...); }
private func IsKnight() { return _inherited(...); }
private func IsAssassin() { return _inherited(); }
private func IsKanderian() { return _inherited(); }
private func IsMystic() { return _inherited(); }
private func IsPirate() { return _inherited(); }
private func IsLoading() { return _inherited(); }
private func IsLoading() { return _inherited(...); }
private func GetAimingDeviation() { return 5; }
private func SpellSucceeded() { return _inherited(...); }
private func SpellFailed() { return _inherited(...); }
public func CanUse() { return _inherited(...); }
public func ControlThrow() { return _inherited(...); }
// Kompatibilität
public func GetBulletCount() { return _inherited(...); }

// Fernkämpfer?
public func AI_IsRanged()
  {
  if (ai_dwFlags & EA69_ForceRanged) return true;
  if (IsMusketEquipped() || IsBowEquipped()) return true;
  if (IsMagiclonk()) if (!IsPaladin()) return true;
  }
  
// KI-Magier können auch im Kämpfen und Klettern zaubern
public func ReadyToMagic(a,b,c,d)
  {
  if (ai_iRange && GetID() == MAGE)
    if (GetProcedure() == "FIGHT" || GetProcedure() == "SCALE" || GetProcedure() == "HANGLE") return true;
  return _inherited(a,b,c,d);
  }
  
public func IsAI()
  {
  return (!!ai_iRange);
  }

public func IsDisabledAI()
{
	return GetEffect("AI", this);
}
  
public func AI_GetBoss() { return ai_pBoss; }
public func AI_SetBoss(object pToBoss) { ai_pBoss=pToBoss; return true; }
  
// Schwierigkeitsgrad setzen
global func AI_SetDifficulty(int iNewDifficulty)
  {
  // KI-Initialisierung
  if(!g_EA69_aNoThrow) AI_InitSystem();
  g_EA69_iDifficulty = iNewDifficulty;
  // Alle Physicals aktualisieren
  for (var obj in FindObjects(Find_OCF(OCF_Alive), Find_Func("IsAI")))
    obj->AI_SetPhysicals();
  }

// Beim Aufdecken Physicals anpassen
public func OnReactivate() { return AI_SetPhysicals(); }
  
public func AI_SetPhysicals()
  {
  // KI-Spezifische Physicals
  // g_EA69_iDifficulty: Statische Variable 1=Normal, 2=Schwer
  if (!ai_iRange) return;
  if (!ai_iStrength) ai_iStrength = 100;
  // Noch ohne Besitzer?
/*  if(GetOwner()==-1)
  {
    var iColor = GetColorDw();
    SetOwner(GetPlayerByIndex(0, C4PT_Script));
    SetColorDw(iColor);
  }*/
  // Ein Paladin? Der braucht als KI auch Mana!
  if(IsPaladin())
  {
    SetPhysical("MagicEnergy", 100000, PHYS_Temporary);
    DoMagicEnergy(100, 0, 1);
  }
  var iStrength = ai_iStrength * (Max(g_EA69_iDifficulty, 1)+1) / 2;
  AI_SetPhysical("Fight", iStrength);
  AI_SetPhysical("Energy", iStrength);
  if (IsMagiclonk()) AI_SetPhysical("Magic", iStrength);
  DoEnergy(10000); // Max. Energie
  // Inventargegenstände markieren
  if(!GetEffect("IntDeathRemoveMarker", this))
  {
    var obj, i;
    // Alle Objekte markieren, damit nicht später eingesammelte Objekte beim Tod mit entfernt werden
    // (ein vom Spieler geklautes Schwert soll natürlich nicht verschwinden)
    while(obj = Contents(i, this))
    {
      if (GetIndexOf(GetID(obj), g_EA69_aRemoveOnDeath) >= 0)
        AddEffect("IntDeathRemoveMarker", obj, 1, 0);
      i++;
    }
    // So, selbst markieren, damit nicht nochmals markiert werden kann
    AddEffect("IntDeathRemoveMarker", this, 1, 0);
  }
  }
  
// Temporäre Phsical auf relativen Wert des Definitionswertes setzen
public func AI_SetPhysical(string szPhysical, int iPercent)
  {
  return SetPhysical(szPhysical, GetPhysical(szPhysical, 0, 0, GetID())*iPercent/100, PHYS_Temporary);
  }
  
// Wolfüberladung: Schlafen deaktiviert KI
protected func Sleeping() { AI_RefDisable(); SetCommand(0, "None"); /*Log("Sleeping");*/}
protected func EndSleeping() { AI_RefEnable(); /*Log("EndSleeping");*/}


/* Tiere an Wänden - Clonks haben hier eh keinen Callback */

protected func ContactLeft()
  {
  // Rudimentäres Überwinden von Wänden
  if (IsAnimal()) if (GetCommand() == "MoveTo") if (GetCommand(0, 2)) if (GetCommand(0, 2) < GetX())
    {
    AddCommand(this, "Jump", 0, GetX() - 20, GetY() - 20);
    AddCommand(this, "MoveTo", 0, GetX() + 50, GetY());
    }
  return _inherited();
  }
  
protected func ContactRight()
  {
  // Rudimentäres Überwinden von Wänden
  if (IsAnimal()) if (GetCommand() == "MoveTo") if (GetCommand(0, 2)) if (GetCommand(0, 2) > GetX())
    {
    AddCommand(this, "Jump", 0, GetX() + 20, GetY() - 20);
    AddCommand(this, "MoveTo", 0, GetX() - 50, GetY());
    }
  return _inherited();
  }


// Totgestellte KI (Aufwecken per externem Script)
public func AI_FeignDeath()
  {
  // Kein Mehrfachaufruf; kein Totstellen in Gebäuden (sieht ja eh keiner)
  if (Contained()) return;
  // Leiche an der Stelle erzeugen, und in die Leiche versetzen
  var pCorpse = CreateObject(GetID(), 0, GetDefBottom()-GetY(), GetOwner());
  SetDir(GetDir(), pCorpse);
  Kill(pCorpse);
  SetName(GetName(), pCorpse);
  Enter(pCorpse);
  SetColorDw(GetColorDw(), pCorpse);
  SetClrModulation(GetClrModulation(), pCorpse);
  SetObjectBlitMode(GetObjectBlitMode(), pCorpse);
  // Keine KI-Aktionen in der Leiche
  AI_RefDisable();
  return true;
  }

func GetMacroCommand() { return _inherited(...); }
func SetActivity() { return _inherited(...); }
func ExecActivity() { return _inherited(...); }
func GetActivityPriority() { return _inherited(...); }
  
local aListActivitys;
local iCurrentActivity;