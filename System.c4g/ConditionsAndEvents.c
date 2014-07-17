#strict 2

global func CheckConditions( aConditions, string szTarget, object pTarget, string szUser, object pUser)
{
//	// keine Bedingungen sind immer erfüllt
//	if(!aConditions) return true;
//	if( GetType(aConditions) == C4V_Array ) if(GetLength(aConditions) < 1) return true;
//
//	var fFulfilled = true; // im Zweifel für den Angeklagten
//	var aCond;
//
//	if( GetType( aConditions ) == C4V_String )
//		fFulfilled = CheckCondition( aConditions, szTarget, pTarget, szUser, pUser );
//	else
//	{
//			for( aCond in aConditions )
//				// die erste nicht-erfüllte Bedingung macht ihn schuldig ;)
//				if(!CheckCondition( aCond, szTarget, pTarget, szUser, pUser ))
//				{
//					fFulfilled = false;
//					break;
//				}
//	}
//
//	return fFulfilled;
	return CheckConditionsDetailed( aConditions, szTarget, pTarget, szUser, pUser)[0];
}

global func CheckConditionsDetailed( aConditions, string szTarget, object pTarget, string szUser, object pUser)
{
	var fFulfilled = [];
	// keine Bedingungen sind immer erfüllt
	if(!aConditions) return [true];
	if( GetType(aConditions) == C4V_Array ) if(GetLength(aConditions) < 1) return [true];

	var fFulfilled = true; // im Zweifel für den Angeklagten
	var iFulfilled = 0;    // Anzahl der erfüllten Bedingungen
	var aCond;

	if( GetType( aConditions ) == C4V_String )
	{
		fFulfilled = CheckCondition( aConditions, szTarget, pTarget, szUser, pUser );
		if(fFulfilled) iFulfilled = 1;
	}
	else
	{
			for( aCond in aConditions )
				// die erste nicht-erfüllte Bedingung macht ihn schuldig ;)
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

global func CheckCondition( aCondition, string szTarget, object pTarget, string szUser, object pUser )
{
	// keine Bedingungen sind immer erfüllt
	if(!aCondition) return true;
	if( GetType(aCondition) != C4V_Array && GetType(aCondition) != C4V_String ) return true;

	var szEval = aCondition;

	szEval = ReplaceAll(szEval,szTarget,Format("Object(%d)",ObjectNumber(pTarget)));
	szEval = ReplaceAll(szEval,szUser, Format("Object(%d)",ObjectNumber(pUser)));

	DebugLog("Evaluating \"%s\"",szEval);

	var result = eval( szEval );
	DebugLog("Evaluation: %v", result);
	return result; //eval( szEval );
}

global func ProcessEvents( aEvents, string szTarget, object pTarget, string szUser, object pUser)
{
	// keine Bedingungen sind immer erfüllt
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
