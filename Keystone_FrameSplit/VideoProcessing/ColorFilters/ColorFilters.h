#pragma once

class CColorFilters
{

public:
	void ApplyColorFilters_UYVY(BYTE* src, int w, int h, int pitch, int WhichFilter)
	{
		switch (WhichFilter)
		{
			case 1:
			{
				//LumaOnly
				FilterLumaOnly(src, w, h, pitch);
				return;
			}

			case 2:
			{
				//RedOnly
				FilterRedOnly(src, w, h, pitch);
				return;
			}

			case 3:
			{
				//GreenOnly
				FilterGreenOnly(src, w, h, pitch);
				return;
			}

			case 4:
			{
				//BlueOnly
				FilterBlueOnly(src, w, h, pitch);
				return;
			}

			case 5:
			{
				//UOnly
				FilterUOnly(src, w, h, pitch);
				return;
			}

			case 6:
			{
				//VOnly
				FilterVOnly(src, w, h, pitch);
				return;
			}
		}
	}


	void FilterLumaOnly(BYTE* src, int w, int h, int pitch)
	{
		BYTE* srcuv = src;

		for(int j = 0; j < h; j++, srcuv += pitch)
		{
			BYTE* cur = srcuv;
			BYTE* end = srcuv + w*2;
            
			for(; cur < end; cur += 2)
			{
				cur[0] = 128;
				//cur[1] = cur[1];
			}
		}
	}

	void FilterUOnly(BYTE* src, int w, int h, int pitch)
	{
		BYTE* srcuv = src;

		for(int j = 0; j < h; j++, srcuv += pitch)
		{
			BYTE* cur = srcuv;
			BYTE* end = srcuv + w*2;
            
			for(; cur < end; cur += 4)
			{
				//cur[0] = cur[0];	//U
				//cur[1] = 128;		//Y
				//cur[2] = 128;		//V
				//cur[3] = 128;		//Y

				cur[0] = cur[0];	//U
				cur[1] = cur[1];	//Y
				cur[2] = 128;		//V
				cur[3] = cur[3];	//Y

				//cur[0] = cur[0];	//U
				//cur[1] = 0;		//Y
				//cur[2] = 128;		//V
				//cur[3] = 0;		//Y

			}
		}
	}

	void FilterVOnly(BYTE* src, int w, int h, int pitch)
	{
		BYTE* srcuv = src;

		for(int j = 0; j < h; j++, srcuv += pitch)
		{
			BYTE* cur = srcuv;
			BYTE* end = srcuv + w*2;
            
			for(; cur < end; cur += 4)
			{
				//cur[0] = cur[0];	//U
				//cur[1] = 128;		//Y
				//cur[2] = 128;		//V
				//cur[3] = 128;		//Y

				cur[0] = 128;		//U
				cur[1] = cur[1];	//Y
				cur[2] = cur[2];	//V
				cur[3] = cur[3];	//Y

				//cur[0] = 128;		//U
				//cur[1] = 0;			//Y
				//cur[2] = cur[2];	//V
				//cur[3] = 0;			//Y

			}
		}
	}

	//Various CSC formulae
	//y = (306*r + 601*g + 117*b)  >> 10;\
	//u = ((-172*r - 340*g + 512*b) >> 10)  + 128;\
	//v = ((512*r - 429*g - 83*b) >> 10) + 128;\

	//r = y + ((v*1436) >> 10);\
	//g = y - ((u*352 + v*731) >> 10);\
	//b = y + ((u*1814) >> 10);\

	//r = y + (1.370705 * (v-128));
	//g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
	//b = y + (1.732446 * (u-128));

	//Very sure these are correct:
	//Ey = 0.299R + 0.587G + 0.114B
	//Ecr = 0.713(R - Ey) = 0.500R - 0.419G - 0.081B
	//Ecb = 0.564(B - Er) = -0.169R - 0.331G + 0.500B 

	//R = Y + 1.403V
	//G = Y - 0.344U - 0.714V
	//B = Y + 1.770U

	void FilterRedOnly(BYTE* src, int w, int h, int pitch)
	{
		//NON OPERATIONAL
		BYTE* srcuv = src;
		//int R, G, B;
		//int Y, U, V;

		for(int j = 0; j < h; j++, srcuv += pitch)
		{
			BYTE* cur = srcuv;
			BYTE* end = srcuv + w*2;
            
			for(; cur < end; cur += 4)
			{
				cur[0] = cur[0];	//U
				cur[1] = cur[1];	//Y
				cur[2] = cur[2];	//V
				cur[3] = cur[3];	//Y
			}
		}
	}
	
	void FilterGreenOnly(BYTE* src, int w, int h, int pitch)
	{
	
	}

	void FilterBlueOnly(BYTE* src, int w, int h, int pitch)
	{
	
	}

};
