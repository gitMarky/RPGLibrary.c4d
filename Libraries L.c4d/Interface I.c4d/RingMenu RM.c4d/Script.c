#strict 2

local pCommandObj;
local button_count;
local ring_selected;
local grow_timer;
local closing;
local close_call;
local select_call;

/* -------------------- globale Funktionen -----------------------*/

global func RingCreateMenu(object obj)
{
	if( !obj ) return( 0 );
	if( !( obj->~CanUseRingMenu() ) ) return( 0 );

	// Altes Menu evtl. schließen

	var existing_menu = LocalN("pComboMenu",obj);

	if( existing_menu ) existing_menu->~ComboClose();

	// Neues Menu öffnen

	var menu = CreateObject( ID_Menu_Ring ,0,0,-1);

	SetPosition( GetX(obj), GetY(obj), menu);

	LocalN("pComboMenu",obj) = menu;
	LocalN("pCommandObj",menu) = obj;

	return( 1 );
}

global func ComboCloseMenu(object obj)
{
	if( !obj ) return( 0 );
	if( !( obj->~CanUseRingMenu() ) ) return( 0 );

	var existing_menu = LocalN("pComboMenu",obj);

	if( existing_menu )
	{
		existing_menu->~ComboClose();

		return( 1 );
	}

	return(0);
}

global func RingAddMenuItem( obj, symbolid, gfx, name, call, par1, par2, par3, par4, target )
//global func RingAddMenuItem( obj, symbolid, gfx, name, call, par1, par2, par3, target )
{
	var menu = LocalN("pComboMenu",obj);

	if( !menu ) return( 0 );

	var button = menu->~RingAddButton( symbolid, gfx, name, call, par1, par2, par3, par4, target );
	//menu->~RingAddButton( symbolid, gfx, name, call, par1, par2, par3, target );

	return button;
}

global func RingSetCloseCall( obj, call, par1, par2, par3, par4 )
{
	var menu = LocalN("pComboMenu",obj);

	if( !menu ) return( 0 );

	menu->~RingAddCloseCall( obj, call, par1, par2, par3, par4 );
}

global func RingSetSelectCall( obj, call, par1, par2, par3, par4 )
{
	var menu = LocalN("pComboMenu",obj);

	if( !menu ) return( 0 );

	menu->~RingAddSelectCall( obj, call, par1, par2, par3, par4 );
}

global func RingSetSelection( obj, iSelection )
{
	var menu = LocalN("pComboMenu",obj);

	if( !menu ) return( 0 );

	menu->~RingSelectItem( iSelection );
}

global func RingMenuTest( obj )
{
	RingCreateMenu( obj );

	RingAddMenuItem( obj, ID1I, "Optionen1", "MenuOptionen");
	RingAddMenuItem( obj, ID2I, "Optionen2", "MenuOptionen");
	RingAddMenuItem( obj, ID3I, "Optionen3", "MenuOptionen");
}

/* -------------------- das Menu -----------------------*/

public func RingAddButton( symbolid, gfx, name, call, par1, par2, par3, par4, target )
//public func RingAddButton( symbolid, gfx, name, call, par1, par2, par3, target )
{
	var button = CreateObject( ID_Menu_RingSymbol ,0,0,-1);
	var graph;

	SetName(name,button);

	graph = gfx;
	if(!gfx) graph = "";

	//SetGraphics(Format("RMenu%s",graph),button,symbolid,1,6);
	var overlayID = 2;
	SetGraphics(gfx,button,symbolid,overlayID,GFXOV_MODE_IngamePicture);
	SetObjDrawTransform(600,0,0,0,600,0,button,overlayID);
	if(target)
	{
		Log("target");
		if(GetClrModulation(target))
			SetClrModulation(GetClrModulation(target),button,2);

	}

	//Local( button_count ) = [ button,call,par1,par2,par3,par4,0,0,0];
	Local( button_count ) = [ button,call,par1,par2,par3,0,0,0,0];

	button_count += 1;

	CalculatePosition();

	PositionButtons();

	return button;

}

public func RingAddCloseCall( obj, call, par1, par2, par3, par4 )
{
	close_call = [ obj, call, par1, par2, par3, par4 ];

	return( 1 );

}

public func RingAddSelectCall( obj, call, par1, par2, par3, par4 )
{
	select_call = [ obj, call, par1, par2, par3, par4 ];

	return 1;

}

public func RingSelectItem( iSelection )
{
	if( iSelection >= button_count ) return(0);

	if(GetEffect("Turn",this)) RemoveEffect( "Turn",this );

	ring_selected = iSelection;

	CalculatePosition();
}

