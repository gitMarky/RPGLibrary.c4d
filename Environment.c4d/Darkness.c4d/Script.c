/*-- Düsternis --*/

#strict 2

//Ob es wirklich dunkel ist.
local darkness;

// Ursprüngliche Himmelsfarben
static DarknessSkyAdjustOrig;

// Kann mittels des Spielzielauswählers ausgewählt werden
//public func IsChooseable() { return(true); }
// Das war einmal. Der Regelwähler behandelt Dunkelheit jetzt gesondert.

protected func Activate(iPlr) {
  MessageWindow(GetDesc(),iPlr);
}

public func Initialize() {
  //ScheduleCall(this(),"PostInitialize",1);
}

private func PostInitialize()	{
  //nur eine Dunkelheit!
  var count = 1;
  for(var d in FindObjects(Find_ID(GetID()),Find_Exclude(this()))) {
    count++;
    RemoveObject(d);
  }
}

public func UpdateLights() {
  var obj;
  
  for(obj in FindObjects(Find_Func("IsLight"))) {
    obj->~Update();
  }
  
  var d = GetDarkness(100);
  for(obj in FindObjects(Find_Func("IsHUD")))
    obj->~UpdateColor(d);
}

global func CalcLight(&alphamod, &sizemod) {
  sizemod = 100+GetDarkness(1000)*3/2/10; // bis zu 250% so groß
  //alphamod = (1000-GetDarkness(1000))/50; // 0-20 alpha werden aufaddiert
  alphamod = (1000-GetDarkness(1000))/20; // 0-20 alpha werden aufaddiert
 
  // keine Dunkelheit: beinahe unsichtbar
  // Genauso bei Dunkelheit = 0
  //if(!IsDark() || !GetDarkness(1000)) alphamod = 30;
  //if(!IsDark() || !GetDarkness(1000)) alphamod = 50;
}

public func InitializePlayer(int iPlr) {
  SetFoW(true,iPlr);
}

public func OnClonkRecruitment(object pClonk) {
  if(ObjectCount(GetID()) > 1)
    return(ScheduleCall(this(),"OnClonkRecruitment",1,0,pClonk));
  /*SetPlrViewRange(700-3*GetDarkness(100),pClonk);
  var tmp = AddLightAmbience(80,pClonk);
  tmp->ChangeColor(RGBa(255,216,132,70));
  SetVisibility(VIS_Owner,tmp);*/
  // jeder Clonk kriegt eine Taschenlampe
  //if(GetDarkness() && IsDark())
    //CreateContents(FLSH,pClonk);
}

public func ChooserFinished() {
  /*for(var pClonk in FindObjects(Find_OCF(OCF_CrewMember), Find_Not(Find_Owner(NO_OWNER))))
  {
    SetPlrViewRange(700-3*GetDarkness(100),pClonk);
    var tmp = AddLightAmbience(80,pClonk);
    tmp->ChangeColor(RGBa(255,255,255,110));
    tmp->SetOwner(pClonk->GetOwner());
    SetVisibility(VIS_Owner,tmp);
    // jeder Clonk kriegt eine Taschenlampe
    if(GetDarkness() && IsDark() && GetPlayerType(pClonk->GetOwner()) != C4PT_Script)
      CreateContents(FLSH,pClonk);
  }*/
}

global func IsDark() {
  var obj;
  if(GetID(this()) != DARK)
    obj = FindObject(DARK);
  else
    obj = this();
  //kein Dunkelheit-Objekt -> Keine Dunkelheit
  if(!obj)
    return(false);

  return(obj);
}

global func GetDarkness(int precision) {
  var dark;
  if(!(dark = IsDark()))
    return(0);

  if(!precision) precision = 10;  

  var val;
  
  var darkness = dark->LocalN("darkness");
  
  val = darkness/(1000/precision);
    
  return(val);
}

global func SetDarkness(int iGrade) {
  var obj;
  if(GetID(this()) != DARK)
    obj = FindObject(DARK);
  else
    obj = this();
  //kein Dunkelheit-Objekt -> Keine Dunkelheit
  if(!obj)
    return(false);


  if(GetType(DarknessSkyAdjustOrig) != C4V_Array)
  	DarknessSkyAdjustOrig = [GetSkyAdjust(), GetSkyAdjust(true)];


  iGrade = BoundBy(iGrade,0,100);

  obj->LocalN("darkness") = iGrade*10;
  //var g = BoundBy(128-iGrade,0,128);
  var g = BoundBy(128-((1280*iGrade)/1000),0,128);

  //var g = BoundBy(128-iGrade*10,0,128);
  //SetGamma(RGB(0,0,0),RGB(g,g,g),RGB(255,255,255),2);
  //SetSkyAdjust(RGB(127+g,127+g,127+g),RGB(100+g,100+g,100+g));
  DarknessApplyGrade(g);

  obj->UpdateLights();
  
  return(true);
}

// iStep: wie viel Änderung der Dunkelheit pro 10 Frames
global func FadeDarkness(int iGrade, int iStep) {
  var obj;
  if(GetID(this()) != DARK)
    obj = FindObject(DARK);
  else
    obj = this();
  //kein Dunkelheit-Objekt -> Keine Dunkelheit
  if(!obj)
    return(false);

  if(!iStep) iStep = 50;

  //iGrade = BoundBy(iGrade,0,10);
  iGrade = BoundBy(iGrade,0,100);
  
  var darkness = obj->LocalN("darkness");
  
  //AddEffect("Fading",obj,20,1,obj,DARK,iGrade*100,darkness,iStep);
  var dest = iGrade*10;
  AddEffect("Fading",obj,20,1,obj,DARK,dest,darkness,iStep);
  
  return(true);
}

