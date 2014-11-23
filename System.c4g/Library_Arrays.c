/*-- Contains helper functions for arrays.
@version 0.1.0
@author Original: forgot, Modified: Marky
--*/

#strict 2

/**
 * Deletes an index from an array.
 * @param iNumber The index to delete.
 * @param aArray The array to delete the item from
 * @return Returns aArray, with the item deleted.
 */
global func DeleteArrayItem(int iNumber, &aArray)
{
	var temp = [];
	for (var cnt; cnt < GetLength(aArray); cnt++)
	{
		if (cnt == iNumber)
			continue;
		var dif = 0;
		if (cnt > iNumber)
			dif = -1;
		temp[cnt + dif] = aArray[cnt];
	}
	
	aArray = temp;
	return aArray;
}

//L�scht ein Item aus einem Array
global func &DropArrayItem(vItem, & aArray)
{
	var index = GetArrayItemPosition(vItem, aArray);
	DeleteArrayItem(index, aArray);
	return aArray;
}

//L�scht ein Item aus einem Array, kann m�glicherweise umsortieren
global func DeleteArrayItem2(iNumber, & aArray)
{
	//Ein ganz leeres Array?
	if (GetLength(aArray) == 1)
		return aArray = CreateArray();
	//Wenn das letzte Element ist diese Funktion auch nciht toller.
	if (GetLength(aArray) - 1 == iNumber)
		return DeleteArrayItem(iNumber, aArray);
	
	//Los!
	var last = aArray[GetLength(aArray) - 1];
	aArray[GetLength(aArray) - 1] = 0;
	SetLength(aArray, GetLength(aArray) - 1);
	aArray[iNumber] = last;
	return aArray;
}

//Sucht ein Item im array
global func GetArrayItemPosition(value, aArray)
{
	var number = -1;
	for (var cnt = 0; cnt < GetLength(aArray); cnt++)
	{
		if (aArray[cnt] == value)
		{
			number = cnt;
			break;
		}
		else
			continue;
	}
	return number;
}

//F�gt ein Item am Ende eines Array ein
global func PushBack(value, & aArray)
{
	return aArray[GetLength(aArray)] = value;
}

//F�gt ein Item am Anfang ein
global func PushFront(value, & aArray)
{
	var aNew = [];
	aNew[GetLength(aNew)] = value;
	for (var cnt = 0; cnt < GetLength(aArray); cnt++)
		aNew[GetLength(aNew)] = aArray[cnt];
	
	aArray = aNew;
	return 1;
}

//F�gt ein Array zu einem bestehendem am Ende hinzu.
global func FusionArrays(array aOldArray, array a2ndArray)
{
	if (GetType(aOldArray) != C4V_Array || GetType(a2ndArray) != C4V_Array)
		return;
	for (var i; i < GetLength(a2ndArray); i++)
		aOldArray[GetLength(aOldArray) + 1] = a2ndArray[i];
	return aOldArray;
}

global func FilterArray(array aOldArray, array aCompare, bool bExclude)
{
	if (GetType(aOldArray) != C4V_Array || GetType(aCompare) != C4V_Array)
		return;
	
	var aNewArray = [];
	for (var i = 0; i < GetLength(aOldArray); i++)
	{
		if (bExclude)
		{
			if (GetArrayItemPosition(aOldArray[i], aCompare) > -1)
				continue;
			else
				PushBack(aOldArray[i], aNewArray);
		}
		else
		{
			if (GetArrayItemPosition(aOldArray[i], aCompare) > -1)
				PushBack(aOldArray[i], aNewArray);
			else
				continue;
		}
	}
	
	aOldArray = aNewArray;
	return aOldArray;
}
