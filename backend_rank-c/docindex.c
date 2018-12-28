/*
 * docindex.c 
 *
 * Author: Faris Khowarizmi Tarmizi (thekill96[at]gmail.com)
 * Last edited: 10 December 2017
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "docindex.h"

DocIndexHead *newDocIndex(){
	DocIndexHead *headMem = malloc(sizeof(DocIndexHead));	
	memset(headMem, '\0', sizeof(DocIndexHead));
	return headMem;
}

void freeDocIndex(DocIndexHead *docIdx){
	if (docIdx->count > 0){
		DocIndexNode *node = docIdx->first;
		while (node != NULL){
			DocIndexNode *nextNode = node->next;
			if (node->fileNameStr != NULL){
				free(node->fileNameStr);
			}
			free(node);
			node = nextNode;
		}
	}
	free(docIdx);
}

void addDocIndex(DocIndexHead *docIdx, char *fileName, int fileSize){
	char *strBuf;
	DocIndexNode *node = malloc(sizeof(DocIndexNode));
	
	node->fileSize = fileSize;
	node->fileNameSize = strlen(fileName) + 1;
	strBuf = malloc(node->fileNameSize);
	strcpy(strBuf, fileName);
	node->fileNameStr = strBuf;
	
	if (docIdx->count == 0){
		docIdx->first = node;
	}
	if (docIdx->last != NULL){
		docIdx->last->next = node;
	}
	docIdx->last = node;
	docIdx->count++;
	
}

int saveDocIndex(DocIndexHead *docIdx, char *outFileName){
	FILE *fout;
	DocIndexFileHead fileHead;
	DocIndexFileNode fileNode;
	DocIndexNode *node;
	int x;
	
	if ((fout = fopen(outFileName, "wb")) == NULL){
		return 0;
	}
	
	fileHead.count = docIdx->count;
	fileHead.arrSize = fileHead.count * sizeof(DocIndexArray);
	fwrite(&fileHead, sizeof(DocIndexFileHead), 1, fout);
	
	node = docIdx->first;
	x = 0;
	while (node != NULL){
		fileNode.indexId = x;
		fileNode.fileSize = node->fileSize;
		fileNode.fileNameSize = node->fileNameSize;
		fwrite(&fileNode, sizeof(DocIndexFileNode), 1, fout);
		fwrite(node->fileNameStr, fileNode.fileNameSize, 1, fout);
		
		x++;
		node = node->next;
	}
	
	fclose(fout);
	return 1;
}

DocIndexArray *loadDocIndexArray(int *arrCount, char *inFileName){
	FILE *fin;
	DocIndexFileHead fileHead;
	DocIndexFileNode fileNode;
	DocIndexArray *array;
	int x, idx;
	char *fileStr;
	
	if ((fin = fopen(inFileName, "rb")) == NULL){
		return NULL;
	}
	
	fread(&fileHead, sizeof(DocIndexFileHead), 1, fin);
	array = malloc(fileHead.arrSize);
	for (x=0; x<fileHead.count; x++){
		fread(&fileNode, sizeof(DocIndexFileNode), 1, fin);
		idx = fileNode.indexId;
		array[idx].fileSize = fileNode.fileSize;
		array[idx].fileNameSize = fileNode.fileNameSize;
		fileStr = malloc(fileNode.fileNameSize);
		fread(fileStr, fileNode.fileNameSize, 1, fin);
		array[idx].fileNameStr = fileStr;
	}
	
	fclose(fin);
	*arrCount = fileHead.count;
	return array;
}

void freeDocIndexArray(DocIndexArray *array, int count){
	int x;
	for (x=0; x<count; x++){
		if (array[x].fileNameStr != NULL){
			free(array[x].fileNameStr);
		}
	}
	free(array);
}
