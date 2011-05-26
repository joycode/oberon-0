#include <cstdio>
#include <hash_map>
#include "Scanner.h"
#include "Parser.h"
#include "ASTNode.h"

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "oberon <SourceFile>\n");
		return -1;
	}

	FILE *fp = NULL;
	if (fopen_s(&fp, argv[1], "r") != 0) {
		fprintf(stderr, "open file failed: %s\n", argv[1]);
		return -1;
	}

	Parser parser;
	ASTNode *result = parser.parse(fp);
	if (result == NULL) {
		fprintf(stderr, "parse failed\n");
	}
	else {
		fprintf(stderr, "parse successful\n");
	}

	getchar();

	return 0;
}
