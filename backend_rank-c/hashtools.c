/*
 * hashtools.c - Routine utama untuk proses indexing menggunakan hash table
 *
 * Author: Faris Khowarizmi Tarmizi (thekill96[at]gmail.com)
 * Last edited: 10 December 2017
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "hashtools.h"

/* Inisialisasi info hash table */
HashTableInfo *initHashTable(int tableCount){
	HashTableInfo *hptr = malloc(sizeof(HashTableInfo));
	HashTableEntry *entriesMem;
	
	/* Definisikan besar memori yg digunakan tabel, max entri, dan kosongkan jumlah entri */
	hptr->tableSize = tableCount * sizeof(HashTableEntry);
	hptr->tableCount = tableCount;
	hptr->entryCount = 0;
	hptr->docCount = 0;
	
	/* Alokasi memori menggunakan malloc */
	/*printf("Allocating %d bytes of memory for %d entries\n", hptr->tableSize, tableCount);*/
	entriesMem = malloc(hptr->tableSize);
	if (entriesMem == NULL){
		printf("Cannot allocate %d bytes of memory!\n", hptr->tableSize);
		return NULL;
	}
	hptr->entry = entriesMem;
	/* Kosongkan tabel entri dengan null-byte */
	memset(entriesMem, '\0', hptr->tableSize);
	
	return hptr;
}

/*  */
HashTableInfo *openHashTable(char *inPath){
	int x, y;
	int entryCount, hashIndex;
	FILE *fileTable;
	char wordBuffer[255];
	char *wordMem;
	HashTableFileHeader  fileHead;
	HashTableFileEntry   fileEntry;
	HashTableFileDocInfo fileDocInfo;
	HashTableInfo *table;
	HashTableEntry *entry;
	HashDocInfo *docInfo;
	
	if ((fileTable = fopen(inPath, "rb")) == NULL){
		printf("Error: cannot open input file %s\n", inPath);
		return NULL;
	}
	
	/* Baca header file table */
	if (!fread(&fileHead, sizeof(HashTableFileHeader), 1, fileTable)){
		printf("Error: cannot read index header from file %s\n", inPath);
		fclose(fileTable);
		return NULL;
	}
	
	/* Inisialiasi tabel sebanyak entry yg ada di file */
	table = initHashTable(fileHead.tableCount);
	/* NOTE: entryCount dinaikkan oleh fungsi newTableEntry,
	         maka counter dikosongkan dulu ;) */
	/*table->entryCount = fileHead.entryCount;*/
	table->entryCount = 0;
	table->docCount = fileHead.docCount;
	
	entryCount = fileHead.entryCount;
	for (x=0; x<entryCount; x++){
		/* Baca entry dari file */
		if (!fread(&fileEntry, sizeof(HashTableFileEntry), 1, fileTable)){
			printf("Error: cannot read docInfo for entry no.%d!\n", x+1);
			freeHashTable(table);
			fclose(fileTable);
			return NULL;
		}
		hashIndex = fileEntry.hashIndex;
		entry = &table->entry[hashIndex];
		/* Baca string untuk entry yg sedang diproses */
		if (!fread(wordBuffer, fileEntry.strSize, 1, fileTable)){
			printf("Error: cannot read string data for hash index %d!\n", hashIndex);
			freeHashTable(table);
			fclose(fileTable);
			return NULL;
		}
		/* Tambahkan entri ke tabel */
		newTableEntry(table, entry, wordBuffer);
		entry->strSize = fileEntry.strSize;
		wordMem = malloc(fileEntry.strSize);
		strcpy(wordMem, wordBuffer);
		entry->strData = wordMem;
		entry->count = fileEntry.count;
		
		/* Punya rujukan docid? */
		if (fileEntry.count > 0){
			HashDocInfo *lastDocInfo = NULL;
			for (y=0; y<fileEntry.count; y++){
				/* Baca node docId */
				if (fread(&fileDocInfo, sizeof(HashTableFileDocInfo), 1, fileTable) == 0){
					printf("Error: cannot read docInfo for hash %d!\n", hashIndex);
					freeHashTable(table);
					fclose(fileTable);
					return NULL;
				}
				docInfo = newDocInfoNode(fileDocInfo.id);
				docInfo->freq = fileDocInfo.freq;
				/* Kalau node pertama, set pointer first ke node pertama */
				if (y==0)
					entry->firstDocInfo = docInfo;
				if (lastDocInfo != NULL)
					lastDocInfo->nextDocInfo = docInfo;
				lastDocInfo = docInfo;
			}
		}
	}
	
	fclose(fileTable);
	return table;
	
}

