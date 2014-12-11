/* Message Box functions */

#strict 2

global func MessageBoxGetTargetByPlayer(object player)
{
	var message_target = GetCursor(GetController(player));
	if (!message_target) message_target = player;
	return message_target;
}

global func MessageBox(object player, string message, object speaker, string portrait, bool display_as_message, bool is_permanent, array text_style)
{
	// Defaults
	if (!player) if (!(player = this)) return false;
	if (!speaker) if (!(speaker = this)) return false;
	if (!message) return false;
	if (!portrait) if (!(portrait = speaker->~GetMsgPortrait())) portrait = "1";
	var message_target = MessageBoxGetTargetByPlayer(player);
	
	// Determine portrait
	var char, i, portrait_string = "";
	
	if (GetChar(portrait) == GetChar("@"))
	{
		// @Portrait: get from this definition
		while (char = GetChar(portrait, ++i)) portrait_string = Format("%s%c", portrait_string, char);
		portrait = Format("Portrait:%i::%x::%s", ID_Helper_DefaultPortrait, GetColorDw(speaker), portrait_string);
	}
	else
	{
		var id = GetPortrait(speaker, 1);
		if (!id) id = GetID(speaker);
		// Some clonks do not have their own portrait, so they use defaults
		if (id == ACLK || id == _ACK)
			id = CLNK;
		if (id == WDSK)
			id = MAGE;
		// Otherwise get it from the speaker
		portrait = Format("Portrait:%i::%x::%s", id, GetColorDw(speaker), portrait);
	}
	var speaker_name_length = 0;
	while (GetChar(GetName(speaker), speaker_name_length)) ++speaker_name_length;
	
	// Determine color
	var color = GetSpeakerColor(speaker);
	// MenuDecoration
	var menu_deco = ID_MenuDeco_Default;
	var menu_caption = "";
	if (!(menu_deco = (speaker->~GetDlgMsgDeco()))) menu_deco = ID_MenuDeco_Default;
	if (GetType(text_style) == C4V_Array)
	{
		if (text_style[gTextStyle_ARRAYPOS_MenuDeco]) menu_deco = text_style[gTextStyle_ARRAYPOS_MenuDeco];
		if (text_style[gTextStyle_ARRAYPOS_MenuCaption]) menu_caption = text_style[gTextStyle_ARRAYPOS_MenuCaption];
	}
	// How to display?
	if (display_as_message)
	{
		CloseMenu(player);
		// Message as custom message, do not stop the player
		CustomMessage(Format("<c %x>%s:</c> %s", color, GetName(speaker), message), 0, GetController(player), 0, 35, 0xFFFFFF, menu_deco, portrait, MSG_HCenter | MSG_Top);
	}
	else
	{
		// Message as menu
		// The player has to stop or else he runs into oblivion
		StopClonk(player, true);
		if (!CreateMenu(ID_MenuIcon_Default, message_target, this, 0, menu_caption, 0, C4MN_Style_Dialog, is_permanent, ID_MenuIcon_Default)) return false;
		if (GetType(text_style) == C4V_Array)
		{
			var display_name, text_color, display_instantly;
			
			display_name = text_style[gTextStyle_ARRAYPOS_Name];
			text_color = text_style[gTextStyle_ARRAYPOS_Color];
			display_instantly = text_style[gTextStyle_ARRAYPOS_Instant];
			if (text_style[gTextStyle_ARRAYPOS_Portrait])
				portrait = text_style[gTextStyle_ARRAYPOS_Portrait];
			
			var text = message;
			if (text_color)
				text = ColorizeString(text, text_color);
			if (display_name)
				text = Format("<c %x>%s:</c> %s", color, GetName(speaker), text);

			AddMenuItem(portrait, "", NONE, message_target, 0, 0, "", C4MN_Add_ImgTextSpec, 0, 0);
			AddMenuItem(text, "", NONE, message_target, 0, 0, "", C4MN_Add_ForceNoDesc, 0, 0);
			
			if (display_instantly)
			{
				var text_length = 0;
				while (GetChar(text, text_length)) ++text_length;
				speaker_name_length += text_length;
			}
		}
		else
		{
			AddMenuItem(portrait, "", NONE, message_target, 0, 0, "", C4MN_Add_ImgTextSpec, 0, 0);
			AddMenuItem(Format("<c %x>%s:</c> %s", color, GetName(speaker), message), "", NONE, message_target, 0, 0, "", C4MN_Add_ForceNoDesc, 0, 0);
		}
		if (menu_deco == NONE || menu_deco == -1)
		{
			SetMenuDecoration(0, message_target);
		}
		else
		{
			SetMenuDecoration(menu_deco, message_target);
		}
		
		SetMenuTextProgress(speaker_name_length + 1, message_target);
	}
	
	return true;
}
  
global func MessageBoxAddText(object player, string text)
{
	var pMsgTarget = MessageBoxGetTargetByPlayer(player);
	return AddMenuItem(text, "", NONE, pMsgTarget, 0, 0, 0, C4MN_Add_ForceNoDesc);
}

global func MessageBoxAddOption(object player, id icon, string option, string command, param, int extra, xpar1, xpar2)
{
	// Optionen
	extra |= C4MN_Add_ForceNoDesc;
	var pMsgTarget = MessageBoxGetTargetByPlayer(player);
	return AddMenuItem(option, command, icon, pMsgTarget, 0, param, 0, extra, xpar1, xpar2);
}