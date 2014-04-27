/*-- Scriptdefiniertes Spielziel --*/

#strict 2
#include GOAL

static g_StoryIndex;

/* Lokale Variablen */

public func IsFulfilled()
{
  return g_StoryIndex < 0;
}

protected func Activate(iPlayer)
{
  return MessageWindow(GameCall("GoalText"), iPlayer);
}
