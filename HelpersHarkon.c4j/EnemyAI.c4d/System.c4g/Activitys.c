/* Objekte, denen die KI ausweichen sollte */

#strict 2

#appendto CLNK

local aListActivitys;
local iCurrentActivity;
local iActivityState;

local iSwapTimer;

local iActTimer;

local iActItemState;
local pActTarget;

local iDialogRandom;

static const ACT_Smith = 1;
static const ACT_Chat  = 2;
static const ACT_Bake  = 3;

func ActDialog(szString, pTalker1, pTalker2)
{
	MessageEx(Format(szString, GetName(pTalker2)), pTalker1);
	Message("", pTalker2);
	return 1;
}

func ActGetDialog(idID, iIndex, pTalker1, pTalker2)
{
	if(idID == _CIN)
	{
		if(iIndex == 0)
			return ActDialog("Hey, das war mein Geld!", pTalker1, pTalker2);
		if(iIndex == 1)
			return ActDialog("Pech gehabt, ich war eher dran!", pTalker2, pTalker1);
		if(iIndex == 2)
			return ActDialog("Pass auf, das nächste Mal wenn ich was finde bekomme ich es!", pTalker1, pTalker2);
		return 0;
	}
	if(idID == SWAP)
	{
		if(!iDialogRandom) iDialogRandom = Random(5)+1;
		if(iDialogRandom == 1)
		{
			if(iIndex == 0)
				return ActDialog("Ich bin deine Ablösung, du kannst gehen.", pTalker1, pTalker2);
			if(iIndex == 1)
				return ActDialog("Ah danke! Ich habe schon ewig gewartet.", pTalker2, pTalker1);
			if(iIndex == 2)
				return ActDialog("Kein Problem, dann ruh dich mal aus.", pTalker1, pTalker2);
		}
		if(iDialogRandom == 2)
		{
			if(iIndex == 0)
				return ActDialog("Hey, Ablösung ist da.", pTalker1, pTalker2);
			if(iIndex == 1)
				return ActDialog("Wurde aber auch Zeit. Ich bin weg.", pTalker2, pTalker1);
		}
		if(iDialogRandom == 3)
		{
			if(iIndex == 0)
				return ActDialog("Du bist also meine Ablösung.", pTalker2, pTalker1);
			if(iIndex == 1)
				return ActDialog("Ja, ich passe jetzt hier auf.", pTalker1, pTalker2);
			if(iIndex == 2)
				return ActDialog("Dann sei wachsam!", pTalker2, pTalker1);
		}
		if(iDialogRandom == 4)
		{
			if(iIndex == 0)
				return ActDialog("Ich bin jetzt dran!", pTalker1, pTalker2);
			if(iIndex == 1)
				return ActDialog("Nicht so stürmisch. Ich gehe jetz schon.", pTalker2, pTalker1);
		}
		if(iDialogRandom == 5)
		{
			if(iIndex == 0)
				return ActDialog("Du hast jetzt Feierabend.", pTalker1, pTalker2);
			if(iIndex == 1)
				return ActDialog("Ah sehr schön.", pTalker2, pTalker1);
		}
		return 0;
	}
	if(idID == MORN)
	{
		if(!iDialogRandom) iDialogRandom = Random(5)+1;
		if(iDialogRandom == 1)
		{
			if(iIndex == 0)
				return ActDialog("Oh %s, wie konnte das nur geschehen?", pTalker1, pTalker2);
			if(iIndex == 1)
				return ActDialog("Möge Sawelô sich deiner Seele annehmen", pTalker1, pTalker2);
			if(iIndex == 2)
				return ActDialog("Auf Wiedersehen im Paradis", pTalker1, pTalker2);
		}
		if(iDialogRandom == 2)
		{
			if(iIndex == 0)
				return ActDialog("%s, wir werden dich Rächen!", pTalker1, pTalker2);
			if(iIndex == 1)
				return ActDialog("Wer immer für deinen Tod verantwortlich ist, der muss sterben!", pTalker1, pTalker2);
		}
		if(iDialogRandom == 3)
		{
			if(iIndex == 0)
				return ActDialog("%s ich vermisse dich.", pTalker1, pTalker2);
			if(iIndex == 1)
				return ActDialog("Du warst mir immer ein guter Freund und Helfer.", pTalker1, pTalker2);
			if(iIndex == 2)
				return ActDialog("Ruhe in Frieden.", pTalker1, pTalker2);
		}
		if(iDialogRandom == 4)
		{
			if(iIndex == 0)
				return ActDialog("Eine Schande, dass jemand wie du, %s, so früh von uns gehen musste.", pTalker1, pTalker2);
			if(iIndex == 1)
				return ActDialog("Dein Mörder wird dafür bezahlen, das schwöre ich dir!", pTalker1, pTalker2);
		}
		if(iDialogRandom == 5)
		{
			if(iIndex == 0)
				return ActDialog("Ich finde keine Worte deinen Tod zu betrauern.", pTalker1, pTalker2);
			if(iIndex == 1)
				return ActDialog("Was werde ich nur ohne dich machen?", pTalker1, pTalker2);
		}
		return 0;
	}
}

