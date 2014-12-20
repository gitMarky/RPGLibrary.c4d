/* Road */

/* Inherits from the basic foundation: BAS7 */

#strict 2

#include BAS7

private func BasementID() { return(_ROA); }
private func BasementWidth() { return(72); }

// indestructable
protected func Damage(change, by_player)
{
}

protected func Consolidated()
{
	SetPosition(GetX(), GetY() + 8);
	_inherited();
}
