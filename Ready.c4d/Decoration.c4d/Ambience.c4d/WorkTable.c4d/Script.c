/*-- Working table, for the chef.
@title Drinking Table
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
	if (!ActIdle())
	{
		AddCommand(worker, "Call", this, product_id, 0, 0, 0, "ProductionStart", 0, 3);
		AddCommand(worker, "Wait", 0, 0, 0, 0, 0, Max(168 - GetActTime(), 25));
		return true;
	}
	
	product = product_id;

	Enter(this, worker);
	SetOwner(GetOwner(worker));

	SetAction("Saw");
	return true;
}

private func Cooking()
{
	if (GetActTime() < 150) return false;
	
	ProductionCancel();
	return true;
}

private func ProductionCancel()
{
	SetAction("Idle");

	var clnk;
	while (clnk = FindCrewContents()) Exit(clnk, 0, 10);

	return true;
}

private func FindCrewContents()
{
	var i = 0, obj;
	while (obj = Contents(i++))
		if (GetOCF(obj) & OCF_CrewMember)
			return obj;
	return 0;
}
  