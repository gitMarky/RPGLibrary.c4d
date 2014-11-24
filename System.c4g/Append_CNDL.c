/* Added the light system to the candle */

#strict 2
#appendto CNDL

local pLight;
public func IsLightSwitch()
{
	return true;
}

protected func ControlDigDouble()
{
	if (GetAction() == "On")
		return SetLightOff();
	return SetLightOn();
}

public func SetLightOff()
{
	if (pLight)
		pLight->~TurnOff();
	
	SetAction("Off");
	if (!Par())
		Sound("Pshshsh");
}

public func SetLightOn()
{
	if (!pLight)
	{
		AddVertex(0, -5);
		pLight = AddLight(150, LightColorCandle(), this);
		pLight->SetActionData(256 * 2 + GetVertexNum() - 1);
		pLight->SetFlicker(true);
	}
	
	pLight->~TurnOn();
	SetAction("On");
}
