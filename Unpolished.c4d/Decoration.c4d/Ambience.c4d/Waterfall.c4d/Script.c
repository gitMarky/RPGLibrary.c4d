/* Drain */

#strict 2

local last_action;
local radius;
local amount;
local drain;
local material_index;
local vel_x, vel_y;

global func PlaceWaterfall(int x_top, int y_top, int x_bottom, int y_bottom, int material, int amount, int radius, int x_dir, int y_dir)
{
	var fall = CreateObject(WFL_, 0, 0, NO_OWNER);
	var drainer = CreateObject(WFL_, 0, 0, NO_OWNER);
	
	if (!material) material = Material("Water");
	if (!amount) amount = 2;
	
	fall->SetPosition(x_top, y_top);
	fall->SetAction("Drain");
	fall->LocalN("amount") = amount;
	fall->LocalN("material_index") = material;
	fall->LocalN("radius") = radius;
	fall->LocalN("vel_x") = x_dir;
	fall->LocalN("vel_y") = y_dir;
	
	drainer->SetPosition(x_bottom, y_bottom);
	drainer->SetAction("DrainLow");
	drainer->LocalN("amount") = amount * 2;
	drainer->LocalN("material_index") = material;
	drainer->LocalN("drain") = true;
	
	return [fall, drainer];
}

protected func Initialize()
{
	SetAction("Drain");
	amount = 2;
	material_index = Material("Water");
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
	if (!drain)
	{
		i = amount;
		while (i--) InsertMaterial(material_index, RandomX(-radius, +radius), 0, vel_x, vel_y);
		return;
	}
	
	ExtractMaterialAmount(0, 0, material_index, amount);
	
	if (GetAction() == "DrainLow")
		Sound("Water", 0, this, 100, 0, true, false, 150);
}

public func SwitchOff()
{
	last_action = GetAction();
	SetAction("Idle");
}

public func SwitchOn()
{
	if (last_action)
	{
		SetAction(last_action);
	}
}
