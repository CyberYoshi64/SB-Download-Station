#include "common.hpp"
#include "keyboard.h"

#define WCHAR_CHAR_CONV_BUF 256

extern int errorcode;
extern C3D_RenderTarget* top;
extern C3D_RenderTarget* bottom;
std::string meta_prjn[256];
std::string meta_title[256];
std::string meta_desc[256];
u32 meta_flags[256];
u32 meta_ver[256];
u32 meta_total;
char metatemp[8];
char metabigtmp[4096];
char* charbuf;
char pMBBuffer[WCHAR_CHAR_CONV_BUF];
char* wchar_to_char(wchar_t wc){
	// char    *pMBBuffer = (char *)malloc( WCHAR_CHAR_CONV_BUF );
	wchar_t *pWCBuffer = &wc;
	wcstombs(pMBBuffer, pWCBuffer, WCHAR_CHAR_CONV_BUF ); // C4996
	return pMBBuffer;
}
Result Meta::init(){ // Initialise arrays
	for (int i=0; i < 256; i++){
		meta_prjn[i] = "";
		meta_title[i] = "";
		meta_desc[i] = "";
		meta_ver[i] = 0;
		meta_flags[i] = 0;
	}
	meta_total=0;
	return 0;
}

void Meta::prepareArrays(std::string fc){
	int term=0x203a3a;
	bool trimf;
	u8 type=0;
	u16 index=0;
	u16 fidx=0;
	std::string intnum="";
	std::string flgnum="";
	size_t j=0;
	init();
	
	while(j < fc.length()-6){
		if (((fc[j]<<16)+(fc[j+2]<<8)+fc[j+4]) == term){
			j += 8; break;
		}
		sprintf(metatemp,"%lc",(fc[j] + (fc[j+1]<<8)));
		if (fc[j] > 47)
			intnum += metatemp;
		j+=2;
	}
	fidx=stoi(intnum);
	intnum="";
	
	for (size_t i=j; i < fc.length()-6; i+=2){
		if (((fc[i]<<16)+(fc[i+2]<<8)+fc[i+4]) == term){
			i += 8; type = (type + 1) % 5;
			if (type == 0){
				meta_ver[index] = stoi(intnum);
				flgnum=""; intnum=""; index++; i+=2;
			}
		}
		if (i < fc.length()-6 && index < 256){
			sprintf(metatemp,"%lc",(char16_t)(fc[i] + (fc[i+1]<<8)));
			trimf=true;
			
			if (type==0) trimf=(((meta_prjn[index]).length())<14);
			if (type==1) trimf=(((meta_title[index]).length())<64);
			if (type==2) trimf=(((meta_desc[index]).length())<512);
			
			if (trimf){
				switch (type){
					case 0: meta_prjn[index] = meta_prjn[index] + metatemp; break;
					case 1: meta_title[index] = meta_title[index] + metatemp; break;
					case 2: meta_desc[index] = meta_desc[index] + metatemp; break;
					case 3: intnum = intnum + metatemp; break;
					case 4: meta_flags[index] += (!(*metatemp-48))<<(flgnum.length()); flgnum = flgnum + metatemp; break;
				}
			}
		}
	}
	meta_total = index;
	
	if (fidx != index){
		errorcode=10010008;
	}
}

void Meta::exit(){
	// There's nothing on the heap yet, so no cleaning up needed.
}
