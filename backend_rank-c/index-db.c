/*
 * by tfa at informatika.unsyiah.ac.id/tfa
 * Modified by Faris Khowarizmi Tarmizi @ 10 December 2017
 * 
 */

#include <sys/time.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hashtools.h"
#include "docindex.h"
#include "index-tools.h"
#include "util.h"
#include "define.h"

int main (void){
	FILE		 *fdat;
	DIR 		 *dp;
	struct dirent  *ep;
	char           dirdata[STRPATH];
	long int       docno, doclen;
	char           path[STRPATH];
	char  	 word[WORDLEN];
	HashTableInfo  *hashTable;
	DocIndexHead   *docIdx;
	StopList       stoplist[TOTLIST];
	
	loadStopList(stoplist);
	
	printf("Data directory: ");
	scanf("%s", dirdata);
	startTiming();
	
	/* Buat inisialisasi hash table */
	hashTable = initHashTable(0x1000);
	
	/* Inisialisasi docindex untuk mencatat info file yg diindex */
	docIdx = newDocIndex();
	
	/* open directory where documents are stored */
	dp = opendir(dirdata);
	if(dp!=NULL){
		docno=0;
		printf("\nWait, reading documents...\n");
		while((ep=readdir(dp))){
			if((strcmp(ep->d_name,".")!=0)&&(strcmp(ep->d_name,"..")!=0)){
				path[0]='\0';
				strcat(path,dirdata);
				strcat(path,ep->d_name);
				printf("path %s \n",path);
				if((fdat=fopen(path,"r"))==NULL){
					printf("Opening %s file [%s] failed...\n",path,ep->d_name);
					return 0;
				}
				else{
					
					if((docno!=0)&&((docno % 1000)==0)){
						printf("%ldth documents\n",docno);
					}
					doclen=0;
					
					/* read the doc and get all words */
					while(!feof(fdat)){
						getword(fdat,word);   /* parse each words */
						if(isalpha(word[0])){
							wordToLower(word);  /* convert word to lower case */
							if(!isstopword(word,TOTLIST,stoplist)){
								addWord(hashTable, docno, word);
							}
						}
						doclen+=strlen(word);
					}
					
					addDocIndex(docIdx, path, doclen);
					
				}
				docno++;
				fclose(fdat);
			}
		}
		
		(void) closedir(dp);
		printf("%ldth documents\n",docno);
		
	}
	else{
		printf("Cannot access data directory...\n");
		return 0;
	}
	
	/* Selesaikan hash table! */
	printf("\nCreating inverted table...\n");
	saveHashTable(hashTable, "index-db/data.tbl");
	saveDocIndex(docIdx, "index-db/data.fdx");
	
	printf("Freeing memory...\n");
	freeDocIndex(docIdx);
	freeHashTable(hashTable);
	
	printf("\nGenerating index complete.\n");
	
	stopTiming();
	printf("Time required: %f seconds\n\n",timingDuration() * 1e-2);
	return 1;
	
}

