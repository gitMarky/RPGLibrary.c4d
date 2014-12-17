/* Fundament */

/* Alle Fundamente leiten ihre Grundfunktionalität von BAS7 ab */

#strict 2

#include BAS7

private func BasementID() { return(_ROA); }
private func BasementWidth() { return(72); }

// indestructable
protected func Damage(iChange, iByPlayer)
{
}

protected func Consolidated()
{
	SetPosition(GetX(), GetY() + 8);
	_inherited();
}
