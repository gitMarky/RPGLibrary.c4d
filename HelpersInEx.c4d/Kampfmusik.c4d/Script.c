#strict

Initialize:
SetAction("Check");
/*
  SetVar(0,Random(2));
  if(Var(0)==0)  SetAction("Check");
  if(Var(0)==1)  SetAction("Check2");
*/
  SetVar(2,Random(15));
  if(Var(2)==0)  if(SetVar(0,FindObject(KNIG,-150,-150,300,300))) { Message("<%s>|Jetzt wird's gefährlich!",Var(0),GetName(Var(0))); Sound("Massaker_01",Var(0)); }
  if(Var(2)==1)  if(SetVar(0,FindObject(KNIG,-150,-150,300,300))) { Message("<%s>|Ich mach dich platt!",Var(0),GetName(Var(0))); Sound("Massaker_02",Var(0)); }
  if(Var(2)==2)  if(SetVar(0,FindObject(KNIG,-150,-150,300,300))) { Message("<%s>|Oh Gott!",Var(0),GetName(Var(0))); Sound("Massaker_03",Var(0)); }
  if(Var(2)==3)  if(SetVar(0,FindObject(KNIG,-150,-150,300,300))) { Message("<%s>|Stirb!",Var(0),GetName(Var(0))); Sound("Massaker_04",Var(0)); }
  if(Var(2)==4)  if(SetVar(0,FindObject(KNIG,-150,-150,300,300))) { Message("<%s>|Der Kampf kann beginnen!",Var(0),GetName(Var(0))); Sound("Massaker_05",Var(0)); }
  if(Var(2)==5)  if(SetVar(0,FindObject(KNIG,-150,-150,300,300))) { Message("<%s>|Nieder mit ihnen!",Var(0),GetName(Var(0))); Sound("Massaker_06",Var(0)); }
  return(1);

Timer:
  var pEnemy;

  if(!FindObject(KNIG,-150,-150,300,300))  return(RemoveObject());
  while(pEnemy = FindObject(0,-150,-150,300,300, OCF_Edible(), 0, 0, 0, pEnemy))  if(ObjectCall(pEnemy,"IsGegner"))  if(GetAlive(pEnemy))  return(0);
  RemoveObject();
  return(1);