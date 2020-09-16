#include "common.hpp"
#include "keyboard.h"

extern int errorcode;

std::string meta_prjn[256];
std::string meta_title[256];
std::string meta_desc[256];
int meta_ver[256];
int meta_total;
char metatemp[256];
char metatemp4[4];
char metabigtmp[4096];

Result Meta::init(){ // Initialise arrays
	for (int i=0; i < 256; i++){
		meta_prjn[i]="";
		meta_title[i]="";
		meta_desc[i]="";
		meta_ver[i]=0;
	}
	meta_total=0;
	return 0;
}

void Meta::prepareArrays(std::string fc){
	fc += "\n";
	char c;
	const char* term=" ::";
	bool trimf;
	u8 type=0;
	u16 index=0;
	u16 fidx=0;
	std::string intnum="";
	size_t j=0;
	init();
	
	while(j < fc.length()-1){
		sprintf(metatemp4,"%c%c%c",fc[j],fc[j+1],fc[j+2]);
		if (strncmp(metatemp4,term,3)==0){
			j += 4; break;
		}
		c = fc[j];
		sprintf(metatemp, "%lc",c);
		if (c > 47)
			intnum += metatemp;
		j++;
	}
	fidx=stoi(intnum);
	intnum="";
	
	for (size_t i=j; i < fc.length()-1; i++){
		sprintf(metatemp4,"%c%c%c",fc[i],fc[i+1],fc[i+2]);
		if (strncmp(metatemp4,term,3)==0){
			i += 4; type = (type + 1) % 4;
			if (type == 0){
				meta_ver[index] = stoi(intnum);
				intnum=""; index++; i++;
			}
		}
		if (i < fc.length()-1 && index < 256){
			c = fc[i];
			sprintf(metatemp, "%lc",c);
			
			trimf=true;
			
			if (type==0) trimf=(((meta_prjn[index]).length())<14);
			if (type==1) trimf=(((meta_title[index]).length())<64);
			if (type==2) trimf=(((meta_desc[index]).length())<512);
			
			if (trimf){
				switch (type){
					case 0: meta_prjn[index] += metatemp; break;
					case 1: meta_title[index] += metatemp; break;
					case 2: meta_desc[index] += metatemp; break;
					case 3: intnum += metatemp; break;
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
