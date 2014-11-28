/*-- Switching a scenario section --*/

#strict 2


static g_iSaveSectIndex;


// Crewobjekt nach Namen suchen - Spielerclonks ignorieren!
global func FindCrewByName(id idCrew, string sName)
{
	var pObj;
	while (pObj = FindObject(idCrew, 0, 0, 0, 0, 0, 0, 0, 0, pObj))
		if (pObj->GetName() == sName)
			if (GetOwner(pObj) == NO_OWNER)
				return pObj;
}
  

global func SaveObjects(fAdjustRespawn)
{
	for(var obj in FindObjects(Find_InRect(-GetX(),-GetY(),LandscapeWidth(),LandscapeHeight())))
		obj->~OnSave();
	GameCall("OnSave");
	
  // Spieler sichern
  var iCount, obj, container;
  for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
  {
    iCount = GetCrewCount(GetPlayerByIndex(i,C4PT_User));
    while (iCount--) {
      obj = GetCrew(GetPlayerByIndex(i,C4PT_User), iCount);
			if(!obj->GetAlive())
				obj->SetAlive(1);
      if( fAdjustRespawn)
      {
        Local(10, obj) = GetX(obj);
        Local(11, obj) = GetY(obj);
				Local(12, obj) = GetEnergy(obj);
				Local(13, obj) = GetWealth(GetOwner(obj));
				Local(14, obj) = GetAction(obj);
				Local(15, obj) = GetPhase(obj);
				Local(16, obj) = GetDir(obj);
				Local(20, obj) = ObjectNumber(obj->GetActionTarget());
				Local(18, obj) = obj->~GetHideValue();
				Local(19, obj) = obj->~IsDisguised();
				if(GetEffect("CarryControlCorps", obj))
				{
					var iEff = GetEffect("CarryControlCorps", obj);
					Local(21, obj) = [EffectVar(0, obj, iEff), EffectVar(1, obj, iEff), EffectVar(2, obj, iEff)];
				}
				else
					Local(21, obj) = 0;
				if(Contained(obj))
					Local(17, obj) = ObjectNumber(Contained(obj));
				else Local(17, obj) = 0;
				container = obj->CreateObject(CXCN, 0, 0, -1);
				while(Contents(0, obj)) Enter(container, Contents(, obj));
				SetName(GetName(obj), container);
      }
      else
			{
				if(Contained(obj)) obj->Exit();
				obj->~UnHide();
				while(Contents(0, obj)) Exit(Contents(, obj));
			}
      Exit(obj);
//      ObjectSetAction(obj, "Walk");
      SaveSectObj(obj);
      SetPosition(100, 100, obj);
    }
  }
  if(this) SaveSectObj(this);
}


global func SaveSectObj(object obj)
  {
  // Kein Objekt?
  if(!obj) return;
  if(GetID(obj) == WCHR) return; // Bleibt da!
	if(GetID(obj) == _CYH) return;
  // Hatten wir das Objekt schon? (Endlosrekursionen sollen vermieden werden)
  var pTest, i;
  while(pTest = Global(20+(i++)*3)) if(pTest == obj) return;
  // Inhalt sichern
  var o2,i=obj->ContentsCount(); while (o2=obj->Contents(--i)) SaveSectObj(o2);
  // Attachte Objekte sichern (Schilde, Amulette, ect...)
  o2 = 0;
  for(o2 in FindObjects(Find_ActionTarget(obj)))
  {
    // Nur wirklich attachte Objekte (z.B. keine kämpfenden)
    if(GetProcedure(o2)=="ATTACH")
      SaveSectObj(o2);
  }
  // Objekt sichern
	Global(20+g_iSaveSectIndex) = [];
	Global(20+g_iSaveSectIndex)[1] = GetX(obj);
  Global(20+g_iSaveSectIndex)[2] = GetY(obj);
  (Global(20+(g_iSaveSectIndex++))[0] = obj)->SetObjectStatus(2);
  }

global func RestoreSectObjs(fReInitPlayer)
  {
	for(var obj in FindObjects(Find_InRect(-GetX(),-GetY(),LandscapeWidth(),LandscapeHeight())))
		obj->~OnRestore();
	GameCall("OnRestore");

  var obj;
  // Alles gespeicherte wieder herstellen
	var i = g_iSaveSectIndex;
  if (g_iSaveSectIndex)
	{
    while (i--)
      if (obj=Global(20+i)[0])
        obj->SetObjectStatus(1);
		i = g_iSaveSectIndex;
		while (i--)
		{
      if (obj=Global(20+i)[0])
			{
				SetPosition(Global(20+i)[1], Global(20+i)[2], obj);
				Global(20+i)=0;
			}
		}
		g_iSaveSectIndex=0;
	}

  // Spielerinventar wiederherstellen
  var iCount, obj, container;
  for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
  {
		var iPlr = GetPlayerByIndex(i,C4PT_User);
		ClearLastPlrCom(iPlr);
    iCount = GetCrewCount(iPlr);
    while (iCount--) {
      obj = GetCrew(GetPlayerByIndex(i,C4PT_User), iCount);
      container = FindCrewByName(CXCN, GetName(obj));
      if(container)
			{
				while(Contents(0, container)) Enter(obj, Contents(, container));
				RemoveObject(container);
			}
			obj->DoLaunch(obj);
    }
//    if(fReInitPlayer) GameCall("PlayerStart", GetPlayerByIndex(i,C4PT_User));
  }
  if(fReInitPlayer) StartSzen();
  }

