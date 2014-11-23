/*-- String manipulation methods
@author TqmTqm
@version 0.1.0
--*/

#strict 2

/**
 * Replaces all occurrences of a substring with another.
 * @param zeichenkette The string that is manipulated.
 * @param suchMuster  The string that should be replaced.
 * @param ersetzenDurch The string will be replaced by this string.
 * @return The manipulated string.
 */
global func ReplaceAll(string zeichenkette, string suchMuster, string ersetzenDurch)
{
	var ersetzt = zeichenkette, i=0, n = GetLength(zeichenkette);
	while( ersetzt != zeichenkette || i==0 )
	{
		if(i > n ) break; // das sollte sicher sein ;)
		i++;
		zeichenkette = ersetzt;
		ersetzt = ReplaceFirst( zeichenkette, suchMuster, ersetzenDurch );
	}
	return(zeichenkette);
}

/**
 * Replaces the first occurrence of a substring with another.
 * @param zeichenkette The string that is manipulated.
 * @param suchMuster  The string that should be replaced.
 * @param ersetzenDurch The string will be replaced by this string.
 * @return The manipulated string.
 */
global func ReplaceFirst(string zeichenkette,string suchMuster,string ersetzenDurch)
{
	for (var i = 0;i<GetLength(zeichenkette);i++)
	{
		if (suchMuster == Substring(i,zeichenkette,GetLength(suchMuster)+i))
		{
			zeichenkette = Format("%s%s%s",Substring(0,zeichenkette,i),ersetzenDurch,Substring(GetLength(suchMuster)+i,zeichenkette,GetLength(zeichenkette)));	
			return(zeichenkette);
		}
	}
	return(zeichenkette);
}

/**
 * Creates a substring of a string.
 * @param start	Index where the substring starts, 0 based.
 * @param zeichenkette The string.
 * @param ende [optional] Index where the substring is cut off. No value or 0 means, that nothing is cut off at the end.
 * @example
 *{@code
 *	Substring(1, "Clonk") -> "lonk"
 *	Substring(1, "Hamburger", 4) -> "amb"
 *}
 */
global func Substring(int start, string zeichenkette, int ende)
{
	var neueZeichenkette = "";
	for (var i = 0; i<GetLength(zeichenkette) && (i < ende || ende == 0); i++)
	{
		if (i >= start)
				neueZeichenkette = Format("%s%s",neueZeichenkette,GetLetter(GetChar(zeichenkette,i)));
	}
	// Korrektur: Failsafe, damit ReplaceFirst funktioniert
	if(!start && !ende) neueZeichenkette = "";

	return(neueZeichenkette);
}

/**
 * Gets the character in the string at a certain position.
 * @param zeichenkette The string.
 * @param i The character at this position will be returned.
 * @return While GetChar() returns an integer, this function returns the actual character at the position.
*/
global func CharAt(string zeichenkette,int i)
{
	return (GetLetter(GetChar(zeichenkette,i)));
}

global func GetLetter(int zahl)
{
	var asciiArray = ["", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", " ", "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?", "", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_", "`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", "", "€", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "§", "", "", "", "", "", "", "", "", "°", "", "²", "³", "´", "µ", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "Ä", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "Ö", "", "", "", "", "", "Ü", "", "", "ß", "", "", "", "", "ä", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "ö", "", "", "", "", "", "ü"];
	return asciiArray[zahl];
}

global func GetLetterCount(string szL, string szString)
{
	var x=0;
	for( var i=0; i<GetLength(szString); i++)
	{
		if(szL == CharAt(szString,i)) x++;
	}
	return x;
}

global func TestGetCharGetLetter()
{
	var textArray = ["^","1","2","3","4","5","6","7","8","9","0","ß","´","q","w","e","r","t","z","u","i","o","p","ü","+","a","s","d","f","g","h","j","k","l","ö","ä","#","<","y","x","c","v","b","n","m",",",".","-",
	                 "°","!","\"","§","$","%","&","/","(",")","=","?","`","Q","W","E","R","T","Z","U","I","O","P","Ü","*","A","S","D","F","G","H","J","K","L","Ö","Ä","'",">","Y","X","C","V","B","N","M",";",":","_",
	                 "²","³","{","[","]","}","\\","€","~","|","µ"," "];

	for(var text in textArray)
	{
		Log("%s == %s : %v", text, GetLetter(GetChar(text)), text == GetLetter(GetChar(text)));
	}
}

global func LogTheAsciiCharArray()
{
	var textArray = ["^","1","2","3","4","5","6","7","8","9","0","ß","´","q","w","e","r","t","z","u","i","o","p","ü","+","a","s","d","f","g","h","j","k","l","ö","ä","#","<","y","x","c","v","b","n","m",",",".","-",
	                 "°","!","\"","§","$","%","&","/","(",")","=","?","`","Q","W","E","R","T","Z","U","I","O","P","Ü","*","A","S","D","F","G","H","J","K","L","Ö","Ä","'",">","Y","X","C","V","B","N","M",";",":","_",
	                 "²","³","{","[","]","}","\\","€","~","|","µ"," "];

	var maxChar = 0;
	var charArray = [];
	for(var i = 0; i < GetLength(textArray); i++)
	{
		charArray[i] = GetChar(textArray[i]);

		if (charArray[i] > maxChar)
			maxChar = charArray[i];
	}

	var sortedTextArray = [];
	var sortedCharArray = [];

	for(var char = 0; char <= maxChar; char++)
	{
		var position = -1;
		for(var index = 0; index < GetLength(charArray); index++)
		{
			if (charArray[index] == char)
			{
				position = index;
				break;
			}
		}

		if (position >= 0)
		{
			sortedTextArray[char] = textArray[position];
			sortedCharArray[char] = charArray[position];

			if (textArray[position] == "\""
			 || textArray[position] == "\\")
			{
				sortedTextArray[char] = Format("\\%s", textArray[position]); // Add the escapes
			}

		}
		else
		{
			sortedTextArray[char] = "";
			sortedCharArray[char] = char;
		}
	}

	Log("Text: %v", textArray);
	Log("Char: %v", charArray);
	Log("Sorting...");
	Log("Text: %v", sortedTextArray);
	Log("Char: %v", sortedCharArray);
}
