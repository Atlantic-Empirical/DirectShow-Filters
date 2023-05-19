#ifndef _SIMD_H_
#define _SIMD_H_

#include <mmintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>

#pragma warning(push)
#pragma warning(disable:4799)
#pragma warning(disable:4309)
#pragma warning(disable:4700)

static __forceinline void movq(__m64 &dst,const __m64 &src) {dst=src;}
static __forceinline void movq(__m64 &dst,const void *src) {dst=*(__m64*)src;}
static __forceinline void movq(void *dst,const __m64 &src) {*(__m64*)dst=src;}
static __forceinline void movntq(void *dst,const __m64 &src) {_mm_stream_pi((__m64*)dst,src);}

static __forceinline void movd(__m64 &dst,int src) {dst=_mm_cvtsi32_si64(src);}
static __forceinline void movd(int &dst,const __m64 &src) {dst=_mm_cvtsi64_si32(src);}
static __forceinline void movd(__m64 &dst,const void *src) {dst=_mm_cvtsi32_si64(*(const int*)src);}
static __forceinline void movd(void *dst,const __m64 &src) {*(int*)dst=_mm_cvtsi64_si32(src);}

static __forceinline void paddb(__m64 &dst,const __m64 &src) {dst=_mm_add_pi8(dst,src);}
static __forceinline void paddb(__m64 &dst,const void *src) {dst=_mm_add_pi8(dst,*(__m64*)src);}
static __forceinline void paddsb(__m64 &dst,const __m64 &src) {dst=_mm_adds_pi8(dst,src);}
static __forceinline void paddusb(__m64 &dst,const __m64 &src) {dst=_mm_adds_pu8(dst,src);}
static __forceinline void paddusb(__m64 &dst,const void *src) {dst=_mm_adds_pu8(dst,*(__m64*)src);}
static __forceinline void paddw(__m64 &dst,const __m64 &src) {dst=_mm_add_pi16(dst,src);}
static __forceinline void paddsw(__m64 &dst,const __m64 &src) {dst=_mm_adds_pi16(dst,src);}
static __forceinline void paddsw(__m64 &dst,const void *src) {dst=_mm_adds_pi16(dst,*(__m64*)src);}
static __forceinline void paddusw(__m64 &dst,const __m64 &src) {dst=_mm_adds_pu16(dst,src);}
static __forceinline void paddd(__m64 &dst,const __m64 &src) {dst=_mm_add_pi32(dst,src);}
static __forceinline void paddd(__m64 &dst,const void *src) {dst=_mm_add_pi32(dst,*(__m64*)src);}

static __forceinline void pmaddwd(__m64 &dst,const void *src) {dst=_mm_madd_pi16(dst,*(__m64*)src);}
static __forceinline void pmaddwd(__m64 &dst,const __m64 &src) {dst=_mm_madd_pi16(dst,src);}

static __forceinline void psubb(__m64 &dst,const __m64 &src) {dst=_mm_sub_pi8(dst,src);}
static __forceinline void psubusb(__m64 &dst,const void *src) {dst=_mm_subs_pu8(dst,*(__m64*)src);}
static __forceinline void psubusb(__m64 &dst,const __m64 &src) {dst=_mm_subs_pu8(dst,src);}
static __forceinline void psubw(__m64 &dst,const __m64 &src) {dst=_mm_sub_pi16(dst,src);}
static __forceinline void psubw(__m64 &dst,const void *src) {dst=_mm_sub_pi16(dst,*(__m64*)src);}
static __forceinline void psubsw(__m64 &dst,const __m64 &src) {dst=_mm_subs_pi16(dst,src);}
static __forceinline void psubsw(__m64 &dst,const void *src) {dst=_mm_subs_pi16(dst,*(__m64*)src);}
static __forceinline void psubusw(__m64 &dst,const __m64 &src) {dst=_mm_subs_pu16(dst,src);}
static __forceinline void psubd(__m64 &dst,const __m64 &src) {dst=_mm_sub_pi32(dst,src);}

