/* Time */

#strict 2
#appendto TIME

#strict 2

/* Constants */

// Gamma-Constants from the developer documentation. Those should be in the System.c4g!

static const GAMMA_Ramp_Global = 		0; // Szenarienglobaler Wert
static const GAMMA_Ramp_Climate = 		1; // Klima/Jahreszeiten (Engineintern benutzt, wenn im Szenario aktiviert!)
static const GAMMA_Ramp_User1 = 		2; // frei
static const GAMMA_Ramp_DayNight = 		3; // Tag/Nacht
static const GAMMA_Ramp_User2 = 		4; // frei
static const GAMMA_Ramp_Lightning = 	5; // Blitze
static const GAMMA_Ramp_Effects = 		6; // Zaubereffekte
static const GAMMA_Ramp_User3 =		 	7; // frei


static const g_TIME_BaseSpeed_SecondsPerTick = 48;

static const g_TIME_Day_Seconds = 86400;
static const g_TIME_Hour_Seconds = 3600;
static const g_TIME_Minute_Seconds = 60;

static const g_TIME_TickInterval_Frames = 10;

static const g_TIME_YearLength = 20; // One year has this many days

/* Variables */

static time_years, time_days, time_hours, time_minutes;
static time_hours_old;
static time_object;

static original_sky_dword; // The sky looked like this before we added the time system
static original_sky_array; // same info as array: [r,g,b,a]
static original_skybg_dword; // The sky background looked like this before we added the time system

static original_mat_dword; // The materials looked like this before we added the time system

static time_sky_dword; // This is what the sky looks like with the current configuration

static time_altDarkness; // false: Color modulation is set by cosine, modulation happens over the 24 hour cycle
						 // true: During day and night the modulation is constant, and modulation by cosine happens during dusk and dawn only.

static daylight, daylight_override;

local time; // Current time, in seconds
local advance_seconds_per_tick; //This many seconds pass per tick


////////////////////////////////////////////////////////////////////
//
// Engine stuff 

protected func Initialize()
{
	advance_seconds_per_tick = g_TIME_BaseSpeed_SecondsPerTick;

	time = Time( 12, 0, 0);

	if(!ObjectCount2(Find_ID(DARK)))
	{
			CreateObject(DARK, 0, 0, NO_OWNER);
	}

	time_altDarkness = true;

	SetAction("Initialize");
}

private func Initialized()
{
	SetDarkness(GameCall("MinDarkness"));

	advance_seconds_per_tick = g_TIME_BaseSpeed_SecondsPerTick * (ObjectCount(TIME) + 1);

	// Andere Objekte des gleichen Typs entfernen
	var pOther;
	while(pOther = FindObject(GetID()))
		RemoveObject(pOther);

	// Jetzt aber mal komfortabel das Objekt speichern...
	time_object = this;

	// Himmelsmodulation speichern - geht nur mit nicht-transparentem Himmel, momentan
	if (!original_sky_dword) original_sky_dword = GetSkyAdjust(false);
	if (!original_skybg_dword) original_skybg_dword = GetSkyAdjust(true);
	if (!original_mat_dword) original_mat_dword = RGBa(255,255,255,0);
	original_sky_array = [ GetRGBaValue(original_sky_dword, 1),
	                       GetRGBaValue(original_sky_dword, 2),
	                       GetRGBaValue(original_sky_dword, 3),
	                       GetRGBaValue(original_sky_dword, 0)];

	FxIntTimeAdvanceTimer();
	AddEffect("IntTimeAdvance", this, 1, g_TIME_TickInterval_Frames, this);
}

private func FxIntTimeAdvanceTimer()
{
	DoTimeProgress(true);
	DoSkyShade();
}

protected func UpdateTransferZone()
{
	// Kleiner Trick, damit der Himmel nach dem Laden aktualisiert wird
	if (GetAction() == "Time") FxIntTimeAdvanceTimer();
}

////////////////////////////////////////////////////////////////////
//
// Die eigentlichen Effekte

// Zählt die Tage, Stunden, usw.
private func DoTimeProgress(bool updateDays)
{
	// Und weiter...
	time += advance_seconds_per_tick;

	// Tage zählen
	while (updateDays && time >= g_TIME_Day_Seconds)
	{
		time_days++;
		time -= g_TIME_Day_Seconds;
	}

	while (time_days >= g_TIME_YearLength)
	{
		time_days -= g_TIME_YearLength;
		time_years++;
	}

	time_hours = time / g_TIME_Hour_Seconds;
	time_minutes = (time - time_hours * g_TIME_Hour_Seconds)/g_TIME_Minute_Seconds;

	// Alle Objekte wissen lassen, dass sich die Zeiten ändern ^^
	if( time_hours != time_hours_old )
	{
		var obj;
		for(obj in FindObjects(Find_Func("UpdateTime", time_hours, time_hours_old)));

		time_hours_old = time_hours;
	}
}


