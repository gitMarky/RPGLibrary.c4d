/*--
Candle holder that can be lit.

@title Chandelier
@author Sven2 or Randrian
@version 0.1.0 --*/

#strict 2

local filter_fall_targets;

local light_object1, light_object2;

protected func Initialize()
{
	SetLightOn();
	
	return true;
}

/**
 * Lets the chandelier fall down.
 */
public func SetOMG()
{
	RemoveEffect("IntChandelierFall", this);
	AddEffect("IntChandelierFall", this, 1, 51, this);
	filter_fall_targets = Find_And(Find_InRect(-35, 100, 70, 70), Find_OCF(OCF_Alive), Find_Not(Find_Owner(NO_OWNER)));
}
  
protected func FxIntChandelierFallTimer()
{
	// hit some targets
	if (FindObject2(filter_fall_targets))
	{
		Sound("TreeDown2");
		
		SetCategory(C4D_Structure);
		return FX_Execute_Kill;
	}
}

/**
 * Sets the chandelier up, such that it hangs at the current position. Falls down, if it is not placed in solid material.
 */  
public func SetHang()
{
	RemoveEffect("IntHang", this);
	AddEffect("IntHang", this, 1, 51, this);
}

protected func FxIntHangTimer()
{
	// falling down check
	if (!GBackSolid(0, -10))
	{
		Sound("TreeDown2");

		SetCategory(C4D_Structure);
		return FX_Execute_Kill;
	}
}

/**
 * Explodes upon impact with a strength of 40. Also casts some debris.
 */
protected func Hit()
{
	// Bam!
	// I do not know about this brick...
	//var brick;
	//if (brick = FindObject2(Find_ID(_BRK), Find_InRect(-35, 0, 70, 50)))
	//{
	//	Split2Components(brick);
	//}
	
	SetLightOff();
	
	Sound("Crystal1");
	Sound("Crystal2");
	Sound("Crystal3");
	Explode(40, CreateObject(ROCK, 0, 13, -1));
	Split2Components();
}

/**
 * Extinguishes the candles.
 */
public func SetLightOff()
{
	if (light_object1) light_object1->~TurnOff();
	if (light_object2) light_object2->~TurnOff();
	
	SetAction("Off");
	if (!Par())
		Sound("Pshshsh");
}

/**
 * Lights the candles.
 */
public func SetLightOn()
{
	SetAction("On");
	
	var light;
	
	AddVertex(-13, 0);
	light = AddLight(200, LightColorCandle(), this);
	light->SetActionData(256 * 2 + GetVertexNum() - 1);
	light->SetFlicker(true);
	light_object1 = light;
	
	AddVertex(+13, 0);
	light = AddLight(200, LightColorCandle(), this);
	light->SetActionData(256 * 2 + GetVertexNum() - 1);
	light->SetFlicker(true);
	light_object2 = light;
}