#include <stdlib.h>

typedef struct ThkJson_t {
	FILE *stream;
	int hasLastProp;
} ThkJson;

ThkJson *initJson(FILE *stream);
void freeJson(ThkJson *json);

void jsonBegin(ThkJson *json);
void jsonEnd(ThkJson *json);
void jsonAddStringProperty(ThkJson *json, char *key, char *val);
void jsonAddIntProperty(ThkJson *json, char *key, int val);
void jsonAddDoubleProperty(ThkJson *json, char *key, double val);
void jsonAddArrayProperty(ThkJson *json, char *key);
void jsonCloseArrayProperty(ThkJson *json);