// By Ringwaul
// Macht tolle Farben.
private func DoSkyShade()
{
	// first determine the time phase we are in
	var sunrise, sunset, night, day;

	sunrise = IsDawn();
	day = IsDay();
	sunset = IsDusk();
	night = IsNight();

	var skyshade = [0,0,0,0]; //R,G,B,A
	var nightcolour = [10,25,40]; // default darkest-night colour
	var daycolour = original_sky_array;//[255,255,255];
	var sunsetcolour = [140,45,10];
	var sunrisecolour = [140,100,70];

	var sunrise_start = GetDawnPeriod()[0];
	var sunrise_end = GetDawnPeriod()[1];
	var sundown_start = GetDuskPeriod()[0];
	var sundown_end = GetDuskPeriod()[1];

	// Sunrise
	if (sunrise)
	{
		var time_since_sunrise = time - sunrise_start;
		// progress in 0..1800
		var progress = time_since_sunrise * 1800 / (sunrise_end - sunrise_start);

		for(var i=0; i<3; ++i)
		{
			var nightfade = Cos(progress/2, nightcolour[i],10);
			var dayfade = daycolour[i] - Cos(progress/2, daycolour[i],10);
			var sunrisefade = Sin(progress, sunrisecolour[i],10);

			skyshade[i] = Min(255,dayfade + nightfade + sunrisefade);
		}

		skyshade[3] = Min(255,progress/2);
	}
	// Day
	else if (day)
	{
		skyshade[0] = 255;
		skyshade[1] = 255;
		skyshade[2] = 255;

		skyshade[3] = 255;
	}
	// Sunset
	else if (sunset)
	{
		var time_since_sunset = time - sundown_start;
		// progress in 0..1800
		var progress = time_since_sunset * 1800 / (sundown_end - sundown_start);

		for(var i=0; i<3; ++i)
		{
			var dayfade = Cos(progress/2, daycolour[i],10);
			var nightfade = nightcolour[i] - Cos(progress/2, nightcolour[i],10);
			var sunsetfade = Sin(progress, sunsetcolour[i],10);

			skyshade[i] = Min(255,dayfade + nightfade + sunsetfade);
		}

		skyshade[3] = Min(255,900-progress/2);
	}
	// Night
	else if (night)
	{
		skyshade[0] = nightcolour[0];
		skyshade[1] = nightcolour[1];
		skyshade[2] = nightcolour[2];

		skyshade[3] = 0;
	}


	// Shade sky.
//	Log("sky color: %d %d %d %d", skyshade[0], skyshade[1], skyshade[2], skyshade[3]);
	time_sky_dword = RGBa(skyshade[0], skyshade[1], skyshade[2], 0);

	// Shade landscape.
	var gamma = [0,0,0];
	var min_gamma = [30,75,120];
	gamma[0] = BoundBy(skyshade[0], min_gamma[0], 128);
	gamma[1] = BoundBy(skyshade[1], min_gamma[1], 128);
	gamma[2] = BoundBy(skyshade[2], min_gamma[2], 128);

	SetGamma(0, RGB(gamma[0], gamma[1], gamma[2]), RGB(127+gamma[0], 127+gamma[1], 127+gamma[2]), GAMMA_Ramp_DayNight);

	// Und zusätzlich Licht aus!

	if (!darkness_object) // der macht das sowieso am Ende
	{
		SetSkyAdjust(time_sky_dword);
	}
	else // Ist fast dasselbe wie die Himmelsfärbung, aber: zeitlich nach hinten versetzt
	{
		var percent;

		if (!time_altDarkness)
		{
			percent = 50 + Cos(time * 360 / g_TIME_Day_Seconds, 50);
		}
		else
		{
			var nodark_start = (sunrise_start + sunrise_end)/2;
			var nodark_end = (sundown_start + sundown_end)/2;

			var dark_start = (sundown_end + g_TIME_Day_Seconds)/2;
			var dark_end = sunrise_start / 2;

			if (Inside(time, nodark_start, nodark_end))
			{
				percent = 0;
			}
			else if (time >= dark_start || time <= dark_end)
			{
				percent = 100;
			}
			else
			{
				var time_shifted = (time + Time(12)) % g_TIME_Day_Seconds - Time(12);

				var phase;
				if (time_shifted >= 0)
					phase = 180 - 180 * (time-dark_end) / (nodark_start-dark_end);
				else
					phase = 180 * (time-nodark_end) / (dark_start - nodark_end);

				percent = 50 - Cos(phase, 50);
			}

		}

		daylight = 100 - percent;

		SetDarkness(DarknessGradeRelative(percent - daylight_override));
	}
}

////////////////////////////////////////////////////////////////////
//
// Berechnungen und so Zeug

private func GetLightIntensity(int iTime)
{

}

////////////////////////////////////////////////////////////////////
//
// Globale Funktionen

