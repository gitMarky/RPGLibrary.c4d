/*-- 
Framework for handling stats, such as skills, in a RPG character.
@title Skill system
@author Marky
@version 0.1.0
  --*/

#strict 2

static const ID_SkillIcons = LF_S;
static const ID_SkillSystem = LF_S;

static aListSkillAttributes, aListSkillSkills, aListSkillPerks;

/* you can overload this in your own scenario in system.c4g - the rest happens automatically */

/**
 * The list of attributes that define a character.
 * @return array An array of stats descriptions. See TODO.
 * @note The default list includes the attributes "Strength", "Dexterity" and "Magic" which range from 0 to 20.
 */
global func RPGAttributeList()
{
	return [
		//	name,			string,					lower limit,	upper limit,	cost function,	availability function
		[	"Strength",		"$AttributeStrength$",	0,				20,				"",				""],
		[	"Dexterity",	"$AttributeDexterity$",	0,				20,				"",				""],
		[	"Magic",		"$AttributeMagic$",		0,				20,				"",				""]
	];
}

/**
 * The list of skills that define a character.
 * @return array An array of stats descriptions. See TODO.
 * @note The default list includes the skill "Blacksmith", which ranges from 0 to 20.
 */
global func RPGSkillList()
{
	return [
		//	name,			string,					lower limit,	upper limit,	cost function,	availability function
		[ "Blacksmith",		"$SkillBlacksmith$",	0,				20,				"",				""],
	];
}

/**
 * The list of perks that define a character.
 * @return array An array of perk descriptions. See TODO.
 * @note The default list includes the perks "Perk1" and "Perk2".
 */ 
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

/**
 * A list of the defined attributes.
 * @return array An array of the names of the attributes that are defined by RPGAttributeList() // TODO.
 * @note You can use the names as parameters in the related functions.
 */
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

/**
 * Returns the index of an attribute in the attribute list.
 * @par szAttribute The name of the attribute.
 * @return int The index in RPGAttributeList(). Returns -1 if the attribute is not in the attribute list.
 */
global func RPGGetAttributeIndex( string szAttribute )
{
	return GetArrayItemPosition( szAttribute, RPGGetAttributeList() );
}

/**
 * Gets the maximum or minimum value of an attribute.
 * @par szAttribute The name of the attribute.
 * @par fUpper Defines which bound to get: True means upper bound, false means lower bound.
 * @return int The requested value.
 */
global func RPGGetAttributeBound( string szAttribute, bool fUpper )
{
	var index = RPGGetAttributeIndex( szAttribute );
	if( index < 0 ) return;

	if( fUpper )
		return RPGAttributeList()[index][3];
	else
		return RPGAttributeList()[index][2];
}

/**
 * Reference to the attribute manager of an object.
 * @par pObj The object that should receive the attributes.
 * @return int The attribute manager is an effect. This returns its effect number.
 * @note You do not need to use this method, it is used internally only.
 */
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

/**
 * Sets the value of an attribute, limited to the valid values of said attribute.
 * @par szAttribute The name of the attribute.
 * @par iAmount The new value of the attribute.
 * @par pObj The object whos attribute value will be set.
 * @return int The attribute value that was actually set.
 */
global func RPGSetAttribute( string szAttribute, int iAmount, object pObj )
{
	var iNum, iIndex;

	iIndex = RPGGetAttributeIndex( szAttribute );
	iNum = RPGAttributeSystem( pObj );

	if(iNum < 0 || iIndex < 0) return -1;
	
	return EffectVar( iIndex, pObj, iNum ) = BoundBy( iAmount, RPGGetAttributeBound(szAttribute,false), RPGGetAttributeBound(szAttribute,true) );
}

/**
 * Increases the attribute value by a specified amount, limited to the valid values of said attribute.
 * @par szAttribute The name of the attribute.
 * @par iAmount Increase the attribute by this amount. Negative values decrease the attribute.
 * @par pObj The attribute of this object will be modified.
 * @return The attribute value that was actually set.
 */
global func RPGAddAttribute( string szAttribute, int iAmount, object pObj )
{
	return RPGSetAttribute( szAttribute, RPGGetAttribute( szAttribute, pObj )+iAmount, pObj );
}

/**
 * Gets the current value of an attribute.
 * @par szAttribute The name of the attribute.
 * @par pObj The object that has the attribute.
 * @return The attribute value.
 */
global func RPGGetAttribute( string szAttribute, object pObj )
{
	var iNum, iIndex;

	iIndex = RPGGetAttributeIndex( szAttribute );
	iNum = RPGAttributeSystem( pObj );

	if(iNum < 0 || iIndex < 0) return;

	return EffectVar( iIndex, pObj, iNum );
}