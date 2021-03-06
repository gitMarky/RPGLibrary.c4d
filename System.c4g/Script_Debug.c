/* There is a new logging mechanism that standardizes the debug and error log messages.
@title Logging Mechanism
@author Marky
@version 0.1.0
@file indexLogging.html
@category #Tutorials
*/

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

/**
 * Checks, if the debug mode is enabled for the source
 * @par source This is a logging channel. This is defined by the log message, so you have to know exactly which channel you want to check.
 * Usually, this is the ID of the object that is writing the log message.
 * @return true, if the debug mode for this source is active, or if {@i g_DebugMode_ALL} is active.
 * @author Marky
 * @version 0.1.0
 */
global func Debug(source)
{
	if (GetType(g_DebugMode) != C4V_Array)
		g_DebugMode = CreateHash();
	
	return HashContains(g_DebugMode, source) || HashContains(g_DebugMode, g_DebugMode_ALL);
}

// with source 'all' you can debug all sources
/**
 * Switches the debug mode on or off for a source.
 * @par switch Pass true, if you want to activate the debug mode for this source.
 * @par source This is a logging channel. This is defined by the log message, so you have to know exactly which channel you want to check.
 * Usually, this is the ID of the object that is writing the log message.
 * @author Marky
 * @version 0.1.0
 */
global func DebugMode(bool switch, source)
{
	if (GetType(g_DebugMode) != C4V_Array)
		g_DebugMode = CreateHash();
	
	if (switch)
	// log this source
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

/**
 * Logs a message with a source and log level.
 * @par message The log message.
 * @par source The logging channel. Specify a string or ID. A value of 0 logs to the channel "global" which can be accessed by the constant {@i g_DebugMode_GLOBAL}.
 * @par logLevel You can use any string, but two are already provided by the constants
 * {@code
 * g_LogLevel_DEBUG
 * g_LogLevel_ERROR
 * }
 * @par ignoreDebug The log level debug is only printed if the debug mode is active for the source, or if {@i ignoreDebug} is true.
 * @link DebugLog(), DebugLogAlways(), ErrorLog(), ErrorLogAlways(), DebugMessage()
 * @author Marky
 * @version 0.1.0
 */
global func LogEx(string message, source, string logLevel, bool ignoreDebug)
{
	if (!source)
		source = g_DebugMode_GLOBAL;
	var sourceString = Format("%v", source);
	if (Debug(source) || ignoreDebug)
	{
		var info = "";
		
		if (this)
			info = Format("%s at (%d,%d)", GetName(), GetX(), GetY());
		
		var logMessage = Format("%.6d - %s [%s] %s: %s", FrameCounter(), logLevel, sourceString, info, message);
		Log(logMessage);
	}
}

/**
 * Logs a message to the debug log level. Logging source is the ID of the calling object. Output happens only if the debug mode
 * is active for that source.
 * @par msg The message. Can be formatted with 9 additional parameters.
 * @link LogEx(), DebugLogAlways()
 * @author Marky
 * @version 0.1.0
 */
global func DebugLog(string msg)
{
	var message = Format(msg, Par(1), Par(2), Par(3), Par(4), Par(5), Par(6), Par(7), Par(8), Par(9));
	LogEx(message, GetID(), g_LogLevel_DEBUG);
}

/**
 * Logs a message to the debug log level. Logging source is the ID of the calling object. Is logged always.
 * @par msg The message. Can be formatted with 9 additional parameters.
 * @link LogEx(), DebugLog()
 * @author Marky
 * @version 0.1.0
 */
global func DebugLogAlways(string msg)
{
	var message = Format(msg, Par(1), Par(2), Par(3), Par(4), Par(5), Par(6), Par(7), Par(8), Par(9));
	LogEx(message, GetID(), g_LogLevel_DEBUG, true);
}

/**
 * Logs a message to the error log level. Logging source is the ID of the calling object. Output happens only if the debug mode
 * is active for that source.
 * @par msg The message. Can be formatted with 9 additional parameters.
 * @link LogEx(), ErrorLogAlways()
 * @author Marky
 * @version 0.1.0
 */
global func ErrorLog(string msg)
{
	var out = Format(msg, Par(1), Par(2), Par(3), Par(4), Par(5), Par(6), Par(7), Par(8), Par(9));
	LogEx(out, GetID(), g_LogLevel_ERROR);
}

/**
 * Logs a message to the error log level. Logging source is the ID of the calling object. Is logged always.
 * @par msg The message. Can be formatted with 9 additional parameters.
 * @link LogEx(), ErrorLog()
 * @author Marky
 * @version 0.1.0
 */
global func ErrorLogAlways(string msg)
{
	var out = Format(msg, Par(1), Par(2), Par(3), Par(4), Par(5), Par(6), Par(7), Par(8), Par(9));
	LogEx(out, GetID(), g_LogLevel_ERROR, true);
}

/**
 * Displays a message above and object with Message().
 * @par msg The message. Can be formatted with 8 additional parameters.
 * @par source Is displayed only, if the debug mode is active for this source.
 * @author Marky
 * @version 0.1.0
 */
global func DebugMessage(string msg, source)
{
	if (Debug(source))
		Message(msg, this, Par(2), Par(3), Par(4), Par(5), Par(6), Par(7), Par(8), Par(9));
}
