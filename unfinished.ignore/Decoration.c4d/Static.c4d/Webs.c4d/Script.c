/*--
Spooky decoration for caves.
@title Cobwebs
@author Unknown
@version 0.1.0 --*/

#strict 2

protected func Initialize()
{
	SetAction("Be");
	if (Random(3))
		SetCategory(C4D_Foreground);
	SetPhase(Random(5), 0);
	SetClrModulation(RGBa(255, 255, 255, 55 + RandomX(-7, 12)));
}

/**
 * Changes the appearance of the object.
 * @par phase Gets passed to SetPhase(). The animation has 5 phases.
 */
public func Set(int phase)
{
	SetPhase(phase);
}

