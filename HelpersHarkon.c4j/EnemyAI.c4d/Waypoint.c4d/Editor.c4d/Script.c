/*-- Pfad - Visualisierung --*/

#strict 2

local lastPos;
local aTools;
local iMode;
local iTool;
local pLastObj;
local pTestObject;
local iTimer;
local iReconnect;

local pMaster;
local iToolID;

func Initialize()
{
	// Bin ich der erste?
	if(!FindObject(GetID()))
	{
		SetAction("Master");
		SetPosition(16,16);
		aTools = [0,0,0,0,0,0];
		for(var i = 0; i < 9; i++)
		{
			aTools[i] = CreateObject(GetID(), 26+35*i, 26, -1);
			aTools[i]->SetAction("Tool");
			aTools[i]->SetDir(0);
			aTools[i]->SetPhase(i);
			aTools[i]->LocalN("pMaster") = this;
			aTools[i]->LocalN("iToolID") = i;
		}
		aTools[0]->SetDir(1);
		iMode = -1;
		SetShape();
		if(GetPlayerCount(C4PT_User))
		{
			var iPlr = GetPlayerByIndex(0, C4PT_User);
			SetOwner(iPlr);
			MakeCrewMember(this, iPlr);
			SetFoW(0, iPlr);
			SetCursor(iPlr, this);
		}
	}
}

func ControlCommand(szCommand, pTarget, iX, iY)
{
	if(pTarget)
	{
		iX = GetX(pTarget);
		iY = GetY(pTarget);
	}
	if(iCreate)
	{
		var wayp = CreateObject(WAYP, iX, iY, -1);
		SetPlrView(GetOwner(), wayp);
		return;
	}
	if(pLastObj)
	{
		pLastObj->SetPosition(iX, iY);
		SetPlrView(GetOwner(), pLastObj);
	}
}

func UpdateTool()
{
	iMode = [-1, Path_MoveTo, Path_Jump, Path_Lift, Path_Door, Path_Stairs, Path_Fly, Path_Remove][iTool];
	for(var i = 0; i < 8; i++)
		aTools[i]->SetDir(0);
	aTools[iTool]->SetDir(1);
	iCreate = 0;
}

func ControlLeft()
{
	iTool--;
	if(iTool == -1) iTool = 7;
	UpdateTool();
	if(lastPos) SetPlrView(GetOwner(), lastPos);
}

func ControlRight()
{
	iTool++;
	if(iTool > 7 ) iTool = 0;
	UpdateTool();
	if(lastPos) SetPlrView(GetOwner(), lastPos);
}

func ControlThrow()
{
	iReconnect = !iReconnect;
	aTools[8]->SetDir(iReconnect);
	if(lastPos) SetPlrView(GetOwner(), lastPos);
}

func ControlDown()
{
	if(pLastObj)
		pLastObj->SetDir(0);
	pLastObj = 0;
	if(lastPos) SetPlrView(GetOwner(), lastPos);
}

func ControlSpecial()
{
	if(pLastObj)
		pLastObj->RemoveObject();
	if(lastPos) SetPlrView(GetOwner(), lastPos);
}

local iCreate;

func ControlDig()
{
	if(pLastObj)
		pLastObj->SetDir(0);
	pLastObj = 0;
	iCreate = !iCreate;
	if(lastPos) SetPlrView(GetOwner(), lastPos);
}

func MouseSelection()
{
	if(!pMaster) return;
	pMaster->SetTool(iToolID);
}

func Selection(pTarget)
{
	if(pMaster) return pMaster->Selection(pTarget);
	lastPos = pTarget;
	if(GetID(pLastObj) == WAYP)
		pLastObj->SetDir(0);
	if(pLastObj == pTarget)
	{
		pLastObj = 0;
		return;
	}
	pTarget->SetDir(1);
	
	if(GetID(pTarget) == WAYP)
	{
		pTarget->SetDir(1);
		if(GetID(pLastObj) == WAYP && pLastObj != pTarget && iMode != -1)
			pLastObj->AddPath(pTarget, iMode, 0, iReconnect);
	}
	
	pLastObj = pTarget;
}

func SetTool(iToolID)
{
	if(pLastObj)
		pLastObj->SetDir(0);
	pLastObj = 0;

	if(iToolID == 8)
	{
		iReconnect = !iReconnect;
		aTools[iToolID]->SetDir(iReconnect);
		return;
	}
	else
	{
		iTool = iToolID;
		UpdateTool();
	}
}

func Timer()
{return;
	var obj = EditCursor();
	if(!obj) { pTestObject = 0; return; }
	if(pTestObject != obj)
	{
		pTestObject = obj;
		iTimer = 15;
		return;
	}
	if(iTimer > 0)
		return iTimer--;
	if(GetID(obj) == GetID())
	{
		for(var i = 0; i < 8; i++)
			aTools[i]->SetDir(0);
		obj->SetDir(1);
		if(obj->GetPhase() == 8)
		{
			iReconnect = !iReconnect;
			obj->SetDir(iReconnect);
			Log("Reconnect: %d", iReconnect);
			return;
		}
		else iMode = [-1, Path_MoveTo, Path_Jump, Path_Lift, Path_Door, Path_Stairs, Path_Fly, Path_Remove][obj->GetPhase()];
	}
	if(GetID(pLastObj) == WAYP)
		pLastObj->SetDir(0);
	if(GetID(obj) == WAYP)
	{
		obj->SetDir(1);
		if(GetID(pLastObj) == WAYP && pLastObj != obj && iMode != -1)
			pLastObj->AddPath(obj, iMode, 0, iReconnect);
	}
	pLastObj = obj;
}
