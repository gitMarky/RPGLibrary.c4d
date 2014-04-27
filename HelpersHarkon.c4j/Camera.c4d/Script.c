/*-- Kamera --*/

#strict 2

static g_pCamera;
static g_pFader;
static g_pSpeaker;
static g_szNextDialog;

local iFading;
local isCam;
local mysound;

static fFirst;

static g_DialogSound;

global func DialogSound(szSound, pSpeaker)
{
	if(g_DialogSound) g_DialogSound->FinishSound();
	if(szSound)
	{
		g_DialogSound = CreateObject(_CAM);
		g_DialogSound->SetSound(szSound, pSpeaker);
	}
}

func FinishSound()
{
	SetAction("MoveAway");
	ScheduleCall(this, "DoFinishSound", 35*60, 1);
}
func MoveAway()
{
	SetPosition(GetX(), GetY()-10);
}
func DoFinishSound()
{
	SetPosition(GetX(), -100000);
	RemoveObject();
}
func SetSound(szSound, pSpeaker)
{
	SetAction("Speak", this);
	SetPosition(GetX(pSpeaker), GetY(pSpeaker));
	mysound = szSound;
}

func DoSetSound(szSound)
{
	return Sound(mysound, 0, this, 0, 0, 0, 0, -1);
}

global func DialogMessage(pSpeaker, Text, Next, ButtonText, fNoCam, szSound)
{
	g_szNextDialog = Next;
	g_pSpeaker = 0;
	
	var szPortrait, szName;
	if(GetType(pSpeaker) == C4V_C4Object)
	{
		g_pSpeaker = pSpeaker;
		AddEffect("IntSpeaking", pSpeaker, 1, 1, 0, _CAM);
		szPortrait = Format("Portrait:%i::%x::%s", pSpeaker->GetPortrait(0, 1), GetColorDw(pSpeaker), pSpeaker->GetPortrait(0, 0));
		szName = Format("<c %x>%s:</c>", MakeReadableColor(pSpeaker->GetColorDw()), pSpeaker->GetName());
		if(szSound) DialogSound(szSound, pSpeaker);
	}
	else if(GetType(pSpeaker) == C4V_Array)
	{
		if(!pSpeaker[3]) pSpeaker[3] = RGB(0,0,255);
		if(!pSpeaker[2])
			szPortrait = Format("%i", pSpeaker[0]);
		else
			szPortrait = Format("Portrait:%i::%x::%s", pSpeaker[0], pSpeaker[3], pSpeaker[2]);
		szName = pSpeaker[1];
		if(szSound) DialogSound(szSound, g_pCamera);
	}
	else if(GetType(pSpeaker) == C4V_C4ID)
	{
		szPortrait = Format("%i", pSpeaker);
		szName = pSpeaker->GetName();
		if(szSound) DialogSound(szSound, g_pCamera);
	}
	
	if(!ButtonText) ButtonText = "Weiter";
	
	if(g_pCamera)
	{
		CreateMenu(WIPF,g_pCamera,this,0,szName,0,C4MN_Style_Dialog);
		AddMenuItem(szPortrait,0,NONE,g_pCamera,0,0,0,5);
		AddMenuItem(Text,0,NONE,g_pCamera);
		AddMenuItem(ButtonText, "DialogMessageStop()", LMM2, g_pCamera, 0, 0, 0, 2, 4);
		if(!fFirst) AddMenuItem("Skip", "DialogMessageSkip()", LMM2, g_pCamera, 0, 0, 0, 2, 6);
	}
	else
	{
		for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
		{
			var pClonk = GetCursor(GetPlayerByIndex(i, C4PT_User));
			CreateMenu(WIPF,pClonk,this,0,szName,0,C4MN_Style_Dialog);
			AddMenuItem(szPortrait,0,NONE,pClonk,0,0,0,5);
			AddMenuItem(Text,0,NONE,pClonk);
			AddMenuItem(ButtonText, "DialogMessageStop()", LMM2, pClonk, 0, 0, 0, 2, 4);
		}
	}
	fFirst = 1;

	if(!fNoCam && g_pCamera && g_pSpeaker) PosCam(g_pSpeaker);
}

