/* Overloaded functions */

#strict

/* Dinge mit verschoben erstellen, damit es mit den Häuser-Innenansichten kompatibel ist */
// Partikel verschoben erzeugen
global func CreateParticle (szName, int iX, int iY, int iXDir, int iYDir, int a, b, pObj, bool fBehind)
{
	var pLayer = GetObjectLayer();
	// irgendwie mochte er mein && nicht... pLayer war 0 und er hat trotzdem alle Aufrufe getätigt
	if (!pObj)
	if (pLayer)
	if (pLayer->~IsInteriorOfBuilding())
	{
		var pDouble = LocalN("dummyObjectOutside", pLayer);
		if( pDouble != this)
		{
			var X = GetX()-pLayer->GetX();
			var Y = GetY()-pLayer->GetY();
			iX += X;
			iY += Y;
			return pDouble->CreateParticle(szName, iX, iY, iXDir, iYDir, a, b, pDouble, 0);
		}
	}
	return _inherited(szName, iX, iY, iXDir, iYDir, a, b, pObj, fBehind);
}

// Objekte beim erzeugen gleich zum Haus hinzufügen
global func CreateObject (id id, int iXOffset, int iYOffset, int iOwner)
{
	var pObj = _inherited(id, iXOffset, iYOffset, iOwner);
	if (!pObj) return;

	var pLayer = GetObjectLayer(pObj);
	if( pLayer) pLayer->~AddToInterior(pObj);
	return pObj;
}

// Inhaltsobjekte auch beim Haus hinzufügen
global func CreateContents (id id, object pObj, int iCount)
{
	var pObj, pLayer;
	if(!iCount) iCount = 1;
	pLayer = GetObjectLayer(pObj);

	for(var i = 0; i < iCount; i++)
	{
		pObj = _inherited(id, pObj);
		if(!pObj) return;
		if(pLayer) pLayer->~AddToInterior(pObj);
	}

	return pObj;
}

global func Message (string strMessage, object pObj)
{
	if(pObj)
	{
		var pLayer = pObj->~GetObjectLayer();
		if(pLayer) if(pLayer->~IsInteriorOfBuilding())
		{
			var pDouble = LocalN("dummyObjectOutside", pLayer);
			if( pDouble != this)
			{
				var X = GetX()-pLayer->GetX();
				var Y = GetY()-pLayer->GetY();
				return CustomMessage (strMessage, pDouble, -1, X, Y+pLayer->GetDefHeight()/2-pObj->GetDefHeight()/2, 0, 0, 0, MSG_Multiple);
			}
		}
	}
	return _inherited(strMessage, pObj, ...);
}

global func Sound (string szSound, bool fGlobal, object pObj, int iVolume, int iPlayer, int iLoopCount)
{
	if(!pObj) pObj = this;

	if (pObj)
	{
		var pLayer = pObj->~GetObjectLayer();
		if(pLayer) if (pLayer->~IsInteriorOfBuilding())
		{
			return _inherited(szSound, fGlobal, LocalN("dummyObjectOutside", pLayer), iVolume, iPlayer, iLoopCount, ...);
		}
	}
	return _inherited(szSound, fGlobal, pObj, iVolume, iPlayer, iLoopCount, ...);
}

/* Ansicht verschieben, damit die auf die Häuser-Innenansicht so aussieht, als wäre sie an derselben Stelle wie das Haus */

static const gScreenFader_EffectVar_RGBa_R = 1;
static const gScreenFader_EffectVar_RGBa_G = 2;
static const gScreenFader_EffectVar_RGBa_B = 3;
static const gScreenFader_EffectVar_Ramp = 5;
static const gScreenFader_EffectVar_Owner = 6;
static const gScreenFader_EffectVar_Background = 7;
static const gScreenFader_EffectVar_Foreground = 8;

static gSetViewOffset_Last_X;
static gSetViewOffset_Last_Y;
static const gSetViewOffset_EffectVar_X = 0;
static const gSetViewOffset_EffectVar_Y = 1;
static const gSetViewOffset_EffectVar_Active = 2;
static const gSetViewOffset_EffectVar_PermanentX = 3;
static const gSetViewOffset_EffectVar_PermanentY = 4;
static const gSetViewOffset_EffectVar_HelpObject = 5;


