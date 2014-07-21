/*-- Neues Script --*/

#strict 2

public func Dusk()
{
		if( GetTemperature()>10)
		{
			var count = RandomX(3,10);
			//CreateObject(FFLY,0,0,-1);
			var dist = 30;

			for(var i = 0; i < count; i++)
			{
				var firefly = CreateObject(FFLY, RandomX(-dist,dist), RandomX(-dist,dist), -1);
				firefly->LocalN("attractionObj") = this;
			}
		}
}
public func AttractsFireflies(){ return true; }
