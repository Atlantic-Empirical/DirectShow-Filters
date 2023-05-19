#include <appincludes.h>
//#include <atlbase.h>

void CKeystoneVideoTwoPin::extract(bool top_field, LPBYTE src)
{
	int pitch = 1440;
	LPBYTE dst = s_buffer;
	if(!top_field) {src += pitch; dst += pitch;}

	//DbgLog((LOG_TRACE,0,TEXT("extract(%d, %08x)"), top, *(DWORD*)src));

	for(int y = 0; y < 480/2; y++, src += pitch*2, dst += pitch*2)
		memcpy(dst, src, pitch);
}

//void memsetd(void* dst, unsigned int c, int nbytes)
//{
//	__asm
//	{
//		mov eax, c
//		mov ecx, nbytes
//		shr ecx, 2
//		mov edi, dst
//		cld
//		rep stosd
//	}
//}
