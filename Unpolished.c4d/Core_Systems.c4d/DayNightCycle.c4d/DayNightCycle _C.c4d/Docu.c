/*--
In this tutorial you will learn how to set up a cut scene in a demo scenario. If you have not done so yet, read 
the [TODO: Link to Scenario Basics: Adding a quest] first.@br
At the moment the NPCs in our scenario are just standing around. We can change this by adding a day night cycle.
{@section The basics}
A day-night-cycle works very similar to a quest. It is defined by an array also, but instead of having different stages it executes events according to the current hour
in game time. An entry in the day-night-cycle array has the following structure:
{@code
[hour when applied: int, conditions: string or array, events: string or array]
}
Since these events happen only once every hour they are not useful for an artificial intelligence. The day-night-cycle may have several entries for the same hour,
with different or the same conditions. Day-night-cycles are typically used to define NPC behaviour, but you can also use it to make items do certain things. This is
also possible with the UpdateTime() function which however applies to all objects of the same kind, when defined in the script. The day-night-cycle is used for
individual objects instead.@br
In our example we will use behaviours to let the NPCs walk around a bit. In the current version there are only a few behaviours defined, so don't expect much. The idea
behind behaviours is the following: An NPC has different priorities for behaviours. These are accessible by constants. They are, in ascending order: {@i gBehaviour_Priority_Idle, 
gBehaviour_Priority_Mission, gBehaviour_Priority_Task, gBehaviour_Priority_Override}.
So idle behaviour, such as walking around can be overriden by a mission behaviour, such as looking for an intruder. Finally, this behaviour can be overriden by single tasks of the
mission, such as searching a certain room.
{@section Adding behaviours to the scenario}
At first we need a time object in our scenario, and we attach the day-night-cycles to our NPCs:
{@code
	CreateObject(TIME);
	AttachDayNightCycle(sorcerer,"Marvin");
	AttachDayNightCycle(knight, "Arthur");
}
Then we define their cycles in the story object, in their respective script files:
{@code
func DayNightCycleMarvin()
{
	return [[6,0,"AddBehaviour(pTarget, gBehaviour_WanderPoint, 20, [[215, 400],30,5,35,50], gBehaviour_Priority_Idle)"],
	        [18,0,"RemoveBehaviour(pTarget, gBehaviour_WanderPoint, gBehaviour_Priority_Idle)"]];
}
}
Which means, that at 6 o'clock Marvin starts walking around the point (215, 400), in with a radius of 30 pixels. He makes 
random pauses of 5 to 35 frames and walks with 50% of his normal speed. At 18 o'clock he stops this behaviour.
{@code
func DayNightCycleArthur()
{
	return [[6,0,"AddBehaviour(pTarget, gBehaviour_WanderArea, 20, [284,336,1,15,75], gBehaviour_Priority_Idle)"],
	        [18,0,"RemoveBehaviour(pTarget, gBehaviour_WanderArea, gBehaviour_Priority_Idle)"]];
}
}
Similarly, Arthur wanders around between the x-coordinates 284 and 336, with pauses of 1 to 15 frames at 75% of his normal speed.
Finally, we have to put something in Marvin's inventory, or else he can snatch away the armor from us when we get it out of his
tower:
{@code
CreateContents(ROCK, sorcerer);
}
And that's it for behaviours at the moment.
@title Demo Scenario: Adding a day night cycle
@category #Tutorials
@file tutorialDemoScenario004DayNightCycle.html
@author Marky
@version 0.1.0
--*/