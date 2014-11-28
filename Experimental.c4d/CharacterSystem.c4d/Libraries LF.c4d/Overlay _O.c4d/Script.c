/*-- 
Basic script for equipment overlay graphics.
@note This is still in work.
@title Overlay
@author Marky
@version 0.1.0
--*/

#strict 2

protected func Initialize()
{
	SetClrModulation(RGB(255,255,255));
	_inherited(...);
}

public func GetOverlayGraphics(){ return ""; }
public func GetOverlayColor(){ return GetClrModulation(); }
