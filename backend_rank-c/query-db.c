#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "hashtools.h"
#include "docindex.h"
#include "query-tools.h"
#include "util.h"
#include "define.h"

#ifdef JSON_OUT
#include "jsontools.h"
#endif

void query(HashTableInfo *, DocIndexArray *, int, char *, int);
int  getQueryTerm(char *, char [MAXQUERY][WORDLEN]);
int  sort(HashTableInfo *, int, char [MAXQUERY][WORDLEN]);

long int totalTerm,totalDocs;
StopList stoplist[TOTLIST];
FileInfo *fileinfo;
#ifdef JSON_OUT
ThkJson *json;
#endif

int main(int argc, char *argv[]){
	int  heapSize, docCount;
	char qry[BUFLEN];
	HashTableInfo *hashTable;
	DocIndexArray *docFiles;
	
	if (argc <= 1) {
		#ifdef JSON_OUT
		printf("{}");
		#else
		printf("Input a query or more!!!\n");
		#endif
		exit(1);
	}
	
	strcpy(qry, argv[1]);
	heapSize = argc > 2 ? atoi(argv[2]) : 15;
	
	/* Baca stoplist */
	loadStopList(stoplist);
	
	/* Load hashtable dalam file dan list file yg diindeks */
	hashTable = openHashTable("index-db/data.tbl");
	docFiles = loadDocIndexArray(&docCount, "index-db/data.fdx");
	
	/* baca parameter */
	totalDocs = hashTable->docCount;
	totalTerm = hashTable->entryCount;
	
	#ifdef JSON_OUT
	json = initJson(NULL);
	jsonBegin(json);
	jsonAddIntProperty(json, "totalTerm", totalTerm);
	jsonAddIntProperty(json, "totalDocs", totalDocs);
	#else
	printf("# Found %ld distict terms in %ld documents\n\n", totalTerm, totalDocs);
	#endif
	
	startTiming();
	query(hashTable, docFiles, docCount, qry, heapSize);
	stopTiming();
	
	#ifdef JSON_OUT
	jsonAddDoubleProperty(json, "queryTime", (double)timingDuration());
	jsonEnd(json);
	freeJson(json);
	#else
	printf("# Time required: %f mseconds\n",timingDuration());
	#endif
	
	/* Tutup hash table */
	freeDocIndexArray(docFiles, docCount);
	freeHashTable(hashTable);
	
	return 0;
}

