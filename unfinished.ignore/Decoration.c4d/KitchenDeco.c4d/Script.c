#strict 2

protected func Initialize()
  {
  SetAction("Exist");
	SetDir(Random(2));
	SetPhase(Random(4));
  return 1;
  }