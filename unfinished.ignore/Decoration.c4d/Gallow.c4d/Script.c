/*-- Galgen --*/

#strict 2

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