func FxIntSpeakingTimer(pSpeaker, iNumber, time)
{
	if( (time/10) % 3 == 0)
		MessageExPermanent(".  ", pSpeaker);
	if( (time/10) % 3 == 1)
		MessageExPermanent(".. ", pSpeaker);
	if( (time/10) % 3 == 2)
		MessageExPermanent("...", pSpeaker);
}
protected func MenuQueryCancel() { Log("!"); return 1; }
global func DialogMessageStop()
{
	if(!g_pCamera)
	{
		for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
			CloseMenu(GetCursor(GetPlayerByIndex(i, C4PT_User)));
	}
	RemoveEffect("IntSpeaking", g_pSpeaker);
	Message("", g_pSpeaker);
	g_pSpeaker = 0;
	if(g_szNextDialog)
		Call(g_szNextDialog);
}

global func DialogMessageSkip()
{
	StopFilm();
	if(g_pSpeaker)
	{
		RemoveEffect("IntSpeaking", g_pSpeaker);
		Message("", g_pSpeaker);
		g_pSpeaker = 0;
	}

	g_fIntroSkiped = 1;
	
	if(GetScenarioSection() != "Map")
	{
		SaveObjects();
		LoadScenarioSection("Map", 0);
		// Alles wiederherstellen
		RestoreSectObjs();
	}

	StartSzen();
}

global func IsFilm() { return !!g_pCamera; }

global func StartFilm()
{
  // Schon ein Film? Nichts machen
  if(g_pCamera) return;
  // Kamera
  g_pCamera = CreateObject(_CAM, 0, 0, -1);
  g_pCamera->SetAction("Fly");
  g_pCamera->SetCategory(2);
  g_pCamera->LocalN("isCam") = 1;
	g_pCamera->SetPlrViewRange(40);
  // Spieler vorbereiten
  for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
  {
		var iPlr = GetPlayerByIndex(i, C4PT_User);
    var iCount = GetCrewCount(iPlr);
    var pObj;
		ClearLastPlrCom(iPlr);
    while (iCount--) {
      pObj = GetCrew(iPlr, iCount);
			if(!pObj) continue;
      SetCrewEnabled(0, pObj);
      SetPlrViewRange(0, pObj);
      SetDivinity(true, pObj);
			pObj->StopClonk();
    }
    SetPlrView(iPlr, g_pCamera);
		if(i == 0)
			SetCursor(iPlr, g_pCamera);
		else
			SetCursor(iPlr, g_pCamera);
  }
}

func CrewSelection(bool fDeselect, bool fCursorOnly)
{
	if(!fDeselect) return;
	
	for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
  {
		var iPlr = GetPlayerByIndex(i, C4PT_User);
		if(!GetCursor(iPlr))
			SetCursor(iPlr, g_pCamera);
	}
}

// Schaden abblocken
global func FxDivinityDamage() { return 0; }

global func FxDivinityEffect(string szNewEffect)
  {
  if(szNewEffect == "Divinity") return -1; // Einer reicht
  }

global func SetDivinity(bool val, object obj)
  {
  if (!obj) if (!(obj = this)) return false;
  if (val)
    {
    if(!GetEffect("Divinity", obj)) AddEffect("Divinity", obj, 1, 0);
    }
  else
    {
    RemoveEffect("Divinity", obj);
    }
  return true;
  }

global func PosCam() // iX, iY oder pObj
{
  if(!g_pCamera)
  {
    StartFilm();
    Par(2) = 1;
  }
  RemoveEffect("IntCamFollow", g_pCamera);
	RemoveEffect("IntCamAttach", g_pCamera);
  // Spielersicht setzen
  for(var i = 0; i < GetPlayerCount(); i++)
    SetPlrView(GetPlayerByIndex(i), g_pCamera);
  g_pCamera->SetAction("Fly");
  if (!Par())
  {
    // Leeres Objekt übergeben, d.h. kein Spieler da.
    return;
  }
  if(GetType(Par())==C4V_C4Object)
  {
		g_pCamera->SetCamTarget(Par());
    if(Par(2))
		{
			g_pCamera->SetPosition(Par()->GetX(), Par()->GetY());
			return g_pCamera->SetAction("Attach", Par());
		}
		AddEffect("IntCamFollow", g_pCamera, 1, 2, g_pCamera);
    SetCommand(g_pCamera, "Call", g_pCamera, Par(), 0, 0, "AttachMe");
    return AddCommand(g_pCamera, "MoveTo", 0, GetX(Par()), GetY(Par()));
  }
  if(Par(2)) return SetPosition(Par(), Par(1), g_pCamera);
  SetCommand(g_pCamera, "Call", g_pCamera, 0, 0, 0, "StopMe");
  AddCommand(g_pCamera, "MoveTo", 0, Par(), Par(1));
}

