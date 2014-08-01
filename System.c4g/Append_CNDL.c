
#strict 2
#appendto CNDL

local pLight;
public func IsLightSwitch(){ return true;}

protected func ControlDigDouble()
{
  if(GetAction() == "On") return(SetLightOff());
  return SetLightOn();
}
/*
public func SetLightOn()
{
	pLight = CreateLight(LIGH, 150, LightColorCandle(), this);
	SetAction("On");
}

public func SetLightOff()
{
	if(pLight) RemoveObject(pLight);
	SetOff();
}
*/

public func SetLightOff()
{
	if(pLight) 	pLight->~TurnOff();
	//SetOff();
	SetAction("Off");
    if(!Par()) Sound("Pshshsh");
}

public func SetLightOn()
{
	//if(!pLight) pLight = AddLight(150, LightColorCandle(), this);
	if(!pLight)
	{
		AddVertex(0,-5);
		pLight = AddLight(150, LightColorCandle(), this);
		pLight -> SetActionData(256*2 + GetVertexNum() - 1);
		pLight->SetFlicker(true);
	}
	pLight->~TurnOn();
	SetAction("On");
}
