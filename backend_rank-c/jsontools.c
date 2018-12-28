/* 
 * Thiekus's Mini JSON Writer Tools (jsontools.c)
 * Copyright (C) Faris Khowarizmi Tarmizi 2017
 * 
 * Email: thekill96[at]gmail.com
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "jsontools.h"

ThkJson *initJson(FILE *stream){
	ThkJson *json = malloc(sizeof(ThkJson));
	
	json->stream = stream == NULL? stdout: stream;
	json->hasLastProp = 0;
	
	return json;
}

void freeJson(ThkJson *json){
	free(json);
}

void jsonBegin(ThkJson *json){
	if (json->hasLastProp)
		fprintf(json->stream, ",");
	fprintf(json->stream, "{");
	json->hasLastProp = 0;
}

void jsonEnd(ThkJson *json){
	fprintf(json->stream, "}");
	json->hasLastProp = 1;
}

void jsonAddStringProperty(ThkJson *json, char *key, char *val){
	if (json->hasLastProp)
		fprintf(json->stream, ",");
	fprintf(json->stream, "\"%s\":\"%s\"", key, val);
	json->hasLastProp = 1;
}

void jsonAddIntProperty(ThkJson *json, char *key, int val){
	if (json->hasLastProp)
		fprintf(json->stream, ",");
	fprintf(json->stream, "\"%s\":%d", key, val);
	json->hasLastProp = 1;
}

void jsonAddDoubleProperty(ThkJson *json, char *key, double val){
	if (json->hasLastProp)
		fprintf(json->stream, ",");
	fprintf(json->stream, "\"%s\":%g", key, val);
	json->hasLastProp = 1;
}

void jsonAddArrayProperty(ThkJson *json, char *key){
	if (json->hasLastProp)
		fprintf(json->stream, ",");
	fprintf(json->stream, "\"%s\":[", key);
	json->hasLastProp = 0;
}

void jsonCloseArrayProperty(ThkJson *json){
	fprintf(json->stream, "]");
	json->hasLastProp = 1;
}
