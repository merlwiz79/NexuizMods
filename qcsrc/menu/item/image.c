#ifdef INTERFACE
CLASS(Image) EXTENDS(Item)
	METHOD(Image, configureImage, void(entity, string))
	METHOD(Image, draw, void(entity))
	METHOD(Image, toString, string(entity))
	METHOD(Image, resizeNotify, void(entity, vector, vector, vector, vector))
	METHOD(Image, updateAspect, void(entity))
	METHOD(Image, setZoom, void(entity, float, float))
	METHOD(Image, drag_setStartPos, float(entity, vector))
	METHOD(Image, drag, float(entity, vector))
	ATTRIB(Image, src, string, string_null)
	ATTRIB(Image, color, vector, '1 1 1')
	ATTRIB(Image, forcedAspect, float, 0)
	ATTRIB(Image, zoomFactor, float, 1)
	ATTRIB(Image, zoomOffset, vector, '0.5 0.5 0')
	ATTRIB(Image, zoomTime, float, 0)
	ATTRIB(Image, start_zoomOffset, vector, '0 0 0')
	ATTRIB(Image, start_coords, vector, '0 0 0')
	ATTRIB(Image, imgOrigin, vector, '0 0 0')
	ATTRIB(Image, imgSize, vector, '0 0 0')
ENDCLASS(Image)
#endif

#ifdef IMPLEMENTATION
string toStringImage(entity me)
{
	return me.src;
}
void configureImageImage(entity me, string path)
{
	me.src = path;
	me.zoomOffset = '0.5 0.5 0';
	me.zoomFactor = 1;
}
void drawImage(entity me)
{
	if (me.zoomFactor > 1)
		draw_SetClip();
	draw_Picture(me.imgOrigin, me.src, me.imgSize, me.color, 1);
	if (me.zoomFactor > 1)
		draw_ClearClip();
}
void updateAspectImage(entity me)
{

	float asp;
	if(me.size_x <= 0 || me.size_y <= 0)
		return;
	if(me.forcedAspect == 0)
	{
		me.imgOrigin = '0 0 0';
		me.imgSize = '1 1 0';
	}
	else
	{

		if(me.forcedAspect < 0)
		{
			vector sz;
			sz = draw_PictureSize(me.src);
			asp = sz_x / sz_y;
		}
		else
			asp = me.forcedAspect;
		if(me.size_x > asp * me.size_y)
		{
			// x too large, so center x-wise
			me.imgSize = eY + eX * (me.size_y * asp / me.size_x);
		}
		else
		{
			// y too large, so center y-wise
			me.imgSize = eX + eY * (me.size_x / (asp * me.size_y));
		}

		if (me.zoomFactor)
		{
			if (me.zoomFactor > 1)
			{
				me.zoomOffset_x = bound(0, me.zoomOffset_x, 1);
				me.zoomOffset_y = bound(0, me.zoomOffset_y, 1);
			}
			me.imgSize = me.imgSize * me.zoomFactor;
		}
		me.imgOrigin_x = 0.5 - me.zoomOffset_x * me.imgSize_x;
		me.imgOrigin_y = 0.5 - me.zoomOffset_y * me.imgSize_y;
	}
}
float drag_setStartPosImage(entity me, vector coords)
{
	//if (me.zoomFactor > 1) //mousewheel zoom may start from a non-zoomed-in image
	{
		me.start_zoomOffset = me.zoomOffset;
		me.start_coords = coords;
	}
	return 1;
}
float dragImage(entity me, vector coords)
{
	if (me.zoomFactor > 1)
	{
		me.zoomOffset_x = me.start_zoomOffset_x + (me.start_coords_x - coords_x) / me.imgSize_x;
		me.zoomOffset_y = me.start_zoomOffset_y + (me.start_coords_y - coords_y) / me.imgSize_y;
		me.updateAspect(me);
	}
	return 1;
}
void setZoomImage(entity me, float z, float atMousePosition)
{
	float prev_zoomFactor;
	prev_zoomFactor = me.zoomFactor;
	if (z < 0) // multiply by the current zoomFactor
		me.zoomFactor *= -z;
	else if (z == 0) // reset (no zoom)
		me.zoomFactor = 1;
	else // directly set
		me.zoomFactor = z;
	me.zoomFactor = bound(1/16, me.zoomFactor, 16);
	if (me.zoomFactor <= 1)
		me.zoomOffset = '0.5 0.5 0';
	else if (atMousePosition && prev_zoomFactor != me.zoomFactor)
		me.zoomOffset = me.start_zoomOffset + (me.start_coords - '0.5 0.5 0') * (1/prev_zoomFactor);
	if (prev_zoomFactor != me.zoomFactor)
		me.zoomTime = time;
	me.updateAspect(me);
}
void resizeNotifyImage(entity me, vector relOrigin, vector relSize, vector absOrigin, vector absSize)
{
	resizeNotifyItem(me, relOrigin, relSize, absOrigin, absSize);
	if(me.forcedAspect == 0)
	{
		me.imgOrigin = '0 0 0';
		me.imgSize = '1 1 0';
	}
	else
	{
		if(absSize_x > me.forcedAspect * absSize_y)
		{
			// x too large, so center x-wise
			me.imgSize = eY + eX * (absSize_y * me.forcedAspect / absSize_x);
		}
		else
		{
			// y too large, so center y-wise
			me.imgSize = eX + eY * (absSize_x / (me.forcedAspect * absSize_y));
		}
		me.imgOrigin = '0.5 0.5 0' - 0.5 * me.imgSize;
	}
}
#endif