static __forceinline void pmullw(__m64 &dst,const __m64 &src) {dst=_mm_mullo_pi16(dst,src);}
static __forceinline void pmullw(__m64 &dst,const void *src) {dst=_mm_mullo_pi16(dst,*(__m64*)src);}
static __forceinline void pmulhw(__m64 &dst,const __m64 &src) {dst=_mm_mulhi_pi16(dst,src);}

static __forceinline void pand(__m64 &dst,const __m64 &src) {dst=_mm_and_si64(dst,src);}
static __forceinline void pand(__m64 &dst,const void *src) {dst=_mm_and_si64(dst,*(__m64*)src);}
static __forceinline void pandn(__m64 &dst,const __m64 &src) {dst=_mm_andnot_si64(dst,src);}
static __forceinline void por(__m64 &dst,const __m64 &src) {dst=_mm_or_si64(dst,src);}
static __forceinline void por(__m64 &dst,const void *src) {dst=_mm_or_si64(dst,*(__m64*)src);}
static __forceinline void pxor(__m64 &dst,const __m64 &src) {dst=_mm_xor_si64(dst,src);}

static __forceinline void pcmpeqb(__m64 &dst,const __m64 &src) {dst=_mm_cmpeq_pi8(dst,src);}
static __forceinline void pcmpeqw(__m64 &dst,const __m64 &src) {dst=_mm_cmpeq_pi16(dst,src);}
static __forceinline void pcmpgtb(__m64 &dst,const __m64 &src) {dst=_mm_cmpgt_pi8(dst,src);}
static __forceinline void pcmpgtw(__m64 &dst,const __m64 &src) {dst=_mm_cmpgt_pi16(dst,src);}

static __forceinline void psllq(__m64 &dst,int i) {dst=_mm_slli_si64(dst,i);}
static __forceinline void pslld(__m64 &dst,int i) {dst=_mm_slli_pi32(dst,i);}
static __forceinline void psllw(__m64 &dst,int i) {dst=_mm_slli_pi16(dst,i);}
static __forceinline void psrlq(__m64 &dst,int i) {dst=_mm_srli_si64(dst,i);}
static __forceinline void psrld(__m64 &dst,int i) {dst=_mm_srli_pi32(dst,i);}
static __forceinline void psrlw(__m64 &dst,int i) {dst=_mm_srli_pi16(dst,i);}
static __forceinline void psraw(__m64 &dst,int i) {dst=_mm_srai_pi16(dst,i);}
static __forceinline void psraw(__m64 &dst,const __m64 &src) {dst=_mm_sra_pi16(dst,src);}
static __forceinline void psrad(__m64 &dst,int i) {dst=_mm_srai_pi32(dst,i);}

static __forceinline void packuswb(__m64 &dst,const __m64 &src) {dst=_mm_packs_pu16(dst,src);}
static __forceinline void packsswb(__m64 &dst,const __m64 &src) {dst=_mm_packs_pi16(dst,src);}
static __forceinline void packssdw(__m64 &dst,const __m64 &src) {dst=_mm_packs_pi32(dst,src);}

static __forceinline void punpcklbw(__m64 &dst,const __m64 &src) {dst=_mm_unpacklo_pi8(dst,src);}
static __forceinline void punpckhbw(__m64 &dst,const __m64 &src) {dst=_mm_unpackhi_pi8(dst,src);}

static __forceinline void punpcklwd(__m64 &dst,const __m64 &src) {dst=_mm_unpacklo_pi16(dst,src);}
static __forceinline void punpcklwd(__m64 &dst,const void *src)  {dst=_mm_unpacklo_pi16(dst,*(__m64*)src);}
static __forceinline void punpckhwd(__m64 &dst,const __m64 &src) {dst=_mm_unpackhi_pi16(dst,src);}
static __forceinline void punpckldq(__m64 &dst,const __m64 &src)  {dst=_mm_unpacklo_pi32(dst,src);}
static __forceinline void punpckldq(__m64 &dst,const void *src)  {dst=_mm_unpacklo_pi32(dst,*(__m64*)src);}

