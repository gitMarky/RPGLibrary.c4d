/*-- Goal defined by script --*/

#strict 2
#include GOAL

static g_StoryIndex;

/* Local variables */

public func IsFulfilled()
{
	return g_StoryIndex < 0;
}

protected func Activate(iPlayer)
{
	return MessageWindow(GameCall("GoalText"), iPlayer);
}
