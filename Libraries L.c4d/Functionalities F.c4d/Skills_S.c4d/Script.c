/*-- Skill system  --*/

#strict 2

static const ID_SkillIcons = LF_S;
static const ID_SkillSystem = LF_S;

static aListSkillAttributes, aListSkillSkills, aListSkillPerks;

/* you can overload this in your own scenario in system.c4g - the rest happens automatically */

global func RPGAttributeList()
{
	return [
		//	name,			string,					lower limit,	upper limit,	cost function,	availability function
		[	"Strength",		"$AttributeStrength$",	0,				20,				"",				""],
		[	"Dexterity",	"$AttributeDexterity$",	0,				20,				"",				""],
		[	"Magic",		"$AttributeMagic$",		0,				20,				"",				""]
	];
}

global func RPGSkillList()
{
	return [
		//	name,			string,					lower limit,	upper limit,	cost function,	availability function
		[ "Blacksmith",		"$SkillBlacksmith$",	0,				20,				"",				""],
	];
}

global func RPGPerkList()
{
		//	name,				string,				parent,		cost function,	availability function
	return [
		[	"Perk1",			"$PerkPerk1$",		"",			"",				""],
		[	"Perk2",			"$PerkPerk2$",		"",			"",				""],
	];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// this is part of the mechanics, do not change anything here
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


global func RPGGetAttributeList()
{
	if(GetType(aListSkillAttributes) != C4V_Array)
	{
		aListSkillAttributes = [];

		for( var item in RPGAttributeList() )
		{
			PushBack( item[0], aListSkillAttributes );
		}
	}

	return aListSkillAttributes;
}
global func RPGGetAttributeIndex( string szAttribute )
{
	return GetArrayItemPosition( szAttribute, RPGGetAttributeList() );
}

global func RPGGetAttributeBound( string szAttribute, bool fUpper )
{
	var index = RPGGetAttributeIndex( szAttribute );
	if( index < 0 ) return;

	if( fUpper )
		return RPGAttributeList()[index][3];
	else
		return RPGAttributeList()[index][2];
}

global func RPGAttributeSystem( object pObj )
{
	if(!pObj) return -1;

	var iNum;
	var szName = "RPGAttributes";

	for( var i=0; i < 10 && !(iNum = GetEffect( szName, pObj )) ; i++ )
	{
		AddEffect( szName, pObj, 40, 0, 0, ID_SkillSystem );
	}

	return iNum;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// attribute manipulations


global func RPGSetAttribute( string szAttribute, int iAmount, object pObj )
{
	var iNum, iIndex;

	iIndex = RPGGetAttributeIndex( szAttribute );
	iNum = RPGAttributeSystem( pObj );

	if(iNum < 0 || iIndex < 0) return -1;
	
	return EffectVar( iIndex, pObj, iNum ) = BoundBy( iAmount, RPGGetAttributeBound(szAttribute,false), RPGGetAttributeBound(szAttribute,true) );
}

global func RPGAddAttribute( string szAttribute, int iAmount, object pObj )
{
	return RPGSetAttribute( szAttribute, RPGGetAttribute( szAttribute, pObj )+iAmount, pObj );
}

global func RPGGetAttribute( string szAttribute, object pObj )
{
	var iNum, iIndex;

	iIndex = RPGGetAttributeIndex( szAttribute );
	iNum = RPGAttributeSystem( pObj );

	if(iNum < 0 || iIndex < 0) return;

	return EffectVar( iIndex, pObj, iNum );
}