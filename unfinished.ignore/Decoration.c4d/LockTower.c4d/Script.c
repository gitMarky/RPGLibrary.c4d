/*-- Wachturm-Tor --*/

#strict 2

/* Initialisierung */

protected func Initialize()
{
	SetAction("Be");
  return 1;
}

func SetDir(iDir)
{
	if(iDir == 1)
		SetSolidMask(15,74,15,74);
	else
		SetSolidMask(15,0,15,74);
	return _inherited(iDir);
}