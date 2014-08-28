/*-- Floor switch --*/

#strict 2

static const _FLS_NeededWeight = 20;
static const _FLS_DownPos      = 4;

local iYPos;
local idSpezial; // only items with matching ID are counted
local iNeededWeight;
local idSuper; // Super ID which handles the floor switch with an object
local iYPosExtra; // additional lower position

protected func Timer10()
{
	// Calculate position by current weight
	var iWgt, obj;
	while (obj = FindObject(idSpezial, -25, -30, 50, 30, 0, 0, 0, NoContainer(), obj))
		if (obj->GetContact(0, -1, 8))
			if (~obj->GetCategory() & C4D_StaticBack)
				iWgt += obj->GetMass();
	if (idSuper)
		if (obj = FindObject(idSuper, -25, -30, 50, 30, 0, 0, 0, NoContainer()))
			if (obj->GetContact(0, -1, 8))
				if (~obj->GetCategory() & C4D_StaticBack)
					iWgt += iNeededWeight + _FLS_NeededWeight;
	var iDesiredY;
	
	// Check weight, either use own value or default value
	if (iNeededWeight)
	{
		if (iWgt >= iNeededWeight)
			iDesiredY = _FLS_DownPos + iYPosExtra;
	}
	else if (iWgt >= _FLS_NeededWeight)
		iDesiredY = _FLS_DownPos + iYPosExtra;
	
	// At desired position?
	if (iDesiredY == iYPos)
		return;
	
	// No target? "stuck"!
	if (!Local())
	{
		iYPos = iDesiredY;
		Sound("ArrowHit");
		Message("$MsgStuck$", this);
		return 1;
	}
	// Sound when the switch starts moving
	if (!iYPos || iYPos == _FLS_DownPos)
		Sound("Elevator");
	
	// Move up/down
	iDesiredY = BoundBy(iDesiredY - iYPos, -1, 1);
	
	while (obj = FindObject(0, -25, -30, 50, 30, 0, 0, 0, NoContainer(), obj))
		if (obj->GetContact(0, -1, 8))
			if (~obj->GetCategory() & C4D_StaticBack)
				obj->SetPosition(obj->GetX(), obj->GetY() + iDesiredY);
	
	SetPosition(GetX(), GetY() + iDesiredY);
	iYPos += iDesiredY;
	
	// Notify connected objects
	var i;
	while (obj = Local(i++))
		if (iDesiredY * Local(i++) > 0)
			obj->ControlDown(this);
		else
			obj->ControlUp(this);
	
	// Sound when stopping
	if (!iYPos || iYPos == _FLS_DownPos)
		Sound("Chuff");
}

public func AddTarget(object pTarget, int iDir)
{
	if (!iDir)
		iDir = 1;
	var i;
	while (Local(i))
		++ ++i;
	Local(i++) = pTarget;
	Local(i) = iDir;
}

public func IsSwitch()
{
	return true;
}
