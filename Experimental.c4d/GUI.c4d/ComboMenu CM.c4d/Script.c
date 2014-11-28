#strict 2

local pCommandObj;
local key_l,key_r,key_u,key_d,key_p,key_s,key_t,key_q;
local sym_l,sym_r,sym_u,sym_d,sym_p,sym_s,sym_t,sym_q;

/*
  Keys:
  "L" Left	Links
  "R" Right	Rechts
  "U" Up	Oben
  "D" Down	Unten
  "P" Primary	Werfen
  "S" Secondary	Graben
  "T" Tertiary	Special1
  "Q" Quartiary Special2
*/

/* -------------------- globale Funktionen -----------------------*/

global func ComboCreateMenu(object obj)
{
	if( !obj ) return;
	if( !( obj->~CanUseComboMenu() ) ) return;

	// Altes Menu evtl. schließen

	var existing_menu = LocalN("pComboMenu",obj);

	if( existing_menu ) existing_menu->~ComboClose();

	// Neues Menu öffnen

	var menu = CreateObject(ID_Menu_Combo,0,0,-1);

	SetPosition( GetX(obj), GetY(obj), menu);

	LocalN("pComboMenu",obj) = menu;
	LocalN("pCommandObj",menu) = obj;

	//ScheduleCall( this, "PositionKeys", 1 );
	AddEffect("PositionKeys",menu,50,1,menu);

	return 1;
}

global func ComboCloseMenu( obj )
{
	if( !obj ) return;
	if( !( obj->~CanUseComboMenu() ) ) return;

	var existing_menu = LocalN("pComboMenu",obj);

	if( existing_menu )
	{
		existing_menu->~ComboClose();

		return 1;
	}

	return;
}

global func ComboAddMenuItem( obj, key, symbolid, name, call, par1, par2, par3, par4, par5 )
{
	var menu = LocalN("pComboMenu",obj);

	if( !menu ) return 0;

	menu->~ComboAddKey( key, symbolid, name, call, par1, par2, par3, par4, par5 );

	return 1;
}

global func ComboMenuTest( obj )
{
	ComboCreateMenu( obj );

	ComboAddMenuItem( obj, "U", ROCK, "Optionen", "MenuOptionen");
	ComboAddMenuItem( obj, "D", ROCK, "Optionen", "MenuOptionen");
	ComboAddMenuItem( obj, "L", ROCK, "Optionen", "MenuOptionen");
	ComboAddMenuItem( obj, "R", ROCK, "Optionen", "MenuOptionen");

	ComboAddMenuItem( obj, "P", ROCK, "Optionen", "MenuOptionen");
	ComboAddMenuItem( obj, "S", ROCK, "Optionen", "MenuOptionen");
	ComboAddMenuItem( obj, "T", ROCK, "Optionen", "MenuOptionen");
	ComboAddMenuItem( obj, "Q", ROCK, "Optionen", "MenuOptionen");
}

/* -------------------- das Menu -----------------------*/

public func ComboAddKey( key, symbolid, name, call, par1, par2, par3, par4, par5 )
{
	if( key == "L" ) { if( key_l ) RemoveObject( key_l ); if( sym_l ) RemoveObject( sym_l ); }
	if( key == "R" ) { if( key_r ) RemoveObject( key_r ); if( sym_r ) RemoveObject( sym_r ); }
	if( key == "U" ) { if( key_u ) RemoveObject( key_u ); if( sym_u ) RemoveObject( sym_u ); }
	if( key == "D" ) { if( key_d ) RemoveObject( key_d ); if( sym_d ) RemoveObject( sym_d ); }
	if( key == "P" ) { if( key_p ) RemoveObject( key_p ); if( sym_p ) RemoveObject( sym_p ); }
	if( key == "S" ) { if( key_s ) RemoveObject( key_s ); if( sym_s ) RemoveObject( sym_s ); }
	if( key == "T" ) { if( key_t ) RemoveObject( key_t ); if( sym_t ) RemoveObject( sym_t ); }
	if( key == "Q" ) { if( key_q ) RemoveObject( key_q ); if( sym_q ) RemoveObject( sym_q ); }

	var pic_key, pic_sym, ang;

	pic_key = CreateObject( ID_Menu_ComboSymbol, 0, 0, -1 );
	//pic_sym = CreateObject( symbolid, 0, 0, -1);

	if( key == "L" ) { key_l = pic_key; sym_l = pic_sym; ang = 270; }
	if( key == "R" ) { key_r = pic_key; sym_r = pic_sym; ang = 90;}
	if( key == "U" ) { key_u = pic_key; sym_u = pic_sym; ang = 0; }
	if( key == "D" ) { key_d = pic_key; sym_d = pic_sym; ang = 180; }
	if( key == "P" ) { key_p = pic_key; sym_p = pic_sym; ang = 315; }
	if( key == "S" ) { key_s = pic_key; sym_s = pic_sym; ang = 45; }
	if( key == "T" ) { key_t = pic_key; sym_t = pic_sym; ang = 225; }
	if( key == "Q" ) { key_q = pic_key; sym_q = pic_sym; ang = 135; }

	pic_key->~SetKeyCommand( pCommandObj, key, name, symbolid, ang, call, par1, par2, par3, par4 );

	//PositionKeys();

	return 1;

}

