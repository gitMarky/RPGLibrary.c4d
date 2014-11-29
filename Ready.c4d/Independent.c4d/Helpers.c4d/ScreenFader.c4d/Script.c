#strict 2

static const gScreenModulation_Ramp_Count = 10;

static const gScreenModulation_Ramp_Building	= 0;
static const gScreenModulation_Ramp_Damage		= 1;
static const gScreenModulation_Ramp_Lensflare 	= 9;

static const gScreenModulation_Layer_Sky 		= -1;
static const gScreenModulation_Layer_Object 	= 0;
static const gScreenModulation_Layer_Material 	= 1;
static const gScreenModulation_Layer_GUI 		= 2;


public func Initialize()
{
	Local(0) = 0;
	Local(1) = 0;
	SetVisibility(VIS_Owner);
	
	var width = Max(LandscapeWidth(), 4000);
	var height = Max(LandscapeHeight(), 3000);
	
	SetObjDrawTransform
	(
		1000 * width / GetDefWidth(GetID()),
		0,
		750 * width - 1000 * GetDefWidth(GetID()),
		0,
		1000 * height / GetDefHeight(GetID()),
		750 * height - 1000 * GetDefHeight(GetID())
	);
	
	SetPosition(1, 1);
	
	SetClrModulation(RGBa(0, 0, 0, 255));
}

public func FxIntCheckCursorTimer(object target, int effect, int time)
{
	var cursor = GetCursor(GetOwner(target));
	var isFocused = false;
	if (cursor)
	{
		if (cursor == target)
			isFocused = true;
	}
	
	if (isFocused)
		SetVisibility(VIS_Owner);
	else
		SetVisibility(VIS_None);
}

/**
Overlays an RGB color on the screen, object specific. The modulation is only active, as long as the player's cursor is on that object.
@par obj The object in question.
@par rgba The new color in dword format. Passing 0 resets the ramp.
@par blit_mode Blit mode constant {@c GFX_BLIT_*}, as in SetObjectBlitMode().
@par ramp_index Every object can have any amount of modulation ramps.
@par layer The modulation can be placed on certain layers, see SetPlrScreenModulation().
@version 0.1.0
@author Codename: Modern Combat Team (Original), Marky (modified)
@example
{@code
static const gScreenModulation_Ramp_Eclipse = 2; // define a new ramp
@br
public func UpdateHealth()
{
	var energy_max = 75;
	var energy_cur = BoundBy(GetEnergy(), 0, energy_max);
	@br
    SetObjScreenModulation(this, RGBa(255,0,0,255 - 2 * (energy_max - energy_cur), 0, gScreenModulation_Ramp_Eclipse, gScreenModulation_Layer_Material);
}
}
@link SetObjScreenModulation
*/
global func SetObjScreenModulation(object obj, int rgba, int blit_mode, int ramp_index, int layer)
{
	if (!obj && !(obj = this))
		return false;
	if (GetOwner(obj) == NO_OWNER)
		return false;
	
	var effect_name = "IntScreenModulation";
	var effect = GetScreenModulationEffect(obj, effect_name);
	if (!effect)
		return false;
	
	var ramp_object = SetScreenModulationRamp(obj, rgba, blit_mode, ramp_index, effect, layer);
	
	if (ramp_object)
	{
		ramp_object->SetOwner(GetOwner(obj));
		AddEffect("IntCheckCursor", obj, 1, 1, ramp_object);
	}
	
	return true;
}

