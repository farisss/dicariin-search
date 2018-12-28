
typedef struct DocIndexNode_t{
	int  fileSize;
	int  fileNameSize;
	char *fileNameStr;
	struct DocIndexNode_t *next;
} DocIndexNode;

typedef struct DocIndexHead_t{
	int count;
	DocIndexNode *first;
	DocIndexNode *last;
} DocIndexHead;

typedef struct DocIndexArray_t{
	int  fileSize;
	int  fileNameSize;
	char *fileNameStr;
} DocIndexArray;

typedef struct DocIndexFileNode_t{
	int indexId;
	int fileSize;
	int fileNameSize;
} DocIndexFileNode;

typedef struct DocIndexFileHead_t{
	int count;
	int arrSize;
} DocIndexFileHead;

DocIndexHead *newDocIndex();
void freeDocIndex(DocIndexHead *docIdx);
void addDocIndex(DocIndexHead *docIdx, char *fileName, int fileSize);
int  saveDocIndex(DocIndexHead *docIdx, char *outFileName);
DocIndexArray *loadDocIndexArray(int *arrCount, char *inFileName);
void freeDocIndexArray(DocIndexArray *array, int count);
