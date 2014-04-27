/*-- Geld-Verwaltung --*/

#strict 2
#appendto CLNK

// Bestimmte Clonks, n‰mlich H‰ndler etc. haben ihr eigenes Geld!
// Standardm‰ﬂig hat ein Clonk aber einfach so viel Geld, wie sein Besitzer

public func GetMoney(){ return GetWealth(GetOwner());}
public func SetMoney( int iAmount ){ return SetWealth(GetOwner(),iAmount); }
public func DoMoney( int iAmount ){ return DoWealth(GetOwner(),iAmount); }
public func GetMoneyInfinite(){ return false; } // braucht man noch nicht