func ExecMoveTo(iXOffset, iYOffset, iDir)
{
	if(!GetCommand(this) && Abs(GetX()-iXOffset) < 8 && Abs(GetY()-iYOffset) < 20)
	{
		if(iDir != -1)
		return SetDir(iDir) || 1;
		return 1;
	}
	else if(!GetCommand(this) && !GetMacroCommand(this))
		SetMacroCommand("MoveTo", 0, iXOffset, iYOffset);
	return 0;
}

func ExecGrab(pItem, iXOffset, iYOffset)
{
	if(!GetCommand(this) && !GetMacroCommand(this) && NearObj(pItem, iXOffset, iYOffset))
	{
		DoGrab(pItem);
		return 1;
	}
	else if(!GetCommand(this) && !GetMacroCommand(this))
	{
		if(!iXOffset && !iYOffset)
			SetMacroCommand("MoveTo", pItem);
		else
			SetMacroCommand("MoveTo", 0, pItem->GetX()+iXOffset, pItem->GetY()+iYOffset);
	}
	return 0;
}

func ExecWalkTo(pItem, iXOffset, iYOffset)
{
	if(NearObj(pItem, iXOffset, iYOffset))
	{
		return 1;
	}
	else if(!GetCommand(this) && !GetMacroCommand(this))
	{
		if(!iXOffset && !iYOffset)
			SetMacroCommand("MoveTo", pItem);
		else
			SetMacroCommand("MoveTo", 0, pItem->GetX()+iXOffset, pItem->GetY()+iYOffset);
	}
	return 0;
}

func Action_UseEnterItem(pItem, szAction, iWait)
{
	if(ExecGrab(pItem) && iActItemState == 0)
	{
			Enter(pItem);
			pItem->SetColorDw(GetColorDw());
			pItem->SetAction(szAction);
			iActItemState = 1;
			if(iWait) AddEffect("IntWait", this, 1, iWait);
			return 0;
	}
	if(iWait && iActItemState == 1)
	{
		iActItemState = 0;
		return 1;
	}
	if(Contained() != pItem && iActItemState == 1)
	{
		iActItemState = 0;
		return 1;
	}
	return 0;
}

func Action_UseGrabWait(pItem, iWait, aExtra)
{
	if(ExecGrab(pItem))
	{
		if(!iWait) iWait = 35;
		AddEffect("IntWait", this, 1, iWait);
		if(ExtraGetValue(aExtra, 1))
		{
			var obj;
			if(ExtraGetValue(aExtra, 2) > 0)
			{
				for(var i = 0; i < ExtraGetValue(aExtra, 2); i++)
					pItem->CreateContents(ExtraGetValue(aExtra, 1));
			}
			else
				for(var i = 0; i < -ExtraGetValue(aExtra, 2); i++)
					if(obj = FindContents(ExtraGetValue(aExtra, 1), pItem))
					{
						obj->Exit();
						obj->RemoveObject();
					}
		}
		iActItemState = 0;
		return 1;
	}
	return 0;
}

func Action_UseGrabAction(pItem, szAction, szActionEnd, szActionTest, iXOffset)
{
	if(!szActionTest) szActionTest = szAction;
	if(iActItemState == 0)
		if(ExecGrab(pItem, iXOffset))
		{
			pItem->SetAction(szAction);
			iActItemState = 1;
			return 0;
		}
	if(iActItemState == 1)
		if(pItem->GetAction() != szActionTest)
		{
			if(szActionEnd) pItem->SetAction(szActionEnd);
			iActItemState = 0;
			return 1;
		}
	return 0;
}

func ExtraGetValue(aExtra, iIndex, iDefault)
{
	if(GetType(aExtra) == C4V_Array && GetLength(aExtra) > iIndex)
		return aExtra[iIndex];
	return iDefault;
}

