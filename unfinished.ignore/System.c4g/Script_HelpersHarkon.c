/* Helper functions from Story of Harkon@version 0.1.0@author Randrian*/
#strict 2

global func PoffClonk(pTarget)
{
	if (!pTarget)
		pTarget = this;
	// Funken und Rauch verwirren, wenn plötzlich einen Clonk ersetzend.
	Sound("Poff", 0, pTarget, 100);
	CastParticles
	(
		"PSpark",
		10,
		8,
		GetX(pTarget) - GetX(),
		GetY(pTarget) - GetY(),
		20,
		180,
		GetColorDw(pTarget),
		GetColorDw(pTarget)
	);
	CastParticles("FSpark", 3, 50, GetX(pTarget) - GetX(), GetY(pTarget) - GetY(), 20, 80);
	CastParticles("Smoke", 3, 50, GetX(pTarget) - GetX(), GetY(pTarget) - GetY(), 20, 180);
}

////////////////////////////////////////////
global func CallOnSeen(szString, pObj, pContext)
{
	if (!pObj)
		pObj = this;
	if (!pContext)
		pContext = pObj;
	if (!pObj)
		return;
	AddEffect("IntWaitToBeSeen", pObj, 1, 1, pObj, 0, szString, pContext);
}

global func FxIntWaitToBeSeenStart(pTarget, iNumber, fTmp, szString, pContext)
{
	if (fTmp)
		return;
	EffectVar(0, pTarget, iNumber) = szString;
	EffectVar(1, pTarget, iNumber) = pContext;
}

global func FxIntWaitToBeSeenTimer(pTarget, iNumber)
{
	if (FindObject2(Find_Distance(150), Find_PlayerType(C4PT_User), Find_OCF(OCF_CrewMember)) || FindObject2(Find_Distance(150), Find_ID(_SUL), Find_Action("")))
	{
		EffectVar(1, pTarget, iNumber)->Call(EffectVar(0, pTarget, iNumber));
		return -1;
	}
}
////////////////////////////////////////////

////////////////////////////////////////////
global func CallOnDeath(szString, pObj, pContext)
{
	if (!pObj)
		pObj = this;
	if (!pContext)
		pContext = pObj;
	if (!pObj)
		return;
	AddEffect("IntWaitToDie", pObj, 1, 0, pObj, 0, szString, pContext);
}

global func FxIntWaitToDieStart(pTarget, iNumber, fTmp, szString, pContext)
{
	if (fTmp)
		return;
	EffectVar(0, pTarget, iNumber) = szString;
	EffectVar(1, pTarget, iNumber) = pContext;
}

global func FxIntWaitToDieStop(pTarget, iNumber, iReason, fTmp)
{
	if (fTmp)
		return;
	if (iReason == 4)
		EffectVar(1, pTarget, iNumber)->Call(EffectVar(0, pTarget, iNumber), pTarget);
}
////////////////////////////////////////////

global func JumpUp()
{
	Jump();
	SetXDir(0);
	SetYDir(GetYDir() / 2);
	SetAction("Jump");
}

////////////////////////////////////////////
global func CallOnPlayerNear(szString, pObj, pContext, iRadius)
{
	if (!iRadius)
		iRadius = 50;
	if (!pObj)
		pObj = this;
	if (!pContext)
		pContext = pObj;
	if (!pObj)
		return;
	AddEffect("IntWaitToPlayerNear", pObj, 1, 1, pObj, 0, szString, pContext, iRadius);
}

global func FxIntWaitToPlayerNearStart(pTarget, iNumber, fTmp, szString, pContext, iRadius)
{
	if (fTmp)
		return;
	EffectVar(0, pTarget, iNumber) = szString;
	EffectVar(1, pTarget, iNumber) = pContext;
	EffectVar(2, pTarget, iNumber) = iRadius;
}

global func FxIntWaitToPlayerNearTimer(pTarget, iNumber)
{
	var obj = 0;
	if (obj = FindObject2
	(
		Find_Distance(EffectVar(2, pTarget, iNumber)),
		Find_PlayerType(C4PT_User),
		Find_OCF(OCF_CrewMember)
	))
	{
		EffectVar(1, pTarget, iNumber)->Call(EffectVar(0, pTarget, iNumber), obj);
		return -1;
	}
}
////////////////////////////////////////////
global func CallOnCondition(szString, pContext, szCondition)
{
	if (!pContext)
		pContext = this;
	if (!pContext)
		return;
	AddEffect("IntWaitForCondition", pContext, 1, 5, pContext, 0, szString, szCondition);
}
global func FxIntWaitForConditionStart(pTarget, iNumber, fTmp, szString, szCondition)
{
	if (fTmp)
		return;
	EffectVar(0, pTarget, iNumber) = szString;
	EffectVar(2, pTarget, iNumber) = szCondition;
}
global func FxIntWaitForConditionTimer(pTarget, iNumber)
{
	var obj = 0;
	var condition = Call(EffectVar(2, pTarget, iNumber));
	if (obj = condition)
	{
		Call(EffectVar(0, pTarget, iNumber), obj);
		return -1;
	}
}
////////////////////////////////////////////

global func LogPos()
{
	Log("%d, %d", GetX(), GetY());
}

// Richtung anpassen, so dass das Objekt zum anderen Objekt schaut
global func SetDirTo(object pTarget, object pThis)
{
	if (GetX(pTarget) > GetX(pThis))
		SetDir(DIR_Right, pThis);
	else if (GetX(pTarget) < GetX(pThis))
		SetDir(DIR_Left, pThis);
}

global func SetDirToPermanent(object pTarget, object pThis)
{
	RemoveEffect("IntSetDirTo", pThis);
	var iEff = AddEffect("IntSetDirTo", pThis, 1, 1, 0, 0);
	EffectVar(0, pThis, iEff) = pTarget;
}

global func RemoveDirToPermanent(object pThis)
{
	RemoveEffect("IntSetDirTo", pThis);
}

global func FxIntSetDirToTimer(pTarget, iEff)
{
	SetDirTo(EffectVar(0, pTarget, iEff), pTarget);
}
  
// Clonk anhalten
global func StopClonk(object pTarget)
{
	if (!pTarget)
		if (!this)
			return;
	SetComDir(COMD_Stop, pTarget);
	SetCommand(pTarget, "None");
	Call("~StopAiming");
}
  
// Crewobjekt nach Namen suchen - Spielerclonks ignorieren!
global func FindCrewByName(id idCrew, string sName)
{
	var pObj;
	while (pObj = FindObject(idCrew, 0, 0, 0, 0, 0, 0, 0, 0, pObj))
		if (pObj->GetName() == sName)
			if (GetOwner(pObj) == NO_OWNER)
				return pObj;
}
