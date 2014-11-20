/*--
A grabbable object that looks like a signpost. It can have a dialogue that is displayed upon pressing ControlUp.

@title Signpost
@author Sven2 (Graphics), Marky (SCript)
@version 0.1.0
--*/

#strict 2

#include LF_R // is readable

/**
 * Starts the dialogue.
 *
 * @param pPlayer The object that activated the book. The dialogue is displayed to this object.
 */ 
protected func ControlUp(object pPlayer)
{
	[$CtrlRead$]
	

	SetDialogue
	(
		[
			[0, -1, 0, "Dies ist die Erste Seite des Buches", 0, MCMC],
			[1, 0, "Zu S.2", "Dies ist die zweite Seite des Buches", 0, MCM2],
			[2, [0, 1], "Zu S.3", "Dies ist die dritte Seite des Buches", 0, MCM3],
			[4, 0, "", "Dies ist ein Info-Text", 0, 0],
			[5, 2, "Genauer Untersuchen", "Hier gibt der Clonk einen Kommentar ab!", -1, [MCMQ]],
			[
				6,
				[0, 1, 2, 5],
				"Zu S.4",
				"Dies ist die vierte Seite des Buches. Außerdem wird es hier farbig und sofort angezeigt|Eine Sensation, bei so einem langen Text",
				0,
				[MCM4],
				[true, RGB(255, 0, 0), true]
			],
			[
				3,
				[1, 2, 6],
				"Zurück zu S.1",
				"Dieser Text erscheint nicht, wir springen direkt zu Seite 1!",
				0,
				[MCMC, 0, 0, 0, 0, 1]
			],
			[
				18,
				-1,
				"Gib mir einen Flint",
				"Hier hast Du ihn",
				0,
				[FLNT],
				0,
				"!FindContents(FLNT,pTarget)",
				"CreateContents(FLNT,pTarget)"
			],
			[
				19,
				-1,
				"Hier hast Du den Flint zurück",
				"Danke",
				0,
				[FLNT],
				0,
				"FindContents(FLNT,pTarget)",
				"!FindContents(FLNT,pTarget) || RemoveObject(FindContents(FLNT,pTarget));"
			],
			[7, -1, "Schließen", "", 0, [MCMX, 0, 0, 0, 0, -1], -1, 0, "CloseMenu(pTarget)"]
		]
	);
	

	StartDialogue(pPlayer);
	return 1;
}

/**
 * Has a default color, override this method if you want a different color.
 */
public func GetDlgMsgColor()
{
	return 10778899;
}