static __forceinline void pminub(__m64 &dst,const __m64 &src) {dst=_mm_min_pu8(dst,src);}
static __forceinline void pminub(__m64 &dst,const void *src) {dst=_mm_min_pu8(dst,*(__m64*)src);}

static __forceinline void pmaxub(__m64 &dst,const __m64 &src) {dst=_mm_max_pu8(dst,src);}
static __forceinline void pmaxub(__m64 &dst,const void *src) {dst=_mm_max_pu8(dst,*(__m64*)src);}

static __forceinline void pavgb(__m64 &dst,const __m64 &src) {dst=_mm_avg_pu8(dst,src);}
static __forceinline void pavgb(__m64 &dst,const void *src) {dst=_mm_avg_pu8(dst,*(__m64*)src);}

static __forceinline void prefetcht0(const void *a) {_mm_prefetch((char*)a,_MM_HINT_T0);}

static __forceinline void movdqu(__m128i &dst,const void *src) {dst=_mm_loadu_si128((__m128i*)src);}
static __forceinline void movdqu(__m128i &dst,const __m128i &src) {dst=_mm_loadu_si128(&src);}
static __forceinline void movdqa(__m128i &dst,const __m128i &src) {dst=src;}
static __forceinline void movntdq(void *dst,const __m128i &src) {_mm_stream_si128((__m128i*)dst,src);}
static __forceinline void movdq2q(__m64 &dst,const __m128i &src) {dst=_mm_movepi64_pi64(src);}
static __forceinline void pand(__m128i &dst,const __m128i &src) {dst=_mm_and_si128(dst,src);}
static __forceinline void pxor(__m128i &dst,const __m128i &src) {dst=_mm_xor_si128(dst,src);}
static __forceinline void punpcklbw(__m128i &dst,const __m128i &src) {dst=_mm_unpacklo_epi8(dst,src);}
static __forceinline void punpckhbw(__m128i &dst,const __m128i &src) {dst=_mm_unpackhi_epi8(dst,src);}
static __forceinline void pmullw(__m128i &dst,const __m128i &src) {dst=_mm_mullo_epi16(dst,src);}
static __forceinline void paddw(__m128i &dst,const __m128i &src) {dst=_mm_add_epi16(dst,src);}
static __forceinline void paddusw(__m128i &dst,const __m128i &src) {dst=_mm_adds_epu16(dst,src);}
static __forceinline void psrlw(__m128i &dst,int i) {dst=_mm_srli_epi16(dst,i);}
static __forceinline void packuswb(__m128i &dst,const __m128i &src) {dst=_mm_packus_epi16(dst,src);}

