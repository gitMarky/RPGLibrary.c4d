/*-- Light --*/

#strict 2

local iColor, bAmbience, iF;
local bFlicker;


//Licht initialisieren, Parameter setzen, etc.
protected func Init(int size, int color, object target, string graphics)
{
	//if (bAmbience && !IsDark())
	if (!bAmbience && !IsDark())
		Schedule("RemoveObject()", 1, 0, this);

	// default values if parameters are missing
	if (!size)
		size = GetCon();
	if (!target)
		target = GetActionTarget();
	
	SetOwner(GetController(target));
	
	// set values
	SetAction("Attach", target);
	SetPosition(GetX(target), GetY(target));
	ChangeColor(color);
	ChangeSize(size);
	ChangeGraphics(graphics);
}

// lost the target -> remove it.
protected func AttachTargetLost()
{
	RemoveObject();
}

public func TurnOff()
{
	SetVisibility(VIS_None);
}

public func TurnOn()
{
	SetVisibility(VIS_All);
}

public func ChangeSize(int new_size)
{
	var alphamod, sizemod;
	CalcLight(alphamod, sizemod);
	
	iF = new_size;
	SetCon(iF * sizemod / 100);
}

// Licht updaten... verändert keine Werte sondern
// passt die Anzeige nur an
public func Update()
{
	ChangeSize(iF);
	ChangeColor(iColor);
}

//Farbe des Lichts ändern
public func ChangeColor(int new_color)
{
	
	iColor = new_color;
	
	if (!IsDark())
	{
		// hide ambient lighs
		if (bAmbience)
			new_color = RGBa(255, 255, 255, 255);
	}
	
	var alphamod, sizemod;
	CalcLight(alphamod, sizemod);
	
	var r, g, b, a;
	SplitRGBaValue(iColor, r, g, b, a);
	new_color = RGBa(r, g, b, Min(a + 60 + alphamod, 255));
	
	SetClrModulation(new_color);
	return new_color;
}

public func ChangeGraphics(string new_graphics)
{
	SetGraphics(new_graphics, 0, GetID());
}

public func IsLight()
{
	return true;
}

/*  Global functions for creating lights --*/

/**
 Creates a light object, with custom ID and graphics.
 @param ID The ID of the light object.
 @param size [optional] The size of the light object. Rough estimate of the radius in pixels.
 @param color The light color.
 @param target [optional] The light attaches to this object.
 @param graphics Custom graphics for the light.
 @link AddLight , AddLightHalf , AddLightCone , AddLightAmbience 
*/
global func CreateLight(id ID, int size, int color, object target, string graphics)
{
	var light = CreateObject(ID, 0, 0, GetController(target));
	light->Init(size, color, target, graphics);
	return light;
}

/**
 Creates a default round light object.
 @param size [optional] The size of the light object. Rough estimate of the radius in pixels.
 @param color The light color.
 @param target [optional] The light attaches to this object.
 @link CreateLight , AddLightHalf , AddLightCone , AddLightAmbience , AddLightPos
*/
global func AddLight(int size, int color, object target)
{
	if (!target)
		if (!(target = this))
			return;
			
	return CreateLight(LIGH, size, color, target);
}

/**
 Creates a default semi-circle light object.
 @param size [optional] The size of the light object. Rough estimate of the radius in pixels.
 @param color The light color.
 @param target [optional] The light attaches to this object.
 @link CreateLight , AddLight, AddLightCone, AddLightAmbience , AddLightPos
*/
global func AddLightHalf(int size, int color, object target)
{
	if (!target)
		if (!(target = this))
			return;
	return CreateLight(LIGH, size, color, target, "Half");
}

/**
 Creates a default light cone, similar to a flash light.
 @param size [optional] The size of the light object. Rough estimate of the length in pixels.
 @param color The light color.
 @param target [optional] The light attaches to this object.
 @link CreateLight , AddLight, AddLightHalf, AddLightAmbience , AddLightPos
*/
global func AddLightCone(int size, int color, object target)
{
	if (!target)
		if (!(target = this))
			return;
	return CreateLight(LGHC, size, color, target);
}

