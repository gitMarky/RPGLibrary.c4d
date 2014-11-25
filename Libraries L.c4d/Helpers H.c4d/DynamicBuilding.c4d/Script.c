#strict 2

/*
 * Overloaded.
 */
global func SetCategory(iCategory, pObj)
{
	if(!pObj) pObj = this;
	if(pObj->GetObjectLayer() && pObj->GetObjectLayer()->~IsDynamicBuilding() && !(iCategory & C4D_Foreground))
		iCategory = iCategory | C4D_Foreground;
	return inherited(iCategory, pObj);
}


/*
 * Variables.
 */

local dummyObjectOutside;
local savedInteriorObjects; 	// Array
local graphicsCategoryArrays; 	// Array
local isVisibleForPlayer; 		// Array

static gDynamicBuilding_CategoryArray;

public func InitializeVariables()
{
	if(!gDynamicBuilding_CategoryArray)
	{
		gDynamicBuilding_CategoryArray = [C4D_StaticBack, C4D_Structure, C4D_Vehicle, C4D_Living, C4D_Object];
	}

	savedInteriorObjects = [];
	graphicsCategoryArrays = [];
	isVisibleForPlayer = [];

	for(var i = 0; i < GetLength(gDynamicBuilding_CategoryArray) + 2; i++)
	{
		graphicsCategoryArrays[i] = [];
	}
}

/*
 * This initializes the interior objects
 */
public func MakeDynamicBuilding()
{
	dummyObjectOutside = CreateObject(GetID(), 0, 0, -1);
	dummyObjectOutside -> SetPosition(GetX(),GetY());
	dummyObjectOutside -> SetGraphics("Inside");

	SetGraphics("Inside");
	SetCategory(C4D_StaticBack);
	//SetPosition(GetX(), 100 - (GetDefBottom()-GetY())); // Setzt das Objekt nach oben, sodass die Unterkante bei 100 ist
	//SetSolidMask(60,0,60,50);

	InitializeVariables();
	UpdateVisibilityForPlayers();

	InitializeInteriorObjects();
	InitializeEffects();
}

/*
 * Overload this for creating custom interior objects or decoration.
 */
public func InitializeInteriorObjects()
{

}

private func InitializeEffects()
{
	if(!GetEffect("IntUpdateGraphics", this))
		AddEffect("IntUpdateGraphics", this, 1, 1, this);

	if(!GetEffect("IntMonitorEntrance", this))
		AddEffect("IntMonitorEntrance", this, 1, 1, this);

	if(!GetEffect("IntMonitorEntrance", dummyObjectOutside))
		AddEffect("IntMonitorEntrance", dummyObjectOutside, 1, 1, this);
}

/*
 * Handles all the variables when 'entering' a building
 */
public func AddToInterior(object pObj)
{
 	if(!pObj || pObj == this) return;
 	for(var object in savedInteriorObjects) if(object == pObj) return;

 	var layerObject = GetObjectLayer(pObj);

	if(layerObject && layerObject != this && layerObject->~IsDynamicBuilding())
	{
		layerObject->RemoveObjectFromInterior(pObj, 1);
	}

	var i = -1;
	while(Contents(++i, pObj)) AddToInterior(Contents(i, pObj));
	for(var attachObj in FindObjects(Find_ActionTarget(pObj))) if(GetProcedure(attachObj) == "ATTACH") AddToInterior(attachObj);
	pObj->~HandleAddToInterior(this); // damit sonstige Dinge wie Alchemiebeutel oder so auch übergeben werden können

	// Layer und Kategorie setzen
	SetObjectLayer(this, pObj);

	// freien Platz im Array suchen
	var freeIndex = -1;
	while(savedInteriorObjects[++freeIndex]);

	savedInteriorObjects[freeIndex] = pObj;
	var categoryIndex = -1;
	// für jede Objektkategorie darin gibt es ein Array
	// alle Objekte, die keine der Kategorien haben, werden noch eine Ebene höher gezeichnet
	// und ganz am Ende kommen Vordergrundobjekte
	while(!(pObj->GetCategory() & gDynamicBuilding_CategoryArray[++categoryIndex]));
	if(pObj->GetCategory() & C4D_Foreground) categoryIndex = GetLength(gDynamicBuilding_CategoryArray) + 1;

	// Objekt in das Grafik-Kategorie-Array einfügen
	var freeIndex = -1;
	while(graphicsCategoryArrays[categoryIndex][++freeIndex]);
	graphicsCategoryArrays[categoryIndex][freeIndex] = pObj;

	// Position anpassen
	var offsetX = GetX(pObj)-GetX(dummyObjectOutside);
	var offsetY = GetY(pObj)-GetY(dummyObjectOutside);
	SetPosition(GetX()+offsetX, GetY()+offsetY, pObj);

	// Sicht für den Spieler anpassen
	if(IsUserCrewMember(pObj))
	{
		SetObjectViewOffset(pObj,GetX(dummyObjectOutside)-GetX(), GetY(dummyObjectOutside)-GetY(), true);

		SetObjScreenModulation(pObj, RGBa(0,0,0,180), 0, gScreenModulation_RampBuilding, 0);

	    isVisibleForPlayer[GetOwner(pObj)]++;
	    UpdateVisibilityForPlayers();
	}
}