local pCamTarget;

func SetCamTarget(obj) { pCamTarget = obj; }

func FxIntCamFollowTimer(target, num)
{
	if(GetAction() != "Attach" && ObjectDistance(pCamTarget)>5)
	{
//		if(EffectVar(0, target, num) != GetX(pCamTarget) && EffectVar(1, target, num) != GetY(pCamTarget))
		{
			SetCommand(this, "Call", this, pCamTarget, 0, 0, "AttachMe");
			AddCommand(this, "MoveTo", 0, GetX(pCamTarget), GetY(pCamTarget));
			var speed = Distance(0, 0, GetXDir(), GetYDir());
			var angle = Angle(GetX(), GetY(), GetX(pCamTarget), GetY(pCamTarget));
			SetXDir( Sin(angle, speed));
			SetYDir(-Cos(angle, speed));
			EffectVar(0, target, num) = GetX(pCamTarget);
			EffectVar(1, target, num) = GetY(pCamTarget);
		}
	}
	else
		return -1;
}

func FxIntCamAttachTimer(target)
{
	if(GetAction() != "Attach")
		return -1;
	if(Contained(pCamTarget) && Contained(pCamTarget) != GetActionTarget())
		SetAction("Attach", Contained(pCamTarget));
	else if(!Contained(pCamTarget) && pCamTarget != GetActionTarget())
		SetAction("Attach", pCamTarget);
}

public func AttachMe(pObj, pTarget)
{
  SetAction("Attach", pTarget);
  SetComDir(COMD_Stop);
  SetXDir(0); SetYDir(0);
	AddEffect("IntCamAttach", this, 1, 1, this);
}

public func StopMe()
{
  SetComDir(COMD_Stop);
  SetXDir(0); SetYDir(0);
}

global func StopFilm()
{
  // Kein Film? Nichts machen
  if(!g_pCamera) return;
	DialogSound();
  // Kamera entfernen
  RemoveObject(g_pCamera);
	if(g_pFader) g_pFader->RemoveObject();
  // Spieler vorbereiten
  var pCam;
  for(var i = 0; i < GetPlayerCount(); i++)
  {
    var iCount = GetCrewCount(GetPlayerByIndex(i));
    var pObj;
    while (iCount--) {
      pObj = GetCrew(GetPlayerByIndex(i), iCount);
      SetCrewEnabled(1, pObj);
      SetPlrViewRange(150, pObj);
      pCam = FindObject2(Find_ID(_CAM), Find_ActionTarget(pObj));
      if(pCam) RemoveObject(pCam);
      SetDivinity(false, pObj);
    }
    SetCursor(GetPlayerByIndex(i), GetHiRank(GetPlayerByIndex(i)));
  }
}

global func FilmFadeDark()
{
	if(!g_pFader)
	{
		g_pFader = CreateObject(_CAM, 0, 0, -1);
		g_pFader->MakeFader(0);
	}
	g_pFader->SetFade(0);
}

global func FilmFadeOut()
{
	if(!g_pFader)
	{
		g_pFader = CreateObject(_CAM, 0, 0, -1);
		g_pFader->MakeFader(0);
	}
	g_pFader->DoFadeOut();
}

global func FilmFadeIn()
{
	if(!g_pFader)
	{
		g_pFader = CreateObject(_CAM, 0, 0, -1);
		g_pFader->MakeFader(1);
	}
	g_pFader->DoFadeIn();
}

func MakeFader(fDark)
{
	SetAction("AttachFader", g_pCamera);
	SetCategory(C4D_Foreground);
	SetObjDrawTransform(1000000,0,0,0,1000000, 0);
	if(fDark) iFading = 0;
	else iFading = 255;
	SetClrModulation(RGBa(255,255,255,iFading));
}

func SetFade(int iValue)
{
	iValue = BoundBy(iValue, 0, 255);
	SetClrModulation(RGBa(255,255,255,iValue));
}

static const g_iFadeTime = 140; // 4*35

