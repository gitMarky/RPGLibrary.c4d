/*-- Light --*/

#strict

local iColor, bAmbience, iF;
local bFlicker;


//Licht initialisieren, Parameter setzen, etc.
protected func Init(int iSize, int iColor, object pTarget, string cGraphics) {
  if(bAmbience && !IsDark())
    Schedule("RemoveObject()",1,0,this());
//Werte
  if(!iSize)
    iSize = GetCon();  
  if(!pTarget)
    pTarget = GetActionTarget();

  SetOwner(GetController(pTarget));
  //TurnOn();

//Setzen und so
  SetAction("Attach",pTarget);
  SetPosition(GetX(pTarget),GetY(pTarget));
  ChangeColor(iColor);
  ChangeSize(iSize);
  ChangeGraphics(cGraphics);
}

//Wenn das Licht kein Ziel mehr hat -> weg damit.
protected func AttachTargetLost() {
  RemoveObject();
}

//Licht ausschalten
public func TurnOff() { SetVisibility(VIS_None); }

//Licht einschalten
public func TurnOn() { SetVisibility(VIS_All); }

// größe des Lichts ändern
public func ChangeSize(int iNewSize) { 
  var alphamod, sizemod;
  CalcLight(alphamod, sizemod);

  iF = iNewSize;
  SetCon(iF*sizemod/100);
}

// Licht updaten... verändert keine Werte sondern
// passt die Anzeige nur an
public func Update() {
  ChangeSize(iF);
  ChangeColor(iColor);
}

//Farbe des Lichts ändern
public func ChangeColor(int iNewColor) {
  
  iColor = iNewColor;
  
  //Wenn keine dunkelheit ist
  if(!IsDark()) {
    //Hintergrundlichter ausblenden
    if(bAmbience)
      iNewColor = RGBa(255,255,255,255);
  }  
  
  var alphamod, sizemod;
  CalcLight(alphamod, sizemod);
  
  var r,g,b,a;
  SplitRGBaValue(iColor,r,g,b,a);
  iNewColor = RGBa(r,g,b,Min(a+60+alphamod,255));
  
  SetClrModulation(iNewColor);
  
  return(iNewColor);
}

//Grafik ändern
public func ChangeGraphics(string cNewGraphics) {
  SetGraphics(cNewGraphics, 0, GetID());
}

//festlegen, dass es ein Licht ist.
public func IsLight() { return(1); }
public func NoWarp() { return(true); }

/*--  Globale Funktionen zur Lichterzeugung  --*/

//Hilfsfunktion
global func CreateLight(id ID, int iSize, int iColor, object pTarget, string sGraphics) {
  var light = CreateObject(ID, 0, 0, GetController(pTarget));
  light->Init(iSize, iColor, pTarget, sGraphics);
  
  return(light);
}

//erzeugt ein Licht mit Größe und Farbe und hängt es an pTarget
global func AddLight(int iSize, int iColor, object pTarget) {
  if(!pTarget)
    if(!(pTarget = this()))
      return();
  
  return(CreateLight(LIGH, iSize, iColor, pTarget));
}


global func AddLightHalf(int iSize, int iColor, object pTarget) {
  if(!pTarget)
    if(!(pTarget = this()))
      return();
  
  return(CreateLight(LIGH, iSize, iColor, pTarget, "Half"));
}

global func AddLightCone(int iSize, int iColor, object pTarget) {
  if(!pTarget)
    if(!(pTarget = this()))
      return();
  
  return(CreateLight(LGHC, iSize, iColor, pTarget));
}

//Ambience-Light, Umgebungslicht, nicht sichtbar wenn keine Dunkelheit herrscht.
global func AddLightAmbience(int iSize, object pTarget, string cGraphics) {
  if(!pTarget)
    if(!(pTarget = this()))
      return();
      
  var light = CreateObject(LIGH,0,0,GetController(pTarget));
  light->LocalN("bAmbience") = true;
  light->Init(iSize*5, RGBa(255,255,255,50), pTarget, cGraphics);
  
  return(light);
}

//Lichtblitz!
global func AddLightFlash(int iSize, int iX, int iY, int iColor) {
  var alphamod, sizemod;
  CalcLight(alphamod, sizemod);
  
  var r,g,b,a;
  SplitRGBaValue(iColor,r,g,b,a);
  iColor = RGBa(r,g,b,Min(a+alphamod,255));

  CreateParticle("LightFlash",iX,iY,0,0,iSize*5*sizemod/100,iColor);
}

global func AddLightPos( int iSize, int iX, int iY, iColor )
{
	var obj = CreateObject(TIM1,0,0,-1);
	SetPosition(iX,iY,obj);
	var light = AddLight(iSize,iColor,obj);

	if(!light) if(obj) RemoveObject(obj);
	return light;
}

public func SetFlicker( bool bSet )
{
	if( bSet )
	{
		if( !GetEffect("LightPulse",this) )
			AddEffect("LightPulse",this,50,1,this);
	}
	else
	{
		var iNr = GetEffect("LightPulse",this);
		if(iNr) RemoveEffect("LightPulse",this,iNr);
		SetObjDrawTransform (1000, 0, 0, 0, 1000, 0); 

	}
	return bFlicker = bSet;
}

protected func FxLightPulseTimer(object pTarget,int iEffectNumber,int iEffectTime)
{
	var iHeight = EffectVar(0,pTarget,iEffectNumber), iWidth = EffectVar(1,pTarget,iEffectNumber);
	var iXSkew = EffectVar(2,pTarget,iEffectNumber), iYSkew = EffectVar(3,pTarget,iEffectNumber);
	iHeight = BoundBy( iHeight +RandomX(-20,20), 800, 1000 );
	iWidth = BoundBy( iWidth +RandomX(-20,20), 800, 1000 );

	iXSkew = BoundBy( iXSkew + RandomX(-40,40), -250,250 );
	iYSkew = BoundBy( iYSkew + RandomX(-40,40), -250,250 );

	SetObjDrawTransform (iWidth, iXSkew, 0, iYSkew, iHeight, 0); 

	EffectVar(0,pTarget,iEffectNumber) = iHeight; EffectVar(1,pTarget,iEffectNumber) = iWidth;
	EffectVar(2,pTarget,iEffectNumber) = iXSkew; EffectVar(3,pTarget,iEffectNumber) = iYSkew;
}


