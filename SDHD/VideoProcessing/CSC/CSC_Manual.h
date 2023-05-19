#pragma once

#include "simd.h"
#include "inttypes.h"
#include "../../Utility/appincludes.h"

#pragma warning (disable: 4244) //conversion from into to char
#pragma warning (disable: 4309) //truncation of constant value

class CSCManual
{
public:

	typedef enum
	{
		NO_STEREO_DECODING=0,
		STEREO_DECODING_INTERLACED,
		STEREO_DECODING_FIELD
	} stereo_decoding_t;

	#define CLIP(in, out)\
	in = in < 0 ? 0 : in;\
	in = in > 255 ? 255 : in;\
	out=in;

	// color conversion functions from Bart Nabbe.
	// corrected by Damien: bad coeficients in YUV2RGB
	//TF: also see fourcc.org for more discussion of YUV/RGB conversion

	//Original - Don't mess this up - seems to be working
	#define YUV2RGB(y, u, v, r, g, b)\
	r = y + ((v*1436) >> 10);\
	g = y - ((u*352 + v*731) >> 10);\
	b = y + ((u*1814) >> 10);\
	r = r < 0 ? 0 : r;\
	g = g < 0 ? 0 : g;\
	b = b < 0 ? 0 : b;\
	r = r > 255 ? 255 : r;\
	g = g > 255 ? 255 : g;\
	b = b > 255 ? 255 : b

	//JerNor's with R and B switched
	//#define YUV2RGB(y, u, v, r, g, b)\
	//  r = y + (u * 2.032);\
	//  g = y - ((u * .394) - (v * .581));\
	//  b = y + ( v * 1.436 );\
	//  r = r < 0 ? 0 : r;\
	//  g = g < 0 ? 0 : g;\
	//  b = b < 0 ? 0 : b;\
	//  r = r > 255 ? 255 : r;\
	//  g = g > 255 ? 255 : g;\
	//  b = b > 255 ? 255 : b

	//Yet another implementation
	//#define YUV2RGB(y, u, v, r, g, b)\
	//  r = 1.164(y - 16) + 1.596(v - 128);\
	//  g = 1.164(y - 16) - .392(u - 128) - .813(v - 128);\
	//  b = 1.164(y - 16) + 2.017(u - 128);\
	//  r = r < 0 ? 0 : r;\
	//  g = g < 0 ? 0 : g;\
	//  b = b < 0 ? 0 : b;\
	//  r = r > 255 ? 255 : r;\
	//  g = g > 255 ? 255 : g;\
	//  b = b > 255 ? 255 : b

	#define RGB2YUV(r, g, b, y, u, v)\
	y = (306*r + 601*g + 117*b)  >> 10;\
	u = ((-172*r - 340*g + 512*b) >> 10)  + 128;\
	v = ((512*r - 429*g - 83*b) >> 10) + 128;\
	y = y < 0 ? 0 : y;\
	u = u < 0 ? 0 : u;\
	v = v < 0 ? 0 : v;\
	y = y > 255 ? 255 : y;\
	u = u > 255 ? 255 : u;\
	v = v > 255 ? 255 : v

	#define REPLPIX(im, pix, index)\
	im[index]=pix[0];\
	im[index+1]=pix[1];\
	im[index+2]=pix[2];\
	im[index+3]=pix[3]

	#define INVPIX(im, index)\
	im[index]=255-im[index];\
	im[index+1]=255-im[index+1];\
	im[index+2]=255-im[index+2];\
	im[index+3]=255-im[index+3]

