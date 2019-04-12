/* 046267 Computer Architecture - Spring 2019 - HW #1 */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define MAX_HISTORY_SIZE 256
#define MAX_BTB_SIZE 32
#define TARGRT_SIZE 30

typedef enum{SNT=0;WNT=1;WT=2;ST=3}FSM;
typedef enum{not_using_share=1,using_share_mid=2;using_share_lsb=3}SHARED;

typedef struct BTBEntey_t{
	uint32_t tag;
	//uint32_t mask;
	uint32_t target;
}BTBEntey;
 

BTBEntey btbentry[MAX_BTB_SIZE];


int logmask(int btbsize){
	int counter =0;
	while(btbsize>=1){
		btbsize=btbsize/2;
		counter++;
	}
}
int dec_index(uint32_t pc, int mask_size){
	int dec_index;
	pc = pc <<2;	
	for (int i=0; i< mask_size; i++){
		Gmask = Gmask << 1 ;
		Gmask =+ 1;
	}
	return dec_index = pc & Gmask;
}
/*
int BIN2DEC(uint32_t mask , int masksize){
	int dec, bit;
	for(int i=0; i< masksize; i++){
		bit = mask%10;
		mask = mask >> 1;
		dec =+ bit*pow(2,i);
	}
	return dec;
}
*/


 bool ENTRY_UP(BTBEntey* btbentry ,uint32_t pc, int tagsize, uint32_t pc_target){
	 if (btbentry == NULL){
		 return false;
	 }
	 pc = pc >> 2;
	 int tmask=1;
	 for(int i=0; i<tagsize; i++){
		 tmask = tmask << 1;
		 tmask=+1;
	 }
	 btbentry->tag = pc & tmask;
	 btbentry-> target = pc_target;
	 return true;
	 
 }

class predictor {
	unsigned int btbsize;
	unsigned int historyREGsize;
	unsigned int tagsize;
	int history_Size;
	FSM FSM_defualt;
	bool is_global_history;
	bool is_global_tables;
	unsigned int GlobalHistBHR;
	SHARED shared;
	unsigned int LocalHistBHR [MAX_BTB_SIZE];
	FSM GlobalTable[MAX_HISTORY_SIZE];
	FSM LocalTable[MAX_HISTORY_SIZE][MAX_BTB_SIZE];
	int PredictorSize;
	int mask_size;
	
	
	public:
		predictor(unsigned btbSize, unsigned historySize, unsigned tagSize, unsigned fsmState,
			bool isGlobalHist, bool isGlobalTable, int Shared): btbsize(btbSize),historyREGsize(historySize),tagsize(tagSize),FSM_defualt(fsmState),is_global_history(isGlobalTable),is_global_tables(isGlobalTable),shared(Shared);
			{
				history_Size=pow(2,historyREGsize);
				GlobalHistBHR=0;
				LocalHistBHR[MAX_BTB_SIZE]={0};
				GlobalTable[MAX_HISTORY_SIZE]= {FSM_defualt};
				for(int i=0; i<MAX_HISTORY_SIZE; i++){
					for(int j=0; j<MAX_BTB_SIZE; j++){
						LocalTable[i][j]=FSM_defualt;
					}
				}
				mask_size = logmask(btbsize);
			}
		/*int dec_index(uint32_t pc){
			int dec_index;
			uint32_t bin_index;
			uint32_t Gmask=0;
			pc = pc <<2;
			
			for (int i=0; i< mask_size; i++ ){
				Gmask = Gmask << 1 ;
				Gmask =+ 1;
			}
			bin_index = pc & Gmask;
			dec_index = BIN2DEC(bin_index, mask_size);
			return dec_index;
		}*/
			
			
		int GetFSMtable (uint32_t pc){
			int histmask=0;
			uint8_t FSMindex,newpc=0;
			unsigned hisorty;
			if (is_global_history){
				hisorty=GlobalHistBHR;
			}
			else{
				pc = pc << 2;
				int index_hist = dec_index(pc, mask_size);
				hisorty = LocalHistBHR[index_hist];
			}
			for (int i=0; i<historyREGsize;i++){
				histmask=histmask <<1;
				histmask=+1;
			}

			FSMindex= hisorty&histmask;
			
			if(shared =not_using_share){
				return FSMindex;
			}
			else if(shared = using_share_mid){
				newpc=(uint8_t)	(pc >> 16);
				FSMindex = FSMindex ^ newpc;	
			}
			else if(shared = using_share_lsb){
				newpc=(uint8_t)	(pc >> 2);
				FSMindex = FSMindex ^ newpc;
			}
			FSMindex= FSMindex & histmask;
			return FSMindex;
		}
		
		bool pred_taken(int HISTindex,int FSMindex){
			FSM taken;
			if(is_global_tables){
				taken = GlobalTable[FSMindex];
			}
			else{
				taken = LocalTable[FSM_index][HISTindex];
			}
			if(taken == SNT || taken == WNT){
				return false;
			}
			else {
				return true;
			}
		}

		
		
		
		int get_states(){
			
				if (is_global_history == true && isGlobalTable == true){
					PredictorSize= btbsize*(tagsize+TARGRT_SIZE+historyREGsize+2*history_Size);
				}
				else if  (is_global_history == false && isGlobalTable == true){
				}
				else if  (is_global_history == true && isGlobalTable == false){
				}
				else {
					PredictorSize= btbsize*(tagsize+TARGRT_SIZE+historyREGsize+2*history_Size);
				}
		}
		
	
}




	





int BP_init(unsigned btbSize, unsigned historySize, unsigned tagSize, unsigned fsmState,
			bool isGlobalHist, bool isGlobalTable, int Shared){
			predictor* PREDICTOR = new predictor(btbSize, historySize,tagSize,fsmState,isGlobalHist, isGlobalTable,Shared);
			
			
	return -1;
}




bool BP_predict(uint32_t pc, uint32_t *dst){
	int HISTindex, tagsize;
	bool taken;
	HISTindex = dec_index(pc, PREDICTOR->mask_size);
	*BTBEntey curr_entry;
	tagsize= PREDICTOR->tagsize;
	ENTRY_UP(&curr_entry,pc,tagsize,0);
	if (curr_entry.tag != btbentry[HISTindex].tag){
		*dst = pc+4;
		return false;
	}
	int FSM_index =PREDICTOR->GetFSMtable(pc);
	if(!(taken = PREDICTOR-> pred_taken(HISTindex, FSM_index)){
		*dst = pc+4;
		return false;
	}
	*dst = (btbentry[HISTindex].target);
	return true;
}

void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){
	
	return;
}

void BP_GetStats(SIM_stats *curStats){
	return;
}

