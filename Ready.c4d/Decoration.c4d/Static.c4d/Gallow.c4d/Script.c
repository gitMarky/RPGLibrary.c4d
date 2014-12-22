/*-- A gallow.
@author Randrian ({@link http://clonkforge.net/pr.php?pr=1415})
@version 0.2.0
--*/

#strict 2

#include LF_D

func MakeReady(pClonk)
{
	pClonk->SetAction("HangReady", this);
	SetVertexXY(0,7,1);
	SetAction("Closed");
}

func DoHang(pClonk)
{
	pClonk->SetAction("Hang", this);
	SetVertexXY(0,6,4);
	SetAction("Open");
}
