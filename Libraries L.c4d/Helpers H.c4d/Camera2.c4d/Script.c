/*-- Kamera --*/

#strict 2
#include LF_R // kann auch Dialoge produzieren

static g_pCamera;
static g_pFader;

local iFading;
local isCam;

public func MenuQueryCancel(){ return 1; }

global func IsFilm() { return !!g_pCamera; }
global func GetFilm() { return g_pCamera; }
global func GetFilmFade()
{
	if(!g_pFader) return -1;
	return LocalN("iFading",g_pFader);
}

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
		var iCount = GetCrewCount(GetPlayerByIndex(i, C4PT_User));
		var pObj;
		var iPlr = GetPlayerByIndex(i, C4PT_User);
		ClearLastPlrCom(iPlr);
		while (iCount--)
		{
			pObj = GetCrew(iPlr, iCount);
			//SetCrewEnabled(0, pObj);
			SetPlrViewRange(0, pObj);
			SetDivinity(true, pObj);
			pObj->StopClonk();
		}
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

global func PosCam( vObjX, iY, bool bInsta) // iX, iY oder pObj
{
	if(!g_pCamera)
	{
		StartFilm();
		bInsta = true;
	}
	// Blu
		RemoveEffect("IntCamFollow", g_pCamera);
		RemoveEffect("IntCamAttach", g_pCamera);
	// Spielersicht setzen
	for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
		SetPlrView(GetPlayerByIndex(i, C4PT_User), g_pCamera);

	g_pCamera->SetAction("Fly");
	if (!vObjX)
	{
	// Leeres Objekt übergeben, d.h. kein Spieler da.
	return;
	}
	
	if(GetType(vObjX)==C4V_C4Object)
	{
		g_pCamera->SetCamTarget(vObjX);
		if(bInsta)
		{
			g_pCamera->SetPosition(vObjX->GetX(), vObjX->GetY());
			return g_pCamera->SetAction("Attach", vObjX);
		}
		AddEffect("IntCamFollow", g_pCamera, 1, 2, g_pCamera);
		SetCommand(g_pCamera, "Call", g_pCamera, vObjX, 0, 0, "AttachMe");
		return AddCommand(g_pCamera, "MoveTo", 0, GetX(vObjX), GetY(vObjX));
	}
	if(bInsta) return SetPosition(vObjX, iY, g_pCamera);
	SetCommand(g_pCamera, "Call", g_pCamera, 0, 0, 0, "StopMe");
	AddCommand(g_pCamera, "MoveTo", 0, vObjX, iY);
}

public func CrewSelection(bool fDeselect, bool fCursorOnly)
{
	if(!fDeselect) return;
	
	for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
	{
		var iPlr = GetPlayerByIndex(i, C4PT_User);
		if(GetCursor(iPlr) != g_pCamera)
			SetCursor(iPlr, g_pCamera);
	}
}



local pCamTarget;

func SetCamTarget(obj) { pCamTarget = obj; }

func FxIntCamFollowTimer(target, num)
{
	if(GetAction() != "Attach" && ObjectDistance(pCamTarget)>5)
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
	// Kamera entfernen
	RemoveObject(g_pCamera);
	if(g_pFader) g_pFader->RemoveObject();
	// Spieler vorbereiten
	var pCam;
	for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
	{
		var iCount = GetCrewCount(GetPlayerByIndex(i, C4PT_User));
		var pObj;
		while (iCount--)
		{
			pObj = GetCrew(GetPlayerByIndex(i, C4PT_User), iCount);
			//SetCrewEnabled(1, pObj);
			SetPlrViewRange(150, pObj);
			pCam = FindObject2(Find_ID(_CAM), Find_ActionTarget(pObj));
			if(pCam) RemoveObject(pCam);
			SetDivinity(false, pObj);
		}
		SetCursor(GetPlayerByIndex(i, C4PT_User), GetHiRank(GetPlayerByIndex(i, C4PT_User)));
	}
}

