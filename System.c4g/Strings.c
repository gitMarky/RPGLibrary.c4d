#strict 2

/*
Funktionssammlung by TqmTqm
Rückgabetyp | Funktionskopf
--------------------------------------------------------------------------------------------
String      | ReplaceAll(string zeichenkette,string suchMuster,string ersetzenDurch)
String      | ReplaceFirst(string zeichenkette,string suchMuster,string ersetzenDurch)
String      | Substring(int start,string zeichenkette,int ende)
String      | CharAt(string zeichenkette,int i)	
String      | GetLetter(int zahl)
--------------------------------------------------------------------------------------------

ReplaceAll
	string zeichenkette -> die zu bearbeitende zeichenkette
	string suchMuster   -> die passage die zu ersetzten is
	string ersetzenDurch-> damit wird beim befund das suchmuster ersetzt
	
	bei ReplaceAll werden alle vorkommen von suchMuster im string zeichenkette durch
	ersetzenDurch ersetzt.
	
ReplaceFirst
	string zeichenkette -> die zu bearbeitende zeichenkette
	string suchMuster   -> die passage die zu ersetzten is
	string ersetzenDurch-> damit wird beim befund das suchmuster ersetzt
	
	bei ReplaceAll wird das erste suchMuster im string zeichenkette durch
	ersetzenDurch ersetzt.
	
Substringing
	int start			-> legt fest wo der neue string starten soll (0 ist ganz vorne)
	string zeichenkette -> die zu bearbeitende zeichenkette
	int ende(optional)  -> legt fest wo das ende des neue strings ist
	
	Substringing erzeugt aus einem string einen teilstring der durche einen bzw 2 
	parameter eingegrenzt wird. Hier dazu 2 Beispiele:
	
	Substring(1,"Clonk") -> "lonk"
	Substring(1,"Hamburger",4) -> "amb"
	
CharAt
	string zeichenkette -> die zu durchsuchende zeichenkette
	int i 				-> position (0 ist das erste zeichen)
	
	CharAt liefert den buchstaben an der position i zurück
	
GetLetter
	int zahl 			-> liefert das zeichen an der i der ascii tabelle
*/


global func ReplaceAll(string zeichenkette,string suchMuster,string ersetzenDurch){
	/*
	// Original-Funktion
	for (var i = 0;i<GetLength(zeichenkette);i++){
		if (suchMuster == Substring(i,zeichenkette,GetLength(suchMuster)+i))
			zeichenkette = Format("%s%s%s",Substring(0,zeichenkette,i),ersetzenDurch,Substring(GetLength(suchMuster)+i,zeichenkette,GetLength(zeichenkette)));	
	}*/
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
global func ReplaceFirst(string zeichenkette,string suchMuster,string ersetzenDurch){
	for (var i = 0;i<GetLength(zeichenkette);i++){
		if (suchMuster == Substring(i,zeichenkette,GetLength(suchMuster)+i))
		{
			zeichenkette = Format("%s%s%s",Substring(0,zeichenkette,i),ersetzenDurch,Substring(GetLength(suchMuster)+i,zeichenkette,GetLength(zeichenkette)));	
			return(zeichenkette);
		}
	}
	return(zeichenkette);
}
global func Substring(int start,string zeichenkette,int ende){
	var neueZeichenkette = "";
	for (var i = 0;i<GetLength(zeichenkette) && (i < ende || ende == 0);i++)
	{
		if (i >= start)
				neueZeichenkette = Format("%s%s",neueZeichenkette,GetLetter(GetChar(zeichenkette,i)));
	}
	// Korrektur: Failsafe, damit ReplaceFirst funktioniert
	if(!start && !ende) neueZeichenkette = "";

	return(neueZeichenkette);
}
global func CharAt(string zeichenkette,int i){
	return (GetLetter(GetChar(zeichenkette,i)));
}
global func GetLetter(int zahl){
	 if(zahl == 228) return "ä";
	 if(zahl == 246) return "ö";
	 if(zahl == 252) return "ü";
	 if(zahl == 223) return "ß";
	 if(zahl == 196) return "Ä";
	 if(zahl == 214) return "Ö";
	 if(zahl == 220) return "Ü";


	//var asciiArray = [" ","?","!","''","#","$","%","&","'","(",")","*","+",",","-",".","/","0","1","2","3","4","5","6","7","8","9",":",";","<","=",">","?","@","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","[","Backslash","]","^","`","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","{","|","}","~"];
	// korrigierte Tabelle: § stimmt allerdings nicht, ist nur ein Platzhalter
	var asciiArray = [" ","!","\"","#","$","%","&","'","(",")","*","+",",","-",".","/","0","1","2","3","4","5","6","7","8","9",":",";","<","=",">","?","@","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","[","/","]","^","§","`","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","{","|","}","~"];
	
	//for(var i=0; i<GetLength(asciiArray);i++)
	//	Log("Array: %d, Char: %d, Sign: %s",i+32,GetChar(asciiArray[i]),asciiArray[i]);
	
	return(asciiArray[zahl-32]);
}


global func GetLetterCount( string szL, string szString)
{
	var x=0;
	for( var i=0; i<GetLength(szString); i++)
	{
		if(szL == CharAt(szString,i)) x++;
	}
	return x;
}
