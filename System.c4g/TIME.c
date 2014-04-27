/*-- Tageszeiten --*/

#strict 2
#appendto TIME

/* Locals */

static days,hours,minutes,seconds;
static sky_red, sky_green, sky_blue, sky_alpha;
static daylight_override, daylight;

local dawn_done, dusk_done;

/* Verzögerte Initialisierung */

//global func GetRealTime(){ return(hours);}

private func Initialized()
{
	_inherited();
	/*
	SetPosition(20,20);
	// Starthelligkeit
	Timing();
	// Steuerungsaktion
	SetAction("Time");*/
}
  
/* Konfiguration */

global func SetTime(d,h,m,s)
{
	days=d;
	hours=h;
	minutes=m;
	seconds=s;

	var time;
	if( time = FindObject(TIME))
		PrivateCall(time,"Timing",true,true);
}

/* Zeitverlauf */  

private func Timing( bool fIgnoreProcess, bool fInstant )
{

	if(!fIgnoreProcess && !GetFilm())
	{

		minutes=minutes+1;
		if(seconds>=60) { seconds=0; minutes++; }
		if(minutes>=60) { minutes=0; hours++; }
		if(hours>=24) { hours=0; days++; }

	}

	var temp,a,t;

	if(hours<4) temp=6000;

	if(hours>=4) if(hours<=9)
	{
		t=(hours*100)+(minutes*100)/60;
		a=(BoundBy(t-400,0,700))/5;
		temp=(BoundBy(6+a,6,100)*100000)/100;
	}

	if(hours>9) if(hours<16)  temp=100000;

	if(hours>=16) if(hours<=21)
	{
		t=(hours*100)+(minutes*100)/60;
		a=(BoundBy(t-1600,0,700))/5;
		//temp=(BoundBy(100-a,6,100)*100000)/100;
		temp=(BoundBy(100-a,6,100)*1000);
		//Log("t/a/temp %d/%d/%d",t,a,temp);
	}

	if(hours>21) temp=6000;
  
	temp=temp/1000;

	//Log("%d:%d:%d Uhr, %d",hours,minutes,seconds,temp);

	temp += daylight_override;

	temp = BoundBy( temp, 6, 100 );

	RestoreSkyColor(temp,fInstant);

	daylight=temp;

	if(IsNight()) if(!dusk_done) TimeEvent("Dusk");
	if(IsDay()) if(!dawn_done) TimeEvent("Dawn");
}

public func TimeEvent( event )
{
	var obj;

	while( obj = FindObject( 0, 0,0,0,0,0,0,0,0,obj ))
	ObjectCall(obj, event);

	if( event == "Dusk" ) { dusk_done = 1; dawn_done = 0; }
	if( event == "Dawn" ) { dusk_done = 0; dawn_done = 1; }
}

protected func UpdateTransferZone(){ return(0);}
  
/* Status */

global func IsDay(){ return(Inside(hours,5,18));}
  
global func IsNight(){ return(!IsDay());}
    
/* Himmelsfarbe */

public func RestoreSkyColor(int i, bool fInstant)
{
/*	var red;
	var green;
	var blue;
	var rgb,rgb_back;
	var t;
	var obj;
	var god_r,god_g,god_b;

	red=BoundBy(255*i/100,0,255);
	green=BoundBy(255*i/100,0,255);
	blue=BoundBy(255*i/100,0,255);

	rgb=RGBa(red,green,blue,sky_alpha);

	rgb_back=RGB( sky_red, sky_green, sky_blue);



	SetSkyAdjust(rgb,rgb_back);

	t=17+i*5/6;
	red=BoundBy(255*t/100,0,255);
	green=BoundBy(255*t/100,0,255);
	blue=BoundBy(255*t/100,0,255);
	rgb=RGB(red,green,blue);

	SetMatAdjust(rgb);*/

	var iPercent;
	iPercent = 100 - i;
	var iMin = GameCall("MinDarkness"), iMax = GameCall("MaxDarkness");
	if(!iMax)
		iMax = 90; // <Nachtfalter> einigen wir uns auf 62
	iPercent = iPercent * (iMax - iMin) / 100 + iMin;
	//Log("iPercent %d", iPercent);
	if( fInstant )
		SetDarkness( iPercent );
	else
		FadeDarkness(iPercent);

}

public func ChangeSkyColor(i)
{
	var red;
	var green;
	var blue;
	var rgb,rgb_back;
	var t;
	var obj;
	var god_r,god_g,god_b;

	red=BoundBy(255*i/100,0,255);
	green=BoundBy(255*i/100,0,255);
	blue=BoundBy(255*i/100,0,255);

	rgb=RGBa(red,green,blue,sky_alpha);
	rgb_back=RGB( sky_red, sky_green, sky_blue);

	SetSkyAdjust(rgb,rgb_back);

}

global func TIME_Tick()
{
	return(48);
	//return(1);
	// return(8); später dann ^^
}

