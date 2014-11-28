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
 * Darkness is an environment object, as such it has an info window. This funcion creates the info window.
 * @param iPlr The player number that requested the information window. Base is 0.
 */
protected func Activate(int iPlr)
{
	MessageWindow(GetDesc(), iPlr);
}

public func Initialize()
{
	ScheduleCall(this, "PostInitialize", 1);
}

private func PostInitialize()
{
	// just one darkness!
	var count = 1;
	for (var d in FindObjects(Find_ID(GetID()), Find_Exclude(this))) 
	{
		count++;
		RemoveObject(d);
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
 * Activates Fog of War for the player.
 * @param iPlr The player number. Base is 0.
 */
public func InitializePlayer(int iPlr)
{
	SetFoW(true, iPlr);
}

/**
 * Creates a light for the clonk.
 * @param pClonk The clonk in question. This is applied to all crew members of the player.
 */
public func OnClonkRecruitment(object pClonk)
{
	if (ObjectCount(GetID()) > 1)
	{
		return ScheduleCall(this, "OnClonkRecruitment", 1, 0, pClonk);
	}
	
	SetPlrViewRange(CalcViewRange(), pClonk);
	var tmp = AddLightAmbience(80, pClonk);
	
	tmp->ChangeColor(RGBa(255, 253, 135, 110)); // a little bit warmer colors...
	
	SetVisibility(VIS_Owner, tmp);
}

////////////////////////////////////////////////////////////////////
//
// Global functions for controlling the object

/**
 * Checks, whether a darkness object exists.
 * @return bool true, if a darkness object exists.
 */
global func IsDark()
{
	//no darkness object -> no darkness
	if (!darkness_object)
		return false;
	return darkness_object;
}

/**
 * Returns the current level of darkness.
 * @param precision [optional] No value or 0 relates to a precision of 10. 
 * @return int Percentual value of darkness, multiplied by precision.
 */
global func GetDarkness(int precision)
{
	if (!darkness_object)
		return 0;
	if (!precision)
		precision = 10;
	
	var val;
	
	var darknessValue = darkness_object->LocalN("darkness");
	
	val = darknessValue / (g_DARK_MaxDarkness / precision);
	
	return val;
}

/**
 * Returns a darkness value relative to the maximal and minimal darkness.
 * @param iGrade The desired relative darkness grade, percentual value from 0 to 100.
 * @return int A darkness value relative to the maximal and minimal darkness. Can be used with SetDarkness()
 * @example {@code
 *			SetDarkness(50); // sets darkness to 50%, regardless of current darkness 
 *			SetDarkness(DarknessGradeRelative(50)); // sets darkness to 45%, if the miminum darkness is 20. 
 * }
 */
global func DarknessGradeRelative(int iGrade)
{
	var iMin = GameCall("MinDarkness"), iMax = GameCall("MaxDarkness");
	if (!iMax)
		iMax = g_DARK_Default_MaxDarkness;
	return BoundBy(iGrade, 0, 100) * (iMax - iMin) / 100 + iMin;
}

/**
 * Absolute darkness, in percent.
 * @return int The return value ranges from 0 to 100.
 */
global func GetDarknessGradeAbs()
{
	return GetDarkness(100);
}

/**
 * Relative darkness, in percent.
 * @return int The return value ranges from 0 to 100.
 */
global func GetDarknessGradeRel()
{
	var iMin = GameCall("MinDarkness"), iMax = GameCall("MaxDarkness");
	if (!iMax)
		iMax = g_DARK_Default_MaxDarkness;
	
	return 100 * (GetDarkness(100) - iMin) / (iMax - iMin);
}

/**
 * Adjusts the current level of darkness.
 * @param iGrade The new level of darkness, percentual value from 0 to 100.
 * @return bool False, if there is no darkness in the scenario. Otherwise returns true. 
 */
global func SetDarkness(int iGrade)
{
	if (!darkness_object)
		return false;
	
	var grade = BoundBy(iGrade, 0, 100);
	
	darkness_object->LocalN("darkness") = 10 * grade;
	DarknessApplyGrade(grade);
	darkness_object->UpdateLights();
	
	return true;
}

/**
 * A "refresh" function for darkness. You should not need to call this function, except in scenario scripts for loading sections and resuming savegames.
 */
global func UpdateDarkness()
{
	if (!darkness_object)
		return false;
	DarknessApplyGrade(darkness_object->LocalN("darkness"));
}

/**
 * Overrides the engine function. This override is necessary such that a custom initial color modulation is saved. 
 * @param clr Primary color modulation.
 * @param backclr dwBackClr Background color modulation.
 * @note Additional info in the original developer documentation.
 * @ignore may be useful for developpers, but not for the average user.
 */
global func SetSkyAdjust(int clr, int backclr)
{
	if (GetID() != DARK && GetID() != TIME)
	{
		if (!original_sky_dword)
			original_sky_dword = clr;
		if (!original_skybg_dword)
			original_skybg_dword = backclr;
	}
	return inherited(clr, backclr, ...);
}

global func SetMatAdjust(int clr)
{
	if (GetID() != DARK && GetID() != TIME)
	{
		if (!original_mat_dword)
			original_mat_dword = clr;
	}
	return inherited(clr, ...);
}

/**
 * Paints the sky, material and sets gamma ramps for a certain level of darkness.
 * @param iGrade The new level of darkness, percentual value from 0 to 100.
 * @note Uses the gamma ramp GAMMA_Ramp_User1 = 2.
 */
global func DarknessApplyGrade(int grade)
{
	grade = BoundBy(grade, 0, 100);
	
	var color = original_sky_dword;
	if (time_object)
		color = time_sky_dword;
	var gammaLight = ((100 - grade / 4) * 255) / 100;
	var skyLight = ((100 - grade) * 255) / 100;
	var matLight = BoundBy(80 + ((100 - grade) * 255) / 100, 0, 255);
	
	SetGamma
	(
		RGB(0, 0, 0),
		HSL(0, grade / 2, gammaLight / 2),
		HSL(0, grade / 2, gammaLight),
		GAMMA_Ramp_User1
	);
	
	var adjustedSky = SetVFromRGBa(color, skyLight);
	var adjustedMat = SetVFromRGBa(original_mat_dword, matLight);
	
	DARK->SetSkyAdjust(adjustedSky, RGB(1, 1, 1));
	DARK->SetMatAdjust(adjustedMat);
}

global func SetVFromRGBa(int rgba, int newVal)
{
	newVal = BoundBy(newVal, 0, 255);
	
	var r, g, b, a;
	SplitRGBaValue(rgba, r, g, b, a);
	return RGBa(r * newVal / 255, g * newVal / 255, b * newVal / 255, a);
}

/**
 * Tells light objects how transparent they should be.
 * @param alphamod Modifier for the alpha channel of the light. The lighter, the more transparent.
 * @param sizemod Modifier for the size of the light. The darker, the larger.
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
