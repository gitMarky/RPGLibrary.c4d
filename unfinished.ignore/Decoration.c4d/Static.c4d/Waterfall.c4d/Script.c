/* Drain */

#strict 2

local amount;
local drain;
local iMaterial;

static const g_WATERFALL_Material_Water = 1;
static const g_WATERFALL_Material_Acid = 2;
static const g_WATERFALL_Material_DuroLava = 3;
static const g_WATERFALL_Material_Oil = 4;

protected func Initialize()
{
	SetAction("Drain");
	amount = 2;
	iMaterial = 1;
	return 1;
}

public func UpdateTransferZone()
{
	if (GetAction() == "DrainLow")
		Sound("Water", 0, this, 100, 0, true, false, 150);
}

private func Draining()
{
	if (GBackSolid()) SetPosition(GetX(), GetY() - 1);
		
	var i;
	var mat;

	if (iMaterial == g_WATERFALL_Material_Water)	mat = Material("Water");
	if (iMaterial == g_WATERFALL_Material_Acid)		mat = Material("Acid");
	if (iMaterial == g_WATERFALL_Material_DuroLava) mat = Material("DuroLava");
	if (iMaterial == g_WATERFALL_Material_Oil)		mat = Material("Oil");
	
	if (!drain)
	{
		i = amount;
		while (i--) InsertMaterial(mat);
		return;
	}
	
	ExtractMaterialAmount(0, 0, mat, amount);
	
	if (GetAction() == "DrainLow")
		Sound("Water", 0, this, 100, 0, true, false, 150);
	return 1;
}
