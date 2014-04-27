/*-- Story --*/

#strict 2

local pTarget;
local iTimer;

func Timer()
{
	var pDead2, pNewTarget;
	if(!pTarget)
	{
	for(var pDead in FindObjects(Find_Action("Dead"), Find_Distance(100)))
	{
		if(GetPhase(pDead) < 4)
			if(PathFree(GetX(), GetY(), pDead->GetX(), pDead->GetY()))
			{
				pDead2 = pDead;
				break;
			}
	}
	pDead = pDead2;
	if(pDead)
	{
		for(pNewTarget in FindObjects(Find_OCF(OCF_Alive), Find_Distance(100), Find_Hostile(GetOwner(pDead))))
		{
			if(PathFree(GetX(), GetY(), pNewTarget->GetX(), pNewTarget->GetY()))
			{
				iTimer = 0;
				pTarget = pNewTarget;
			}
		}
	}
	}
	if(pTarget)
	{
		iTimer++;
		if(iTimer%10 != 0) return;
		if(iTimer > 100) pTarget = 0;
		else if(!PathFree(GetX(), GetY(), pTarget->GetX(), pTarget->GetY()) && !pTarget->Contained()) pTarget = 0;
		if(!pTarget) return;
		Message("Töte niemanden an diesem heiligen Ort!", this);
		DrawLightning(GetX(),GetY(),pTarget->GetX(), pTarget->GetY());
		Punch(pTarget, 5);
	}
}

func DrawLightning(x0, y0, x1, y1)
{
  var dis=Distance(x0,y0,x1,y1)/2;
  var lgh = CreateObject(SLGH,(x0+x1)/2-GetX(), (y0+y1)/2-GetY()+100,-1);
	if(!lgh) return Log("Errror NoLightning");
  SetCon(dis, lgh);
  
 

  
  SetR(Angle(x0,y0,x1,y1), lgh);
  SetPosition((x0+x1)/2, (y0+y1)/2, lgh);
  

  return lgh;
}
