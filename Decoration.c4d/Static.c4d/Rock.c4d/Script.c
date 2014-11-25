/* Big Rock */

#strict 2

protected func Initialize()
{
	SetP(Random(6));
}

protected func SetP(int phase)
{
	SetAction("Standard");
	SetPhase(phase);
	SetSolidMask(phase * 32, 0, 32, 32);
}