int saveHashTable(HashTableInfo *table, char *outPath){
	FILE *fileTable;
	int x;
	int tableCount = table->tableCount;
	HashTableFileHeader  fileHead;
	HashTableFileEntry   fileEntry;
	HashTableFileDocInfo fileDocInfo;
	HashDocInfo *docInfo;
	
	if ((fileTable = fopen(outPath, "wb")) == NULL){
		printf("Error: cannot create output file %s\n", outPath);
		return 0;
	}
	
	/* Tulis header dari file table */
	fileHead.tableSize = table->tableSize;
	fileHead.tableCount = table->tableCount;
	fileHead.entryCount = table->entryCount;
	fileHead.docCount = table->docCount;
	fwrite(&fileHead, sizeof(HashTableFileHeader), 1, fileTable);
	
	for (x=0; x<tableCount; x++){
		HashTableEntry *entry = &table->entry[x];
		/* Pastikan hanya array yg memiliki entry yg dimasukkan */
		if (entry->hasEntry){
			/* Tulis entry */
			fileEntry.hashIndex = x;
			fileEntry.strSize = entry->strSize;
			fileEntry.count = entry->count;
			/*printf("%d\n", entry->count);*/
			fwrite(&fileEntry, sizeof(HashTableFileEntry), 1, fileTable);
			/* Tulis string */
			fwrite(entry->strData, entry->strSize, 1, fileTable);
			/* Tulis docId */
			docInfo = entry->firstDocInfo;
			while (docInfo != NULL){
				fileDocInfo.id = docInfo->id;
				fileDocInfo.freq = docInfo->freq;
				fwrite(&fileDocInfo, sizeof(HashTableFileDocInfo), 1, fileTable);
				docInfo = docInfo->nextDocInfo;
			}
		}
	}
	
	fclose(fileTable);
	return 1;
	
}

/* Membebaskan hash table beserta seluruh isinya */
void freeHashTable(HashTableInfo *table){
	
	int x;
	int tableCount = table->tableCount;
	
	/* Bebaskan entri satu persatu jika ada */
	for (x=0; x<tableCount; x++){
		HashTableEntry *entry = table->entry;
		entry = &entry[x];
		if (entry->hasEntry){
			if (entry->firstDocInfo != NULL){
				deleteDocInfoNodes(entry->firstDocInfo);
				entry->firstDocInfo = NULL;
			}
			if (entry->strData != NULL)
				free(entry->strData);
		}
	}
	free(table->entry);
	free(table);
	
}

/* Menginisialisasikan lokasi dari entry baru */
void newTableEntry(HashTableInfo *table, HashTableEntry *entry, char *word){
	
	char *strNode;
	if (table->entryCount == table->tableCount){
		printf("Cannot allocate table entry anymore! [max:%d]\n", table->tableCount);
		exit(-1);
	}
	
	/* Tandai kalau array ini ada entrynya */
	entry->hasEntry = 1;
	entry->strSize = strlen(word) + 1;
	strNode = malloc(entry->strSize);
	strcpy(strNode, word);
	entry->strData = strNode;
	entry->count = 0;
	entry->firstDocInfo = NULL;
	
	/* Tambahkan jumlah entri pada tabel */
	table->entryCount++;
	
}

