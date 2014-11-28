/* Message Color */

#strict 2

// Färbt einen mehrzeiligen Text ein
global func ColorizeString(string sText, int dwColor)
{
	return Format("<c %x>%s</c>", dwColor, sText);
}

global func GetColorDlg(object pSpeaker)
{
	var dwClr = pSpeaker->~GetDlgMsgColor();
	if (!dwClr)
		dwClr = GetColorDw(pSpeaker);
	if (!dwClr)
		dwClr = 255;
	// Farbe lesbar machen
	dwClr = MakeReadableColor(dwClr);
	return dwClr;
}


// Hellt dunkle Farben auf
global func MakeReadableColor(int dwColor)
{
	// Helligkeit ermitteln
	// 50% rot, 87% grün, 27% blau (Max 164 * 255)
	var r = dwColor >> 16 & 255, g = dwColor >> 8 & 255, b = dwColor & 255;
	var iLightness = r * 50 + g * 87 + b * 27;
	// Oberhalb 35/164 (*255) ist OK; dann nur Alpha verwerfen
	if (iLightness >= 8925)
		return dwColor & 16777215;
	// Sonst aufhellen
	return RGB(Min(r + 50, 255), Min(g + 50, 255), Min(b + 50, 255));
}