/**
 Creates an ambient light object, which is not deleted if the darkness is removed.
 @param size [optional] The size of the light object. Rough estimate of the radius in pixels.
 @param target [optional] The light attaches to this object.
 @param graphics Custom graphics for the light.
 @link CreateLight, AddLight , AddLightHalf , AddLightCone , AddLightPos
*/
global func AddLightAmbience(int size, object target, string graphics)
{
	if (!target)
		if (!(target = this))
			return;
	
	var light = CreateObject(LIGH, 0, 0, GetController(target));
	light->LocalN("bAmbience") = true;
	light->Init(size * 5, RGBa(255, 255, 255, 50), target, graphics);
	return light;
}

/**
 Creates a default round light particle that flashes once.
 @param size [optional] The size of the light object. Rough estimate of the radius in pixels.
 @param x The x-coordinate, local coordinates in object context.
 @param y The y-coordinate, local coordinates in object context.
 @param color The light color.
*/
global func AddLightFlash(int size, int x, int y, int color)
{
	var alphamod, sizemod;
	CalcLight(alphamod, sizemod);
	
	var r, g, b, a;
	SplitRGBaValue(color, r, g, b, a);
	color = RGBa(r, g, b, Min(a + alphamod, 255));
	
	CreateParticle("LightFlash", x, y, 0, 0, size * 5 * sizemod / 100, color);
}

/**
 Creates a default round light object at a given position.
 @param size [optional] The size of the light object. Rough estimate of the radius in pixels.
 @param x The absolute x-coordinate in the landscape.
 @param y The absolute y-coordinate in the landscape.
 @param color The light color.
 @link CreateLight, AddLight , AddLightHalf , AddLightCone , AddLightAmbience 
*/
global func AddLightPos(int size, int x, int y, color)
{
	var obj = CreateObject(TIM1, 0, 0, -1);
	SetPosition(x, y, obj);
	var light = AddLight(size, color, obj);
	
	if (!light)
		if (obj)
			RemoveObject(obj);
	return light;
}

/**
 Changes the flickering of the light.
 @par do_flicker A value of true means, that the light should flicker.
 */
public func SetFlicker(bool do_flicker)
{
	if (do_flicker)
	{
		if (!GetEffect("LightPulse", this))
			AddEffect("LightPulse", this, 50, 1, this);
	}
	else
	{
		var iNr = GetEffect("LightPulse", this);
		if (iNr)
			RemoveEffect("LightPulse", this, iNr);
		SetObjDrawTransform(1000, 0, 0, 0, 1000, 0);
	}
	return bFlicker = do_flicker;
}

protected func FxLightPulseTimer(object pTarget, int iEffectNumber, int iEffectTime)
{
	var iHeight = EffectVar(0, pTarget, iEffectNumber), iWidth = EffectVar(1, pTarget, iEffectNumber);
	var iXSkew = EffectVar(2, pTarget, iEffectNumber), iYSkew = EffectVar(3, pTarget, iEffectNumber);
	iHeight = BoundBy(iHeight + RandomX(-20, 20), 800, 1000);
	iWidth = BoundBy(iWidth + RandomX(-20, 20), 800, 1000);
	
	iXSkew = BoundBy(iXSkew + RandomX(-40, 40), -250, 250);
	iYSkew = BoundBy(iYSkew + RandomX(-40, 40), -250, 250);
	
	SetObjDrawTransform(iWidth, iXSkew, 0, iYSkew, iHeight, 0);
	
	EffectVar(0, pTarget, iEffectNumber) = iHeight;
	EffectVar(1, pTarget, iEffectNumber) = iWidth;
	EffectVar(2, pTarget, iEffectNumber) = iXSkew;
	EffectVar(3, pTarget, iEffectNumber) = iYSkew;
}