func TestActivityPossible(pItem, aExtra)
{
	var idID = NONE;
	if(GetType(pItem) == C4V_C4Object) idID = GetID(pItem);
	if(GetType(pItem) == C4V_C4ID) idID = pItem;
	if(idID == WODC)
	{
		if(ExtraGetValue(aExtra, 1, WOOD) != WOOD) return 0;
//		Log("%d %d %d", ContentsCount(WOOD, pItem),ExtraGetValue(aExtra, 2),ContentsCount(WOOD, pItem)+ExtraGetValue(aExtra, 2));
		if(!Inside(ContentsCount(WOOD, pItem)+ExtraGetValue(aExtra, 2), 0, 25)) return 0;
//		Log("!IsInside");
	}
	if(idID == SWAP)
	{
		if(!aExtra[1] || !aExtra[1]->GetAlive()) return 0;
		var pAct = aExtra[1]->GetActivity();
		if(pAct) return 0;
		if(FrameCounter()-iSwapTimer > 35*60);
			return !Random(500);
	}
	if(idID == DEAD)
	{
		var found = 0;
		for(var pDead in FindObjects(Find_OCF(OCF_Living), Find_Action("Dead"), Find_Not(Find_OCF(OCF_Alive)), Find_Func("IsClonk")))
			if(GetEffect("IntFoundMarker", pDead) && !GetEffect("IntFoundMarker2", pDead))
				return 1;
		return 0;
	}
	if(idID == GRAV)
	{
		var found = 0;
		for(var pDead in FindObjects(Find_OCF(OCF_Living), Find_Action("Dead"), Find_Not(Find_OCF(OCF_Alive)), Find_Func("IsClonk")))
			if(GetEffect("IntFoundMarker2", pDead) && !GetEffect("IntFoundMarkerTimer", pDead))
				return 1;
		return 0;
	}
	return 1;
}

func SwapInArray(aArray, pTarget1, pTarget2)
{
	if(GetType(aArray) == C4V_C4Object)
	{
		if(aArray == pTarget1) return pTarget2;
		if(aArray == pTarget2) return pTarget1;
	}
	if(GetType(aArray) == C4V_Array)
	{
		var new_array = [];
		for(item in aArray)
			PushBack(SwapInArray(item, pTarget1, pTarget2), new_array);
		return new_array;
	}
	return aArray;
}

func SwapLocal(szName, pTarget1, pTarget2)
{
	var swap = SwapInArray(LocalN(szName, pTarget1),  pTarget1, pTarget2);
	LocalN(szName, pTarget1) = SwapInArray(LocalN(szName, pTarget2),  pTarget1, pTarget2);
	LocalN(szName, pTarget2) = swap;
}

func GetValueID(aValue)
{
	var idID = NONE;
	var pItem;
	if(GetType(aValue) == C4V_Array) pItem = aValue[0];
	else pItem = aValue;
	if(GetType(pItem) == C4V_C4Object) idID = GetID(pItem);
	if(GetType(pItem) == C4V_C4ID) idID = pItem;
	return idID;
}

func StopActivityItem(pItem, aExtra)
{
	var idID = NONE;
	if(GetType(pItem) == C4V_C4Object) idID = GetID(pItem);
	if(GetType(pItem) == C4V_C4ID) idID = pItem;
//	Log("%v %v %v", pItem, aExtra, iActItemState);
	RemoveEffect("IntWait", this);
	if(idID == PROC) // Process
  {
		if(iActItemState == 1)
		{
			SetGraphics(0, this, 0, 23, 0);
			SetAction("KneelUp");
			return 1;
		}
		return 0;
  }
  if(idID == PICK || idID == BULD) // Pick
  {
		if(iActItemState == 1)
		{
			SetAction("Walk");
			return 1;
		}
		return 0;
  }
  if(idID == LOOK) // Umschauen
  {
		ai_iAlarmedCounter = 0;
		SetAction("Walk");
		return 1;
  }
	if(idID == TALK) // Sprechen
	{
		if(iActItemState >= 1)
		{
			ScheduleCall(aExtra[1], "FinishActivity", 1, 1);
		}
		return 0;
	}
	if(idID == TAK2)
	{
		ScheduleCall(aExtra[1], "FinishActivity", 1, 1);
		return 0;
	}
	// Totengräber
	if(idID == DEAD)
	{
		if(iActItemState == 2)
		{
			ObjectCall(this, "PutDownCorps");
			AddEffect("IntWait", this, 1, 12*2);
			RemoveEffect("IntFoundMarker2", pActTarget);
			return 1;
		}
	}
	if(idID == GRAV)
	{
		if(iActItemState == 2)
		{
			ObjectCall(this, "PutDownCorps");
			AddEffect("IntWait", this, 1, 12*2);
			return 0;
		}
		if(iActItemState == 3)
		{
			SetAction("Walk");
			return 1;
		}
	}
	if(idID == WAKE)
	{
		RemoveEffect("IntFoundMarker", aExtra[1]);
		return 1;
	}
}

