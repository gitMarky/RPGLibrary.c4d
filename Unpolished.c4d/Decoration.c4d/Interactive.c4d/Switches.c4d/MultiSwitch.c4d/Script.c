/*-- Schalter --*/

#strict

//Parameter 0 - Typ des zu kontrollierenden Objekts
public SearchTarget:
  if(GreaterThan(ObjectDistance(FindObject(Par(0),0,0,-1,-1)),500)) return(0);
  return(FindObject(Par(0),0,0,-1,-1));

//Parameter 0 - Typ des zu kontrollierenden Objekts
public SetTarget:
  if(Equal(SearchTarget(Par(0)),0)) return(Message("Kein Ziel in Reichweite",this()));
  SetLocal(0,SearchTarget(Par(0)));
  Message("Ziel ist jetzt|%i",this(),GetID(Local(0)));
  return(Local(0));


func ControlRight(clnk) {
  var phase=GetPhase();
  if(GetAction()ne"SwitchRight") {
    SetAction("SwitchRight");
    if(phase) SetPhase(13-phase);
    Sound("Click");
    ObjectCall(Local(0),"SwitchRight");
  }}

func ControlLeft(clnk) {
  var phase=GetPhase();
  if(GetAction()ne"SwitchLeft") {
    SetAction("SwitchLeft");
    if(phase) SetPhase(13-phase);
    Sound("Click");
    ObjectCall(Local(0),"SwitchLeft");
  }}