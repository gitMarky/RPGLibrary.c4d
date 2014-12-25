#strict 2

local alpha;

public func FadeMeOut()
{
	SetClrModulation(RGBa(255, 255, 255, alpha++));
	if (alpha == 255)
		return RemoveObject();
	return true;
}

public func Hit()
{
	return Sound("RockHit*");
}