/**
Overlays an RGB color on the screen, player specific. 
@par player The index of the player, 0 based.
@par rgba The new color in dword format. Passing 0 resets the ramp.
@par blit_mode Blit mode constant {@c GFX_BLIT_*}, as in SetObjectBlitMode().
@par ramp_index Every player can have up to 10 screen modulation ramps. Three of them
     are already in use and accessible by the constants
     {@table
       {@tr {@th Constant} {@th Value} {@th Description}}
       {@tr {@td gScreenModulation_Ramp_Building}  {@td 0} {@td Modulation from entering a building or similar}}
       {@tr {@td gScreenModulation_Ramp_Damage}    {@td 1} {@td Modulation from taking damage}}
       {@tr {@td gScreenModulation_Ramp_Lensflare} {@td 9} {@td Modulation from lens flare effect}}
     }
@par layer The modulation can be placed on certain layers:
     {@table
       {@tr {@th Constant} {@th Value} {@th Description}}    
       {@tr {@td gScreenModulation_Layer_Sky}      {@td -1} {@td Modulation of the sky only, without changing the value set by SetSkyAdjust().}}
       {@tr {@td gScreenModulation_Layer_Object}   {@td 0}  {@td Modulation of sky and objects, but not of the landscape and GUI.}}
       {@tr {@td gScreenModulation_Layer_Material} {@td 1}  {@td Modulation of sky, objects, and landscape, but not the GUI.}}
       {@tr {@td gScreenModulation_Layer_GUI}      {@td 2}  {@td Modulation of sky, objects, landscape and the GUI.}}
     }
@version 0.1.0
@author Codename: Modern Combat Team (Original), Marky (modified)
@example
{@code
static const gScreenModulation_Ramp_Eclipse = 2; // define a new ramp
@br
public func StartEclipse()
{
  // make everything very dark, instead of the HUD for all regular players
  for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
    SetPlrScreenModulation(GetPlayerByIndex(i, C4PT_User), RGBa(0,0,0,128), 0, gScreenModulation_Ramp_Eclipse, gScreenModulation_Layer_Material);
}
@br
public func EndEclipse()
{
  // reset the ramp for every player
  for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
    SetPlrScreenModulation(GetPlayerByIndex(i, C4PT_User), 0, 0, gScreenModulation_Ramp_Eclipse, gScreenModulation_Layer_Material);
}
}
@link SetObjScreenModulation
*/
global func SetPlrScreenModulation(int player, int rgba, int blit_mode, int ramp_index, int layer)
{
	if (player == NO_OWNER)
		return false;
	
	var effect_name = "IntPlrScreenModulation";
	var effect = GetScreenModulationEffect(0, effect_name);
	if (!effect)
		return false;
	
	var ramp_object = SetScreenModulationRamp(0, rgba, blit_mode, gScreenModulation_Ramp_Count * player + ramp_index, effect, layer);
	
	if (ramp_object)
	{
		ramp_object->~SetOwner(player);
	}
	
	CleanUpScreenModulationEffect(0, effect, effect_name);
	
	return true;
}

/**
Not intended for docu. Creates (if necessary) and returns the screen modulation effect for an object.
@ignore
*/
global func GetScreenModulationEffect(object obj, string effect_name)
{
	var effect = GetEffect(effect_name, obj);
	
	if (!effect)
		effect = AddEffect(effect_name, obj, 1);
	return effect;
}

/**
Not intended for docu. Removes the screen modulation effect from an object.
@ignore
*/
global func CleanUpScreenModulationEffect(object obj, int effect, string effect_name)
{
	// remove unused effects
	var removeEffect = true;
	for (var i = 0; i < gScreenModulation_Ramp_Count; i++)
	{
		removeEffect &= !EffectVar(i, obj, effect);
	}
	
	if (removeEffect)
	{
		DebugLog("Remove changed effect");
		RemoveEffect(effect_name, obj);
	}
}

/**
Not intended for docu. Creates the screen modulation ramp for a modulation effect.
@ignore
*/
global func SetScreenModulationRamp(object obj, int rgba, int blit_mode, int ramp_index, int effect, int layer)
{
	var ramp_object = EffectVar(ramp_index, obj, effect);
	
	// no color?
	var r, g, b, a;
	SplitRGBaValue(rgba, r, g, b, a);
	if (!(r + g + b + a) || a == 255)
	{
		if (ramp_object)
			RemoveObject(ramp_object);
		return false;
	}
	
	if (!ramp_object)
		ramp_object = EffectVar(ramp_index, obj, effect) = CreateObject(ID_Helper_ScreenFader, 0, 0, -1);
	
	if (ramp_object)
	{
		// Modulation...
		SetObjectBlitMode(blit_mode, ramp_object);
		SetClrModulation(rgba, ramp_object);
		
		SetVisibility(VIS_Owner, ramp_object);
		
		if (layer == gScreenModulation_Layer_Material)
			ramp_object->SetCategory(GetDefCategory(GetID(ramp_object)) | C4D_Foreground);
		else if (layer == gScreenModulation_Layer_GUI)
			ramp_object->SetCategory(GetDefCategory(GetID(ramp_object)) | C4D_Foreground | C4D_Object);
		else if (layer == gScreenModulation_Layer_Sky)
			ramp_object->SetCategory(GetDefCategory(GetID(ramp_object)) | C4D_Background);
	}
	
	return ramp_object;
}