public func Destruction() {
  SetDarkness(0);
  UpdateLights();
  for(var light in FindObjects(Find_Func("IsLight"),Find_Func("LocalN(\"bAmbience\")")))
    RemoveObject(light);
}

func FxFadingStart(object pTarget, int iEffectNumber, int iTemp, int endgrade, int startgrade, int frames) {
  if(iTemp) return;
  EffectVar(0, pTarget, iEffectNumber) = endgrade;
  EffectVar(1, pTarget, iEffectNumber) = startgrade;
  EffectVar(2, pTarget, iEffectNumber) = frames;  
  //EffectVar(3, pTarget, iEffectNumber) = 0;  
}

func FxFadingTimer(object pTarget, int iEffectNumber, int iEffectTime) {
  var grade = EffectVar(1,pTarget,iEffectNumber);
  var end = EffectVar(0,pTarget,iEffectNumber);
  var fade = EffectVar(2,pTarget,iEffectNumber);
  var go = (end-grade)/fade; //EffectVar(3,pTarget,iEffectNumber);
  //if(grade == end)
  //  return(-1);

  if( end > grade )
	  go = Max(go,1);
  else
	  go = Min(go,-1);
  
  //go += fade;

	  //if(go >= 10) {
	  if(go >= 0)
	  {
			if(grade > end)
			{
				grade = end;
			}
			else
			{
				grade = Min(grade+go, end);
			}
	  }
	  else
	  {
			if(grade > end)
			{
				grade = Max(grade+go,end);
			}
			else
			{
				grade = end;
			}
	  }
    go = 0;
    //Log("Fading: %d->%d, change %d",grade+go,end,go);
   
    EffectVar(1,pTarget,iEffectNumber) = grade;
	EffectVar(2,pTarget,iEffectNumber) = Max( EffectVar(2,pTarget,iEffectNumber)-1,1);

    //Log("%d -> %d -> %d",grade,g,end);
  
    //var g = BoundBy(128-grade*100/1000,0,128);
    var g = BoundBy(128-((128*grade)/1000),0,128);
 
    //SetGamma(RGB(0,0,0),RGB(g,g,g),RGB(255,255,255),2);
    //SetSkyAdjust(RGB(127+g,127+g,127+g),RGB(100+g,100+g,100+g));
	DarknessApplyGrade(g);

    pTarget->LocalN("darkness") = grade;
    
    UpdateLights();

  if(grade == end)  return(-1);
  //EffectVar(3,pTarget,iEffectNumber) = go;

}

func FxFadingEffect(string szNewEffectName, object pTarget, int iEffectNumber, int iNewEffectNumber, int endgrade, int stargrade, int frames) {
  if(szNewEffectName == "Fading")
    return -3;
}

func FxFadingAdd(object pTarget, int iEffectNumber, string szNewEffectName, int iNewEffectTimer, int endgrade, int stargrade, int frames) {
  EffectVar(0,pTarget,iEffectNumber) = endgrade;
  EffectVar(2,pTarget,iEffectNumber) = frames;
}

/*
global func DarknessApplyGrade( int g )
{

    SetGamma(RGB(0,0,0),RGB(g,g,64+g/2),RGB(255,255,255),2);
 
    DARK -> SetSkyAdjust(RGBa(
    (127+g) * GetRGBValue(DarknessSkyAdjustOrig[0],1) / 255,
    (127+g) * GetRGBValue(DarknessSkyAdjustOrig[0],2) / 255,
    (127+g) * GetRGBValue(DarknessSkyAdjustOrig[0],3) / 255,
    BoundBy(255-g*2,0,255) ),
    RGB(1,1,1)
    );

   	DARK -> SetMatAdjust(RGB(
    BoundBy(127+g,0,255) * GetRGBValue(DarknessSkyAdjustOrig[0],1) / 255,
    BoundBy(127+g,0,255) * GetRGBValue(DarknessSkyAdjustOrig[0],2) / 255,
    BoundBy(127+g,0,255) * GetRGBValue(DarknessSkyAdjustOrig[0],3) / 255 ));

}
*/
global func DarknessApplyGrade( int g )
{

	//Log("Darkness %d, Grade %d",GetDarkness(1000)/10,g);

    //SetGamma(RGB(0,0,0),RGB(g,g,64+g/2),RGB(255,255,255),2);
    //SetGamma(RGB(0,0,0),RGB(g,g,32+(3*g)/4),RGB(255,255,255),2);
    SetGamma(RGB(0,0,0),RGB(32+(96*g)/128,32+(96*g)/128,64+g/2),RGB(255,255,255),2);
 
    DARK -> SetSkyAdjust(RGBa(
    (127+g) * GetRGBValue(DarknessSkyAdjustOrig[0],1) / 255,
    (127+g) * GetRGBValue(DarknessSkyAdjustOrig[0],2) / 255,
    (127+g) * GetRGBValue(DarknessSkyAdjustOrig[0],3) / 255,
    BoundBy(255-2*g,0,255) ),
    RGB(1,1,1)
    );

   	DARK -> SetMatAdjust(RGB(
    BoundBy(160+g,0,255) * GetRGBValue(DarknessSkyAdjustOrig[0],1) / 255,
    BoundBy(160+g,0,255) * GetRGBValue(DarknessSkyAdjustOrig[0],2) / 255,
    BoundBy(160+g,0,255) * GetRGBValue(DarknessSkyAdjustOrig[0],3) / 255 ));

}
