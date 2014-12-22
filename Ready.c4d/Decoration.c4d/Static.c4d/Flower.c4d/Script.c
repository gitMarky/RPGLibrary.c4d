/*-- A decorative flower.
@author Knight Settlers Pack
@verion 0.2.0
--*/

#strict 2

protected func Initialize()
{
	// Random graphics
	SetAction("Flower");
	SetPhase(Random(2));
	if (!Random(20))
		SetPhase(2);
	// Random direction
	if (Random(2))
		SetDir(DIR_Right);
	// Rotate according to surface
	var x_off = 18 * GetCon() / 100;
	var y_off = 15 * GetCon() / 100;
	var slope = GetSolidOffset(-x_off, y_off) - GetSolidOffset(x_off, y_off);
	SetR(slope);
	// Adjust height
	while (!GBackSolid(0, 5))
		SetPosition(GetX(), GetY() + 1);
	// stays behind trees
	MoveBehindTrees();
}

private func GetSolidOffset(int x, int y)
{
	var i;
	for (i = -15; GBackSolid(x, y - i) && (i < 15); i++);
	return i;
}

private func MoveBehindTrees()
{
	var obj;
	while (obj = FindObject(0, 1, 1, 0, 0, OCF_Chop, 0, 0, NoContainer(), obj))
		if (obj->~IsTree() && (obj->GetCategory() & C4D_StaticBack))
			SetObjectOrder(obj, this, 1);
}

public func OnShockwaveHit(level, x, y)
{
	var con = (40 * GetCon()) / 100;
	level = 40 + level / 2;
	for (var cnt = 0; cnt < 15 + Random(10); cnt++)
		CreateParticle
		(
			"GrassBlade",
			RandomX(-con / 2, con / 2),
			-1,
			RandomX(-level / 3, level / 3),
			RandomX(-2 * level / 3, -level / 3),
			30 + Random(30),
			RGB(255, 255, 255),
			0,
			0
		);
	return RemoveObject();
}

protected func CheckEarth()
{
	if (GetMaterial(0, 10) != Material("Earth"))
		DoCon(-10, this);
	return true;
}

public func CanBeHitByShockwaves()
{
	return true;
}
public func BlastObjectsShockwaveCheck()
{
	return true;
}
