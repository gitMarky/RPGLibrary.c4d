/*-- Added the light system to the torch --*/

#strict 2
#appendto TORC

local pLight;
public func IsLightSwitch()
{
	return true;
}

public func Activate()
{
	if (GetAction() == "Burning")
		return SetLightOff();
	SetLightOn();
	return true;
}
  
public func SetLightOff()
{
	if (pLight)
		pLight->~TurnOff();
	SetAction("Idle");
	if (!Par())
		Sound("Pshshsh");
}

public func SetLightOn()
{
	if (!pLight)
	{
		AddVertex(0, -10);
		pLight = AddLight(150, LightColorCandle(), this);
		pLight->SetActionData(256 * 2 + GetVertexNum() - 1);
		pLight->SetFlicker(true);
	}
	pLight->~TurnOn();
	
	SetAction("Burning");
}
