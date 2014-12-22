/*-- Last resting place. Beware of ghosts.
@title Grave
@author Randrian ({@link http://clonkforge.net/pr.php?pr=1415})
@version 0.2.0
--*/

#strict 2

protected func ControlUp(object pPlayer)
{
	[$CtrlRead$]
	var ghost = Contents();
	
	// Spieler vorbereiten (Action halten, weil Push)
	var sText = Format("Hier ruht in Frieden %s", GetName(ghost));
	if (!ghost)
		sText = "Das Grab ist leer.";
	var szPortrait = Format("Portrait:%i::%x::%s", GetID(), GetColorDw(), "1");
	CreateMenu
	(
		GetID(),
		pPlayer,
		this,
		0,
		Format("<c %x>%s:</c>", GetColorDw(), GetName()),
		0,
		C4MN_Style_Dialog
	);
	AddMenuItem(szPortrait, 0, NONE, pPlayer, 0, 0, 0, 5);
	AddMenuItem(sText, 0, NONE, pPlayer);
	AddMenuItem("Abbrechen", "Stop", MCMX, pPlayer, 0, pPlayer);
	return 1;
}

public func GetDlgMsgColor()
{
	return 10778899;
}
