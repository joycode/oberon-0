#include <hash_map>
#include "Scanner.h"
#include "Parser.h"


void testPointer2(Scanner **ret)
{
	*ret = new Scanner(NULL);
}

int main(int argc, char **argv)
{
	Scanner *scan1 = NULL;
	Scanner **scan2 = NULL;

	testPointer2(&scan1);
	testPointer2(scan2);

	return 0;
}