/**
 * The time of dawn.
 * @return Array with two entries. The first entry defines the start time, the second entry defines the end time in the Time() format.
 * @note By default this is the time between 4:00 and 7:00.
 */
global func GetDawnPeriod(){ return [Time(  4, 0, 0), Time(  7, 0, 0)]; }
/**
 * The time of dusk.
 * @return Array with two entries. The first entry defines the start time, the second entry defines the end time in the Time() format.
 * @note By default this is the time between 18:00 and 21:00.
 */
global func GetDuskPeriod(){ return [Time( 18, 0, 0), Time( 21, 0, 0)]; }

/**
 * Calculates a time value in seconds, so that it can be compared with the ingame time.
 * @param hours time on the clock, in hours. Uses the 24 hour format, values greater that 24 will be converted to a value between 0 and 23 hours.
 * @param minutes time on the clock, in minutes. Uses the 60 minute format, values greater that 60 will be converted to a value between 0 and 59 minutes.
 * @param secondstime on the clock, in seconds. Uses the 60 second format, values greater that 60 will be converted to a value between 0 and 59 seconds.
 * @return The time entered, in seconds
 * @example {@code
 * public func IsItTeaTime()
 * {
 *    var currentTime = GetTime();
 *    return Time(15, 0, 0) < currentTime && currentTime < Time(17, 0, 0);
 * }
 * }
 * Returns true between 15 o'clock and 17 o'clock.
 */
global func Time(int hours, int minutes, int seconds)
{
	hours	= hours % 24;
	minutes	= minutes % 60;
	seconds	= seconds % 60;
	return hours * g_TIME_Hour_Seconds + minutes * g_TIME_Minute_Seconds + seconds;
}

/**
 * Gets the ingame time.
 * @return The time, in seconds.
 */
global func GetTime()
{
	if (!time_object) return Time(12, 0, 0);
	return LocalN("time", time_object);
}

/**
 * Sets the current ingame time.
 * @param seconds The time value, in seconds.
 * @example {@code
 * SetSeconds(Time(12, 30, 5));
 * }
 * Sets the time to 12:30:05.
 * @link SetTime()
 */
global func SetSeconds(int seconds)
{
	if (!time_object) return;
	LocalN("time", time_object) = seconds;
}

/**
 * Sets the current ingame time.
 * @param days The new day.
 * @param hours The new hour.
 * @param minutes The new minute.
 * @param seconds The new seconds.
 * @link SetSeconds(), Time()
 */
global func SetTime(int days, int hours, int minutes, int seconds)
{
	if (!time_object) return;
	LocalN("time", time_object) = g_TIME_Day_Seconds * days + Time(hours,minutes,seconds);
}

/**
 * Tells how fast the game time advances.
 * @return Every 10 frames, the game time advances this many seconds.
 * @note The default speed is regulated by the following constants:
 * {@code
 * static const g_TIME_BaseSpeed_SecondsPerTick // default is 48
 * static const g_TIME_TickInterval_Frames // default is 10
 * }
 */
global func GetTimeSpeed()
{
	if(!time_object) return 0;
	return LocalN("advance_seconds_per_tick", time_object);
}

/**
 * Defines how fast the game time advances.
 * @param speed Every 10 frames, the game time advances this many seconds.
 * @return The new speed value.
 * @note The default speed is regulated by the following constants:
 * {@code
 * static const g_TIME_BaseSpeed_SecondsPerTick // default is 48
 * static const g_TIME_TickInterval_Frames // default is 10
 * }
 */
global func SetTimeSpeed(int speed)
{
	if(!time_object) return 0;
	return LocalN("advance_seconds_per_tick", time_object) = speed;
}

/**
 * Is it dawn?
 * @return true, if the time is dawn.
 * @link GetDawnPeriod()
 */
global func IsDawn() { if(!time_object) return false; var curTime = GetTime(); return GetDawnPeriod()[0] <= curTime && curTime < GetDawnPeriod()[1]; }
/**
 * Is it day?
 * @return true, if the time is after dawn and before dusk.
 * @link GetDawnPeriod(), GetDuskPeriod()
 */
global func IsDay()  { if(!time_object) return true;  var curTime = GetTime(); return GetDawnPeriod()[1] <= curTime && curTime < GetDuskPeriod()[0]; }
/**
 * Is it dusk?
 * @return true, if the time is dusk.
 * @link GetDuskPeriod()
 */
global func IsDusk() { if(!time_object) return false; var curTime = GetTime(); return GetDuskPeriod()[0] <= curTime && curTime < GetDuskPeriod()[1]; }
/**
 * Is it night?
 * @return true, if the time is after dusk and before dawn.
 * @link GetDawnPeriod(), GetDuskPeriod()
 */
global func IsNight(){ if(!time_object) return false; var curTime = GetTime(); return GetDuskPeriod()[1] <= curTime || curTime < GetDawnPeriod()[0]; }

