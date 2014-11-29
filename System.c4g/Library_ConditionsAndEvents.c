/*
An ingame script parser, which is used for dynamic scripts.

@title Conditions and Events
@author Marky
@version 0.1.0
*/

#strict 2

/**
 * Calls CheckConditionsDetailed() with the given parameters and returns true, if all conditions are fulfilled.
 * @par aConditions Passed on.
 * @par szTarget Passed on.
 * @par pTarget Passed on.
 * @par szUser Passed on.
 * @par pUser Passed on.
 * @return true, if all conditions are fulfilled@brfalse, if at least one of the conditions is not fulfilled.
 */
global func CheckConditions( aConditions, string szTarget, object pTarget, string szUser, object pUser)
{
	return CheckConditionsDetailed( aConditions, szTarget, pTarget, szUser, pUser)[0];
}

/**
 * Evaluates an array of conditions and returns if all of them are fulfilled and how many of them are fulfilled.
 * The function evaluates the conditions by calling CheckConditions()
 * @par aConditions Accepts a string or an array of strings. Passed to CheckCondition().
 * @par szTarget String identifier for an object. All occurrences of szTarget will be replaced with a reference
 * to pTarget.
 * @par pTarget Replaces the szTarget string.
 * @par szUser String identifier for an object. All occurrences of szUser will be replaced with a reference
 * to pUser.
 * @par pUser Replaces the szUser string.
 * @return array Position [0] contains a boolean value, which is true if all conditions are fulfilled.@br
 *               Position [1] contains an integer value, which counts the amount of fulfilled conditions.@br
 *               If position [i] = x, then the first x conditions are fulfilled.
 * @note The conditions are parsed by their order in the array, from first to last. Parsing stops at the
 * first condition that is not fulfilled. 
 */
global func CheckConditionsDetailed( aConditions, string szTarget, object pTarget, string szUser, object pUser)
{
	var fFulfilled = [];
	// no condition is always fulfilled
	if(!aConditions) return [true];
	if( aConditions == "") return [true];
	if( GetType(aConditions) == C4V_Array ) if(GetLength(aConditions) < 1) return [true];

	var fFulfilled = true; // in favor of the suspect
	var iFulfilled = 0;    // number of fulfilled conditions
	var aCond;

	if( GetType( aConditions ) == C4V_String )
	{
		fFulfilled = CheckCondition( aConditions, szTarget, pTarget, szUser, pUser );
		if(fFulfilled) iFulfilled = 1;
	}
	else
	{
			for( aCond in aConditions )
				// a non-fulfilled condition? The suspect is found guilty! ;)
				if(!CheckCondition( aCond, szTarget, pTarget, szUser, pUser ))
				{
					fFulfilled = false;
					break;
				}
				else
				{
					iFulfilled++;
				}
	}


	return [fFulfilled, iFulfilled];
}

/**
 * Evaluates a single condition and returns whether it is fulfilled.@br
 * The function has two string placeholders that can be replaced with object references: szTarget and szUser.
 * 
 * @par aConditions The condition. This has to consist of valid C4Script commands as any other object script, as 
 * this string will be passed to the eval()-function. Passing no string is allowed, and counts as always fulfilled.
 * @par szTarget String identifier for an object. All occurrences of szTarget will be replaced with a reference
 * to pTarget before evaluating the script.
 * @par pTarget Replaces the szTarget string.
 * @par szUser String identifier for an object. All occurrences of szUser will be replaced with a reference
 * to pUser before evaluating the script.
 * @par pUser Replaces the szUser string. 
 * @return bool the return value of the eval()-function.
 * @note This is intended for conditions, but it can be used for generic scripts in other ways, too. Be careful
 * with the input:
 * {@code
 * CheckCondition("RemoveObject(szUser)", "blub", 0, "szUser", CreateObject(HUT2))
 * }
 * creates a hut and tries to delete it. If deleting the hut fails, then the function returns false, but the hut is 
 * still there. 
 * Similarly, the creation of the hut can change the landscape, which is not reverted even if the hut is successfully 
 * deleted.
 */
global func CheckCondition( aCondition, string szTarget, object pTarget, string szUser, object pUser )
{
	// no condition is always fulfilled
	if(!aCondition) return true;
	if( GetType(aCondition) != C4V_Array && GetType(aCondition) != C4V_String ) return true;

	var szEval = aCondition;

	szEval = ReplaceAll(szEval,szTarget,Format("Object(%d)",ObjectNumber(pTarget)));
	szEval = ReplaceAll(szEval,szUser, Format("Object(%d)",ObjectNumber(pUser)));

	DebugLog("Evaluating \"%s\"",szEval);

	var result = eval( szEval );
	DebugLog("Evaluation: %v", result);
	return result;
}