func GetActivityItemPriority(pItem, aExtra)
{
	var idID = NONE;
	if(GetType(pItem) == C4V_C4Object) idID = GetID(pItem);
	if(GetType(pItem) == C4V_C4ID) idID = pItem;
	
	if(idID == CURY) return 2;
	if(idID == GRAV) return 2;
	if(idID == DEAD) return 2;
	if(idID == MORN) return 2;
	if(idID == TALK) return 2;
	if(idID == TAK2) return 2;
	
	if(idID == CORP) return 3;
	if(idID == WAKE) return 3;
	if(idID == LOOK) return 3;
	if(idID == ARLM) return 3;
	
	if(idID == PTRL) return 3;
	
	if(idID == SWAP) return 2;
	if(idID == _CIN) return 2;
	if(idID == MOVE) return 2;
	return 1;
}

func ExecActivityItem(pItem, aExtra) // 1 for keep 0 for finish
{
	var idID = NONE;
	if(GetType(pItem) == C4V_C4Object) idID = GetID(pItem);
	if(GetType(pItem) == C4V_C4ID) idID = pItem;
	if(idID == ANVL) // Ambos
		return Action_UseEnterItem(pItem, "Forge");
	if(idID == DPOT) // Teigtrog
		return Action_UseEnterItem(pItem, "Mix");
	if(idID == CLD_) // Kochtopf
		return Action_UseEnterItem(pItem, "Cook");
	if(idID == SAW_) // Säge
		return Action_UseEnterItem(pItem, "Saw");

	if(idID == WDP_) // Holzhaufen
	{
		if(Action_UseGrabWait(pItem))
		{
			this->~PickUpLog();
			return 1;
		}
		return 0;
	}
	
	if(idID == WTB_) // Küchentisch
		return Action_UseEnterItem(pItem, ExtraGetValue(aExtra, 1, "Hack"));
	if(idID == BED1) // Bett
		return Action_UseEnterItem(pItem, "Occupied", ExtraGetValue(aExtra, 1, 35*3));
	
	if(idID == CHST || idID == WODC || idID == LORY) // Truhe
		return Action_UseGrabWait(pItem, 35, aExtra);
	if(idID == ACLD) // Alchemie
	{
		if(Action_UseGrabWait(pItem, 35*2))
		{
			pItem->SetAction("Brew");
			return 1;
		}
		return 0;
	}
	if(idID == PROC) // Process
  {
		var pos = ExtraGetValue(aExtra, 1, [GetX(), GetY(), GetDir()]);
		if(iActItemState == 0 && ExecMoveTo(pos[0], pos[1], pos[2]))
		{
			SetGraphics(0, this, ExtraGetValue(aExtra, 2, CRYS), 23, 1);
			SetObjDrawTransform(1000, 0, 8000*(-1+2*GetDir()), 0, 1000, 7000, this, 23);
			AddEffect("IntWait", this, 1, 35*4);
			SetAction("Process");
			iActItemState = 1;
			return 0;
		}
		if(iActItemState == 1)
		{
			SetGraphics(0, this, 0, 23, 0);
			SetAction("KneelUp");
			return 1;
		}
		return 0;
  }
	if(idID == PICK) // Pick
  {
		var pos = ExtraGetValue(aExtra, 1, [GetX(), GetY(), GetDir()]);
		if(iActItemState == 0 && ExecMoveTo(pos[0], pos[1], pos[2]))
		{
			AddEffect("IntWait", this, 1, 8*3*4+1);
			SetAction("Pick");
			iActItemState = 1;
			return 0;
		}
		if(iActItemState == 1)
		{
			SetAction("Walk");
			return 1;
		}
		return 0;
  }
	if(idID == BULD) // Build
  {
		var pos = ExtraGetValue(aExtra, 1, [GetX(), GetY(), GetDir()]);
		if(iActItemState == 0 && ExecMoveTo(pos[0], pos[1], pos[2]))
		{
			AddEffect("IntWait", this, 1, 8*3*4+1);
			SetAction("Build");
			iActItemState = 1;
			return 0;
		}
		if(iActItemState == 1)
		{
			SetAction("Walk");
			return 1;
		}
		return 0;
  }
  if(idID == MOVE)
	{
		var pos = ExtraGetValue(aExtra, 1, [GetX(), GetY(), GetDir()]);
		return ExecMoveTo(pos[0], pos[1], pos[2]);
	}
	if(idID == FOLW)
	{
		if(!aExtra[1] || !GetAlive(aExtra[1])) return 1;
		ExecWalkTo(aExtra[1]);
		return 0;
	}
	if(idID == WAIT)
	{
		if(!aExtra[1] || !GetAlive(aExtra[1])) return 1;
		return 0;
	}
	if(idID == LOOK) // Umschauen
  {
		var pos = ExtraGetValue(aExtra, 1, [GetX(), GetY(), GetDir()]);
		if(iActItemState == 0 && ExecMoveTo(pos[0], pos[1], pos[2]))
		{
			AddEffect("IntWait", this, 1, 16*6);
			SetAction("LookAround");
			ai_iAlarmedCounter = 1;
			iActItemState = 1;
			SetDir(pos[2]);
			return 0;
		}
		if(iActItemState == 1)
		{
			iActItemState = 2;
			SetDir(!GetDir());
			AddEffect("IntWait", this, 1, 16*6);
			return 0;
		}
		if(iActItemState == 2)
		{
			ai_iAlarmedCounter = 0;
			SetAction("Walk");
			return 1;
		}
		return 0;
  }
	if(idID == FRGE) // Esse
	{
		if(Action_UseGrabWait(pItem, 35*2))
		{
			pItem->SetAction("Burning");
			return 1;
		}
		return 0;
	}
	if(idID == OVEN) // Küchenofen
		return Action_UseGrabAction(pItem, "Burning", "OpenDoor", 0, 8);
	
	if(idID == ALRM) // Renne zu einem Alarm hin
	{
		if(Abs(GetX()-aExtra[1]) < 30) return 1;
		if(!GetCommand() && !GetMacroCommand())
			SetMacroCommand("MoveTo", 0, aExtra[1], aExtra[2]);
		return 0;
	}
	if(idID == _CIN) // Münze
	{
		if(pItem->Contained())
		{
			FinishActivity();
			SetActivity([TALK, pItem->Contained(), _CIN]);
			pItem->Contained()->SetActivity([TAK2, this]);
			return 0;
		}
		if(ExecWalkTo(pItem))
		{
			SetCommand(this, "None");
			SetComDir(COMD_Stop);
			MessageEx(GetStringCoin(), this);
			SetAction("KneelDown");
			SetXDir(0);
			pItem->PickUp(this);
			AddEffect("IntWait", this, 1, 35);
			return 1;
		}
		else return 0;
	}
	if(idID == SWAP) // Wechseln
	{
		// Partner tot? :-(
		if(!aExtra[1] || !GetAlive(aExtra[1])) return 1;
		if(ExecWalkTo(aExtra[1]))
		{
			SwapLocal("ai_aPatrolPoints", this, aExtra[1]);
			SwapLocal("ai_pHome", this, aExtra[1]);
			SwapLocal("ai_iHomeX", this, aExtra[1]);
			SwapLocal("ai_iHomeY", this, aExtra[1]);
			SwapLocal("aListActivitys", this, aExtra[1]);
			iSwapTimer = FrameCounter();
			LocalN("iSwapTimer", aExtra[1]) = FrameCounter();
			return 1;
		}
		return 0;
	}
	if(idID == TALK) // Sprechen
	{
		// Partner tot? :-(
		if(!aExtra[1] || !GetAlive(aExtra[1])) return 1;
		if(iActItemState == 0 && ExecWalkTo(aExtra[1]))
		{
/*			SetComDir(COMD_Stop);
			SetCommand(this, "None");*/
			
			SetCommand(aExtra[1], "None");
			aExtra[1]->SetComDir(COMD_Stop);
			
			SetMacroCommand("MoveTo", 0, aExtra[1]->GetX()-10+10*aExtra[1]->GetDir(), aExtra[1]->GetY());
			if(ExtraGetValue(aExtra,  2) != _CIN) aExtra[1]->SetActivity([TAK2, this]);
			iActItemState = 1;
			iActTimer = 0;
			iDialogRandom = 0;
		}
		if(iActItemState >= 1)
		{
			SetMacroCommand("MoveTo", 0, aExtra[1]->GetX()-10+10*aExtra[1]->GetDir(), aExtra[1]->GetY());
			if(ExtraGetValue(aExtra,  2))
			{
				if(!ActGetDialog(ExtraGetValue(aExtra,  2), iActItemState-1, this, aExtra[1]))
				{
					aExtra[1]->FinishActivity();
					return 1;
				}
				SetDir(GetX()< aExtra[1]->GetX());
//				AddEffect("IntWait", this, 1, 35);
				if(iActTimer > 35) { iActItemState++; iActTimer = 0; }
			}
			else
			{
				var szMessage = [".","..","...","","",""][(iActTimer/3)%6];
				var szMessage2 = [".","..","...","","",""][(iActTimer/3+3)%6];
				Message(szMessage, this);
				Message(szMessage2, aExtra[1]);
				SetDir(GetX()< aExtra[1]->GetX());
				if(iActTimer > 36*3)
				{
					aExtra[1]->FinishActivity();
					return 1;
				}
			}
		}
		return 0;
	}
	if(idID == TAK2)
	{
		SetDir(GetX()< aExtra[1]->GetX());
		if(!aExtra[1] || !GetAlive(aExtra[1])) return 1;
		return 0;
	}
	if(idID == MORN) // Betrauern
	{
		// Opfer weg?
		if(!aExtra[1] || Contained(aExtra[1])) return 1;
		if(iActItemState == 0 && ExecWalkTo(aExtra[1]))
		{
			aExtra[1]->SetComDir(COMD_Stop);
			var obj;
			for(obj in FindObjects(Find_OCF(OCF_CrewMember), Find_Exclude(this), Find_InRect(-30,-20,60,40)))
			{
				var pAct = obj->GetActivity();
				if(pAct && GetValueID(pAct) == MORN)
					return 0;
			}
			SetMacroCommand("MoveTo", 0, aExtra[1]->GetX()-10+10*aExtra[1]->GetDir(), aExtra[1]->GetY());
			iActItemState = 1;
			iActTimer = 0;
			iDialogRandom = 0;
			AddEffect("IntWait", this, 1, 35);
			return 0;
		}
		if(iActItemState >= 1)
		{
			SetMacroCommand("MoveTo", 0, aExtra[1]->GetX()-10+10*aExtra[1]->GetDir(), aExtra[1]->GetY());
		
			if(!ActGetDialog(MORN, iActItemState-1, this, aExtra[1]))
			{
				aExtra[1]->FinishActivity();
				return 1;
			}
			SetDir(GetX()< aExtra[1]->GetX());
			if(iActTimer > 35*2) { iActItemState++; iActTimer = 0; }
		}
		return 0;
	}
	// Totengräber
	if(idID == DEAD)
	{
		if(iActItemState == 0)
		{
			var pos = ExtraGetValue(aExtra, 1, [GetX(), GetY(), GetDir()]);
			var found = 0;
			for(var pDead in FindObjects(Find_OCF(OCF_Living), Find_Action("Dead"), Find_Not(Find_OCF(OCF_Alive)), Find_Func("IsClonk")))
			{
				if(GetEffect("IntFoundMarker", pDead) && !GetEffect("IntFoundMarker2", pDead))
				{
					found = 1;
					break;
				}
			}
			if(!found) return 1;
			iActItemState = 1;
			pActTarget = pDead;
			AddEffect("IntFoundMarker2", pDead, 1, 0);
		}
		if(iActItemState == 1)
		{
			if(ExecMoveTo(GetX(pActTarget), GetY(pActTarget), GetDir(pActTarget)))
			{
				iActItemState = 2;
				ObjectCall(this, "CarryCorps", pActTarget);
			}
			else return 0;
		}
		if(iActItemState == 2)
		{
			var pos = ExtraGetValue(aExtra, 1, [GetX(), GetY(), GetDir()]);
			if(ExecMoveTo(pos[0], pos[1], pos[2]))
			{
				iActItemState = 3;
				ObjectCall(this, "PutDownCorps");
				AddEffect("IntDeadAttracktMourners", pActTarget, 1, 1, pActTarget);
				AddEffect("IntWait", this, 1, 12*2);
				return 1;
			}
			else return 0;
		}
	}
	if(idID == GRAV)
	{
		if(iActItemState == 0)
		{
			var pos = ExtraGetValue(aExtra, 1, [GetX(), GetY(), GetDir()]);
			var found = 0;
			for(var pDead in FindObjects(Find_OCF(OCF_Living), Find_Action("Dead"), Find_Not(Find_OCF(OCF_Alive)), Find_Func("IsClonk")))
			{
				if(GetEffect("IntFoundMarker2", pDead) && !GetEffect("IntFoundMarker3", pDead) && !GetEffect("IntDeadAttracktMourners", pDead))
				{
					found = 1;
					break;
				}
			}
			var pGrave = FindObject2(Find_ID(1GRV), Find_Action("Free"), Sort_Random());
			if(!found) return 1;
			iActItemState = 1;
			pActTarget = [pDead, pGrave];
		}
		if(iActItemState == 1)
		{
			var pDead = pActTarget[0];
			if(ExecMoveTo(GetX(pDead), GetY(pDead), GetDir(pDead)) && GetProcedure() == "WALK")
			{
				iActItemState = 2;
				ObjectCall(this, "CarryCorps", pDead);
			}
			else return 0;
		}
		if(iActItemState == 2)
		{
			var pGrave = pActTarget[1];
			if(ExecMoveTo(GetX(pGrave), GetY(pGrave), GetDir(pGrave)))
			{
				iActItemState = 3;
				ObjectCall(this, "PutDownCorps");
				AddEffect("IntWait", this, 1, 12*2);
				return 0;
			}
			else return 0;
		}
		if(iActItemState == 3)
		{
			iActItemState = 4;
			SetAction("DigHole", pActTarget[0], pActTarget[1]);
			AddEffect("IntWait", this, 1, 16*2 * 5);
			AddEffect("IntFoundMarker3", pActTarget[0], 1, 0);
			return 1;
		}
	}
	if(idID == WAKE) // Schlafenden Kollegen aufwecken
	{
		var pTarget = aExtra[1];
		if(!pTarget || !GetEffect("SleepNSpell", pTarget)) return 1;
		if(iActItemState == 0)
		{
			if(ExecMoveTo(GetX(pTarget), GetY(pTarget), GetDir(pTarget)))
			{
				iActItemState = 1;
				MessageEx(this->~GetStringWakeUp(), this, GetName(pTarget));
				AddEffect("IntWait", this, 1, 35);
			}
			return 0;
		}
		if(iActItemState == 1)
		{
			RemoveEffect("IntFoundMarker", pTarget);
			RemoveEffect("SleepNSpell", pTarget);
			return 1;
		}
	}
	if(idID == CORP) // Toten Kollegen entdecken
	{
		var pTarget = aExtra[1];
		if(!pTarget || GetAlive(pTarget)) return 1;
		if(iActItemState == 0)
		{
			if(ExecMoveTo(GetX(pTarget), GetY(pTarget), GetDir(pTarget)))
			{
				iActItemState = 1;
				MessageEx(this->~GetStringHurt(), this, GetName(pTarget));
				AddEffect("IntWait", this, 1, 35);
			}
			return 0;
		}
		if(iActItemState == 1)
		{
			iActItemState = 2;
			MessageEx(this->~GetStringDead(), this, GetName(pTarget));
			AddEffect("IntWait", this, 1, 35);
			AddEffect("IntFoundMarker", pTarget, 1, 0);
			return 0;
		}
		if(iActItemState == 2)
		{
			ai_iAlarmedCounter = 1;
			var pNewTarget = AI_FindNearestEnemy(1);
			if (ai_idFirstEncounterCB) AI_DoEncounter(pTarget, 1);
			FinishActivity();
			// Nur wenn ein neuer Gegner da ist!
			if(pNewTarget)
			{
				MessageEx(GetStringSeenMurder(), this, GetName(pTarget));
				ai_iAlarmedCounter = 0;
				pNewTarget->~DoShow(250);
				ai_pTarget = pNewTarget;
			}
			else
			{
				SetActivity([NOTY, [GetX(), GetY(), GetDir()], pTarget]);
				SetActivity([LOOK, [GetX(), GetY(), GetDir()], pTarget]);
			}
      return 0;
		}
	}
	if(idID == CURY) // Nach Geräuschen suchen
	{
		var pTarget = aExtra[1];
		if(!pTarget) return 1;
		if(iActItemState == 0)
		{
			if(ExecMoveTo(GetX(pTarget), GetY(pTarget), GetDir(pTarget)))
			{
				if(GetID(pTarget) == _SWT && pTarget->~ControlReset())
          MessageEx(GetStringSwitch(), this);
        else MessageEx(GetStringMouse(), this);
				AddEffect("IntWait", this, 1, 35);
				return 1;
			}
			return 0;
		}
	}
	return 1;
}

