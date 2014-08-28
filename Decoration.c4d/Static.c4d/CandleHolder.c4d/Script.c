/* Chandelier */

#strict 2

local aFallTargetsFilter;

local pLight1, pLight2;

protected func Initialize()
{
	SetLightOn();
	
	return true;
}

public func SetOMG()
{
	RemoveEffect("IntChandelierFall", this);
	AddEffect("IntChandelierFall", this, 1, 51, this);
	aFallTargetsFilter = Find_And(Find_InRect(-35, 100, 70, 70), Find_OCF(OCF_Alive), Find_Not(Find_Owner(NO_OWNER)));
}
  
protected func FxIntChandelierFallTimer()
{
	// hit some targets
	if (FindObject2(aFallTargetsFilter))
	{
		Sound("TreeDown2");

		SetCategory(C4D_Structure);
		return FX_Execute_Kill;
	}
}

  
public func SetHang()
{
	RemoveEffect("IntHang", this);
	AddEffect("IntHang", this, 1, 51, this);
}

protected func FxIntHangTimer()
{
	// Runterfallcheck
	if (!GBackSolid(0, -10))
	{
		Sound("TreeDown2");
		//SetAction("Off");
		SetCategory(C4D_Structure);
		return FX_Execute_Kill;
	}
}

protected func Hit()
{
	// Rumms!
	var pBrick;
	if (pBrick = FindObject2(Find_ID(_BRK), Find_InRect(-35, 0, 70, 50)))
	{
		Split2Components(pBrick);
	}
	Sound("Crystal1");
	Sound("Crystal2");
	Sound("Crystal3");
	Explode(40, CreateObject(CNDL, 0, 13, -1));
	Split2Components();
}

public func SetLightOff()
{
	if (pLight1)
		pLight1->~TurnOff();
	if (pLight2)
		pLight2->~TurnOff();
	SetAction("Off");
	if (!Par())
		Sound("Pshshsh");
}

public func SetLightOn()
{
	SetAction("On");
	
	var pLight;
	
	AddVertex(-13, 0);
	pLight = AddLight(200, LightColorCandle(), this);
	pLight->SetActionData(256 * 2 + GetVertexNum() - 1);
	pLight->SetFlicker(true);
	pLight1 = pLight;
	
	AddVertex(+13, 0);
	pLight = AddLight(200, LightColorCandle(), this);
	pLight->SetActionData(256 * 2 + GetVertexNum() - 1);
	pLight->SetFlicker(true);
	pLight2 = pLight;
}