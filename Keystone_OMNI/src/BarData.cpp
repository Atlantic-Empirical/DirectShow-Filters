#include "appincludes.h"

static long GetLineWeight_Horizontal(LPBYTE linebuf, int pitch, bool luma)
{
	long out = 0;
	for (int i = 0; i < pitch; i+=4)
	{
		if (luma == true)
		{
			out += linebuf[i];
			out += linebuf[i+2];
		}
		else
		{
			out += linebuf[i+1];
			out += linebuf[i+3];
		}
	}
	return out;
}

static long GetLineWeight_Vertical(LPBYTE linebuf, int pitch, int h, bool luma)
{
	long out = 0;
	if (luma == true)
	{
		//we need to go all the way down adding up byte 0
		for (int y = 0; y<h; y++)
		{
			out += linebuf[y*pitch];
		}
	}
	else
	{
		//we need to go all the way down adding up byte 1
		for (int y = 0; y<h; y++)
		{
			out += linebuf[(y*pitch)+1];
		}
	}
	return out;
}

static bool CheckLine_Horizontal(LPBYTE linebuf, int pitch, int BDDT_luma_tolerance, int BDDT_chroma_tolerance)
{
	// First check line weight.
	long line_weight = GetLineWeight_Horizontal(linebuf, pitch, true);
	if (line_weight > ((pitch/2) * (16 + BDDT_luma_tolerance)))
	{
		// This line is too heavy in LUMA.
		return true;
	}
	line_weight = GetLineWeight_Horizontal(linebuf, pitch, false);
	if ((line_weight > ((pitch/2) * (128 + BDDT_chroma_tolerance))) || (line_weight > ((pitch/2) * (128 + BDDT_chroma_tolerance))))
	{
		// This line is too heavy or light in chroma.
		return true;
	}

	bool luma_check = false;
	bool chroma_check = false;
	//OK now do a pixel by pixel check.
	for (int i = 0; i<pitch; i+=4)
	{
		luma_check = ((linebuf[i] > (16 + BDDT_luma_tolerance)) || (linebuf[i+2] > (16 + BDDT_luma_tolerance)));
		chroma_check = ((linebuf[i+1] > (128 + BDDT_chroma_tolerance)) || (linebuf[i+1] < (128 - BDDT_chroma_tolerance)) || (linebuf[i+3] > (128 + BDDT_chroma_tolerance)) || (linebuf[i+3] < (128 - BDDT_chroma_tolerance)));
		if ((luma_check==true)||(chroma_check==true)) return true;
	}
	//line has sufficient black data
	return false;
}

static bool CheckLine_Vertical(LPBYTE linebuf, int pitch, int h, int BDDT_luma_tolerance, int BDDT_chroma_tolerance)
{
	// First check line weight.
	long line_weight = GetLineWeight_Vertical(linebuf, pitch, h, true);
	if (line_weight > (h * (16 + BDDT_luma_tolerance)))
	{
		// This line is too heavy in LUMA.
		return true;
	}
	line_weight = GetLineWeight_Vertical(linebuf, pitch, h, false);
	if ((line_weight > (h * (128 + BDDT_chroma_tolerance))) || (line_weight < (h * (128 - BDDT_chroma_tolerance))))
	{
		// This line is too heavy or light in chroma.
		return true;
	}
	
	bool luma_check = false;
	bool chroma_check = false;
	//OK now do a pixel by pixel check.
	for (int i = 0; i<h*pitch; i+=pitch)
	{
		luma_check = (linebuf[i] > (16 + BDDT_luma_tolerance)) || (linebuf[i+2] > (16 + BDDT_luma_tolerance));
		//chroma_check = ((linebuf[i+1] > (128 + BDDT_chroma_tolerance)) || (linebuf[i+1] < (128 - BDDT_chroma_tolerance)) || (linebuf[i+3] > (128 + BDDT_chroma_tolerance)) || (linebuf[i+3] < (128 - BDDT_chroma_tolerance)));
		if ((luma_check==true)||(chroma_check==true))
		{
			return true;
		}
	}

	return false;
}

static bool Check_OverallFrameDarkness(LPBYTE buf, int pitch, int w, int h, int BDDT_luma_tolerance, int BDDT_chroma_tolerance)
{
	//add up all luma in frame
	//add up all chroma in frame

	long luma = 0;
	long chroma = 0;

	for (int i = 0; i < (pitch*h); i+=4)
	{
		luma += buf[i];
		luma += buf[i+2];
		chroma += buf[i+1];
		chroma += buf[i+3];		
	}

	long luma_avg_val = luma / ((pitch*h)/2);
	if (luma_avg_val > (16 + BDDT_luma_tolerance)) return true; //frame is not too dark

	long chroma_avg_val = chroma / ((pitch*h)/2);
	if ((chroma_avg_val > (128 + BDDT_chroma_tolerance)) || (chroma_avg_val < (128 - BDDT_chroma_tolerance))) return true; //frame has a low luma val but the chroma is sufficient to give a workable brightness.

	return false;	
}


