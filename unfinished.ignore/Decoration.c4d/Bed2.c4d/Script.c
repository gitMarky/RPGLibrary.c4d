/*-- Bett --*/

#strict 2

#include BED1

func Zzz()
{
	var iTime = GetActTime();
	CreateParticle("Zzz",8,-23,2+(iTime%50)*3/25+RandomX(-1,+1),-5,60,
                 RGBa(255,255,255,128));
}