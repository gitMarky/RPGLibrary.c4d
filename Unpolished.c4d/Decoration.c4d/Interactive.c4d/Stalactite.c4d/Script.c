/*-- Just hanging around
@version 0.2.0
@author Unknown, Gecko (graphics)
--*/

#strict 2
#include LF_D

local growth;
local child;
local can_fall;

public func IsTrampolinObject()
{
	return true;
}

public func Initialize()
{
	SetAction("Decoration");
	SetPhase(Random(4));
}

// new stalactite
public func Grow()
{
	SetObjDrawTransform(1000, 0, 0, 0, 0);
	growth = 100;
}

public func Check()
{
	// still growing
	if (growth)
	{ 
		growth -= 1;
		SetObjDrawTransform(1000, 0, 0, 0, 10 * (100 - growth), -growth * 190);
		return true;
	}
	
	if (can_fall)
	{
		var clnk;
		// grown stalactites may fall on clonks
		for (clnk in FindObjects(Find_InRect(-25, 0, 50, 100), Find_OCF(OCF_Alive), Find_OCF(OCF_CrewMember))) 
			StartFalling();
	}
}

public func StartFalling()
{
	if (GetAction() == "Falling") return;
	if (growth) return;
	
	// Und wenn wir noch keinen Nachwuchs haben einen neuen Stalaktit heranwachsen lassen	    
	if (!child) child = CreateObject(GetID(), 0, 19)->Grow();
	Sound("Earthquake");
	
	ChangeAction("Falling");
	
	AddEffect("IntFalling", this, 50, 5, this);
	
	// remove the top 2 vertices
	RemoveVertex(0);
	RemoveVertex(0);
}

public func FxIntFallingTimer()
{
	var clnk;
	
	for (clnk in FindObjects(Find_InRect(-7, -19, 14, 50), Find_OCF(OCF_Alive), Find_OCF(OCF_CrewMember))) 
		DoEnergy(-1 - Random(5), clnk);
}

// Am Boden in die Einzelteile zerfallen
public func Hit()
{
	var obj;
	Sound("RockBreak*");
	Sound("Blast2", 0, 0, 50);
	// links oben
	ObjectSetAction(obj = CreateObject(_STP, -4, -4, GetOwner()), "Exist");
	SetPhase(GetPhase() * 2, obj);
	SetRDir(Random(20) - 10, obj);
	SetSpeed(Random(30) - 15, -Random(35) + 10, obj);

	// rechts oben
	ObjectSetAction(obj = CreateObject(_STP, 3, -4, GetOwner()), "Exist");
	SetPhase(GetPhase() * 2 + 1, obj);
	SetRDir(Random(20) - 10, obj);
	SetSpeed(Random(30) - 15, -Random(35) + 10, obj);

	// unten
	ObjectSetAction(obj = CreateObject(_STP, 0, 12, GetOwner()), "LameExist");
	SetPhase(GetPhase(), obj);
	SetRDir(Random(20) - 10, obj);
	SetSpeed(Random(10) - 15, -Random(35) + 10, obj);

	
	RemoveObject();
}

public func OnShockwaveHit()
{
	if (can_fall)
	{
		StartFalling();
	}
	
	return true;
}

public func SetFallFrequency(int fall)
{
	can_fall = fall;
}