public func AttachTargetLost()
{
	if(isCam)
		SetAction("Fly");
	else
		RemoveObject();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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
	SetCategory(C4D_Object|C4D_Foreground|C4D_IgnoreFoW);
	SetObjDrawTransform(1000000,0,0,0,1000000, 0);
	if(fDark)
		SetFade(0);
	else 
		SetFade(255);
	//SetClrModulation(RGBa(255,255,255,iFading));
}

func SetFade(int iValue)
{
	iValue = BoundBy(iValue, 0, 255);
	SetClrModulation(RGBa(255,255,255,iValue));
	iFading = iValue;
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

global func FadeViewOut(pObj,fFilm)
{
	if(!pObj) pObj = this;

	var pCam = FindObject(_CAM, 0, 0, 0, 0, 0, 0, pObj);
	if(pCam) RemoveObject(pCam);
	for(var i = 1; i <= 15; i++)
		pObj->Schedule(Format("SetPlrViewRange(%d)", 150-i*10), i);

}

global func FadeToZero(pObj)
{
	if(!pObj) pObj = this;

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



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

global func ForceFlyStart( object pTarget, int iInterval )
{
	if( ForceFly( pTarget ) ) return;
	var fly = CreateObject(_CAM,0,0,-1);
	fly->~SetForceFly( pTarget, iInterval);
}

global func ForceFly( object pTarget )
{
	return FindObject2( Find_ID(_CAM), Find_Action("Fly"), Find_ActionTarget(pTarget) );
}

global func ForceFlyStop( object pTarget )
{
	var fly;
	while( fly = ForceFly(pTarget)) RemoveObject( fly );
}

global func ForceFlyTo( int iX, int iY, pTarget, int iInterval, int iMaxSpeed )
{
	var fly;
	if(!(fly=ForceFly(pTarget))) ForceFlyStart(pTarget, iInterval);
	if(!(fly=ForceFly(pTarget))) return;

	fly->~SetForceFlyTo(iX,iY);
	if(iMaxSpeed) SetPhysical("Float",iMaxSpeed*10,2,fly);
}

public func SetForceFly( object pTarget, int iInterval )
{
	SetCategory(C4D_Vehicle);
	SetPosition(GetX(pTarget),GetY(pTarget));
	pCamTarget = pTarget;
	SetAction("Fly",pTarget);
	AddEffect("ForceFly",this,50,1,this,0,iInterval);
}

public func SetForceFlyTo( int iX, int iY)
{
	SetCommand(this, "Call", this, 0, 0, 0, "StopMe");
	AddCommand(this, "MoveTo", 0, iX, iY);
}
func FxForceFlyStart(object pTarget, int iNumber, int iTemp, int iInterval)
{
	EffectVar(0,pTarget,iNumber) = iInterval;
	EffectVar(1,pTarget,iNumber) = 0;
}

func FxForceFlyTimer(object pTarget, int iEffectNumber, int iEffectTime)
{
	if(!pCamTarget) { RemoveObject(); return -1;}
	/*
	if(GetAction() != "Fly")
		return -1;
	if(Contained(pCamTarget) && Contained(pCamTarget) != GetActionTarget())
		SetAction("Fly", Contained(pCamTarget));
	else if(!Contained(pCamTarget) && pCamTarget != GetActionTarget())
		SetAction("Fly", pCamTarget);
	 */
	var dy = 0;

	if(!(iEffectTime%EffectVar(0,pTarget,iEffectNumber)))
	{
		EffectVar(1,pTarget,iEffectNumber) = (EffectVar(1,pTarget,iEffectNumber)+360/EffectVar(0,pTarget,iEffectNumber))%360;
	}

	if( EffectVar(0,pTarget,iEffectNumber))
		dy = Sin(EffectVar(1,pTarget,iEffectNumber),2);

	//Log("%d %d",GetX(),GetY());

	// Objekt schweben lassen
	SetPosition(GetX(),GetY()+dy,pCamTarget);
	pCamTarget->SetXDir();
	pCamTarget->SetYDir();
	// Rotation langsam bremsen
	if(iEffectTime%10) pCamTarget->SetRDir(GetRDir(pCamTarget)-BoundBy(GetRDir(pCamTarget),-1,1));
}
