/* Türen */

#strict 2

// Türe
#appendto CPDP
// Turmtor
#appendto CPGL
#appendto CPGR
// Zugbrücke
#appendto CPBL
#appendto CPBR

local fCounter;

func KIOpen(pClonk)
{
	fCounter++;
	var iWait = 0;
	if(!this->~IsOpen()) iWait = DoOpen(pClonk);
	if(iWait) pClonk->~AddMacroCommand("Wait", iWait);
}

func KIClose(pClonk)
{
	fCounter--;
	if(fCounter == 0) DoClose(pClonk);
}

func DoOpen(pClonk)
{
	if(GetID() == CPDP)
	{
		_inherited(pClonk);
		return 0;
	}
	if(GetID() == CPGL || GetID() == CPGR)
	{
		return this->~Open(pClonk);
		//return 25;
	}
	if(GetID() == CPBL || GetID() == CPBR)
	{
		this->~ControlDown(pClonk);
		if(this->~IsOpening())
			return (21-GetPhase())*3;
		return 65;
	}
}

func DoClose(pClonk)
{
	if(GetID() == CPDP)
	{
		_inherited(pClonk);
		return 0;
	}
	if(GetID() == CPGL || GetID() == CPGR)
	{
		this->~Close(pClonk);
		return 5;
	}
	if(GetID() == CPBL || GetID() == CPBR)
	{
		this->~ControlUp(pClonk);
		return 65;
	}
}

private func Close()
{
  if (!GetActionTarget()) return 0;
	//if (GetY() > GetActionTarget()->GetY() + 30) return(0);
  SetComDir(COMD_Down);
  return 1;
}

private func Open()
{
  if (!GetActionTarget()) return 0;
	//if (GetY() < GetActionTarget()->GetY() - 10) return(0);
  SetComDir(COMD_Up);
	return Max((25- (GetActionTarget()->GetY()-GetY()+15))*2-20,0);
//  return(1);
}

func IsDoor() { return 1; }
