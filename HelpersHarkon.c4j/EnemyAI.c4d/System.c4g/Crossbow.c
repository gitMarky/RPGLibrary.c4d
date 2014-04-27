/* Nicht durch Türen schiessen */

#strict 2

#appendto CRBW

/* Ausgangsgeschwindigkeit und -position, wenn der Ritter mit dem Bogen in angegebene Richtung schießt */
public func GetCrossbowOut(pBow, iAngle, fDeviate, &iOutX, &iOutY, &iOutR, &iOutXDir, &iOutYDir, &iOutRDir)
  {
  // Austrittsgeschwindigkeit ermitteln
  iOutXDir = Sin(iAngle, iPower);
  iOutYDir = -Cos(iAngle, iPower);
  // Austrittsposition...
  iOutX = Sin(iAngle, 25 * GetCon() / 100);
  iOutY = -Cos(iAngle, 25 * GetCon() / 100);
  // Pfeildrehung
  iOutR = iAngle;
  if(iAngle > 0) iOutRDir = 3; else iOutRDir = -3;
	var x = GetX(), y = GetY(), x2 = GetX()+iOutX*(-1+2*Contained()->GetDir()), y2 = GetY()+iOutY;
	var res = PathFree2(x, y, x2, y2);
	if(!res)
	{
		iOutX = (x-GetX())*(-1+2*Contained()->GetDir());
		iOutY = y-GetY();
	}
  // Fertig
  return 1;
  } 