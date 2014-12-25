/*-- Blows dangerous materials.
@title Fire trap
@author Stephan Breer (Die Pyramide von Clucatan)
@version 0.2.0
--*/

#strict 2

local blow_poison;

public func ActivateAlternating()
{
	SetAction("Fire");
}

public func ActivateOnce()
{
	SetAction("SingleBlow");
}

public func ActivatePermanently()
{
	SetAction("Permanent");
}

public func Deactive()
{
	SetAction("Idle");
}

public func BlowPoison(bool poison)
{
	blow_poison = poison;
}

private func Flame()
{
	var radius_origin = 2;
	var radius_velocity = 25;

	var x_origin = +Sin(GetR(), radius_origin);
	var y_origin = -Cos(GetR(), radius_origin);
	
	var x_vel = +Sin(GetR(), radius_velocity);
	var y_vel = -Cos(GetR(), radius_velocity);
	
	var color;
	
	if (blow_poison)
	{
		color = RGB(0, RandomX(128, 255), 0);
	}
	else
	{
		color = RGB(RandomX(128, 255), RandomX(96, 191), 0);
	}
	
	CreateParticle("Blast", x_origin, y_origin, x_vel, y_vel, RandomX(80, 120), color);
	
	
	var targets = FindObjects(Find_Distance(60), Find_OCF(OCF_Alive | OCF_Living), Find_NoContainer(), Sort_Distance());
	
	for (var target in targets)
	{
		if (Abs(Angle(GetX(), GetY(), target->GetX(), target->GetY()) - GetR()) < 5)
		{
			if (blow_poison)
			{
				DoEnergy(-2, target);
			}
			else
			{
				Incinerate(target);
			}
		}
	}
}

private func FireSound()
{
		Sound("BlowTorch", 0, this, 50);
}
