/*-- Decorative saw.
@title Saw
@author Randrian ({@link http://clonkforge.net/pr.php?pr=1415})
@version 0.2.0
 --*/

#strict 2

local product;

public func ContainedDown()
{
	[$TxtCancelproduction$|Image=WKS2]
	ProductionCancel();
	return true;
}

public func ProductionStart(worker, product_id)
{	
	// only if active
	if (!ActIdle())
	{
		// try again later
		AddCommand(worker, "Call", this, product_id, 0, 0, 0, "ProductionStart", 0, 3);
		AddCommand(worker, "Wait", 0, 0, 0, 0, 0, Max(168 - GetActTime(), 25));
		return true;
	}
	
	// save product
	product = product_id;

	// grab the clonk and set color
	Enter(this, worker);
	SetOwner(GetOwner(worker));

	SetAction("Saw");
	return true;
}

private func Sawing()
{
	if (GetID(FindCrewContents()) == _LBJ)
	{
		SetGraphics("Lumberjack");
		FindCrewContents()->PutLogDown();
	}
	else
		SetGraphics();
		
	// not done yet
	if (GetActTime() < 150) return;
	
	// done, exit clonk
	ProductionCancel();
}

private func ProductionCancel()
{
	if (GetAction() == "Saw") SetAction("Idle");

	var clnk;
	while (clnk = FindCrewContents()) Exit(clnk, 0, 10);
}

private func FindCrewContents()
{
	var i = 0, obj;
	
	while (obj = Contents(i++))
		if (GetOCF(obj) & OCF_CrewMember)
			return obj;
			
	return 0;
}
  
private func Effects()
{
	Sound("Build2");
}
