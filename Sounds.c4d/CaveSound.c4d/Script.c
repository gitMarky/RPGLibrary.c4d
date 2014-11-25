/* Höhlen Ambiente */

#strict 2

protected func Initialize()
{
  Start();
}

public func Start()
{
  if(GetAction() == "Drip") return;
  SetAction("Drip");
  Sound("CaveAmbient", 1, 0, 0, 0, 1);
}

public func Stop()
{
  if(GetAction() != "Drip") return;
  SetAction("Drip");
  Sound("CaveAmbient", 1, 0, 0, 0, 1);
}

protected func MakeANoise() 
{
  Sound("CaveDrip*", 1, 0, Random(100));
}
