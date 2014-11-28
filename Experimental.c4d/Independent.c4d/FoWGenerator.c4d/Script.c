/*-- FoW-Generator --*/

#strict 2

// TODO: translate

local w,h; // Breite/Höhe des Objektes
local iHiddenObjCnt; // Anzahl unsichtbarer Objekte (in Local(i))

static const FOW_PlayerSearchRangeX = 120, // Zu w/h dazugezählte Suchreichweite für Clonks,
              FOW_PlayerSearchRangeY = 120; //   die den FoW entfernen

protected func Initialize()
{
	// Sicherheitshalber: FoW-Generator für alle Spieler
	SetOwner(-1);
	// Anderes Placement-Objekt gefunden? Dann platzieren
	var pOther;
	if (pOther = FindObject(GetID(), 0, 0, 0, 0, 0, "Placement"))
	{
		ShadowInit(GetX(pOther), GetY(pOther), GetX(), GetY());
		return RemoveObject(pOther);
	}
	// Ansonsten Platzierung starten
	return SetAction("Placement");
}

public func ShadowInit(int x1, int y1, int x2, int y2)
{
	// Positionieren
	w = Abs(x2 - x1);
	h = Abs(y2 - y1);
	w = h = (w + h) / 2; // Solange FoW immer quadratisch ist, besser gleich breit wie hoch...
	SetPosition((x1 + x2) / 2, (y1 + y2) / 2);
	SetShape(w / -2, h / -2, w, h);
	// Starte deaktiviert und sichtbar
	SetAction("Passive");
	MakeVisible();
}
  
public func MakeVisible(bool fShowRange)
{
	SetVisibility(VIS_All);
	SetShape(w / -2, h / -2, w, h);
	if (fShowRange)
	{
		var w2 = w + FOW_PlayerSearchRangeX * 2;
		var h2 = h + FOW_PlayerSearchRangeY * 2;
		SetObjDrawTransform(w2 * 1000, 0, w2 * w * 500, 0, h2 * 1000, h2 * h * 500);
		SetClrModulation(RGB(0, 255, 0));
	}
	else
	{
		SetObjDrawTransform(w * 1000, 0, w * w * 500, 0, h * 1000, h * h * 500);
		SetClrModulation(RGB(255, 0, 0));
	}
}
  
protected func UpdateTransferZone()
{
	if (GetVisibility() && GetVisibility() != VIS_None)
		SetVisibility(VIS_None);
	SetShape(w / -2, h / -2, w, h); // h4x
	if (GetAction() == "Active")
		SetPlrViewRange(Min((w + h) / -2 + 40, -1));
}
  
public func MakeInvisible()
{
	SetVisibility(VIS_None);
	SetClrModulation();
	SetShape(-1, -1, 2, 2);
	SetObjDrawTransform(1000, 0, 0, 0, 1000, 0);
}
  
public func Activate()
{
	// Macht das Objekt "Scharf": FoW wird gesetzt, Objekte unterhalb deaktiviert
	// Mehrfache Aufrufe sind OK; zusätzlich gefundene Objekte werden dann aufgenommen
	SetAction("Active");
	var pObj, pObjExclude;
	while (pObj = FindObject(0, w / -2, h / -2, w, h, 0, 0, 0, 0, pObjExclude))
		if (GetID(pObj) != _WSC && GetID() != CHST && GetID(Contained()) != CHST)
			// Schilder, Kisten und Gegenstände darin überspringen
			SetObjectStatus(C4OS_INACTIVE, Local(iHiddenObjCnt++) = pObj);
		else
			pObjExclude = pObj; // Beim Schild weitersuchen
	// FoW generieren
	SetPlrViewRange(Min((w + h) / -2 + 40, -1));
}
  
public func Deactivate()
{
	// Entschärft das Objekt: FoW wird entfernt und Objekte eingefadet
	SetAction("Passive");
	var pObj;
	while (iHiddenObjCnt)
		if (pObj = Local(--iHiddenObjCnt))
		{
			if (GetObjectStatus(pObj) != C4OS_INACTIVE)
				continue;
			SetObjectStatus(C4OS_NORMAL, pObj);
			// Callback ans Objekt für spezifische Reinitialisierungen
			pObj->~OnReactivate();
		}
	SetPlrViewRange();
}
  
  
// Clonksuche zum Entfernen des FoW
protected func CheckClonk()
{
	var o;
	while (o = FindObject
		(
			0,
			w / -2 - FOW_PlayerSearchRangeX,
			h / -2 - FOW_PlayerSearchRangeY,
			w + FOW_PlayerSearchRangeX * 2,
			h + FOW_PlayerSearchRangeX * 2,
			OCF_CrewMember,
			0,
			0,
			NoContainer(),
			o
		))
		if (GetController(o) >= 0)
		{
			Deactivate();
			// 2do: Callback ans Szenarienscript für spezielle Bereiche?
			//return RemoveObject();
		}
	
	if (!o)
		Activate();
}



// Debug-Funktionen
global func FOW_Show()
{
	var o;
	while (o = FindObject(_FOW, 0, 0, 0, 0, 0, 0, 0, 0, o))
		if (GetAction(o) != "Placement")
			o->MakeVisible();
}
  
global func FOW_ShowRange()
{
	var o;
	while (o = FindObject(_FOW, 0, 0, 0, 0, 0, 0, 0, 0, o))
		if (GetAction(o) != "Placement")
			o->MakeVisible(true);
}
  
global func FOW_Hide()
{
	var o;
	while (o = FindObject(_FOW, 0, 0, 0, 0, 0, 0, 0, 0, o))
		if (GetAction(o) != "Placement")
			o->MakeInvisible();
}
  
global func FOW_Activate()
{
	var i;
	var o;
	while (o = FindObject(_FOW, 0, 0, 0, 0, 0, 0, 0, 0, o))
		if (GetAction(o) != "Placement")
			Schedule(Format("Object(%d)->Activate()", ObjectNumber(o)), i++ % 20 + 1); // Verzögert aktivieren, damit FindObject-Schleifen nicht alle gleich laufen
}
  
global func FOW_Deactivate()
{
	var o;
	while (o = FindObject(_FOW, 0, 0, 0, 0, 0, 0, 0, 0, o))
		if (GetAction(o) != "Placement")
			o->Deactivate();
}