/*
 * Handles all the variables when 'leaving' a building
 */
public func RemoveObjectFromInterior(pObj)
{
 	if(!pObj || pObj == this) return;
 	var index = GetArrayItemPosition(pObj, savedInteriorObjects);
 	if(index < 0) return;

	var i = -1;
	while(Contents(++i, pObj)) RemoveObjectFromInterior(Contents(i, pObj));
	for(var attachObj in FindObjects(Find_ActionTarget(pObj))) if(GetProcedure(attachObj) == "ATTACH") RemoveObjectFromInterior(attachObj);
	pObj->~HandleRemoveFromInterior(this); // damit sonstige Dinge wie Alchemiebeutel oder so auch übergeben werden können


	// Layer und Kategorie zurücksetzen
	SetObjectLayer(0, pObj);

	// Aus der Liste löschen
	savedInteriorObjects[index] = 0;

	// Ebenso aus der Grafikliste löschen
	for(var categoryIndex = 0; categoryIndex < GetLength(graphicsCategoryArrays); categoryIndex++)
	{
		var j = GetArrayItemPosition(pObj, graphicsCategoryArrays[categoryIndex]);
		if(j >= 0) graphicsCategoryArrays[categoryIndex][j] = 0;
	}

	// Position anpassen
	var offsetX = GetX(pObj)-GetX();
	var offsetY = GetY(pObj)-GetY();
	SetPosition(GetX(dummyObjectOutside)+offsetX, GetY(dummyObjectOutside)+offsetY, pObj);

	// Sicht für den Spieler anpassen
	if(IsUserCrewMember(pObj))
	{
		SetObjectViewOffset(pObj, 0, 0, true);

		SetObjScreenModulation(pObj, RGBa(0,0,0,255), 0, gScreenModulation_RampBuilding, 0);

	    isVisibleForPlayer[GetOwner(pObj)]--;
	    UpdateVisibilityForPlayers();
	}
}

/*
 * Draws the interior graphics
 */
func FxIntUpdateGraphicsTimer(object pTarget, int iNumber, int iTime)
{
	var i = 1;
	var newIndex = -1;
	var pDummy = pTarget->LocalN("dummyObjectOutside");
	if(!pDummy) { DebugLog("No dummy, removing effect"); return -1;}

	// Alle Objekte innerhalb des Gebäudes beim Objekt selbst und beim Dummy anzeigen
	for(var categoryIndex = 0; categoryIndex < GetLength(graphicsCategoryArrays); categoryIndex++)
	{
		for(var obj in graphicsCategoryArrays[categoryIndex])
		{
			if(!obj || obj == this || obj->Contained()) continue;
			if(obj->GetVisibility() & VIS_OverlayOnly) continue;

			CopyObjectGraphicsToLayer(obj, i, pTarget);
			CopyObjectGraphicsToLayer(obj, i, pDummy);
			i++;
		}
	}

	// Alte Grafiken löschen, beim Objekt selbst und beim Dummy
	if(i < EffectVar(0,pTarget,iNumber))
	{
		newIndex = i;
		while(i < EffectVar(0,pTarget,iNumber)+2)
		{
			SetGraphics(0, pTarget, 0, i, 0);
			SetGraphics(0, pDummy, 0, i, 0);
			i++;
		}
	}
	else
	{
		newIndex = i;
	}

	EffectVar(0,pTarget,iNumber) = newIndex;

	// Der Dummy bekommt einmal die Aussenansicht oben drauf geklebt
	SetGraphics("Outside", pDummy, GetID(pDummy), newIndex, GFXOV_MODE_ExtraGraphics);
	// Und nochmal die Innenansicht, welche nur für alle Leute innerhalb des Gebäudes sichtbar ist.
	CopyObjectGraphicsToLayer(pTarget, newIndex+1, pDummy);
}