HRESULT CKeystone::BDDT_GenerateBarData(LPBYTE buf, int w, int h)
{
	int pitch = w * 2;
	double prev_val;
	bool val_has_changed = false;
	bool line_is_threshold = false;

	//=========================================
	//	FRAME DARKNESS CHECK
	//=====================
	if (Check_OverallFrameDarkness(buf, pitch, w, h, BDDT_luma_tolerance, BDDT_chroma_tolerance) != true)
	{
		NotifyEvent(EC_KEYSTONE_BARDATA_TOP_BOTTOM, 0, h + 1);
		NotifyEvent(EC_KEYSTONE_BARDATA_LEFT_RIGHT, 0, w + 1);
		NotifyEvent(EC_KEYSTONE_BARDATA_FRAME_TOO_DARK, 0, 0);
	}

	//=========================================
	//	TOP BAR
	//=====================
	
	for (int y = 0; y<h/2; y++)
	{
		line_is_threshold = CheckLine_Horizontal(buf + (pitch*y), pitch, BDDT_luma_tolerance, BDDT_chroma_tolerance);
		if (line_is_threshold == true)
		{
			//Check the next two lines, if they are both invalid we've found the top bar.
			if (CheckLine_Horizontal(buf + (pitch* (y+1)), pitch, BDDT_luma_tolerance, BDDT_chroma_tolerance) && CheckLine_Horizontal(buf + (pitch* (y+2)), pitch, BDDT_luma_tolerance, BDDT_chroma_tolerance))
			{
				//we've found it
				prev_val = BDDT_top_bar;
				//BDDT_top_bar = y;
				//if (prev_val != BDDT_top_bar) val_has_changed = true;
				BDDT_top_bar = prev_val + ((y - BDDT_top_bar) / BDDT_weight);
				if (prev_val != fround(BDDT_top_bar, 0)) val_has_changed = true;
				goto FindBottomBar;
			}
		}
	}
	BDDT_top_bar = 0;


	//=========================================
	//	BOTTOM BAR
	//=====================
FindBottomBar:
	for (int y=h-1; y>h/2; y--)
	{
		line_is_threshold = CheckLine_Horizontal(buf + (pitch*y), pitch, BDDT_luma_tolerance, BDDT_chroma_tolerance);
		if (line_is_threshold == true)
		{
			if (y>h-2)
			{
				//We're at the very bottom of the frame
				prev_val = BDDT_bottom_bar;
				//BDDT_bottom_bar = y;
				//if (prev_val != BDDT_bottom_bar) val_has_changed = true;
				BDDT_bottom_bar = prev_val + ((y - BDDT_bottom_bar) / BDDT_weight);
				if (prev_val != fround(BDDT_bottom_bar, 0)) val_has_changed = true;
				goto FindLeftBar;
			}
			else
			{
				//Check the next two lines, if they are both invalid we've found the top bar.
				if (CheckLine_Horizontal(buf + (pitch* (y+1)), pitch, BDDT_luma_tolerance, BDDT_chroma_tolerance) && CheckLine_Horizontal(buf + (pitch* (y+2)), pitch, BDDT_luma_tolerance, BDDT_chroma_tolerance))
				{
					//we've found it
					prev_val = BDDT_bottom_bar;
					//BDDT_bottom_bar = y;
					//if (prev_val != BDDT_bottom_bar) val_has_changed = true;
					BDDT_bottom_bar = prev_val + ((y - BDDT_bottom_bar) / BDDT_weight);
					if (prev_val != fround(BDDT_bottom_bar, 0)) val_has_changed = true;
					goto FindLeftBar;
				}
			}
		}
	}
	BDDT_bottom_bar = h;


	//=========================================
	//	LEFT BAR
	//=====================
FindLeftBar:
	for (int x = 0; x<w/2; x++)
	{
		line_is_threshold = CheckLine_Vertical(buf + (x*2), pitch, h, BDDT_luma_tolerance, BDDT_chroma_tolerance);
		if (line_is_threshold == true)
		{
			//Check the next two lines, if they are both invalid we've found the top bar.
			if (CheckLine_Vertical(buf + ((x+1)*2), pitch, h, BDDT_luma_tolerance, BDDT_chroma_tolerance) && CheckLine_Vertical(buf + ((x+2)*2), pitch, h, BDDT_luma_tolerance, BDDT_chroma_tolerance))
			{
				//we've found it
				prev_val = BDDT_left_bar;
				//BDDT_left_bar = x;
				//if (prev_val != BDDT_left_bar) val_has_changed = true;
				BDDT_left_bar = prev_val + ((x - BDDT_left_bar) / BDDT_weight);
				if (prev_val != fround(BDDT_left_bar, 0)) val_has_changed = true;
				goto FindRightBar;
			}
		}
	}
	BDDT_left_bar = 0;


	//=========================================
	//	RIGHT BAR
	//=====================
FindRightBar:
	for (int x = w-1; x>w/2; x--)
	{
		line_is_threshold = CheckLine_Vertical(buf + (x*2), pitch, h, BDDT_luma_tolerance, BDDT_chroma_tolerance);
		if (line_is_threshold == true)
		{
			//Check the next two lines, if they are both invalid we've found the top bar.
			if (CheckLine_Vertical(buf + (x*2), pitch, h, BDDT_luma_tolerance, BDDT_chroma_tolerance) && CheckLine_Vertical(buf + (x*2), pitch, h, BDDT_luma_tolerance, BDDT_chroma_tolerance))
			{
				//we've found it
				prev_val = BDDT_right_bar;
				//BDDT_right_bar = x;
				//if (prev_val != BDDT_right_bar) val_has_changed = true;
				BDDT_right_bar = prev_val + ((x - BDDT_right_bar) / BDDT_weight);
				if (prev_val != fround(BDDT_right_bar, 0)) val_has_changed = true;
				goto WereDone;
			}
		}
	}
	BDDT_right_bar = w;

WereDone:

	if (BDDT_burn_demarcation == true)
	{
		int start_line_byte_ref = 0;

		if (fround(BDDT_top_bar, 0) > 2)
		{
			//BURN HORIZONTAL LINE @ top_bar
			start_line_byte_ref = (fround(BDDT_top_bar,0)*pitch) - (2*pitch);
			for (int ix = start_line_byte_ref; ix < start_line_byte_ref + (pitch*2); ix+=4)
			{
				memset(buf + ix    , 80, 1);	//Y: Luma
				memset(buf + ix + 1, 16, 1);	//U: Cb
				memset(buf + ix + 2, 80, 1);	//Y: Luma 
				memset(buf + ix + 3, 16, 1);	//V: Cr
			}
		}

		//BURN HORIZONTAL LINE @ bottom_bar
		if (fround(BDDT_bottom_bar,0) < h-2)
		{
			start_line_byte_ref = (fround(BDDT_bottom_bar,0)*pitch);
			for (int ix = start_line_byte_ref; ix < start_line_byte_ref + (pitch*2); ix+=4)
			{
				memset(buf + ix    , 80, 1);	//Y: Luma
				memset(buf + ix + 1, 16, 1);	//U: Cb
				memset(buf + ix + 2, 80, 1);	//Y: Luma 
				memset(buf + ix + 3, 16, 1);	//V: Cr
			}
		}

		if (fround(BDDT_left_bar,0) > 2)
		{
			//BURN VERTICAL LINE @ left_bar
			for (int ix = fround(BDDT_left_bar,0)*2; ix < pitch*h; ix+=pitch)
			{
				memset(buf + ix    , 80, 1);	//Y: Luma
				memset(buf + ix + 1, 16, 1);	//U: Cb
				memset(buf + ix + 2, 80, 1);	//Y: Luma 
				memset(buf + ix + 3, 16, 1);	//V: Cr
			}
		}

		if (fround(BDDT_right_bar,0) < w-2)
		{
			//BURN VERTICAL LINE @ right_bar
			for (int ix = ((fround(BDDT_right_bar,0)*2)-2); ix < pitch*h; ix+=pitch)
			{
				memset(buf + ix    , 80, 1);	//Y: Luma
				memset(buf + ix + 1, 16, 1);	//U: Cb
				memset(buf + ix + 2, 80, 1);	//Y: Luma 
				memset(buf + ix + 3, 16, 1);	//V: Cr
			}
		}
	}

	if (val_has_changed == true)
	{

		//=========================================
		//	REALITY CHECK - keep the values in realiztic margains
		//=====================

		long out_top = fround(BDDT_top_bar, 0);
		long out_bottom = fround(BDDT_bottom_bar, 0);
		long out_left = fround(BDDT_left_bar, 0);
		long out_right = fround(BDDT_right_bar, 0);

		if ((out_top > 1080) || (out_top < 0))
		{
			 BDDT_top_bar = 0;
			 out_top = 0;
		}
		if ((out_bottom > 1080) || (out_bottom < 0))
		{
			 BDDT_bottom_bar = 0;
			 out_bottom = 0;
		}
		if ((out_left > 1920) || (out_left < 0))
		{
			BDDT_left_bar = 0;
			out_left = 0;
		}
		if ((out_right > 1920) || (out_right < 0))
		{
			BDDT_right_bar = 0;
			out_right = 0;
		}


		//=========================================
		//	NOTIFY CLIENT
		//=====================

		if (out_right == w-1) out_right = w; //because when 1 is added we want w +1
		if (out_bottom == h-1) out_bottom = h; //same
		if (out_top == 0) out_top = -1; //because when 1 is added below we want the value to be 0
		if (out_left == 0) out_left = -1; //same
 
		//+1 to give 1-base line numbers (1-480 instead of 0-479)
		NotifyEvent(EC_KEYSTONE_BARDATA_TOP_BOTTOM, out_top + 1, out_bottom + 1);
		NotifyEvent(EC_KEYSTONE_BARDATA_LEFT_RIGHT, out_left + 1, out_right + 1);

	}
	BDDT_weight+=0.1;
	return S_OK;
}
