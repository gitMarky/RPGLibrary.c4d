/* Neues Script */

#strict 2

global func SetGraphics(string szGfxName, object pObj, id idSrcDef, int iOverlay, int iOverlayMode, string szAction, int dwBlitMode, object pOverlayObject)
{
	if(this && !pObj)
	{
		pObj = this;
	}


	var epxectedLength = iOverlay+1;

	if(!LocalN2("GetGraphicsInfo",pObj))
	{
		CreateLocalN2("GetGraphicsInfo",pObj, CreateArray(epxectedLength));
	}

	var infolist = LocalN2("GetGraphicsInfo",pObj);
	//Log("CreatedInfoList: %v",infolist);
	if(infolist)
	{
		if(GetType(infolist) != C4V_Array)
		{
			LocalN2("GetGraphicsInfo",pObj) = CreateArray(epxectedLength);
		}
		else
		{
			if(GetLength(infolist) < epxectedLength)
			{
				SetLength(LocalN2("GetGraphicsInfo",pObj), epxectedLength);
			}

			var infos = [ szGfxName, idSrcDef, iOverlayMode, szAction, dwBlitMode, pOverlayObject ];
			LocalN2("GetGraphicsInfo",pObj)[iOverlay] = infos;
		}
	}
	//Log("ResultInfoList: %v",LocalN2("GetGraphicsInfo",pObj));
	return _inherited(szGfxName, pObj, idSrcDef, iOverlay, iOverlayMode, szAction, dwBlitMode, pOverlayObject);
}

global func GetGraphics( int iOverlay, object pObj, int iIndex)
{
	if(this && !pObj) pObj = this;
	if(!pObj) return 0;

	var infolist = LocalN2("GetGraphicsInfo",pObj);
	if(!infolist || GetType(infolist) != C4V_Array || iIndex < -1 || iOverlay >= GetLength(infolist)) return 0;

	if(iOverlay == -1) return infolist;

	return infolist[iOverlay][iIndex];
}
