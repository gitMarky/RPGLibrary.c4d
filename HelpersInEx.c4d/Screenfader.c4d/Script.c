/* Fades the screen in some color */

#strict 2

local color,timer;

public func ScreenFadeOut()
{
	timer = 0;
	SetAction("FadeOut");
	return 1;
}

public func ScreenFadeIn()
{
	timer = 0;
	SetAction("FadeIn");
	return 1;
}

public func ScreenFadingOut()
{
	++timer;
	color = 255 - timer;
	SetGamma(0, RGB(color / 2, color / 2, color / 2), RGB(color, color, color), 2);
	if (color > 0)
		return 0;
	timer = 0;
	SetAction("None");
	return 1;
}

public func ScreenFadingIn()
{
	++timer;
	color = timer;
	SetGamma(0, RGB(color / 2, color / 2, color / 2), RGB(color, color, color), 2);
	if (color < 255)
		return 0;
	ResetGamma(2);
	timer = 0;
	SetAction("None");
	return 1;
}