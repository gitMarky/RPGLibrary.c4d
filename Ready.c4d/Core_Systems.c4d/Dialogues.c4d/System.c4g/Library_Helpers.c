/* Helper functions */

#strict 2

/**
 Change direction of one object so that it looks at another.
 @par target The object will look at this object.
 @par looker This object will change its direction.
 */
global func SetDirTo(object target, object looker)
{
	if (GetX(target) > GetX(looker))
		SetDir(DIR_Right, looker);
	else if (GetX(target) < GetX(looker))
		SetDir(DIR_Left, looker);
}

/**
 Stops the Clonk, but does not change velocity.
 @par target The clonk who will be stopped
 @par keep_action [optional] If set to true, then the clonk will keep riding. Otherwise the action will be set to "Walk".
 @par keep_menu_open [optional] If set to true, then the current menu will be kept open.
 @return true, if the object could be stopped.
 */
global func StopClonk(object target, bool keep_action, bool keep_menu_open)
{
	if (!target) if (!this) return false;
	
	if (!keep_action)
	{
		if (!WildcardMatch(GetAction(target), "Ride*"))
		{
			if (target)
				ObjectSetAction(target, "Walk");
			else
				SetAction("Walk");
		}
	}
	
	SetComDir(COMD_Stop, target);
	SetCommand(target, "None");
	Call("~StopAiming");
	
	if (!keep_menu_open) CloseMenu(target);
	
	return true;
}

/**
 Prints a message with the object's name in its user color on the object.
 @par message The message
 @par target The object
 */
global func MessageEx(string message, object target, v1, v2, v3, v4, v5, v6, v7, v8)
{
	var color = GetSpeakerColor(target);
	if (!target) color = 16777215;
	message = ColorizeString(Format("%s:|%s", GetName(target), message), color);
	message = Format(message, v1, v2, v3, v4, v5, v6, v7, v8);

	return Message(message, target);
}


/**
 Colors the string with the given color.
 @par text The string
 @par color The color, in dword format.
 */
global func ColorizeString(string text, int color)
{
	return Format("<c %x>%s</c>", color, text);
}


global func GetSpeakerColor(object target)
{
	if (!target) return 16777215;

	var color = target->~GetDlgMsgColor();
	if (!color) color = GetColorDw(target);
	if (!color) color = 255;
	// make readable
	color = MakeReadableColor(color);
	return color;
}


global func MakeReadableColor(int dwColor)
{
	// determine lightness
	// 50% red, 87% green, 27% blue (max 164 * 255)
	var r = dwColor >> 16 & 255, g = dwColor >> 8 & 255, b = dwColor & 255;
	var iLightness = r * 50 + g * 87 + b * 27;
	// more than 35/164 (*255) is OK; in this case discard alpha only
	if (iLightness >= 8925)
		return dwColor & 16777215;
	// make lighter
	return RGB(Min(r + 50, 255), Min(g + 50, 255), Min(b + 50, 255));
}
