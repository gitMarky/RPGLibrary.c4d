#strict 2

/**
 * Checks if the object is crew member of a non-AI player.
 * @return true, if the object is a crew member and the player type of the owner is {@i C4PT_User}.
 */
global func IsUserCrewMember(object pObj)
{
	return GetOCF(pObj) & OCF_CrewMember && GetPlayerType(GetOwner(pObj)) == C4PT_User;
}