func FxIntDeadAttracktMournersTimer(pTarget, iNumber, iTime)
{
	// Haben wir einen Trauergast? und lebt er noch?
	if(EffectVar(0, pTarget, iNumber) && EffectVar(0, pTarget, iNumber)->GetAlive())
	{
		var pAct = EffectVar(0, pTarget, iNumber)->GetActivity();
		// Ist er noch beim trauern?
		if(GetValueID(pAct) == MORN)
			return;
		// Ansonsten neuen suchen
		EffectVar(0, pTarget, iNumber) = 0;
	}
	if(iTime > 35*60) return -1;
	// Suche Trauergast
	for(var pObj in FindObjects(Find_OCF(OCF_CrewMember), Find_Allied(GetOwner(pTarget)), Find_Action("Walk"), Sort_Random()))
	{
		var pAct = pObj->GetActivity();
		if(pAct) continue;
		if(pObj->LocalN("ai_iNoSpecial")) return;
		break;
	}
	if(pObj)
	{
		EffectVar(0, pTarget, iNumber) = pObj;
		pObj->SetActivity([MORN, pTarget]);
	}
}

func SetActivity(aActivity, fClear)
{
	if(fClear)
	{
		while(FinishActivity());
		SetMacroCommand("None");
		iCurrentActivity = [aActivity];
	}
	else
	{
		SetMacroCommand("None");
		StopActivity();
		PushFront(aActivity, iCurrentActivity);
	}
	iActItemState = 0;
	iActTimer = 0;
}

