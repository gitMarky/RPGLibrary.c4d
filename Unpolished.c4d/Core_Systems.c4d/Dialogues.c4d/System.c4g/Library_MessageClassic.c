
#strict 2


// Nachricht in Clonkfarbe mit Namen
global func MessageEx(string sMessage, object pTarget, v1, v2, v3, v4, v5, v6, v7, v8)
{
	var dwColor = GetColorDw(pTarget);
	if (!dwColor)
		dwColor = 255;
	dwColor = MakeReadableColor(dwColor);
	if (!pTarget)
		dwColor = 16777215;
	sMessage = ColorizeString(Format("%s:|%s", GetName(pTarget), sMessage), dwColor);
	sMessage = Format(sMessage, v1, v2, v3, v4, v5, v6, v7, v8);
	// Nachricht ausgeben
	return Message(sMessage, pTarget);
}