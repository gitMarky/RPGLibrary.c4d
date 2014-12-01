/*-- 
Controls the gamma fading at night.

@title Darkness
@author Hazard Team (original), Marky (modified)
@version 0.1.0
--*/

#strict 2

static darkness_object;

static const g_DARK_MaxDarkness = 1000;
static const g_DARK_Default_MaxDarkness = 55;

////////////////////////////////////////////////////////////////////
//
// The environment object with info menus

local darkness;

/**
 Darkness is an environment object, as such it has an info window. This funcion creates the info window.
 @par player_index The player number that requested the information window. Base is 0.
 */
protected func Activate(int player_index)
{
	MessageWindow(GetDesc(), player_index);
}

public func Initialize()
{
	ScheduleCall(this, "PostInitialize", 1);
}

private func PostInitialize()
{
	// just one darkness!
	var count = 1;
	for (var instance in FindObjects(Find_ID(GetID()), Find_Exclude(this))) 
	{
		count++;
		RemoveObject(instance);
	}
	
	// save it
	if (!darkness_object)
		darkness_object = this;
	
	// save original sky color - this is done in the time object, but maybe there
	// is a use case for darkness without time?
	if (!original_sky_dword)
		original_sky_dword = GetSkyAdjust(false);
	if (!original_skybg_dword)
		original_skybg_dword = GetSkyAdjust(true);
	if (!original_mat_dword)
		original_mat_dword = RGBa(255, 255, 255, 0);

	// very important
	if (!GetDarkness(g_DARK_MaxDarkness))
		SetDarkness(count);
}

/**
 Activates Fog of War for the player.
 @par player_index The player number. Base is 0.
 */
public func InitializePlayer(int player_index)
{
	SetFoW(true, player_index);
}

/**
 Creates a light for the clonk.
 @par clonk The clonk in question. This is applied to all crew members of the player.
 */
public func OnClonkRecruitment(object clonk)
{
	if (ObjectCount(GetID()) > 1)
	{
		return ScheduleCall(this, "OnClonkRecruitment", 1, 0, clonk);
	}
	
	SetPlrViewRange(CalcViewRange(), clonk);
	var tmp = AddLightAmbience(80, clonk);
	
	tmp->ChangeColor(LightColorClonk()); // a little bit warmer colors...
	
	SetVisibility(VIS_Owner, tmp);
}

////////////////////////////////////////////////////////////////////
//
// Global functions for controlling the object

/**
 Checks, whether a darkness object exists.
 @return bool true, if a darkness object exists.
 */
global func IsDark()
{
	//no darkness object -> no darkness
	if (!darkness_object)
		return false;
	return darkness_object;
}

/**
 Returns the current level of darkness.
 @par precision [optional] No value or 0 relates to a precision of 10. 
 @return int Percentual value of darkness, multiplied by precision.
 */
global func GetDarkness(int precision)
{
	if (!darkness_object)
		return 0;
	if (!precision)
		precision = 10;
	
	var value;
	
	var darknessValue = darkness_object->LocalN("darkness");
	
	value = darknessValue / (g_DARK_MaxDarkness / precision);
	
	return value;
}

/**
 Returns a darkness value relative to the maximal and minimal darkness.
 @par grade The desired relative darkness grade, percentual value from 0 to 100.
 @return int A darkness value relative to the maximal and minimal darkness. Can be used with SetDarkness()
 @example {@code
 			SetDarkness(50); // sets darkness to 50%, regardless of current darkness 
 			SetDarkness(DarknessGradeRelative(50)); // sets darkness to 45%, if the miminum darkness is 20. 
 }
 */
global func DarknessGradeRelative(int grade)
{
	var min = GameCall("MinDarkness"), max = GameCall("MaxDarkness");
	if (!max)
		max = g_DARK_Default_MaxDarkness;
	return BoundBy(grade, 0, 100) * (max - min) / 100 + min;
}

/**
 Absolute darkness, in percent.
 @return int The return value ranges from 0 to 100.
 */
global func GetDarknessGradeAbs()
{
	return GetDarkness(100);
}

/**
 Relative darkness, in percent.
 @return int The return value ranges from 0 to 100.
 */
global func GetDarknessGradeRel()
{
	var min = GameCall("MinDarkness"), max = GameCall("MaxDarkness");
	if (!max)
		max = g_DARK_Default_MaxDarkness;
	
	return 100 * (GetDarkness(100) - min) / (max - min);
}

/**
 Adjusts the current level of darkness.
 @par grade The new level of darkness, percentual value from 0 to 100.
 @return bool False, if there is no darkness in the scenario. Otherwise returns true. 
 */
