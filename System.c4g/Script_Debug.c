/* Debug - Logging */

#strict 2

// What do we need?
//- Logging level
//- Logging source
//- visiblity of logs

static const g_DebugMode_ALL = "all";
static const g_DebugMode_GLOBAL = "global";

static const g_LogLevel_DEBUG = "DEBUG";
static const g_LogLevel_ERROR = "ERROR";

static g_DebugMode;

global func Debug(source)
{
	if (GetType(g_DebugMode) != C4V_Array) g_DebugMode = CreateHash();
	
	return HashContains(g_DebugMode, source) || HashContains(g_DebugMode, g_DebugMode_ALL);
}

// with source 'all' you can debug all sources
global func DebugMode(bool switch, source)
{
	if (GetType(g_DebugMode) != C4V_Array) g_DebugMode = CreateHash();

	if (switch) // log this source
	{
		if (!HashContains(g_DebugMode, source))
			HashPut(g_DebugMode, source, true);
	}
	else
	{
		if (HashContains(g_DebugMode, source))
			HashErase(g_DebugMode, source);
	}
}


global func LogEx(string message, source, string logLevel, bool ignoreDebug)
{
	if (!source) source = g_DebugMode_GLOBAL;
	var sourceString = Format("%v",source);
	if (Debug(source) || ignoreDebug)
	{
		var info = "";
		
		if (this)
			info = Format("%s at (%d,%d)",GetName(),GetX(),GetY());
		
		var logMessage = Format("%.6d - %s [%s] %s: %s", FrameCounter(), logLevel, sourceString, info, message);
		Log(logMessage);
	}
}

global func DebugLog(string msg)
{
	var message = Format(msg,Par(1),Par(2),Par(3),Par(4),Par(5),Par(6),Par(7),Par(8),Par(9));
	LogEx(message, GetID(), g_LogLevel_DEBUG);
}

global func DebugLogAlways(string msg)
{
	var message = Format(msg,Par(1),Par(2),Par(3),Par(4),Par(5),Par(6),Par(7),Par(8),Par(9));
	LogEx(message, GetID(), g_LogLevel_DEBUG, true);
}

// Logs an error message
global func ErrorLog(string msg)
{
	var out = Format(msg,Par(1),Par(2),Par(3),Par(4),Par(5),Par(6),Par(7),Par(8),Par(9));
	LogEx(out, GetID(), g_LogLevel_ERROR);
}

global func ErrorLogAlways(string msg)
{
	var out = Format(msg,Par(1),Par(2),Par(3),Par(4),Par(5),Par(6),Par(7),Par(8),Par(9));
	LogEx(out, GetID(), g_LogLevel_ERROR, true);
}

// Displays the message if the debug mode is on
// always per-object
global func DebugMessage(string msg, name)
{
	if(Debug(name))
		Message(msg,this,Par(2),Par(3),Par(4),Par(5),Par(6),Par(7),Par(8),Par(9));
}