//======================================= MMX ======================================
struct Tmmx
{
 static __forceinline void pmaxub(__m64 &mmr1,const __m64 &mmr2)
  {
   mmr1=_mm_subs_pu8(mmr1,mmr2);
   mmr1=_mm_adds_pu8(mmr1,mmr2);
  }
 static __forceinline void pmaxub(__m64 &mmr1,const void *mmr2)
  {
   pmaxub(mmr1,*(__m64*)mmr2);
  }
 static __forceinline void pminub(__m64 &mmr1,const __m64 &mmr2)
  {
   __m64 mmrw;
   pcmpeqb (mmrw,mmrw );
   psubusb (mmrw,mmr2 );
   paddusb (mmr1, mmrw);
   psubusb (mmr1, mmrw);
  }
 static __forceinline void pminub(__m64 &mmr1,const void *mmr2)
  {
   pminub(mmr1,*(__m64*)mmr2);
  }
 static __forceinline void pmaxsw(__m64 &a,const __m64 &b)
  {
   psubusw(a,b);
   paddw(a,b);
  }
 static __forceinline void pminsw(__m64 &mmr1,const __m64 &mmr2)
  {
   __m64 mmrw;
   pcmpeqw(mmrw,mmrw);
   mmrw=_mm_cmpeq_pi16(mmrw,mmrw);
   mmrw=_mm_subs_pu16(mmrw,mmr2);
   mmr1=_mm_adds_pu16(mmr1,mmrw);
   mmr1=_mm_subs_pu16(mmr1,mmrw);
  }
 static __forceinline void pavgb(__m64 &rega,__m64 regb)
  {
   __m64 regr;
   __m64 regfe=_mm_set1_pi8(/*0xfe*/-2);
   movq    (regr,rega);
   por     (regr,regb);
   pxor    (regb,rega);
   pand    (regb,regfe);
   psrlq   (regb,1);
   psubb   (regr,regb);
   rega=regr;
  }
 static __forceinline void pavgb(__m64 &rega,const void *regb)
  {
   pavgb(rega,*(__m64*)regb);
  }
 static __forceinline void v_pavgb(__m64 &mmr1,const __m64 &mmr2,__m64 &mmrw,const __m64 &smask)
  {
   movq( mmrw,mmr2 );
   pand( mmrw, smask );
   psrlw( mmrw,1 );
   pand( mmr1,smask );
   psrlw( mmr1,1 );
   paddusb( mmr1,mmrw );
  }
 static __forceinline void v_pavgb(__m64 &mmr1,const void *mmr2,__m64 &mmrw,const __m64 &smask)
  {
   v_pavgb(mmr1,*(__m64*)mmr2,mmrw,smask);
  }
 static __forceinline void sfence(void)
  {
  } 
 static __forceinline void movntq(void *dst,const __m64 &src)
  {
   movq(dst,src);
  }
 static __forceinline void v_pminub(__m64 &mmr1,const __m64 &mmr2,__m64 &mmrw)
  {
   pcmpeqb (mmrw,mmrw );
   psubusb (mmrw,mmr2 );
   paddusb (mmr1, mmrw);
   psubusb (mmr1, mmrw);
  }
 static __forceinline void pmulhuw(__m64 &mm3,const __m64 &mm2)
  {
   __m64 mm5;
   movq   ( mm5, mm2);              // mm5 = multiplier to move toward new
   psraw  ( mm5, 15 );              // mm5 = words filled with high bit of multiplier words
   pand   ( mm5, mm3);              // mm5 = wordwise chroma to add/subtract iff high bit of mulitplier is on
   pmulhw ( mm3, mm2);              // mm3 = signed product of chroma and multiplier
   paddw  ( mm3, mm5);              // mm3 = unsigned product of chroma and multiplier
  }
 static __forceinline void prefetchnta(const void*)
  {
  }
 static __forceinline __m64 _mm_shuffle_pi16_0(__m64 mm3)
  {
   __m64 mm2;
   const __m64 qwLowWord=_mm_set_pi32(0x00000000,0x0000FFFF);
   pand    (mm3, qwLowWord);          // mm3 = same limited to low word 
   movq    (mm2, mm3);              // mm2 = same
   psllq   (mm3, 16 );              // mm3 = moved to second word
   por     (mm2, mm3);              // mm2 = copied to first and second words
   movq    (mm3, mm2);              // mm3 = same
   psllq   (mm3, 32 );              // mm3 = moved to third and fourth words
   por     (mm2, mm3);              // mm2 = low word copied to all four words
   return mm2;
  } 
 static __forceinline __m64 _mm_shuffle_pi16_1(const __m64 &src)
  {
   __m64 w0=_mm_srli_si64(_mm_and_si64(src,_mm_set_pi32(0x00000000,0xFFFF0000)),16);
   __m64 w1=_mm_and_si64(src,_mm_set_pi32(0x00000000,0x0000FFFF));
   return _mm_or_si64(_mm_or_si64(_mm_or_si64(_mm_slli_si64(w1,48),_mm_slli_si64(w1,32)),_mm_slli_si64(w1,16)),w0);
  } 
 static __forceinline __m64 _mm_shuffle_pi16_14(const __m64 &src)
  {
   __m64 w34=_mm_and_si64(src,_mm_set_pi32(0x00000000,0x0000FFFF));
   __m64 w12=_mm_srli_si64(_mm_and_si64(src,_mm_set_pi32(0xffffffff,0x00000000)),32);
   return _mm_or_si64(w12,_mm_or_si64(_mm_slli_si64(w34,32),_mm_slli_si64(w34,48)));
  } 
 static __forceinline void psadbw(__m64 &DestMM,const __m64 &SourceMM)
  {
   __m64 TempMM;
   const __m64 qwLowByte=_mm_set_pi8(0x00,0x00,0x00,0x00,0x00,0x00,0x00,-1);
   movq      (TempMM, SourceMM );
   psubusb   (TempMM, DestMM );
   psubusb   (DestMM, SourceMM );
   por       (DestMM, TempMM );
   movq      (TempMM, DestMM );
   psrlw     (DestMM, 8 );
   paddusb   (TempMM, DestMM );
   movq      (DestMM, TempMM );
   psrld     (TempMM, 16 );
   paddusb   (DestMM, TempMM );
   movq      (TempMM, DestMM );
   psrlq     (DestMM, 32 );
   paddusb   (DestMM, TempMM );
   pand      (DestMM, qwLowByte);
  }
 static __forceinline __m64 min_pu8(const __m64 &mm1,const __m64 &mm2)
  {
   __m64 mm0=mm1;
   pminub(mm0,mm2);
   return mm0;
  }
 static __forceinline __m64 max_pu8(const __m64 &mm1,const __m64 &mm2)
  {
   __m64 mm0=mm1;
   pmaxub(mm0,mm2);
   return mm0;
  }
 static __forceinline __m64 min_pi16(const __m64 &mm1,const __m64 &mm2)
  {
   __m64 mm0=mm1;
   pminsw(mm0,mm2);
   return mm0;
  }
 static __forceinline __m64 max_pi16(const __m64 &mm1,const __m64 &mm2)
  {
   __m64 mm0=mm1;
   pmaxsw(mm0,mm2);
   return mm0;
  }
};

