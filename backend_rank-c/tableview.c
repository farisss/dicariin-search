/* tableview.c - coded by Thiekus */
#include <stdio.h>
#include <stdlib.h>
#include "hashtools.h"
#include "docindex.h"

int main(int argc, char *argv[]){
	HashTableInfo *hashTable = openHashTable("index-db/data.tbl");
	
	printf("\n#=============== Statistik file hash table ===============#\n");
	printf("\t* Besar entry yang dapat ditampung: %d entri\n", hashTable->tableCount);
	printf("\t* Besar memori yang dialokasikan: %d bytes\n", hashTable->tableSize);
	printf("\t* Banyak kata-kata yang unik: %d kata\n", hashTable->entryCount);
	printf("\t* Banyak dokumen yang diindeks: %d\n", hashTable->docCount);
	
	if (argc > 1){
		int docCount, hashIndex, x;
		DocIndexArray *docArray = loadDocIndexArray(&docCount, "index-db/data.fdx");
		for (x=1; x<argc; x++){
			char *word = argv[x];
			printf("\n\t* String: %s\n", word);
			hashIndex = queryHashIndex(hashTable, word);
			if (hashIndex >= 0){
				HashTableEntry *entry = &hashTable->entry[hashIndex];
				int docCount = entry->count;
				printf("\t|_* Hash Index: %d\n", hashIndex);
				printf("\t|_* Jumlah dokumen yang merujuk: %d\n", docCount);
				if (docCount > 0){
					HashDocInfo *docInfo = entry->firstDocInfo;
					while (docInfo != NULL){
						printf("\t  |_* DocID %3d [%16s]: %3d word\n", docInfo->id, docArray[docInfo->id].fileNameStr, docInfo->freq);
						docInfo = docInfo->nextDocInfo;
					}
				}
			}
			else {
				printf("\t  Kata tidak ditemukan!\n");
			}
		}
		freeDocIndexArray(docArray, docCount);
	}
	
	printf("\nWell done ;)\n\n");
	
	freeHashTable(hashTable);
	
	return EXIT_SUCCESS;
	
}