public func ComboInput( key )
{
	if(GetEffect("Turn",this)) return 0;

	var query;

	if( pCommandObj->~MenuQueryCancel() ) query = 1;

	// Eingabe tätigen

	if( key == "L" ) RingRotate( -1 );
	if( key == "R" ) RingRotate( +1 );

	// Funktion anzeigen
	if( key == "U" )
	{ 
		var button = Local(ring_selected)[0];

		PlayerMessage(GetOwner(pCommandObj),GetName(button),button);

		//pCommandObj->~OnRingMenuSelected( button );
		if( select_call )
		{
			ScheduleCall( select_call[0], select_call[1], 1, 0, Local(ring_selected), select_call[2], select_call[3], select_call[4] );
		}

	}

	//if( key eq "D" ) if( key_d ) key_d->~ReceiveInput();

	if( key == "P" )
	{
		var command = Local(ring_selected)[1];
		var p1,p2,p3,p4;

		p1 = Local(ring_selected)[2];
		p2 = Local(ring_selected)[3];
		p3 = Local(ring_selected)[4];
		p4 = Local(ring_selected)[5];

		ObjectCall(pCommandObj, command, p1,p2,p3,p4);

		Sound("rmenu_select",0,pCommandObj,0,GetOwner(pCommandObj)+1);
	}

	if( key == "S" )
	{
		if( close_call )
		{
			ScheduleCall( close_call[0], close_call[1], 1, 0, close_call[2], close_call[3], close_call[4], close_call[5] );
		}

		 ComboClose();

		Sound("rmenu_close",0,pCommandObj,0,GetOwner(pCommandObj)+1);
	}

	return( 1 );
} 

public func RingRotate( iChange )
{
	if(GetEffect("Turn",this)) return(0);

	Sound("rmenu_rotate",0,pCommandObj,0,GetOwner(pCommandObj)+1);

	AddEffect ("Turn", this, 300, 1, this, 0, iChange );

}

public func PositionButtons()
{
	if( !pCommandObj ) return(ComboRemove());

	if(!closing)
	{
		if(grow_timer < 10) grow_timer += 1;
	}
	else
	{
		grow_timer -= 1;

		if(grow_timer <= 0 ) return(ComboRemove());
	}

	var x, y, button;

	x = GetX( pCommandObj );
	y = GetY( pCommandObj );

	var dx, dy, i;

	for( var i = 0; i<button_count; i++ )
	{
		button = Local(i)[0];

		dx = grow_timer*(Local(i)[7])/10;
		dy = grow_timer*(Local(i)[8])/10;

		SetPosition(x+dx,y+dy,button);

		SetClrModulation(RGBa(255,255,255,( 255*(10-grow_timer))/10 ), button);
		//SetClrModulation(RGBa(255,255,255,( 255*(10-grow_timer))/10 ), button, 2);
		var r,g,b;
		r = GetRGBaValue(GetClrModulation(button,2),1);
		g = GetRGBaValue(GetClrModulation(button,2),2);
		b = GetRGBaValue(GetClrModulation(button,2),3);

		SetClrModulation(RGBa(r,g,b,( 255*(10-grow_timer))/10 ), button, 2);
	}

}


public func CalculatePosition( iRotChange )
{
	var radius, angle, i, r, x, y;

	var radius = BoundBy( 35 + BoundBy( button_count -5,0,100)*5, 40, 100 );

	var angle = 360 / button_count;

	for( var i = 0; i<button_count; i++ )
	{
		var r = i + ring_selected;

		if( r >= button_count ) r -= button_count;

		x = +Sin( i*angle +iRotChange, radius );
		y = -Cos( i*angle +iRotChange, radius );

		Local(r)[7]=x;
		Local(r)[8]=y;
	}
}


public func ComboClose()
{
	closing = 1;
}

public func ComboRemove()
{
	for( var i=0; i<button_count; i++)
	{
		if(Local(i)[0]) RemoveObject(Local(i)[0]);
	}

	button_count = 0;

	RemoveObject(this);
}




public func FxTurnStart ( pTarget, iEffectNumber, iTemp, var1, var2, var3, var4)
{
	EffectVar( 0, pTarget, iEffectNumber) = var1;
	EffectVar( 1, pTarget, iEffectNumber) = 60 / LocalN("button_count",pTarget);
}

public func FxTurnTimer ( pTarget, iEffectNumber, iEffectTime)
{
	pTarget->~CalculatePosition( -6*EffectVar( 0, pTarget, iEffectNumber)*iEffectTime );

	if(iEffectTime >= EffectVar( 1, pTarget, iEffectNumber)) return(-1);
}

public func FxTurnStop ( pTarget, iEffectNumber, iReason, fTemp)
{
	if(!fTemp)
	{
		ring_selected += EffectVar( 0, pTarget, iEffectNumber);

		if( LocalN("ring_selected",pTarget) < 0 ) LocalN("ring_selected",pTarget) += LocalN("button_count",pTarget);
		if( LocalN("ring_selected",pTarget) >= LocalN("button_count",pTarget) ) LocalN("ring_selected",pTarget) -= LocalN("button_count",pTarget);

		pTarget->~CalculatePosition();
		pTarget->~ComboInput( "U" ); // Funktion anzeigen
	}

}