void query(HashTableInfo *table, DocIndexArray *files, int filesCount, char *query, int heapSize){
	int i, t, tempd;
	int fw, docNo, wordFreq;
	HashDocInfo *docInfo;
	Heap *heap;
	HashTableEntry *entry;
	double *accumulator;
	char queryTerm[MAXQUERY][WORDLEN];
	double s_q_d, tempr;
	int  heapMemSize, index;
	int  totalQTerm;
	int  totalEntry = table->entryCount;
	#ifdef JSON_OUT
	int  resultCount;
	#endif
	
	if (query == NULL){
		return;
	}
	totalQTerm = getQueryTerm(query, queryTerm);
	if (totalQTerm == 0){
		#ifdef JSON_OUT
		jsonAddIntProperty(json, "resultCount", 0);
		#else
		printf("All query terms are stopword\n");
		#endif
		return;
	}
	/* Alokasikan heap */
	heapMemSize = sizeof(Heap) * heapSize;
	heap = (Heap*) malloc(heapMemSize);
	memset(heap, '\0', heapMemSize);
	
	/* sort query terms in ascending order based on fw of the term,
	   rare term is more interesting and should be examined first */
	sort(table, totalQTerm, queryTerm);
	
	/* Set accumulator dan kosongkan isinya */
	accumulator = (double *) malloc(sizeof(double) * totalEntry);
	for(i = 0; i < totalEntry; i++){
		accumulator[i] = 0;
	}
	
	/* Iterasi query yang masuk */
	for (i=0; i<totalQTerm; i++){
		index = queryHashIndex(table, queryTerm[i]);
		if (index < 0){
			#ifndef JSON_OUT
			printf("# Word ['%s'] is not indexed\n", queryTerm[i]);
			#endif
		}
		else {
			entry = &table->entry[index];
			fw = entry->count;
			#ifndef JSON_OUT
			printf("# Word ['%s'], fw (num of doc containing the word) = %d\n", queryTerm[i], fw);
			#endif
					
			/* for each pair, show the result */
			docInfo = entry->firstDocInfo;
			for(t=0; t<fw; t++){
				docNo    = docInfo->id;
				wordFreq = docInfo->freq;
				s_q_d    = log(totalEntry / (float)fw + 1) * log(wordFreq + 1);
				accumulator[docNo] += s_q_d;
				docInfo = docInfo->nextDocInfo;
			}
		}
	}
	
	/* Normalize accumulator by document length */
	for(i=0; i<filesCount; i++){
		accumulator[i] += accumulator[i] / files[i].fileSize;  /*using L(D)*/
	}
	
	/* build heap of size heapSize */
	for(i=0; i<totalEntry; i++){
		if(i < heapSize){
			buildHeap(heap, i, i, accumulator[i]);
		}
		else{
			/* compare new value with the root of the heap, if the new value is
			   larger then the root, insert the new value into the heap */
			if(accumulator[i] > heap[0].ranked){
				heap[0].ranked = heap[heapSize-1].ranked;
				heap[0].docno = heap[heapSize-1].docno;
				heap[heapSize-1].ranked = accumulator[i];
				heap[heapSize-1].docno = i;
			}
					
			/* adjust heap */
			for(t=(heapSize/2)-1; t>=0; t--){
				adjustHeap(heap, t ,heapSize);
			}
		}
	}
			
	/* sort heap: bubble sort */
	for(i=heapSize-1; i>0; i--){
		for(t=0; t<i; t++){
			if(heap[t].ranked < heap[t+1].ranked){
				tempr = heap[t].ranked;
				tempd = heap[t].docno;
				heap[t].ranked = heap[t+1].ranked;
				heap[t].docno = heap[t+1].docno;
				heap[t+1].ranked = tempr;
				heap[t+1].docno = tempd;
			}
		}
	}
	
	#ifdef JSON_OUT
	resultCount = 0;
	jsonAddIntProperty(json, "heapSize", heapSize);
	jsonAddArrayProperty(json, "results");
	#else
	printf("\n# Top %dth documents are:\n", heapSize);
	#endif
	
	/* print heap */
	for(i=0; i < heapSize; i++){
		#ifdef JSON_OUT
		if (heap[i].ranked != 0){
			jsonBegin(json);
			jsonAddIntProperty(json, "docNo", heap[i].docno);
			/*jsonAddStringProperty(json, "docTitle", files[heap[i].docno].fileNameStr);*/
			jsonAddStringProperty(json, "docPath", files[heap[i].docno].fileNameStr);
			jsonAddIntProperty(json, "rankNo", i+1);
			jsonAddDoubleProperty(json, "rankValue", (double)heap[i].ranked);
			jsonEnd(json);
			resultCount++;
		}
		#else
		printf("\t%ld\t\t%f\n", heap[i].docno, heap[i].ranked);
		#endif
	}
	#ifdef JSON_OUT
	jsonCloseArrayProperty(json);
	jsonAddIntProperty(json, "resultCount", resultCount);
	#else
	printf("\n");
	#endif
	
	free(heap);
	
}

/* getQueryTerm is the function to parse the query into
   several terms */
int getQueryTerm(char *query, char queryterm[MAXQUERY][WORDLEN]){
	char word[WORDLEN];
	char *pw=word;
	char *pq=query;
	int  totalqueryterm=0;
	
	query[strlen(query)]='\0';     /* replace character '\n' with '\0'I*/
	while(*pq!='\0'){
		while(isspace(*pq)){
			pq++;
		}
		
		while(*pq!='\0'){
			if(!isalnum(*pq)){
				pq++;
				break;
			}
			*pw=*pq;
			pq++;
			pw++;
		}
		*pw='\0';
		if(strlen(word)!=0){
			/* check whether the word is a stopword, if
			   it is not, then add to queryterm array */
			if(!isstopword(word,TOTLIST,stoplist)){
				strcpy(queryterm[totalqueryterm++],word);
			}
		}
		word[0]='\0';
		pw=word;
	}
	
	return totalqueryterm;
}

/* sort is a function to sort query terms by their frequency value in ascending
   order, rare term should examine first in order to heuristically limited the
   accumulator */
int sort(HashTableInfo *table, int totalqterm, char queryterm[MAXQUERY][WORDLEN]){
	int i,t,pos;
	char temp[WORDLEN];
	float fw1,fw2;
	HashTableEntry *entry = table->entry;
	
	for(i=totalqterm-1; i>0; i--){
		for(t=0; t<i; t++){
			pos = queryHashIndex(table, queryterm[t]);
			fw1 = entry[pos].count;
			pos = queryHashIndex(table, queryterm[t+1]);
			fw2 = entry[pos].count;
			if(fw1 > fw2){              /*sort in ascending order*/
				strcpy(temp, queryterm[t]);
				strcpy(queryterm[t], queryterm[t+1]);
				strcpy(queryterm[t+1], temp);
			}
		}
	}
	
	return 1;
}
