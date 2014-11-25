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



/* Dialogstil-Nachrichten */

global func MsgBoxGetTargetByPlayer(object pPlayer)
{
	var pMsgTarget = GetCursor(GetController(pPlayer));
	if (!pMsgTarget)
		pMsgTarget = pPlayer;
	return pMsgTarget;
}

global func MsgBox(object pPlayer, string sMessage, object pSpeaker, string szPortrait, bool fAsMessage, bool fPermanent, array aOptions)
{
	// Defaults
	if (!pPlayer)
		if (!(pPlayer = this))
			return false;
	if (!pSpeaker)
		if (!(pSpeaker = this))
			return false;
	if (!sMessage)
		return false;
	if (!szPortrait)
		if (!(szPortrait = pSpeaker->~GetMsgPortrait()))
			szPortrait = "1";
	var pMsgTarget = MsgBoxGetTargetByPlayer(pPlayer);
	// Portrait ermitteln
	var c, i, p2 = "";
	if (GetChar(szPortrait) == GetChar("@"))
	{
		// @Portrait: Aus dieser Definition holen
		while (c = GetChar(szPortrait, ++i))
			p2 = Format("%s%c", p2, c);
		szPortrait = Format("Portrait:%i::%x::%s", DG69, GetColorDw(pSpeaker), p2);
	}
	else
	{
		var id = GetPortrait(pSpeaker, 1); //GetID(pSpeaker);
		if (!id)
			id = GetID(pSpeaker);
		// Manche Clonks haben kein eignes Portrait und verwenden Standartportraits
		if (id == ACLK || id == _ACK)
			id = CLNK;
		if (id == WDSK)
			id = MAGE;
		// Ansonsten direkt vom Sprecher
		szPortrait = Format("Portrait:%i::%x::%s", id, GetColorDw(pSpeaker), szPortrait);
	}
	var iSpeakerNameLen = 0;
	while (GetChar(GetName(pSpeaker), iSpeakerNameLen))
		++iSpeakerNameLen;
	// Farbe ermitteln
	//var dwClr = pSpeaker->~GetDlgMsgColor();
	//if (!dwClr) dwClr = GetColorDw(pSpeaker);
	//if (!dwClr) dwClr = 255;
	var dwClr = GetColorDlg(pSpeaker);
	// Farbe lesbar machen
	//dwClr = MakeReadableColor( dwClr );
	// MenuDecoration
	var idMenuDeco = MD69;
	var szMenuCaption = "";
	if (!(idMenuDeco = (pSpeaker->~GetDlgMsgDeco())))
		idMenuDeco = MD69;
	if (GetType(aOptions) == C4V_Array)
	{
		if (aOptions[5])
			idMenuDeco = aOptions[5];
		if (aOptions[6])
			szMenuCaption = aOptions[6];
	}
	// Wie ausgeben?
	if (fAsMessage)
	{
		CloseMenu(pPlayer);
		// Nachricht als reguläre Message; Spieler nicht stoppen
		CustomMessage
		(
			Format("<c %x>%s:</c> %s", dwClr, GetName(pSpeaker), sMessage),
			0,
			GetController(pPlayer),
			0,
			35,
			0xFFFFFF,
			idMenuDeco,
			szPortrait,
			MSG_HCenter | MSG_Top
		);
	}
	else
	{
		// Nachricht als Menü
		// Spieler soll anhalten, damit er nicht ins Verderben läuft
		StopClonkEx(pPlayer);
		if (!CreateMenu(_CAM, pMsgTarget, this, 0, szMenuCaption, 0, C4MN_Style_Dialog, fPermanent, _CAM))
			return false;
		if (GetType(aOptions) == C4V_Array)
		{
			var fName, dwColor, fInstant;
			
			fName = aOptions[0];
			dwColor = aOptions[1];
			fInstant = aOptions[2];
			if (aOptions[3])
				szPortrait = aOptions[3];
			
			var szMsgBox = sMessage;
			if (dwColor)
				szMsgBox = ColorizeString(szMsgBox, dwColor);
			if (fName)
				szMsgBox = Format("<c %x>%s:</c> %s", dwClr, GetName(pSpeaker), szMsgBox);
			AddMenuItem(szPortrait, "", NONE, pMsgTarget, 0, 0, "", C4MN_Add_ImgTextSpec, 0, 0);
			AddMenuItem(szMsgBox, "", NONE, pMsgTarget, 0, 0, "", C4MN_Add_ForceNoDesc, 0, 0);
			
			if (fInstant)
			{
				var iMsgBoxLen = 0;
				while (GetChar(szMsgBox, iMsgBoxLen))
					++iMsgBoxLen;
				iSpeakerNameLen += iMsgBoxLen;
			}
			//SetMenuTextProgress(iSpeakerNameLen+1, pMsgTarget);
		}
		else
		{
			AddMenuItem(szPortrait, "", NONE, pMsgTarget, 0, 0, "", C4MN_Add_ImgTextSpec, 0, 0);
			AddMenuItem
			(
				Format("<c %x>%s:</c> %s", dwClr, GetName(pSpeaker), sMessage),
				"",
				NONE,
				pMsgTarget,
				0,
				0,
				"",
				C4MN_Add_ForceNoDesc,
				0,
				0
			);
			//SetMenuDecoration(MD69, pMsgTarget);
			//SetMenuTextProgress(iSpeakerNameLen+1, pMsgTarget);
		}
		if (idMenuDeco == NONE || idMenuDeco == -1)
			SetMenuDecoration(0, pMsgTarget);
		else
		{
			SetMenuDecoration(idMenuDeco, pMsgTarget);
		}
		
		SetMenuTextProgress(iSpeakerNameLen + 1, pMsgTarget);
	}
	
	return true;
}
  
global func MsgBoxAddText(object pPlayer, string sText)
{
	var pMsgTarget = MsgBoxGetTargetByPlayer(pPlayer);
	return AddMenuItem(sText, "", NONE, pMsgTarget, 0, 0, 0, C4MN_Add_ForceNoDesc);
}

global func MsgBoxAddOption(object pPlayer, id idIcon, string sOption, string sCommand, vParam, int extra, xPar)
{
	// Optionen
	extra |= C4MN_Add_ForceNoDesc;
	var pMsgTarget = MsgBoxGetTargetByPlayer(pPlayer);
	return AddMenuItem(sOption, sCommand, idIcon, pMsgTarget, 0, vParam, 0, extra, xPar);
}
  
  
/* Hilfsfunktionen */

// Clonk anhalten
global func StopClonk(object target)
{
	if (!target)
		if (!this)
			return false;
	SetComDir(COMD_Stop, target);
	SetCommand(target, "None");
	Call("~StopAiming");
	return true;
}
  
// Clonk für Dialog vorbereiten: Anhalten, Menüs schließen, etc.
// Aber nicht aufhören zu reiten!
global func StopClonkEx(object target, bool keep_action)
{
	if (!target)
		if (!this)
			return;
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
	StopClonk(target);
	CloseMenu(target);
	ComboCloseMenu(target);
	CloseCharMenu(target);
}

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

