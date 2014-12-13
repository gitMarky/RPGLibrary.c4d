/*-- Basic script for deco objects.
@title Decoration
@author Marky
@version 0.2.0
--*/

#strict 2

static const gDeco_Random = -1;

global func PlaceDecoration(id decoration, int x, int y, int phase, int angle, int size, bool solid, int solid_offset_y)
{
	if (!(decoration->~IsDeco())) return 0;
	
	var obj = CreateObject(decoration, 0, 0, NO_OWNER);
	
	phase = obj->Decoration(phase);
	
	if (!solid)
	{
		if (!size)
		{
			size = 1000;
		}
		
		var fsin=Sin(angle, size), fcos=Cos(angle, size);
		obj->SetObjDrawTransform (+fcos, +fsin, 0, -fsin, +fcos, 0);
	}
	else
	{
		obj->SetSolid(solid, solid_offset_y);
	}
}

local is_solid;

public func IsDeco()
{
	return true;
}

public func Decoration(int phase)
{
	if (SetAction("Decoration"))
	{
		if (phase == gDeco_Random)
		{
			phase = GetActMapVal("Length", "Decoration", this->GetID());
			SetPhase(Random(phase));
		}
		else
		{
			SetPhase(phase);
		}
	}
	
	return phase;
}

public func Transformation(int angle, int size, bool mirrored)
{
	if (is_solid)
	{
		if (size || mirrored)
		{
			ErrorLog("The object has a solid mask and thus can not be scaled or mirrored");
		}
		
		// set angle if the rotated solid mask flag is set...
	}
	else
	{
		if (!size)
		{
			size = 1000;
		}
		
		var fsin=Sin(angle, size), fcos=Cos(angle, size);
		SetObjDrawTransform (+fcos, +fsin, 0, -fsin, +fcos, 0);
	}
}

public func SetSolid(bool solid, int solid_offset_y)
{
	is_solid = solid;

	if (solid)
	{
		var phase = GetPhase();
		
		var width = GetActMapVal("Facet", "Decoration", this->GetID(), 2);
		var height = GetActMapVal("Facet", "Decoration", this->GetID(), 3);

		SetSolidMask(phase * width, SolidMaskOffsetY() + solid_offset_y, width, height);
	}
	else
	{
		SetSolidMask(0, 0, 0, 0);
	}
}

public func SolidMaskOffsetY(){ return 0;}