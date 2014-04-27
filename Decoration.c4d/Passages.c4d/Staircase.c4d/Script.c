/*-- Treppenhaus --*/

#strict

#include CPT1 // Treppenhaus

/* Treppenhaus */

local pTargetUp;
local pTargetDown;

func SetTargetDown(obj) { pTargetDown = obj; }
func SetTargetUp(obj) { pTargetUp = obj; }

protected func Initialize()
  {
  // Tür ist immer offen
  SetEntrance(1);
  // Transferzone
  UpdateTransferZone();
  }

private func FindStaircase(int ox, int oy)
{
  // Anderes Treppenhaus in entsprechender Richtung suchen
  var obj;
  while(obj = FindObject(0, ox,oy, 0,0, OCF_Fullcon(), 0,0, NoContainer(), obj) )
    if (obj->~IsStaircase())
      return(obj);
  // Keins gefunden
  return(0);
}

private func FindStaircaseAbove()
{
	if(pTargetUp) return pTargetUp;
	var obj = FindStaircase(0, -CastlePartHeight()/2);
	if(obj) return obj;
	obj = FindStaircase(0, -CastlePartHeight()/2-10);
  return( obj );
}

private func FindStaircaseBelow()
{
	if(pTargetDown) return pTargetDown;
	var obj = FindStaircase(0, CastlePartHeight()/2);
	if(obj) return obj;
	obj = FindStaircase(0, CastlePartHeight()/2 + 10);
  return obj;
}

/* Status */

public func CastlePartHeight() { return (140); }

/* Transfer */

public func UpdateTransferZone()
{
  // Nur mit darüberliegendem Niedergang
  if (FindStaircaseAbove() )
    if (GetOCF() & OCF_Fullcon() )
      return(SetTransferZone(-8, -80, 16, 60));
  // Ohne darüberliegenden Niedergang (kein Auf- oder Abstieg möglich)
  return(SetTransferZone());
}

public func ControlTransfer(object pObj, int tx, int ty)
{
  // Aufstieg
  if (Inside(tx - GetX(), -20, 20) && Inside(ty - GetY(), -80, -50) )
    if (Contained(pObj) == this() || GetY(pObj) > GetY() - 50)
      return(HandleTransferAscend(pObj, tx, ty) );

  // Abstieg
  if (Inside(tx - GetX(), -20, 20) && Inside(ty - GetY(), -20, 20) )
    if (Contained(pObj) == this() || GetY(pObj) < GetY() - 50)
      return(HandleTransferDescend(pObj, tx, ty) );

  // Transfer nicht möglich
  return(0);
}