public func UpdateVisibilityForPlayers()
{
	SetVisibility(VIS_Local);

	var bitMaskForLocalVisibility = 0;

	for(var i = 0; i < GetLength(isVisibleForPlayer); i++)
	{
		bitMaskForLocalVisibility += (2**i)*isVisibleForPlayer[i];
	}

	Local(0) = bitMaskForLocalVisibility; // für VIS_Local
}

public func FxIntMonitorEntranceTimer(object pTarget)
{
	var entrances = pTarget->~EntranceZonesAndDirections();
	for(var i = 0; i < GetLength(entrances); i++)
	{
	    var aEntrance = entrances[i][0];
	    for(var pObj in FindObjects(aEntrance))
	    {
	        var iDir = entrances[i][1];
	        if(iDir)
	        {
	        	if(iDir == COMD_Up && pObj->GetYDir() >= 0) continue;
	        	if(iDir == COMD_Down && pObj->GetYDir() <= 0) continue;
	        	if(iDir == COMD_Right && pObj->GetXDir() <= 0) continue;
	        	if(iDir == COMD_Left && pObj->GetXDir() >= 0) continue;
	        }
	        if(pTarget->~IsInteriorOfBuilding())
	        	RemoveObjectFromInterior(pObj);
	        else
	        	AddToInterior(pObj);
	    }
	}
}

protected func Collection2(object pObj)
{
	HandleObjectEntered(pObj);
	_inherited(pObj);
}

private func HandleObjectEntered(object pObj)
{
	if(GetProcedure(pObj) == "ATTACH" && IsUserCrewMember(pObj->GetActionTarget())) return;

	if(GetObjectLayer(pObj) == this)
	{
		RemoveObjectFromInterior(pObj);
	}
	else
	{
		AddToInterior(pObj);
	}
}

public func CopyObjectGraphicsToLayer(object obj, int layer, object copyTo)
{
	SetGraphics(0, copyTo, 0, layer, GFXOV_MODE_Object, 0, 0, obj);
	SetObjDrawTransform(1000, 0, (GetX(obj)-GetX())*1000, 0, 1000, (GetY(obj)-GetY())*1000, copyTo, layer);
	if(obj->GetClrModulation())
		SetClrModulation(obj->GetClrModulation(), copyTo, layer);
	else
		SetClrModulation(RGB(255,255,255), copyTo, layer);
}

global func IsObjectInsideBuilding(object obj, object building)
{
	if(!obj) obj = this;
	if(!building) building = this;
	var savedObjects = LocalN("savedInteriorObjects",building);
	if(!savedObjects) return false;

	return (obj->GetObjectLayer() == building) && (GetArrayItemPosition(obj, savedObjects));
}

public func IsInteriorOfBuilding()
{
	return (dummyObjectOutside != 0) && (GetType(savedInteriorObjects) == C4V_Array);
}

//public func EntranceZonesAndDirections()
//{
//	if(IsInteriorOfBuilding())
//		return  [[Find_InRect(-30-10,-25,20,50),COMD_Left],[Find_InRect(+30-10,-25,20,50),COMD_Right]];
//	return [[Find_InRect(-30-10,-25,20,50),COMD_Right],[Find_InRect(+30-10,-25,20,50),COMD_Left]];
//}
