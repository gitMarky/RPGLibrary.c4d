/*-- Klotz --*/

#strict 2

local iXDir,iYDir;
local iOldXDir, iOldYDir;

static const _BLK_MoveSound = "Elevator",
             _BLK_HitSound  = "Discharge";

protected func Initialize()
  {
  SetAction("Block");
  }

public func SetStartPosition()
{
  iOldXDir = GetX(); iOldYDir = GetY();
}

public func ControlUp(object pController)
  {
  if(GetXDir())
   return;
  // 2do: sound
  if (iYDir>=0) Sound(_BLK_MoveSound);
  iYDir = -1;
  iXDir=0;
  return UpdateComDir();
  }

public func ControlDown(object pController)
  {
  if(GetXDir())
   return;
  // 2do: sound
  if (iYDir<=0) Sound(_BLK_MoveSound);
  iYDir = 1;
  iXDir=0;
  return UpdateComDir();
  }

public func ControlLeft(object pController)
  {
  if(Abs(GetYDir())>2)
   return;
  // 2do: sound
  if (iXDir>=0) Sound(_BLK_MoveSound);
  SetYDir();
  iXDir = -1;
  iYDir=0;
  return UpdateComDir();
  }

public func ControlRight(object pController)
  {
  if(Abs(GetYDir())>2)
   return;
  // 2do: sound
  if (iXDir<=0) Sound(_BLK_MoveSound);
  SetYDir();
  iXDir = 1;
  iYDir=0;
  return UpdateComDir();
  }

protected func ContactLeft() { if (iXDir>=0) return; Sound(_BLK_HitSound); return iXDir=0; }
protected func ContactRight() { if (iXDir<=0) return; Sound(_BLK_HitSound); return iXDir=0; }
//protected func ContactBottom() { if (iYDir<=0) return(); Sound(_BLK_HitSound); return(iYDir=0); }

protected func Hit()
  {
  Sound(_BLK_HitSound);
  // 2do: sound
  SetXDir(); SetYDir();
  iXDir=iYDir=0; SetComDir();
  }

private func UpdateComDir() // nur für iXDir|iYDir != 0
  {
  // Puzzling, isn't it?
  return SetComDir(iXDir*(iYDir-2)+!iXDir*(iYDir-1)*2+5);
  }
