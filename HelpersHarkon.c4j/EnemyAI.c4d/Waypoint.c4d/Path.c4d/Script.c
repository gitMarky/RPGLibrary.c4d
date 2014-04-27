/*-- Pfad - Visualisierung --*/

#strict

local so,eo, sx,sy, ex,ey, flag, jtpk, path;

protected func Timer() // Schaut nach Änderungen
{
  if(!so) return(RemoveObject());
  if(!(so->GetPathTarget(path))) return(RemoveObject());
  // Irgendeine klitzekleine Änderung?
  if(so->GetX() != sx ||
     so->GetY() != sy ||
     eo->GetX() != ex ||
     eo->GetY() != ey ||
     so->GetPathFlag(path) != flag ||
     so->GetPathTarget(path) != eo)

       Set(so, so->GetPathTarget(path), so->GetPathFlag(path), 0, path);
}

public func Set(object start, object end, int flags, int jetpack, int iPath) {
  so = start;
  eo = end;
  sx = start->GetX();
  sy = start->GetY();
  ex = end->GetX();
  ey = end->GetY();
  flag = flags;
  jtpk = jetpack;
  path = iPath;

  SetObjectBlitMode(GFX_BLIT_Additive);
  SetClrModulation(MyColor(flags,jetpack,start));

  var sx = GetX(start);
  var sy = GetY(start);
  var ex = GetX(end);
  var ey = GetY(end);
  var cl = 1000*Distance(sx,sy,ex,ey)/GetActMapVal("Facet",GetAction(),0,3);
  var w = 650;
  var r = Angle(sx,sy,ex,ey)-180;

  var fsin = -Sin(r, 1000), fcos = Cos(r, 1000);

  var xoff = -4;
  var yoff = 0;

  var width = +fcos*w/1000, height = +fcos*cl/1000;
  var xskew = +fsin*cl/1000, yskew = -fsin*w/1000;

  var xadjust = +fcos*xoff + fsin*yoff;
  var yadjust = -fsin*xoff + fcos*yoff;

  // set matrix values
  SetObjDrawTransform (
    width, xskew, xadjust,
    yskew, height, yadjust
  );

}

private func MyColor(int flags, int jetpack, object start) {
  SetAction("Vis",start);
  if(jetpack) {
    SetAction("Vis3",start);
    return(RGB(255,0,0));
  }
  else {
    if(flags == Path_MoveTo) return(RGB(0,255,0));
    if(flags == Path_Jump) return(RGB(255,255,0));
		if(flags == Path_Door) return(RGB(255,0,0));
    if(flags == Path_Backflip) {
      SetAction("Vis3",start);
      return(RGB(255,255,0));
    }
    if(flags == Path_Fly) {
      SetAction("Vis3",start);
      return(RGB(0,50,255));
    }
    SetAction("Vis2",start);
    if(flags == Path_Lift) return(RGB(255,0,255));
  }
}
