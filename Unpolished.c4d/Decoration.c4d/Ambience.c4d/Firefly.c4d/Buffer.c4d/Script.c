/*-- Generates fireflies when it starts getting dark.
@title Firefly generator
@author Marky
@version 0.1.0
 --*/

#strict 2

public func Dusk()
{
	if (GetTemperature() > gFirefly_Min_Temperature)
	{
		var count = RandomX(3, 10);
		var dist = 30;
		
		for (var i = 0; i < count; i++)
		{
			var firefly = CreateObject(FFLY, RandomX(-dist, dist), RandomX(-dist, dist), -1);
			firefly->LocalN("attraction_object") = this;
		}
	}
}

public func AttractsFireflies()
{
	return true;
}