global func SetViewOffset (int iPlayer, int iX, int iY, bool noSave)
{
	if(GetType(gSetViewOffset_Last_X) != C4V_Array) gSetViewOffset_Last_X = [];
	if(GetType(gSetViewOffset_Last_Y) != C4V_Array) gSetViewOffset_Last_Y = [];

	if(!noSave)
	{
		gSetViewOffset_Last_X[iPlayer] = iX;
		gSetViewOffset_Last_Y[iPlayer] = iY;
	}

	return _inherited(iPlayer, iX, iY);
}

global func SetObjectViewOffset(object pObj, int iX, int iY, bool permanent)
{
	if(!pObj) pObj = this;
	if(!pObj) return false;

	var effectName = "IntViewOffset";

	var effect = GetEffect(effectName, pObj);

	if(!effect) effect = AddEffect(effectName, pObj, 1, 1);
	if(!effect) return false;

	if(!permanent)
	{
		EffectVar(gSetViewOffset_EffectVar_X, pObj, effect) = iX;
		EffectVar(gSetViewOffset_EffectVar_Y, pObj, effect) = iY;
	}
	else
	{
		EffectVar(gSetViewOffset_EffectVar_PermanentX, pObj, effect) = iX;
		EffectVar(gSetViewOffset_EffectVar_PermanentY, pObj, effect) = iY;
	}

	// Es soll ein Update des Offsets erfolgen!
	EffectVar(gSetViewOffset_EffectVar_Active, pObj, effect) = 0;

	// nicht benötigte Effekte wieder entfernen
	if( !EffectVar(gSetViewOffset_EffectVar_PermanentX, pObj, effect) &&
		!EffectVar(gSetViewOffset_EffectVar_PermanentY, pObj, effect) &&
		!EffectVar(gSetViewOffset_EffectVar_X, pObj, effect) &&
		!EffectVar(gSetViewOffset_EffectVar_Y, pObj, effect))
	{
		Log("Remove changed effect");
		RemoveEffect(effectName, pObj);
		return false;
	}

	return true;
}

global func FxIntViewOffsetTimer(object target, int effect, int time)
{
	var owner = GetOwner(target);

	if(!EffectVar(gSetViewOffset_EffectVar_HelpObject, target, effect))
		EffectVar(gSetViewOffset_EffectVar_HelpObject, target, effect) = CreateObject(TIM1,0,0,-1);

	if(EffectVar(gSetViewOffset_EffectVar_HelpObject, target, effect))
	{
		SetPosition( GetX(target) + EffectVar(gSetViewOffset_EffectVar_X, target, effect) + EffectVar(gSetViewOffset_EffectVar_PermanentX, target, effect),
				     GetY(target) + EffectVar(gSetViewOffset_EffectVar_Y, target, effect) + EffectVar(gSetViewOffset_EffectVar_PermanentY, target, effect),
					 EffectVar(gSetViewOffset_EffectVar_HelpObject, target, effect));

		if (GetCursor(owner) == target)
			SetPlrView(owner, EffectVar(gSetViewOffset_EffectVar_HelpObject, target, effect));
	}

//	if (GetCursor(owner) == target && !EffectVar(gSetViewOffset_EffectVar_Active, target, effect))
//	{
//		EffectVar(gSetViewOffset_EffectVar_Active, target, effect) = 1;
//		SetViewOffset(owner, EffectVar(gSetViewOffset_EffectVar_X, target, effect) + EffectVar(gSetViewOffset_EffectVar_PermanentX, target, effect),
//							 EffectVar(gSetViewOffset_EffectVar_Y, target, effect) + EffectVar(gSetViewOffset_EffectVar_PermanentY, target, effect), true);
//	}
//	else if(GetCursor(owner) != target && EffectVar(gSetViewOffset_EffectVar_Active, target, effect))
//	{
//		EffectVar(gSetViewOffset_EffectVar_Active, target, effect) = 0;
//		ResetViewOffset(owner);
//	}
}

global func FxIntViewOffsetStop(object target, int effect, int reason, bool temp)
{
	if(temp) return;
	ResetViewOffset(GetOwner(target));
}

global func ResetViewOffset(int player)
{
	if(!gSetViewOffset_Last_X || !gSetViewOffset_Last_Y) return SetViewOffset(player, 0, 0, false);
	return SetViewOffset(player, gSetViewOffset_Last_X[player], gSetViewOffset_Last_Y[player], true, true);
}