/* Membuat node docInfo baru */
HashDocInfo *newDocInfoNode(int docIdVal){
	HashDocInfo *docMem = malloc(sizeof(HashDocInfo));
	
	docMem->id = docIdVal;
	docMem->nextDocInfo = NULL;
	
	return docMem;
}

/* Menghapus satu node saja
   WARNING: node yg selanjutnya tidak dihapus apabila ada & menyebabkan pointer ke memori hilang!
            gunakan prosedur deletedocInfosFrom untuk hapus secara rekursif */
void deleteDocInfoNode(HashDocInfo *node){
	free(node);
}

/* Menghapus node secara rekursif */
void deleteDocInfoNodes(HashDocInfo *node){
	HashDocInfo *nextNode = node->nextDocInfo;
	deleteDocInfoNode(node);
	/* Proses hapus node berikutnya jika ada */
	if (nextNode != NULL)
		deleteDocInfoNodes(nextNode);
}

int queryHashIndex(HashTableInfo *table, char *word){
	int hash = calculateHash(word, table->tableCount);
	
	/* Loop hingga ketemu entry dengan string yg sama karena hash collision */
	while (1){
		HashTableEntry *entry = &table->entry[hash];
		if (entry->hasEntry == 0)
			return -1;
		else if (strcmp(word, entry->strData) == 0)
			return hash;
		hash++;
		if (hash > table->tableCount){
			hash = 0;
		}
	}
	return hash;
}

/* Tambahkan kata dan docId ke hash table */
void addWord(HashTableInfo *table, int docId, char *word){
	
	/* Dapatkan jumlah max entry dan hitung hash utuk word yg masuk */
	int tableCount = table->tableCount;
	int hash = calculateHash(word, tableCount);
	int docInfoCount;
	HashTableEntry *entry;
	HashDocInfo *docInfo, *lastDocInfo;
	
	/* Memastikan tidak terjadi hash collision, string dicek dalam loop */
	while(1){
		entry = &table->entry[hash];
		if (entry->hasEntry){
			if (strcmp(entry->strData, word) == 0){
				break; /* string sama? gunakan entry array ini */
			}
		}
		else{
			newTableEntry(table, entry, word); /* buat entri baru */
			break;
		}
		/* Kalau terjadi colission (tidak di break), tambahkan nilai 1 keatas */
		hash++;
		if (hash > tableCount)
			hash = 0;
	}
	
	/* Ambil pointer dari node pertama, dan dapatkan docid terakhir (jika iya)
	   jika ada, lakukan iterasi sampai ketemu node dengan docId yg sama */
	docInfoCount = entry->count;
	docInfo = entry->firstDocInfo;
	lastDocInfo = NULL;
	if (docInfoCount > 0){
		HashDocInfo *ptrDocInfo = docInfo;
		docInfo = NULL;
		while (ptrDocInfo != NULL){
			if (ptrDocInfo->id == docId){
				docInfo = ptrDocInfo;
				break;
			}
			lastDocInfo = ptrDocInfo;
			ptrDocInfo = ptrDocInfo->nextDocInfo;
		}
	}
	/* Node dengan docId yg diinputkan belum ada? Buat baru... */
	if (docInfo == NULL){
		docInfo = newDocInfoNode(docId);
		if (entry->count == 0)
			entry->firstDocInfo = docInfo;
		if (lastDocInfo != NULL)
			lastDocInfo->nextDocInfo = docInfo;
		entry->count++;
	}
	/* Pastikan dapat doc id terbesar */
	if (docId+1 > table->docCount){
		table->docCount = docId+1;
	}
	/* Jangan lupa tambahkan frekuensinya :) */
	docInfo->freq++;
	
}

/* Hashing menggunakan algoritma Jenkins hash dengan penyesuaian untuk hash table
   https://en.wikipedia.org/wiki/Jenkins_hash_function */
int calculateHash(char *word, int max){
	int i = 0;
	int length = strlen(word);
	unsigned long hash = 0;
	while (i != length) {
		hash += word[i++];
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	if (max > 0)
		hash %= max;
	return hash;
}
