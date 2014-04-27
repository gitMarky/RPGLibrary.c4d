/* Objekte, denen die KI ausweichen sollte */

#strict 2

#appendto ROCK

func Hit() { if(Local(10)) RemoveObject(); }

func ShootAt(obj, fForce, Height, faktor)
{
	var dx = GetX(obj)-GetX();
	var dy = GetY(obj)-GetY();
	var vx = dx/faktor - fForce;
	var vy = dy/faktor -Height;

	var rock = CreateObject(ROCK);
	rock->Local(10) = 1;
	rock->SetXDir(vx);
	rock->SetYDir(vy);
}