//===================================== MMXEXT =====================================
struct Tmmxext
{
 static __forceinline void pmaxub(__m64 &mmr1,const __m64 &mmr2)
  {
   mmr1=_mm_max_pu8(mmr1,mmr2);
  }
 static __forceinline void pmaxub(__m64 &mmr1,const void *mmr2)
  {
   pmaxub(mmr1,*(__m64*)mmr2);
  }
 static __forceinline void pminub(__m64 &mmr1,const __m64 &mmr2)
  {
   mmr1=_mm_min_pu8(mmr1,mmr2);
  }
 static __forceinline void pminub(__m64 &mmr1,const void *mmr2)
  {
   pminub(mmr1,*(__m64*)mmr2);
  }
 static __forceinline void pminsw(__m64 &mmr1,const __m64 &mmr2)
  {
   mmr1=_mm_min_pi16(mmr1,mmr2);
  }
 static __forceinline void pavgb(__m64 &mmr1,const __m64 &mmr2)
  {
   mmr1=_mm_avg_pu8(mmr1,mmr2);
  }
 static __forceinline void pavgb(__m64 &mmr1,const void *mmr2)
  {
   mmr1=_mm_avg_pu8(mmr1,*(__m64*)mmr2);
  }
 static __forceinline void v_pavgb(__m64 &mmr1,const __m64 &mmr2,__m64,__m64)
  {
   mmr1=_mm_avg_pu8(mmr1,mmr2);
  }
 static __forceinline void v_pavgb(__m64 &mmr1,const void *mmr2,__m64,__m64)
  {
   mmr1=_mm_avg_pu8(mmr1,*(__m64*)mmr2);
  }
 static __forceinline void sfence(void)
  {
   _mm_sfence();
  } 
 static __forceinline void movntq(void *dst,const __m64 &src)
  {
   _mm_stream_pi((__m64*)dst,src);
  }
 static __forceinline void v_pminub(__m64 &mmr1,const __m64 &mmr2,__m64)
  {
   mmr1=_mm_min_pu8(mmr1,mmr2);
  }
 static __forceinline void pmulhuw(__m64 &mmr1,const __m64 &mmr2)
  {
   mmr1=_mm_mulhi_pu16(mmr1,mmr2);
  }
 static __forceinline void prefetchnta(const void *ptr)
  {
   _mm_prefetch((const char*)ptr,_MM_HINT_NTA);
  }
 static __forceinline __m64 _mm_shuffle_pi16_0(const __m64 &src)
  {
   return _mm_shuffle_pi16(src,0); 
  } 
 static __forceinline __m64 _mm_shuffle_pi16_1(const __m64 &src)
  {
   return _mm_shuffle_pi16(src,1); 
  } 
 static __forceinline __m64 _mm_shuffle_pi16_14(const __m64 &src)
  {
   return _mm_shuffle_pi16(src,(3 << 2) + 2); 
  } 
 static __forceinline void psadbw(__m64 &mm3,const __m64 &mm2)
  {
   mm3=_mm_sad_pu8(mm3,mm2);
  }
 static __forceinline __m64 min_pu8(const __m64 &mm1,const __m64 &mm2)
  {
   return _mm_min_pu8(mm1,mm2);
  }
 static __forceinline __m64 max_pu8(const __m64 &mm1,const __m64 &mm2)
  {
   return _mm_max_pu8(mm1,mm2);
  }
 static __forceinline __m64 min_pi16(const __m64 &mm1,const __m64 &mm2)
  {
   return _mm_min_pi16(mm1,mm2);
  }
 static __forceinline __m64 max_pi16(const __m64 &mm1,const __m64 &mm2)
  {
   return _mm_max_pi16(mm1,mm2);
  }
};