/**
 * Evaluates an array of events by calling CheckCondition() on each event string.
 * @par aConditions Accepts a string or an array of strings. Passed to CheckCondition().
 * @par szTarget String identifier for an object. All occurrences of szTarget will be replaced with a reference
 * to pTarget.
 * @par pTarget Replaces the szTarget string.
 * @par szUser String identifier for an object. All occurrences of szUser will be replaced with a reference
 * to pUser.
 * @par pUser Replaces the szUser string.
 * @note This is essentially the same as CheckConditionsDetailed(), but it evaluates all scripts in the array
 * and does not stop if one of the scripts returns false.
 */
global func ProcessEvents( aEvents, string szTarget, object pTarget, string szUser, object pUser)
{
	// no events? Evaluate nothing
	if(!aEvents) return true;
	
	if( GetType(aEvents) == C4V_Array ) if(GetLength(aEvents) < 1) return true;

	var aEv;

	if( GetType( aEvents ) == C4V_String )
		CheckCondition( aEvents, szTarget, pTarget, szUser, pUser );
	else
	{
		for( aEv in aEvents )
			CheckCondition( aEv, szTarget, pTarget, szUser, pUser );
	}

}

/*--
Events and conditions are a key element of the RPG Library: They are used in the key features,
such as quests, dialogues and AI behaviour.@br
The core idea behind dialogues and other systems is saving small scripts or function calls in arrays,
so that they can be called and executed by eval() later and with certain parameters. This is useful, because you can have an
object do something, then execute scripts from the array and then do something else.
On the downside, reading scripts from an array and executing them is slower, harder to debug, and 
more exploitable, so you have to know what you are doing when you use them.@br
In most cases the script is for user-defined keywords, that are replaced with object references
before the engine passes the string to the eval() function. The keywords should also make the scripts
easy to understand. At the moment two keywords are supported.
{@section Conditions}
Conditions are usually used in an if()-call: Certain information gets displayed, the player gets
a reward, a quest starts if the conditions are met.
See CheckConditions(), CheckConditionsDetailed().@br
For example, the function call
{@code  CheckConditionsDetailed(["FindContents(GOLD, customer)", "Enter(vendor, FindContents(GOLD, customer))", "CreateContents(FLNT, customer)"], "vendor", this, "customer", target);} 
Returns [true, 3] if all scripts can be executed. The string "vendor" in the commands will be replaced with a reference to the calling object, and the 
string "customer" will be replaced with a reference to the variable "target".@br
If the customer has no gold, then the second and third command will not be executed and the return 
value is [false, 0].@br
If the customer has gold and the gold can be transferred to the vendor, then a flint is created in the
customer's inventory. If the firestone cannot be created, then the gold is not given back to the customer.@br
To give the gold back you need to write a command, that is executed only if the flint is not created. This is not hard with a little logic:@br
{@code "CreateContents(FLNT, customer) || Enter(customer, FindContents(GOLD, vendor)"}
Should do the trick. The ||-operator stops if the flint can be created, but it gives the gold back, in case the flint is not created.
This is the best we can do in this case and if giving back the gold should fail, then that is bad luck.
{@section Events}
Events actually use the same mechanic for parsing scripts as the conditions do. The difference is, that ProcessEvents()
executes all scripts that are passed via array, no matter if one of the scripts returns false.@br
They also save the purpose of making the script more readable. In the above example we just used conditions,
now we use conditions and events:@br
{@code
if (CheckConditions(["FindContents(GOLD, customer)", "Enter(vendor, FindContents(GOLD, customer))"))
{
	ProvessEvents(["CreateContents(FLNT, customer) || Enter(customer, FindContents(GOLD, vendor)"]);
}
}
The script still does the same as before, but may be more readable.
@note
The two examples may seem unnecessarily complex, because they parse a string with keywords and 
evaluate that string with eval(). In a normal situation you can just call FindContents() without 
parsing anything, which is a lot more practical and performs better.@br
For this reason we use events and conditions only in automated scripts. We use a data structure, 
usually an array, and parse the information to create menus or variables. Sometimes we have 
if() blocks in our automation, that allow the user to specify behaviour. This is where events and 
conditions are used.@br
The automation usually is inherited from a library, so you can call local functions in the executing 
object. This means that you can model more complex commands with local functions as well:@br
{@code
if (CheckConditions(["CanPurchase(FLNT, customer, vendor)"], "vendor", this, "customer", target))
{
ProcessEvents(["DoPurchase(FLNT, customer, vendor)", "SayThanks(vendor)"], "vendor", this, "customer", target);
}
}
@title Using events and conditions
@category #Tutorials
@file tutorialEventsAndConditions.html
@author Marky
--*/