	//TO RGB24
	static HRESULT RGB32ToRGB24(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT UYVYToRGB24(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT YUY2ToRGB24(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT ARGB4444ToRGB24(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT RGB8ToRGB24(LPBYTE src, int w, int h, LPBYTE dst, DWORD * pal);

	//TO RGB32
	static HRESULT RGB24ToRGB32(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT UYVYToRGB32(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT YUY2ToRGB32(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT ARGB4444ToRGB32(LPBYTE src, int W, int H, LPBYTE dst);

	//TO UYVY
	static HRESULT RGB24ToUYVY(LPBYTE src, int W, int H, LPBYTE dst);
	//static HRESULT RGB32ToUYVY(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT RGB32ToUYVY(LPBYTE src, int srcpitch, int w, int h, LPBYTE dst);
	//static HRESULT YUY2ToUYVY(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT YUY2ToUYVY(LPBYTE buf, int W, int H);
	static HRESULT ARGB4444ToUYVY(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT RGB8ToUYVY(LPBYTE src, int W, int H, LPBYTE dst, DWORD* pal);

	//TOAUVAVY
	static HRESULT ARGB4444ToAAUYVY(LPBYTE src, int W, int H, LPBYTE dst);
	
	//TO YUY2
	static HRESULT RGB24ToYUY2(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT RGB32ToYUY2(LPBYTE src, int W, int H, LPBYTE dst);
	//static HRESULT UYVYToYUY2(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT UYVYToYUY2(LPBYTE buf, int W, int H);
	static HRESULT ARGB4444ToYUY2(LPBYTE src, int W, int H, LPBYTE dst);

	//TO ARGB4444
	static HRESULT RGB24ToARGB4444(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT RGB32ToARGB4444(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT UYVYToARGB4444(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT YUY2ToARGB4444(LPBYTE src, int W, int H, LPBYTE dst);

	// change a 16bit stereo image (8bit/channel) into two 8bit images on top of each other
	static HRESULT StereoDecode( LPBYTE src, int W, int H, LPBYTE * dst);


	//ETC
	static HRESULT SaveBufferToFile_A(const char * FileName, const char * Mode, LPBYTE buf, int length);
	
	//GABOR STUFF
	/*
		Y = R *  .299 + G *  .587 + B *  .114;
		U = R * -.169 + G * -.332 + B *  .500 + 128;
		V = R *  .500 + G * -.419 + B * -.0813 + 128;
	*/
	static void InitCC();
	static DWORD RGBToYUV(BYTE r, BYTE g, BYTE b);

};
















//template<bool RGB24,bool DUPL> struct Tmmx_ConvertRGBtoYUY2
//{
// /*****************************  
//  * MMX code by Klaus Post
//  * Updated for PC Levels/Rec.709
//  *   by Ian Brabham Nov 2004
//  *
//  * - Notes on MMX:
//  * Fractions are one bit less than integer code,
//  *  but otherwise the algorithm is the same, except
//  *  r_y and b_y are calculated at the same time.
//  * Order of execution has been changed much for better pairing possibilities.
//  * It is important that the 64bit values are 8 byte-aligned
//  *  otherwise it will give a huge penalty when accessing them.
//  * Instructions pair rather ok, instructions from the top is merged
//  *  into last part, to avoid dependency stalls.
//  *  (paired instructions are indented by a space)
//  *****************************/
//
// static void mmx_ConvertRGBtoYUY2(const unsigned char *SRC0,unsigned char *DST,int src_pitch, int dst_pitch,int w, int h, int MATRIX)
//  {
//   int lwidth_bytes; 
//   if (RGB24)
//    lwidth_bytes = w*3;    // Width in bytes
//   else
//   lwidth_bytes = w<<2;    // Width in bytes
//   //SRC+=src_pitch*(h-1);       // ;Move source to bottom line (read top->bottom)
//
//   static const int fraction[4] ={0x00084000,    //=(16.5) << 15 = 0x84000
//                                  0x00084000,
//                                  0x00004000,    //=(0.5) << 15 = 0x4000
//                                  0x00004000};
//   static const __int64 __declspec(align(8)) cybgr_64[4] ={i64(0x000020DE40870C88),
//                                                           i64(0x0000175C4EA507ED),
//                                                           i64(0x000026464B230E97),
//                                                           i64(0x00001B335B92093B)};
//   static const int y1y2_mult[4]={0x00004A85,    //=(255./219.) << 14
//                                  0x00004A85,
//                                  0x00004000,    //=1 << 14
//                                  0x00004000};
//   static const __int64 __declspec(align(8)) fpix_mul[4] ={i64(0x0000503300003F74),    //=(1/((1-0.299)*255/112)<<15+0.5),  (1/((1-0.114)*255/112)<<15+0.5)
//                                                           i64(0x0000476400003C97),    //=(1/((1-0.2125)*255/112)<<15+0.5), (1/((1-0.0721)*255/112)<<15+0.5)
//                                                           i64(0x00005AF1000047F4),    //=(1/((1-0.299)*255/127)<<15+0.5),  (1/((1-0.114)*255/127)<<15+0.5)
//                                                           i64(0x000050F3000044B4)};   //=(1/((1-0.2125)*255/127)<<15+0.5), (1/((1-0.0721)*255/127)<<15+0.5)
//   static const int sub_32[4]   ={0x0000FFE0,    //=-16*2
//                                  0x0000FFE0,
//                                  0x00000000,    //=0
//                                  0x00000000};
//   __m64 rb_mask =_mm_set_pi32(0x0000ffff,0x0000ffff);    //=Mask for unpacked R and B
//   __m64 fpix_add=_mm_set_pi32(0x00808000,0x00808000);    //=(128.5) << 16
//   __m64 chroma_mask2=_mm_set_pi16(-1,0x0000,-1,0x0000);
//   const __m64 mm0=_mm_cvtsi32_si64(fraction[MATRIX]);
//   const __m64 mm7=*(__m64*)(cybgr_64+MATRIX);
//   const __m64 mm5=_mm_cvtsi32_si64(y1y2_mult[MATRIX]);
//   for (int y=0;y<h;SRC0+=src_pitch,DST+=dst_pitch,y++)
//    {
//     unsigned char SRC[4096];memcpy(SRC,SRC0,lwidth_bytes);
//     __m64 mm2,mm1;
//     movq        (mm2,SRC);              // mm2= XXR2 G2B2 XXR1 G1B1
//     punpcklbw   (mm1,mm2);             // mm1= XXxx R1xx G1xx B1xx
//     if (RGB24)
//      psllq       (mm2,8);               //Compensate for RGB24
//     for (int RGBOFFSET=0,YUVOFFSET=0;RGBOFFSET<lwidth_bytes;)
//      {
//       __m64 mm6,mm4,mm3;
//       punpckhbw   (mm2,mm0);             //mm2= 00XX 00R2 00G2 00B2 
//       psrlw       (mm1,8);               //mm1= 00XX 00R1 00G1 00B1
//
//       movq        (mm6,mm1);             //mm6= 00XX 00R1 00G1 00B1
//        pmaddwd     (mm1,mm7);             //mm1= v2v2 v2v2 v1v1 v1v1   y1 //(cyb*rgb[0] + cyg*rgb[1] + cyr*rgb[2] + 0x108000)
//       if (DUPL)
//        paddw       (mm6,mm6);             //mm6 = accumulated RGB values (for b_y and r_y)
//       else
//        paddw       (mm6,mm2);             //mm6 = accumulated RGB values (for b_y and r_y)
//
//        pmaddwd  (  mm2,mm7    );       //mm2= w2w2 w2w2 w1w1 w1w1   y2 //(cyb*rgbnext[0] + cyg*rgbnext[1] + cyr*rgbnext[2] + 0x108000)
//       paddd     ( mm1,mm0     );        //Add rounding fraction (16.5)<<15 to lower dword only
//        paddd    (  mm2,mm0    );         //Add rounding fraction (16.5)<<15 to lower dword only
//       movq      ( mm3,mm1     );
//        movq     (  mm4,mm2    );
//       psrlq     ( mm3,32      );
//        pand     (  mm6,rb_mask);       //Clear out accumulated G-value mm6= 0000 RRRR 0000 BBBB
//       psrlq     ( mm4,32      );
//        paddd    (  mm1,mm3    );
//       paddd     ( mm2,mm4     );
//        psrld    (  mm1,15     );         //mm1= xxxx xxxx 0000 00y1 final value
//       movd      ( mm3,sub_32[MATRIX]);//mm3 = -32
//        psrld    (  mm2,15     );         //mm2= xxxx xxxx 0000 00y2 final value
//       paddw     ( mm3,mm1     );
//        pslld    (  mm6,14     );         //Shift up accumulated R and B values (<<15 in C)
//       if (DUPL)
//        paddw       (mm3,mm1);             //mm3 = y1+y1-32
//       else
//        paddw       (mm3,mm2);             //mm3 = y1+y2-32
//
//        psllq      ( mm2,16 );             //mm2 Y2 shifted up (to clear fraction) mm2 ready
//       pmaddwd     (mm3,mm5 );            //mm3=scaled_y (latency 2 cycles)
//        por        ( mm1,mm2);             //mm1 = 0000 0000 00Y2 00Y1
//       punpckldq   (mm3,mm3);             //Move scaled_y to upper dword mm3=SCAL ED_Y SCAL ED_Y 
//        movq       ( mm2,fpix_mul+MATRIX);
//       psubd       (mm6,mm3);             //mm6 = b_y and r_y
//        movq       ( mm4,fpix_add);
//       psrad       (mm6,14);              //Shift down b_y and r_y (>>10 in C-code) 
//        movq       ( mm3,chroma_mask2);
//       pmaddwd     (mm6,mm2);             //Mult b_y and r_y 
//       YUVOFFSET+=4;
//       paddd       (mm6,mm4);             //Add 0x808000 to r_y and b_y 
//       RGBOFFSET+=(RGB24?6:8);
//       pand        (mm6,mm3);             //Clear out fractions
//        movq       ( mm2,SRC+RGBOFFSET); //mm2= XXR2 G2B2 XXR1 G1B1
//       packuswb    (mm6,mm6);             //mm6 = VV00 UU00 VV00 UU00
//       por         (mm6,mm1);             //Or luma and chroma together                   
//        punpcklbw  ( mm1,mm2);             //mm1= XXxx R1xx G1xx B1xx
//       movd        (DST+YUVOFFSET-4,mm6); //Store final pixel                                           
//       if (RGB24)
//        psllq       (mm2,8);               //Compensate for RGB24
//      } 
//    }
//   _mm_empty();
//  }
//};
//
//template<int uyvy,int rgb32> struct Tmmx_ConvertYUY2toRGB
//{
//private:
// static __forceinline void GET_Y(__m64 &mma,int uyvy,const unsigned char* const edx)
//  {
//   if (uyvy)
//    psrlw (mma,8);
//   else
//    {
//     static const int ofs_x00FF_00FF_00FF_00FF=16;
//     pand (mma,edx+ofs_x00FF_00FF_00FF_00FF);
//    }
//  }
// static __forceinline void GET_UV(__m64 &mma,int uyvy,const unsigned char* const edx)
//  {
//   GET_Y(mma,1-uyvy,edx);
//  }
// static __forceinline bool YUV2RGB_INNER_LOOP(int no_next_pixel,const unsigned char* &esi,const unsigned char* const ecx,unsigned char* &edi,const unsigned char* const edx,__m64 &mm0,__m64 &mm1,__m64 &mm2,__m64 &mm3,__m64 &mm4,__m64 &mm5,__m64 &mm6,__m64 &mm7)
//  {
//   //This YUV422->RGB conversion code uses only four MMX registers per
//   //source dword, so I convert two dwords in parallel.  Lines corresponding
//   //to the "second pipe" are indented an extra space.  There's almost no
//   //overlap, except at the end and in the three lines marked ***.
//   //revised 4july,2002 to properly set alpha in rgb32 to default "on" & other small memory optimizations
//   static const int ofs_x0000_0000_0010_0010=0;
//   static const int ofs_x0080_0080_0080_0080=8;
//   static const int ofs_x00002000_00002000=24;
//   static const int ofs_xFF000000_FF000000=32;
//   static const int ofs_cy=40;
//   static const int ofs_crv=48;
//   static const int ofs_cgu_cgv=56;
//   static const int ofs_cbu=64;
//   bool ret;
//
//   movd           (mm0,esi);
//    movd          ( mm5,esi+4);
//   movq           (mm1,mm0);
//   GET_Y          (mm0,uyvy,edx);       // mm0 = __________Y1__Y0
//    movq          ( mm4,mm5);
//   GET_UV         (mm1,uyvy,edx);       // mm1 = __________V0__U0
//    GET_Y         ( mm4,uyvy,edx);      // mm4 = __________Y3__Y2
//   movq           (mm2,mm5);         // *** avoid reload from [esi+4]
//    GET_UV        ( mm5,uyvy,edx);      // mm5 = __________V2__U2
//   psubw          (mm0,edx+ofs_x0000_0000_0010_0010);      // (Y-16)
//    movd          ( mm6,esi+8-4*(no_next_pixel));
//   GET_UV         (mm2,uyvy,edx);       // mm2 = __________V2__U2
//    psubw         ( mm4,edx+ofs_x0000_0000_0010_0010);     // (Y-16)
//   paddw          (mm2,mm1);         // 2*UV1=UV0+UV2
//    GET_UV        ( mm6,uyvy,edx);      // mm6 = __________V4__U4
//   psubw          (mm1,edx+ofs_x0080_0080_0080_0080);      // (UV-128)
//    paddw         ( mm6,mm5);        // 2*UV3=UV2+UV4
//   psllq          (mm2,32);
//    psubw         ( mm5,edx+ofs_x0080_0080_0080_0080);     // (UV-128)
//   punpcklwd      (mm0,mm2);         // mm0 = ______Y1______Y0
//    psllq         ( mm6,32);
//   pmaddwd        (mm0,edx+ofs_cy);        // (Y-16)*(255./219.)<<14
//    punpcklwd     ( mm4,mm6);
//   paddw          (mm1,mm1);         // 2*UV0=UV0+UV0
//    pmaddwd       ( mm4,edx+ofs_cy);
//    paddw         ( mm5,mm5);        // 2*UV2=UV2+UV2
//   paddw          (mm1,mm2);         // mm1 = __V1__U1__V0__U0 * 2
//   paddd          (mm0,edx+ofs_x00002000_00002000);        // +=0.5<<14
//    paddw         ( mm5,mm6);        // mm5 = __V3__U3__V2__U2 * 2
//   movq           (mm2,mm1);
//    paddd         ( mm4,edx+ofs_x00002000_00002000);       // +=0.5<<14
//   movq           (mm3,mm1);
//    movq          ( mm6,mm5);
//   pmaddwd        (mm1,edx+ofs_crv);
//    movq          ( mm7,mm5);
//   paddd          (mm1,mm0);
//    pmaddwd       ( mm5,edx+ofs_crv);
//   psrad          (mm1,14);          // mm1 = RRRRRRRRrrrrrrrr
//    paddd         ( mm5,mm4);
//   pmaddwd        (mm2,edx+ofs_cgu_cgv);
//    psrad         ( mm5,14);
//   paddd          (mm2,mm0);
//    pmaddwd       ( mm6,edx+ofs_cgu_cgv);
//   psrad          (mm2,14);          // mm2 = GGGGGGGGgggggggg
//    paddd         ( mm6,mm4);
//   pmaddwd        (mm3,edx+ofs_cbu);
//    psrad         ( mm6,14);
//   paddd          (mm3,mm0);
//    pmaddwd       ( mm7,edx+ofs_cbu);
//   esi+=8;
//   edi+=12+4*rgb32;
//   if (!no_next_pixel)
//    ret=esi<ecx;//cmp             esi,ecx
//   else
//    ret=true;
//   psrad           (mm3,14  );        // mm3 = BBBBBBBBbbbbbbbb
//    paddd          ( mm7,mm4);
//   pxor            (mm0,mm0 );
//    psrad          ( mm7,14 );
//   packssdw        (mm3,mm2 );// mm3 = GGGGggggBBBBbbbb
//    packssdw       ( mm7,mm6);
//   packssdw        (mm1,mm0 );// mm1 = ________RRRRrrrr
//    packssdw       ( mm5,mm0);        // *** avoid pxor mm4,mm4
//   movq            (mm2,mm3 );
//    movq           ( mm6,mm7);
//   punpcklwd       (mm2,mm1 );// mm2 = RRRRBBBBrrrrbbbb
//    punpcklwd      ( mm6,mm5);
//   punpckhwd       (mm3,mm1 );// mm3 = ____GGGG____gggg
//    punpckhwd      ( mm7,mm5);
//   movq            (mm0,mm2 );
//    movq           ( mm4,mm6);
//   punpcklwd       (mm0,mm3 );// mm0 = ____rrrrggggbbbb
//    punpcklwd      ( mm4,mm7);
//   if (!rgb32)
//    {
//     psllq         (  mm0,16);
//      psllq        (   mm4,16);
//    }
//   punpckhwd       (mm2,mm3 );// mm2 = ____RRRRGGGGBBBB
//    punpckhwd      ( mm6,mm7);
//   packuswb        (mm0,mm2 );// mm0 = __RRGGBB__rrggbb <- ta dah!
//    packuswb       ( mm4,mm6);
//
//   if (rgb32)
//    {
//     por (mm0, edx+ofs_xFF000000_FF000000);    // set alpha channels "on"
//      por (mm4, edx+ofs_xFF000000_FF000000);
//     movq    (edi-16,mm0);    // store the quadwords independently
//      movq   ( edi-8,mm4);
//    }
//   else
//    {
//     psrlq   (mm0,8     );      // pack the two quadwords into 12 bytes
//     psllq   (mm4,8     );      // (note: the two shifts above leave
//     movd    (edi-12,mm0);    // mm0,4 = __RRGGBBrrggbb__)
//     psrlq   (mm0,32    );
//     por     (mm4,mm0   );
//     movd    (edi-8,mm4 );
//     psrlq   (mm4,32    );
//     movd    (edi-4,mm4 );
//    }
//   return ret;
//  }
//
//public:
// static void mmx_ConvertYUY2toRGB(const BYTE* src,BYTE* dst,const BYTE* src_end,int src_pitch,int dst_pitch,int row_size,int matrix)  //0=rec601, 1=rec709, 3=PC_601, 7=PC_709
//  {
//   static const __int64 yuv2rgb_constants[4][9]=
//    {
//     {i64(0x00000000000100010), //rec601
//      i64(0x00080008000800080),
//      i64(0x000FF00FF00FF00FF),
//      i64(0x00000200000002000),
//      i64(0x0FF000000FF000000),
//      i64(0x000004A8500004A85),
//      i64(0x03313000033130000),
//      i64(0x0E5FCF377E5FCF377),
//      i64(0x00000408D0000408D)},
//
//     {i64(0x00000000000100010), //rec709
//      i64(0x00080008000800080),
//      i64(0x000FF00FF00FF00FF),
//      i64(0x00000200000002000),
//      i64(0x0FF000000FF000000),
//      i64(0x000004A8500004A85),
//      i64(0x03960000039600000),
//      i64(0x0EEF5F930EEF5F930),
//      i64(0x00000439B0000439B)},
//
//     {i64(0x00000000000000000), //PC601
//      i64(0x00080008000800080),
//      i64(0x000FF00FF00FF00FF),
//      i64(0x00000200000002000),
//      i64(0x0FF000000FF000000),
//      i64(0x00000400000004000),
//      i64(0x02D0B00002D0B0000),
//      i64(0x0E90FF4F2E90FF4F2),
//      i64(0x0000038ED000038ED)},
//
//     {i64(0x00000000000000000), //PC709
//      i64(0x00080008000800080),
//      i64(0x000FF00FF00FF00FF),
//      i64(0x00000200000002000),
//      i64(0x0FF000000FF000000),
//      i64(0x00000400000004000),
//      i64(0x03299000032990000),
//      i64(0x0F0F8F9FEF0F8F9FE),
//      i64(0x000003B9F00003B9F)}
//    };
//   const unsigned char *edx=(const unsigned char*)yuv2rgb_constants[matrix];
//   __m64 mm0,mm1,mm2,mm3,mm4,mm5,mm6,mm7;
//   for (;src!=src_end;src+=src_pitch,dst+=dst_pitch)
//    {
//     const unsigned char *srcLn=src,*srcLnEnd=srcLn+row_size-8;
//     unsigned char *dstLn=dst;
//     while (YUV2RGB_INNER_LOOP(0,srcLn,srcLnEnd,dstLn,edx,mm0,mm1,mm2,mm3,mm4,mm5,mm6,mm7));
//     YUV2RGB_INNER_LOOP(1,srcLn,srcLnEnd,dstLn,edx,mm0,mm1,mm2,mm3,mm4,mm5,mm6,mm7);
//    }
//   _mm_empty();
//  }
//};
