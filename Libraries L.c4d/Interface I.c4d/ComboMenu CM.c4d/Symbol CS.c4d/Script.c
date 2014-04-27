/* Symbol */

#strict 2

local key_obj, key_call, key_par1, key_par2, key_par3, key_par4, key_par5;
local key_func;

public func SetKeyCommand( pCommandObj, key, name, symbolid, ang, call, par1, par2, par3, par4 )
{
	SetCategory(GetCategory() | C4D_MouseSelect); // Anwählbar

	SetGraphics( 0  , this, ID_Menu_RingSymbol, 1, GFXOV_MODE_IngamePicture );
	SetGraphics( key, this, 0, 0 );
	SetName( name );

	//if( symbolid ) SetGraphics(Format("%i",symbolid),this(),1,1);
	if( symbolid )
	{
		SetGraphics(0, this, symbolid, 2, GFXOV_MODE_IngamePicture );
		SetObjDrawTransform( 600, 0, 0,0, 600, 0,this,2);
	}

	//		dx = Sin(iAngle, iRadius);
	//		dy = -Cos(iAngle, iRadius);

	SetObjDrawTransform( 500, 0, Sin( -ang, 25*1000 ),0, 500, Cos( -ang, 25*1000 ),this);

	key_obj = pCommandObj;
	key_call = call;
	key_par1 = par1;
	key_par2 = par2;
	key_par3 = par3;
	key_par4 = par4;
	//key_par5 = par5;

	key_func = key;

	SetOwner(GetOwner( pCommandObj ));
	SetVisibility( VIS_Owner );
}

public func ReceiveInput()
{
	ObjectCall( key_obj, key_call, key_par1, key_par2, key_par3, key_par4, key_par5);
}

protected func MouseSelection(iByPlr)
{
	LocalN("pComboMenu",key_obj)->~ComboInput( key_func );

	return( 1 );
}