func GetActivity()
{
	if(GetLength(iCurrentActivity))
		return iCurrentActivity[0];
}

func GetActivityPriority()
{
	if(!iCurrentActivity) return;
	var pItem = iCurrentActivity[0];
	var aExtra = [];
	if(GetType(pItem) == C4V_Array)
	{
		aExtra = pItem;
		pItem = pItem[0];
	}
	return GetActivityItemPriority(pItem, aExtra);
}

func FinishActivity()
{
	if(!GetLength(iCurrentActivity)) return 0;
	StopActivity();
	iActItemState = 0;
	iActTimer = 0;
	DeleteArrayItem(0, iCurrentActivity);
	return 1;
}

func StopActivity()
{
	if(!iCurrentActivity) return;
	var pItem = iCurrentActivity[0];
	var aExtra = [];
	if(GetType(pItem) == C4V_Array)
	{
		aExtra = pItem;
		pItem = pItem[0];
	}
	StopActivityItem(pItem, aExtra);
}

func AddActivity(aActivity)
{
	StopActivity();
  if(!aListActivitys) aListActivitys = [aActivity];
  else aListActivitys[GetLength(aListActivitys)] = aActivity;
}

func NearObj(obj, iXOffset, iYOffset)
{
	if(Abs(obj->GetX()+iXOffset - GetX()) < 8)
		if(Abs(obj->GetDefBottom()-10+iYOffset - GetY()) < 20)
			return 1;
}

