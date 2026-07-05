#include <unistd.h>

int main()
{
	truncate ("file.txt", 45);
	return 0;
}
