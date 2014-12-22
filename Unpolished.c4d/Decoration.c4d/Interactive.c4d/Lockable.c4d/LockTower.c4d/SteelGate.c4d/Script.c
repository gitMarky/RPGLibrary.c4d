/*-- Wachturm-Tor --*/

#strict 2

/* Initialisierung */

protected func Initialize()
{
	SetAction("Gate");
  SetComDir(COMD_Up);
  return 1;
}

/* Steuerung */

public func Activate()
{
  Sound("Click");
  Sound("SteelGate1");
  if (GetComDir() == COMD_Up)  return CloseMe();
  return OpenMe();
}

protected func CloseMe(pClonk, fKi)
{
  SetComDir(COMD_Down);
}

protected func OpenMe(pClonk, fKi)
{
  SetComDir(COMD_Up);
}