/* Contains helper functions for arrays.
@version 0.1.0
@author Original: forgot, Modified: Marky
@note Refactored in v0.2.0: FusionArrays() removed, added functionality to PushBack() and PushFront().
*/

#strict 2

/**
 Deletes an index from an array.
 @par index The index to delete.
 @par aArray The array to delete the item from.
 @return aArray, with the item deleted.
 */
global func DeleteArrayItem(int index, &aArray)
{
	var temp = [];
	for (var i; i < GetLength(aArray); i++)
	{
		if (i == index)
			continue;
		var dif = 0;
		if (i > index)
			dif = -1;
		temp[i + dif] = aArray[i];
	}
	
	aArray = temp;
	return aArray;
}

/**
 Deletes content from an array.
 @par value The item, that is the content that should be deleted.
 @par aArray The array to delete the item from.
 @return aArray, with the item deleted.
 @example
 {@code
   DeleteArrayItem("b", ["a", "b", "c"]);
 }
 Returns {@c ["a", "c"]}.
 */
global func &DropArrayItem(value, &aArray)
{
	var index = GetArrayItemPosition(value, aArray);
	DeleteArrayItem(index, aArray);
	return aArray;
}

/**
 Deletes an index from an array. The function moves the last index 
 of the array to the index that should be deleted and returns the 
 shortened array.
 @par index The index to delete.
 @par aArray The array to delete the item from.
 @return aArray, with the item deleted.
 @example
 {@code
	DeleteArrayItem2(1, ["a", "b", "c", "d"]);
 }
 Returns {@c ["a", "d", "c"]}.]
 */
global func DeleteArrayItem2(index, &aArray)
{
	if (GetLength(aArray) <= 1) return aArray = CreateArray();
	if (GetLength(aArray) - 1 == index) return DeleteArrayItem(index, aArray);

	var last = aArray[GetLength(aArray) - 1];
	aArray[GetLength(aArray) - 1] = 0;
	SetLength(aArray, GetLength(aArray) - 1);
	aArray[index] = last;
	return aArray;
}

/**
 Returns the index of a value in the array, if it exists.
 @par value The position of this value will be returned.
 @par aArray The array that contains the value.
 @return The position, starting with 0, of the value in the array. 
 Returns -1 if the array does not contain that value.
 */
global func GetArrayItemPosition(value, aArray)
{
	var number = -1;
	for (var i = 0; i < GetLength(aArray); i++)
	{
		if (aArray[i] == value)
		{
			number = i;
			break;
		}
		else
			continue;
	}
	return number;
}

global func ArrayTypecheck(aArray, value, type)
{
	var passed_typecheck = true;
	
	if (GetType(value) != C4V_Array)
	{
		if (GetType(value) != type)
		{
			ErrorLog(Format("The parameter is of invalid type: %v - only %v is allowed", value, type));
			passed_typecheck = false;
		}
	}
	else
	{
		if (type != C4V_Array)
		{
			for (var item in value)
			if (GetType(item) != type)
			{
				ErrorLog(Format("The parameter is of invalid type: %v - only %v is allowed (passed array: %v)", item, type, value));
				passed_typecheck = false;
			}
		}
	}
	
	
	return passed_typecheck;
}

/**
 Adds an entry to the end of the array.
 @par value The value that should be added. Can be a single value, or an array. 
 In case of an array, all the contents of the array are added, in order.
 @par aArray The value will be added to this array.
 @par typecheck [optional] Must specify a type, such as C4V_String. If this is set, 
 then the type of {@c value} will be checked against that value.
 @par add_array_as_one_entry if true and if {@c value} is an array, than {@c value}
  itself will be added instead of its contents.
 */
global func PushBack(value, &aArray, typecheck, bool add_array_as_one_entry)
{
	// create array if necessary
	if (GetType(aArray) != C4V_Array) aArray = [];
	
	if (typecheck && !ArrayTypecheck(aArray, value, typecheck)) return 0;

	if (GetType(value) == C4V_Array && !add_array_as_one_entry)
	{
		for (var item in value)
			aArray[GetLength(aArray)] = item;
	}
	else
	{
		aArray[GetLength(aArray)] = value;
	}
	return aArray[GetLength(aArray)] = value;
}

/**
 Adds an entry to the front of the array.
 @par value The value that should be added. Can be a single value, or an array. 
 In case of an array, all the contents of the array are added, in order.
 @par aArray The value will be added to this array.
 @par typecheck [optional] Must specify a type, such as C4V_String. If this is set, 
 then the type of {@c value} will be checked against that value.
 @par add_array_as_one_entry if true and if {@c value} is an array, than {@c value}
  itself will be added instead of its contents.
 */
global func PushFront(value, &aArray, typecheck, bool add_array_as_one_entry)
{
	// create array if necessary
	if (GetType(aArray) != C4V_Array) aArray = [];
	
	if (typecheck && !ArrayTypecheck(aArray, value, typecheck)) return 0;
	
	var temp = [];

	if (GetType(value) == C4V_Array && !add_array_as_one_entry)
	{
		for (var item in value)
			temp[GetLength(temp)] = item;
	}
	else
	{
		temp[GetLength(temp)] = value;
	}
	
	for (var item in aArray)
		temp[GetLength(temp)] = item;
		
	aArray = temp;
	return aArray;
}

/**
 Filters an array.
 @par aArray This array will be filtered.
 @par filter This is the filter.
 @par exclude If this is false, then all items in {@code filter} will remain in the array.
  If this is true, then all items that are not in {@code filter} will remain in the array.
 @return The filtered array {@c aArray}.
 @example
 {@code
  FilterArray(["a", "b", "c", "d"], ["b", "e"]); // returns ["b"]
  FilterArray(["a", "b", "c", "d"], ["b", "e"], true); // returns ["a", "c", "d"]
 }
 */
global func FilterArray(array &aArray, array filter, bool exclude)
{
	if (GetType(aArray) != C4V_Array || GetType(filter) != C4V_Array)
		return;
	
	var temp = [];
	for (var i = 0; i < GetLength(aArray); i++)
	{
		if (exclude)
		{
			if (GetArrayItemPosition(aArray[i], filter) > -1)
				continue;
			else
				PushBack(aArray[i], temp);
		}
		else
		{
			if (GetArrayItemPosition(aArray[i], filter) > -1)
				PushBack(aArray[i], temp);
			else
				continue;
		}
	}
	
	aArray = temp;
	return aArray;
}