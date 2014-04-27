/* Der Screen-Fader */

#strict

local color,timer;

ScreenFadeOut:
  timer=0;
  SetAction("FadeOut");
  return(1);

ScreenFadeIn:
  timer=0;
  SetAction("FadeIn");
  return(1);

ScreenFadingOut:
  ++timer;
  color = 255 - timer;
  SetGamma(0, RGB(color/2, color/2, color/2), RGB(color,color,color), 2);
  if(color>0)  return(0);
  timer=0;
  SetAction("None");
  return(1);

ScreenFadingIn:
  ++timer;
  color = timer;
  SetGamma(0, RGB(color/2, color/2, color/2), RGB(color,color,color), 2);
  if(color < 255) return(0);
  ResetGamma(2);
  timer=0;
  SetAction("None");
  return(1);