func DoGrab(obj)
{
	SetComDir(COMD_Stop);
	SetCommand(this, "None");
	SetAction("Push", obj);
	obj->~Grabbed(this, 1);
}

func ExecActivity()
{
	iActTimer++;
	if(!iCurrentActivity) iCurrentActivity = [];
  if(GetEffect("IntWait", this)) return 1;
  if(!GetLength(iCurrentActivity))
	{
		if(!aListActivitys) return;
		var list = aListActivitys[Random(GetLength(aListActivitys))];
		for(var pItem in list)
		{
			var aExtra = [];
			if(GetType(pItem) == C4V_Array)
			{
				aExtra = pItem;
				pItem = pItem[0];
			}
			if( !(TestActivityPossible(pItem, aExtra) ) ) return 0;
		}
		for(var pItem in list)
			PushBack(pItem, iCurrentActivity);
	}
	var pItem = iCurrentActivity[0];
	var aExtra = [];
	if(GetType(pItem) == C4V_Array)
	{
		aExtra = pItem;
		pItem = pItem[0];
	}
	if(ExecActivityItem(pItem, aExtra))
	{
		DeleteArrayItem(0, iCurrentActivity);
		iActItemState = 0;
	}
  return 1;
}

func Death()
{
	while(FinishActivity());
	return _inherited(...);
}