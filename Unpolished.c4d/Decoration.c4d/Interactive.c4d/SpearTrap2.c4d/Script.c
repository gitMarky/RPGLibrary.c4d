/*--
@title Spear trap 2
@version 0.2.0
@author Stephan Breer (Die Pyramide von Clucatan)
--*/


#strict

private func CheckHero()
{
  if(Var()=FindObject(0,-10,-10,20,20,OCF_Alive()|OCF_CrewMember()))
    DoEnergy(-5,Var());
}
