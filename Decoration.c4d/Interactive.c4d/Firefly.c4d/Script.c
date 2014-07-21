/*-- Zap --*/

#strict 2

local attractionObj;

protected func Initialize()
{
	SetAction("Fly");
	SetPhase(Random(6));

	var lightColour = RGBa(220, 255, 180, 110);
	var light = AddLightAmbience(RandomX(8,15),this);

	var color = light->ChangeColor(lightColour);

	FadeInNew(this, 0, false);
}

private func Flying()
{
	var xdir,ydir;

	var fleeFromObj = FindObject2(Find_Distance(50), Find_Category(C4D_Object), Find_OCF(OCF_HitSpeed1), Find_NoContainer());
	if(fleeFromObj != 0)
	{
		xdir = BoundBy(GetX() - GetX(fleeFromObj), -1, 1);
		ydir = BoundBy(GetY() - GetY(fleeFromObj), -1, 1);
		if(xdir == 0) xdir = Random(2) * 2 - 1;
		if(ydir == 0) ydir = Random(2) * 2 - 1;
		xdir = RandomX(5 * xdir, 10 * xdir);
		ydir = RandomX(5 * ydir, 10 * ydir);

		SetSpeed(xdir, ydir);

		return;
	}

	if(Random(4)) return;

	xdir = Random(13) - 6;
	ydir = Random(9) - 4;

	var dest_x = GetX(attractionObj);
	var dest_y = GetY(attractionObj)-15;

	if(attractionObj != 0 && Distance(dest_x, dest_y, GetX(),GetY()) > RoamingDistance())
	{
		xdir = BoundBy(dest_x - GetX(), -1, 1);
		ydir = BoundBy(dest_y - GetY(), -1, 1);
		xdir = RandomX(xdir, 6 * xdir);
		ydir = RandomX(ydir, 6 * ydir);
	}

	SetSpeed(xdir,ydir);
}

private func RoamingDistance()
{
	return 30;
}

protected func Check()
{
	// Verschüttet oder im Wasser?: Sterben
	if(GBackSemiSolid()) return Death();

	// Tod durch Temperatur
	TemperatureCheck();

	if(!attractionObj) attractionObj = FindObject2(Find_Distance(RoamingDistance()*10), Find_Func("AttractsFireflies"));
	if(!attractionObj) return Death();
}

private func TemperatureCheck()
{
	var temperature = GetTemperature();
	if(temperature <= 10 && !Random(3)) return Death();
	if(temperature <= 0) Death();
}

public func Dawn()
{
		FadeOutNew(this,255,false,Random(3));
}

//
//public func UpdateTime(hours, hoursOld)
//{
//	if(IsDawn())
//	{
//		FadeOutNew(this,255,false,Random(3));
//	}
//}

public func CatchBlow() { RemoveObject(); }
public func Damage()	{ RemoveObject(); }
protected func Death()	{ RemoveObject(); }
