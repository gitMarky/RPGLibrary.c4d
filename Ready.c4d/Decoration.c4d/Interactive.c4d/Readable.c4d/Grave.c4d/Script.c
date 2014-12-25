/*-- Last resting place. Beware of ghosts.
@title Grave
@author Randrian ({@link http://clonkforge.net/pr.php?pr=1415})
@version 0.2.0
--*/

#strict 2

#include LF_R

protected func ControlUp(object controller)
{
	[$CtrlRead$]
	StartDialogue(controller);
	return true;
}

protected func Initialize()
{
	var text_contents = [
		DlgOption(0, -1)->Text("$GraveEmpty$"),
		DlgOptionCancelAlways()
	];

	SetDialogue(text_contents);
}

public func Collection2(object corpse)
{
	var inscription;
	
	var i = Random(3);
	
	     if (i == 0) inscription = "$Inscription0$";
	else if (i == 1) inscription = "$Inscription1$";
	else if (i == 2) inscription = "$Inscription2$";
	
	inscription = Format(inscription, corpse->GetName());

	var text_contents = [
		DlgOption(0, -1)->Text(inscription),
		DlgOptionCancelAlways()
	];

	SetDialogue(text_contents);
}


public func GetDlgMsgColor()
{
	return 10778899;
}
