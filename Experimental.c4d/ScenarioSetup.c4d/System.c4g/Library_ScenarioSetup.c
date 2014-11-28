/* Setting up the scenario */

#strict 2

global func DoInitPlayer(int player)
{
	var index = 0;
	while(GetPlayerByIndex(index, C4PT_User) != player && index < GetPlayerCount(C4PT_User)) index++;
	var pClonk = GetHiRank(player);
	SetFoW(1, player);
	SetPlrViewRange(0, pClonk);
	pClonk->SetRelaunch();

	// Storyobject erzeugen
	if(!FindObject(_STY))
	{
		CreateObject(_STY)->~Start();
	}
}


global func StartSzen()
{
	for(var i = 0; i < GetPlayerCount(C4PT_User); i++)
	{
		GetCursor(GetPlayerByIndex(i,C4PT_User))->~StopClonkEx();
		GameCall("PlayerStart", GetPlayerByIndex(i,C4PT_User));
	}
	GameCall("DoStartSzen", GetPlayerByIndex(i,C4PT_User));

	// Spielziel erzeugen
	if(!FindObject(SCRG)) CreateObject(SCRG);
	
	// Neu Initialisierung
	var story = FindObject(_STY);
	if(GetID() == _STY) story = this;
	if(!story) story = CreateObject(_STY);
	story->~DoStartSzen();

	// Gemeinschaftskonto
	if(!FindObject(TACC) && GetPlayerCount(C4PT_User) > 1) CreateObject(TACC);
}