global func SetDarkness(int grade)
{
	if (!darkness_object)
		return false;
	
	var grade_temp = BoundBy(grade, 0, 100);
	
	darkness_object->LocalN("darkness") = 10 * grade_temp;
	DarknessApplyGrade(grade_temp);
	darkness_object->UpdateLights();
	
	return true;
}

/**
 A "refresh" function for darkness. You should not need to call this function, except in scenario scripts for loading sections and resuming savegames.
 */
global func UpdateDarkness()
{
	if (!darkness_object)
		return false;
	DarknessApplyGrade(darkness_object->LocalN("darkness"));
}

/**
 Overrides the engine function. This override is necessary such that a custom initial color modulation is saved. 
 @par color_foreground Primary color modulation.
 @par color_background Background color modulation.
 @note Additional info in the original developer documentation.
 @ignore may be useful for developpers, but not for the average user.
 */
global func SetSkyAdjust(int color_foreground, int color_background)
{
	if (GetID() != DARK && GetID() != TIME) // those should stay hardcoded
	{
		if (!original_sky_dword) original_sky_dword = color_foreground;
		if (!original_skybg_dword) original_skybg_dword = color_background;
	}
	return inherited(color_foreground, color_background, ...);
}

global func SetMatAdjust(int color)
{
	if (GetID() != DARK && GetID() != TIME)
	{
		if (!original_mat_dword)
			original_mat_dword = color;
	}
	return inherited(color, ...);
}

/**
 Paints the sky, material and sets gamma ramps for a certain level of darkness.
 @par grade The new level of darkness, percentual value from 0 to 100.
 @note Uses the gamma ramp GAMMA_Ramp_User1 = 2.
 */
global func DarknessApplyGrade(int grade)
{
	var grade_bounded = BoundBy(grade, 0, 100);
	
	var color = original_sky_dword;
	if (time_object)
		color = time_sky_dword;
	var gammaLight = ((100 - grade_bounded / 4) * 255) / 100;
	var skyLight = ((100 - grade_bounded) * 255) / 100;
	var matLight = BoundBy(80 + ((100 - grade_bounded) * 255) / 100, 0, 255);
	
	SetGamma
	(
		RGB(0, 0, 0),
		HSL(0, grade_bounded / 2, gammaLight / 2),
		HSL(0, grade_bounded / 2, gammaLight),
		GAMMA_Ramp_User1
	);
	
	var adjustedSky = SetVFromRGBa(color, skyLight);
	var adjustedMat = SetVFromRGBa(original_mat_dword, matLight);
	
	DARK->SetSkyAdjust(adjustedSky, RGB(1, 1, 1));
	DARK->SetMatAdjust(adjustedMat);
}

global func SetVFromRGBa(int rgba, int new_value)
{
	new_value = BoundBy(new_value, 0, 255);
	
	var r, g, b, a;
	SplitRGBaValue(rgba, r, g, b, a);
	return RGBa(r * new_value / 255, g * new_value / 255, b * new_value / 255, a);
}

/**
 Tells light objects how transparent they should be.
 @par alphamod Modifier for the alpha channel of the light. The lighter, the more transparent.
 @par sizemod Modifier for the size of the light. The darker, the larger.
 */
global func CalcLight(int &alphamod, int &sizemod)
{
	sizemod = 100 + GetDarkness(g_DARK_MaxDarkness) * 3 / 2 / 10; // up to 250% of the size
	alphamod = (g_DARK_MaxDarkness - GetDarkness(g_DARK_MaxDarkness)) / 50; // 0-20 alpha are being added
	
	// no darkness: nearly invisible
	// the same at darkness = 0
	if (!IsDark() || !GetDarkness(g_DARK_MaxDarkness))
	{
		alphamod = 30;
	}
}

////////////////////////////////////////////////////////////////////
//
// Other

public func UpdateLights()
{
	var obj;
	
	for (obj in FindObjects(Find_Func("IsLight"))) 
	{
		obj->~Update();
	}
	
	var viewRange = CalcViewRange();
	for (obj in FindObjects(Find_OCF(OCF_CrewMember))) 
	{
		obj->SetPlrViewRange(viewRange);
	}
}

private func CalcViewRange()
{
	var iMax = GameCall("MaxPlrViewRange");
	if (!iMax)
	{
		iMax = 700;
	}
	return (700 - 6 * GetDarkness(100)) * iMax / 700;
}

public func Destruction()
{
	SetDarkness(0);
	UpdateLights();
	for (var light in FindObjects(Find_Func("IsLight"), Find_Func("LocalN(\"bAmbience\")"))) 
	{
		RemoveObject(light);
	}
}
