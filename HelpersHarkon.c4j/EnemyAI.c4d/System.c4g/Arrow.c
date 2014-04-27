/* Objekte, denen die KI ausweichen sollte */

#strict 2

#appendto ARRW

local aPoints;
local aSpeed;
local aActTime;

func Launch()
{
	aPoints = [ [GetX(), GetY()] ];
	aSpeed = [ [GetXDir(), GetYDir()] ];
	aActTime = [GetActTime()];
	return _inherited(...);
}

private func InFlight()
{
	aPoints[GetLength(aPoints)] = [GetX(), GetY()];
	aSpeed[GetLength(aSpeed)] = [GetXDir(), GetYDir()];
	aActTime[GetLength(aActTime)] = GetActTime();
	SetR(Angle(0,0,GetXDir(), GetYDir()));
	if(GetActTime() < 5) return;
	return _inherited(...);
}

func LogAll()
{
	Log("P%v", aPoints);
	Log("S%v", aSpeed);
	Log("A%v", aActTime);
}

private func InFlight()
{
 	aPoints[GetLength(aPoints)] = [GetX(), GetY()];
	aSpeed[GetLength(aSpeed)] = [GetXDir(), GetYDir()];
	aActTime[GetLength(aActTime)] = GetActTime();
	SetR(Angle(0,0,GetXDir(), GetYDir()));
//	if(GetActTime() < 5) return;
	
 // Wenn der Pfeil keine Geschwindigkeit mehr hat (schwimmt oder so?) dann nicht mehr ueberpruefen
 if(!GetXDir() && !GetYDir())
 if(!GetEffect("PendingToRemove",this))
  AddEffect("PendingToRemove",this,1,10,this);
 
 if(lastX || lastY)
 {
   // Wenn der Pfeil durch irgendwas gebeamt wurde oder Aehnliches muss eine Sicherheit her
   var bSkip=false;
   if(estX || estY)
   if(Distance(GetX(),GetY(),estX,estY) > Max(1,Max(Abs(GetXDir()),Abs(GetYDir())))*2){bSkip=true;}
   
   if(!bSkip)
   for(var obj in FindObjects(Find_OnLine(lastX-GetX(),lastY-GetY(),0,0),
                     Find_Exclude(this), Find_Hostile(GetOwner()), // no friendy fire
                     Find_NoContainer(),
                                         Find_Layer(GetObjectLayer()),
                     Find_Or(Find_OCF(OCF_Alive),Find_Func("IsArrowTarget",this)))){   
      // In der Trefferzone?
      if (obj->~IsOutsideArrowHitZone(this)) continue;
      
      // Darf das Objekt getroffen werden?     
      var iSkip=false;
      var iEffect;
      for(var i=0;iEffect=GetEffect("IntNoArrowHit",this(),i);i++)
      {
       if(EffectVar(0,this(),GetEffect("IntNoArrowHit",this(),i)) != obj)continue;
       iSkip=true;
       break;
      }
      
      if(iSkip)continue;
      
      HitTarget(obj);
      break;
    }
 }
 
 lastX=GetX();
 lastY=GetY();
 estX=GetX()+GetXDir();
 estY=GetY()+GetYDir();
 return(1);
}

// Wie SimFlight, beachtet jedoch auch SolidMasks
global func SimFlightX(&iX, &iY, &iXDir, &iYDir, idID, iIteration, iPrecision)
{
	var	aPoints = [ [iX, iY] ];
	var aSpeed = [ [iXDir, iYDir] ];
	// Variablen
	var obj, time, time2;
	// Koordinatenprezesion
	iX*=iPrecision; iY*=iPrecision;
//	iXDir*=2; iYDir*=20;
	// Testobject
	if(idID) obj = CreateObject(idID);
	else obj = CreateObject(ROCK);
	// Flugsimulator
	while(1)
	{
		// Schwerkraft
//		if(!Mod(time, iPrecision))
		{
			iYDir += GetGravity()*iPrecision/500;
/*			if(time2==2){ iYDir+=25; time2=0;}
			else time2++;*/
		}
		// Geschwindigkeit
		if(time)
		{
			iX+=iXDir; iY+=iYDir;
		}
		// Hängengeblieben?
		SetPosition((iX+iPrecision/2)/iPrecision, (iY+iPrecision/2)/iPrecision, obj);
		obj->SetR(Angle(0,0,iXDir, iYDir));
//		obj->CreateParticle("Blast", 0, 0, 0, 0, 50, RGB(255));

		aPoints[GetLength(aPoints)] = [iX/iPrecision, iY/iPrecision];
		aSpeed[GetLength(aSpeed)] = [iXDir, iYDir];

		for(var i = -2; i < 2; i++)
		{
			SetPosition((iX+iPrecision/2)/iPrecision, (iY+iPrecision/2)/iPrecision+i, obj);
			if(Stuck(obj))
			{
				iX/=iPrecision; iY/=iPrecision; //iXDir; iYDir/=20;
				RemoveObject(obj);
				return 1;
			}
		}
		// Die Zeit vergeht
		time++;
		if(iIteration) if(time>iIteration)
		{
			RemoveObject(obj);
			return 0;
		}
	}
}