func DoFadeOut()
{
	SetFade(255);
	for(var i = 1; i <= g_iFadeTime; i++)
    Schedule(Format("SetFade(%d)", 255-i*255/g_iFadeTime), i);
}

func DoFadeIn()
{
	SetFade(0);
	for(var i = 1; i <= g_iFadeTime; i++)
    Schedule(Format("SetFade(%d)", i*255/g_iFadeTime), i);
}

//global func FilmFadeOut() { return FadeViewOut(g_pCamera,1); }
//global func FilmFadeIn()  { return FadeViewIn(g_pCamera); }

global func FadeViewOut(pObj,fFilm)
{
  if(!pObj) pObj = this;
  // Nicht fï¿½r Scriptspieler
//  if(GetPlayerType(GetOwner(pObj))!=C4PT_User) return;
  var pCam = FindObject(_CAM, 0, 0, 0, 0, 0, 0, pObj);
  if(pCam) RemoveObject(pCam);
  for(var i = 1; i <= 15; i++)
    pObj->Schedule(Format("SetPlrViewRange(%d)", 150-i*10), i);
//  pObj->Schedule("FadeToZero()", 39);
}

global func FadeToZero(pObj)
{
  if(!pObj) pObj = this;
  // Nicht fï¿½r Scriptspieler
  if(GetPlayerType(GetOwner(pObj))!=C4PT_User) return;
  if(FindObject2(Find_ID(_CAM), Find_ActionTarget(pObj))) return;
  var pCam = CreateObject(_CAM, 0, 0, GetOwner(pObj));
  pCam->SetAction("Attach", pObj);
  pCam->SetCategory(C4D_Foreground);
  pCam->SetObjDrawTransform(1000000,0,500000,0,1000000, 500000);
  pCam->SetVisibility(VIS_Owner);
}

global func FadeViewIn(pObj)
{
  if(!pObj) pObj = this;
  if(pObj->~IsClonk()) return;
  // Nicht für Scriptspieler
//  if(GetPlayerType(GetOwner(pObj))!=C4PT_User) return;
  var pCam = FindObject2(Find_ID(_CAM), Find_ActionTarget(pObj));
  if(pCam) RemoveObject(pCam);
  for(var i = 12; i <= 15; i++)
    pObj->Schedule(Format("SetPlrViewRange(%d)", i*10), i-11);
}

public func AttachTargetLost()
{
  if(isCam) SetAction("Fly");
  else RemoveObject();
}


/* Protagonist */
// Sprecher/Hauptakteur auf Spielerseite

static g_pLastProtagonist;

// Protagonisten setzen
// Folgende Aufrufe von GetProtagonist geben bevorzugt dieses Objekt zurï¿½ck
global func SetProtagonist(object new_protagonist)
{
  g_pLastProtagonist = new_protagonist;
}

// Protagonisten ermitteln
// Bevorzugt vom angegebenem Typ; wenn es den nicht gibt auch einen anderen.
global func GetProtagonist(id preferred_type, bool forced_type)
{
  // Letzten Protagonisten lï¿½schen, wenn er keinem Spieler mehr gehï¿½rt
  if (g_pLastProtagonist)
  {
    if (GetPlayerType(GetOwner(g_pLastProtagonist)) != C4PT_User) g_pLastProtagonist = 0;
    if (!GetAlive(g_pLastProtagonist)) g_pLastProtagonist = 0;
  }
  // Ist ein Typ angegeben? Dann diesen Typ suchen
  if (preferred_type)
  {
    // Letzter passt? Na wunderbar!
    // Sonst suchen
    if (!g_pLastProtagonist || (GetID(g_pLastProtagonist) != preferred_type))
    {
      g_pLastProtagonist = FindObject2(Find_PlayerType(C4PT_User), Find_OCF(OCF_Alive), Find_OCF(OCF_CrewMember), Find_ID(preferred_type));
    }
  }
  // Kein Typ gewï¿½nscht oder noch keinen Protagonisten gefunden? Dann irgendeinen, wenn das nicht verboten ist
  if (!g_pLastProtagonist && !forced_type)
  {
    g_pLastProtagonist = FindObject2(Find_PlayerType(C4PT_User), Find_OCF(OCF_Alive), Find_OCF(OCF_CrewMember));
  }
  // Zurï¿½ckgeben wenn gefunden
  return g_pLastProtagonist;
}
