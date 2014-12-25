/*-- Rope bridge that reacts to mass placed on it.
@author Randrian 
@version 0.2.0
 --*/

#strict 2

local aParts;
local aWeight;
local aHeights;
local aVelocity;
local aPosition;
local aObjects;

local iLength;
local iMiddle;

local pMaster;

protected func Initialize()
{
	SetAction("Initialize");
	iLength = 31;
	iMiddle = iLength / 2 + 1;
}

public func Initialized()
{
	aParts = CreateArray(iLength);
	aVelocity = CreateArray(iLength);
	aPosition = CreateArray(iLength);
	SetGraphics(0, this, ROCK);
	for (var i = 0; i < iLength; i++)
	{
		aParts[i] = CreateObject(GetID());
		if (i == 0)
			aParts[i]->MakeChild(this, "End", 0);
		else if (i == iLength - 1)
		{
			aParts[i]->MakeChild(this, "End", 1);
		}
		else
			aParts[i]->MakeChild(this, "Part", i % 2 + Random(3) * 2);
		aParts[i]->SetPosition((i - iMiddle) * 6 + GetX(), GetY());
	}
	SetShape(-3 * iLength - 12, -25, 6 * iLength + 12, iLength + 20);
}

public func MakeChild(pParent, szAction, iPhase)
{
	pMaster = pParent;
	SetAction(szAction);
	SetPhase(iPhase);
}

public func SetBridgeLength(iNewLength)
{
	if (pMaster)
		return pMaster->SetBridgeLength(iNewLength);
	for (pPart in aParts) 
		if (pPart)
			RemoveObject(pPart);
	iLength = iNewLength;
	iMiddle = iLength / 2 + 1;
	SetAction("Initialize");
}

public func WeightBridge()
{
	// Only every 5 Frames searching for Objects
	//  if(!(GetActTime()%5) || !aWeight)
	WeightAndHeight();
	if (!(GetActTime() % 6))
		UpdateVelocity();
	var i = 0;
	
	var fSound;
	for (i = 0; i < iLength; i++)
	{
		if (aVelocity[i])
			fSound = 1;
		
		aPosition[i] += aVelocity[i];
		if (aObjects[i])
			for (var pObj in aObjects[i]) 
				SetPosition(GetX(pObj), GetY(pObj) + GetY() + aPosition[i] / 10 - GetY(aParts[i]), pObj);
		aParts[i]->SetPosition((i - iMiddle) * 6 + GetX(), GetY() + aPosition[i] / 10);
		if (i > 1 && i < iLength - 1)
			SetDTRotation
			(
				Angle(GetX(aParts[i - 1]), GetY(aParts[i - 1]), GetX(aParts[i + 1]), GetY(aParts[i + 1]), 10) - 900,
				0,
				0,
				aParts[i]
			);
	}
	if (fSound && Random(2))
		Sound("WoodKnack*", 0, aParts[Random(GetLength(aParts))]);
	return;
}

private func UpdateVelocity()
{
	for (var i = 1; i < iLength - 1; i++)
	{
		aVelocity[i] = (aHeights[i] * 10 - aPosition[i]) / 4;
	}
}

private func WeightAndHeight()
{
	var iMax;
	aHeights = CreateArray(iLength);
	aWeight = CreateArray(iLength);
	aObjects = CreateArray(iLength);
	var i;
	for (var obj in FindObjects(Find_AtRect(-6 * iMiddle, -5, 6 * iLength, iLength + 10), Find_Container())) 
		if (obj->GetContact(0, -1, 8))
			if (~obj->GetCategory() & C4D_StaticBack)
			{
				i = BoundBy((GetX(obj) - GetX()) / 6 + iMiddle, 0, iLength - 1);
				aWeight[i] += obj->GetMass();
				if (!aObjects[i])
					aObjects[i] = [obj];
				else
					aObjects[i][GetLength(aObjects[i])] = obj;
			}
	// Find out maximal weight
	i = iLength;
	while (i--)
	{
		aWeight[i] = BoundBy(aWeight[i] * (iLength / 2 - Abs(i - iLength / 2)) / iLength / 2, 0, 50);
		iMax += aWeight[i] / 3;
	}
	// Add weight in the middle
	iMax += 25;
	aWeight[iMiddle] += 25;
	// Weight weights!
	for (var i = 0; i < iLength; i++)
	{
		if (aWeight[i])
		{
			aWeight[i] = aWeight[i] * (iLength * 2 / 3) / iMax;
			// Then apply it to the height
			for (var j = 0; j < iLength; j++)
			{
				if (j < i)
					aHeights[j] += (-Pow(j - i, 2) + Pow(i, 2)) * aWeight[i] / Pow(i, 2);
				else
					aHeights[j] += (-Pow(j - i, 2) + Pow(iLength - i, 2)) * aWeight[i] / Pow(iLength - i, 2);
			}
		}
	}
}

private func SetDTRotation(int r, int xoff, int yoff, object obj)
{
	var fsin = -Sin(r, 1000, 10), fcos = Cos(r, 1000, 10);
	// set matrix values
	SetObjDrawTransform
	(
		+fcos,
		+fsin,
		(1000 - fcos) * xoff - fsin * yoff,
		-fsin,
		+fcos,
		(1000 - fcos) * yoff + fsin * xoff,
		obj
	);
}