public func ComboInput( key )
{
	var query;

	if( pCommandObj->~MenuQueryCancel() ) query = 1;

	// Ungültige Eingaben schließen normal das Menu

	if( key == "L" ) if( !key_l ) if( query ) return( 0 );
	if( key == "R" ) if( !key_r ) if( query ) return( 0 );
	if( key == "U" ) if( !key_u ) if( query ) return( 0 );
	if( key == "D" ) if( !key_d ) if( query ) return( 0 );
	if( key == "P" ) if( !key_p ) if( query ) return( 0 );
	if( key == "S" ) if( !key_s ) if( query ) return( 0 );
	if( key == "T" ) if( !key_t ) if( query ) return( 0 );
	if( key == "Q" ) if( !key_q ) if( query ) return( 0 );

	// Eingabe tätigen

	if( key == "L" ) if( key_l ) key_l->~ReceiveInput();
	if( key == "R" ) if( key_r ) key_r->~ReceiveInput();
	if( key == "U" ) if( key_u ) key_u->~ReceiveInput();
	if( key == "D" ) if( key_d ) key_d->~ReceiveInput();
	if( key == "P" ) if( key_p ) key_p->~ReceiveInput();
	if( key == "S" ) if( key_s ) key_s->~ReceiveInput();
	if( key == "T" ) if( key_t ) key_t->~ReceiveInput();
	if( key == "Q" ) if( key_q ) key_q->~ReceiveInput();

	ComboClose(); // Eine Eingabe schließt das Menü

	return( 1 );
} 

public func FxPositionKeysTimer()
{
	if( !pCommandObj ) return(RemoveObject());

	var x, y;

	x = GetX( pCommandObj );
	y = GetY( pCommandObj );

	var dx, dy;
/*
	dxn = -35; dyn = -35;
	dxp = +35; dyp = +35;

	if(( x + dxn ) < 5 ) dxn = 5 - x;
	if(( x + dxp ) > ( LandscapeWidth() - 5 )) dxp = LandscapeWidth() - x;

	if(( y + dyn ) < 5 ) dyn = 5 - y;
	if(( y + dyp ) > ( LandscapeHeight() - 5 )) dyp = LandscapeWidth() - y;

	if( key_l ) { SetPosition( x + dxn, y, key_l); SetXDir( 0, key_l); SetYDir( 0, key_l); }
	if( key_r ) { SetPosition( x + dxp, y, key_r); SetXDir( 0, key_r); SetYDir( 0, key_r); }
	if( key_u ) { SetPosition( x, y + dyn, key_u); SetXDir( 0, key_u); SetYDir( 0, key_u); }
	if( key_d ) { SetPosition( x, y + dyp, key_d); SetXDir( 0, key_d); SetYDir( 0, key_d); }

	if( key_p ) { SetPosition( x + dxn, y + dyn, key_p); SetXDir( 0, key_p); SetYDir( 0, key_p); }
	if( key_s ) { SetPosition( x + dxp, y + dyn, key_s); SetXDir( 0, key_s); SetYDir( 0, key_s); }
	if( key_t ) { SetPosition( x + dxn, y + dyp, key_t); SetXDir( 0, key_t); SetYDir( 0, key_t); }
	if( key_q ) { SetPosition( x + dxp, y + dyp, key_q); SetXDir( 0, key_q); SetYDir( 0, key_q); }
*/

	var aKeys = [ key_l, key_r, key_u, key_d, key_p, key_s, key_t, key_q ];
	var pKey;
	var iAngle, iRadius = 50;

	for( pKey in aKeys )
	{
		if( pKey == key_u ) iAngle = 0;
		if( pKey == key_s ) iAngle = 45;
		if( pKey == key_r ) iAngle = 90;
		if( pKey == key_q ) iAngle = 135;
		if( pKey == key_d ) iAngle = 180;
		if( pKey == key_t ) iAngle = 225;
		if( pKey == key_l ) iAngle = 270;
		if( pKey == key_p ) iAngle = 315;

		dx = Sin(iAngle, iRadius);
		dy = -Cos(iAngle, iRadius);

		if(( x + dx ) < 5 ) dx = 5 - x;
		if(( x + dx ) > ( LandscapeWidth() - 5 )) dx = LandscapeWidth() - x;

		if(( y + dy ) < 5 ) dy = 5 - y;
		if(( y + dy ) > ( LandscapeHeight() - 5 )) dy = LandscapeHeight() - y;

		SetPosition( x + dx, y + dy, pKey); SetXDir( 0, pKey); SetYDir( 0, pKey); 
	}

	//ScheduleCall( this, "PositionKeys", 1 );
}

public func ComboClose()
{
	if( key_l ) RemoveObject( key_l );
	if( key_r ) RemoveObject( key_r );
	if( key_u ) RemoveObject( key_u );
	if( key_d ) RemoveObject( key_d );
	if( key_p ) RemoveObject( key_p );
	if( key_s ) RemoveObject( key_s );
	if( key_t ) RemoveObject( key_t );
	if( key_q ) RemoveObject( key_q );

	if( sym_l ) RemoveObject( sym_l );
	if( sym_r ) RemoveObject( sym_r );
	if( sym_u ) RemoveObject( sym_u );
	if( sym_d ) RemoveObject( sym_d );
	if( sym_p ) RemoveObject( sym_p );
	if( sym_s ) RemoveObject( sym_s );
	if( sym_t ) RemoveObject( sym_t );
	if( sym_q ) RemoveObject( sym_q );

	RemoveObject();
}

