/* Twines */

#strict 2

protected func Initialize()
{
	SetAction("Be");
	if (Random(3))
		SetCategory(C4D_Foreground);
	SetPhase(Random(5), 0);
}