static __forceinline __m64 _mm_absdif_u8(__m64 mm1,__m64 mm2)
{
 __m64 mm7=mm1;
 mm1=_mm_subs_pu8(mm1,mm2);
 mm2=_mm_subs_pu8(mm2,mm7);
 return _mm_or_si64(mm2,mm1);
}

static __forceinline __m64 _mm_abs_16(const __m64 &mm0)
{
 __m64 mm6=_mm_srai_pi16(mm0,15);
 return _mm_sub_pi16(_mm_xor_si64(mm0,mm6),mm6);
}

static __forceinline __m64 _mm_absdif_s16(__m64 mm0,__m64 mm1)
{
 __m64 mm2=mm0;
 mm0=_mm_cmpgt_pi16(mm0,mm1);
 __m64 mm4=mm2;
 mm2=_mm_xor_si64(mm2,mm1);
 mm2=_mm_and_si64(mm2,mm0);
 __m64 mm3=mm2;
 mm4=_mm_xor_si64(mm4,mm2);
 mm1=_mm_xor_si64(mm1,mm3);
 return mm1=_mm_sub_pi16(mm1,mm4);
}
static __forceinline void memadd(unsigned char *dst,const unsigned char *src,unsigned int len)
{
 __m64 *dst8=(__m64*)dst;const __m64 *src8=(__m64*)src;
 for (unsigned int i=0;i<len/8;i++,src8++,dst8++)
  *dst8=_mm_adds_pu8(*src8,*dst8);
}

#pragma warning(pop)

#endif
