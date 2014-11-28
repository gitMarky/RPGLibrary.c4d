/* Neues Script */

#strict 2

global func ClonkMessage(string szString, object pObj)
{
	Message
	(
		ColorizeString(Format("<%s>|%s", GetName(pObj), szString), GetPlrColorDw(GetOwner(pObj))),
		pObj
	);
}




/* Dialogstil-Nachrichten */

  
/* Hilfsfunktionen */

global func ShoutMsg(sMessage, pSpeaker, szPortrait, iExcludePlayer, fPermanent)
{
	var id = GetID(pSpeaker);
	// Portrait
	if (!szPortrait)
		if (!(szPortrait = pSpeaker->~GetMsgPortrait()))
			szPortrait = "1";
	// Portrait ermitteln
	if (GetChar(szPortrait) == GetChar("@"))
	{
		// @Portrait: Aus der Dialog Definition holen
		var c, i, p2 = "";
		while (c = GetChar(szPortrait, ++i))
			p2 = Format("%s%c", p2, c);
		szPortrait = Format("Portrait:%i::%x::%s", DG69, GetOriginalColorDw(pSpeaker), p2);
	}
	else
	{
		// Manche Clonks haben kein eignes Portrait und verwenden Standardportraits
		if (id == ACLK || id == _ACK)
			id = CLNK;
		if (id == WDSK)
			id = MAGE;
		// Ansonsten direkt vom Sprecher
		szPortrait = Format("Portrait:%i::%x::%s", id, GetOriginalColorDw(pSpeaker), szPortrait);
	}
	// Farbe ermitteln
	var dwClr = pSpeaker->~GetDlgMsgColor();
	if (!dwClr)
		dwClr = GetOriginalColorDw(pSpeaker);
	if (!dwClr)
		dwClr = 255;
	// Permanent @
	var szPermanent = "";
	if (fPermanent)
		szPermanent = "@";
	// Message ausgeben
	if (iExcludePlayer)
	{
		var i = GetPlayerCount(C4PT_User);
		while (i--)
			if (GetPlayerByIndex(i, C4PT_User) != iExcludePlayer - 1)
				CustomMessage
				(
					Format("%s<c %x>%s:</c> %s", szPermanent, dwClr, GetName(pSpeaker), sMessage),
					0,
					GetPlayerByIndex(i, C4PT_User),
					90,
					70,
					0xFFFFFF,
					MD69,
					szPortrait,
					0
				);
	}
	else
		CustomMessage
		(
			Format("%s<c %x>%s:</c> %s", szPermanent, dwClr, GetName(pSpeaker), sMessage),
			0,
			-1,
			90,
			70,
			0xFFFFFF,
			MD69,
			szPortrait,
			0
		);
}

