/*--
Appears at night and moves around a certain point. Reacts to thrown objects.

@title Firefly
@author Marky
@version 0.1.0
--*/

#strict 2

static const gFirefly_Min_Temperature = 10;

local attraction_object;

protected func Initialize()
{
	SetAction("Fly");
	SetPhase(Random(6));
	
	var light_olour = RGBa(220, 255, 180, 110);
	var light = AddLightAmbience(RandomX(8, 15), this);
	
	light->ChangeColor(light_olour);
	
	FadeInNew(this, 0, false);
}

private func Flying()
{
	var xdir, ydir;
	
	var fleeFromObj = FindObject2( Find_Distance(50), Find_Category(C4D_Object), Find_OCF(OCF_HitSpeed1), Find_NoContainer());
	if (fleeFromObj != 0)
	{
		xdir = BoundBy(GetX() - GetX(fleeFromObj), -1, 1);
		ydir = BoundBy(GetY() - GetY(fleeFromObj), -1, 1);
		if (xdir == 0)
			xdir = Random(2) * 2 - 1;
		if (ydir == 0)
			ydir = Random(2) * 2 - 1;
		xdir = RandomX(5 * xdir, 10 * xdir);
		ydir = RandomX(5 * ydir, 10 * ydir);
		
		SetSpeed(xdir, ydir);
		
		return;
	}
	
	if (Random(4)) return;
	
	xdir = Random(13) - 6;
	ydir = Random(9) - 4;
	
	var dest_x = GetX(attraction_object);
	var dest_y = GetY(attraction_object) - 15;
	
	if (attraction_object != 0 && Distance(dest_x, dest_y, GetX(), GetY()) > RoamingDistance())
	{
		xdir = BoundBy(dest_x - GetX(), -1, 1);
		ydir = BoundBy(dest_y - GetY(), -1, 1);
		xdir = RandomX(xdir, 6 * xdir);
		ydir = RandomX(ydir, 6 * ydir);
	}
	
	SetSpeed(xdir, ydir);
}

/**
 * The firefly should not move away from its attraction point further than this distance.
 * @return int The radius of the movement circle around the attraction point. Default value is 30.
 * @note Overload this function if you want the fireflies to move further or not so far away from their attraction point.
 */
private func RoamingDistance()
{
	return 30;
}

protected func Check()
{
	// Die if you are stuck or in water
	if (GBackSemiSolid()) return Death();
	
	// Die if the temperature is too low
	TemperatureCheck();
	
	if (!attraction_object) attraction_object = FindObject2(Find_Distance(RoamingDistance() * 10), Find_Func("AttractsFireflies"));
	if (!attraction_object) return Death();
}

/**
 * The firefly dies if the temperature is below 10. Overload this function if you do not want the firefly to die from low temperature.
 */
private func TemperatureCheck()
{
	var temperature = GetTemperature();
	if (temperature <= gFirefly_Min_Temperature && !Random(3)) return Death();
	if (temperature <= Min(gFirefly_Min_Temperature, 0)) Death();
}

public func Dawn()
{
	FadeOutNew(this, 255, false, Random(3));
}

public func CatchBlow()
{
	RemoveObject();
}
public func Damage()
{
	RemoveObject();
}
protected func Death()
{
	RemoveObject();
}
