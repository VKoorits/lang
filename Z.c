#include <stdio.h>

int main () {
	FILE* f = fopen("example.lc", "rb");
	fseek(f, 68, SEEK_SET);
	char c;
	fread(&c, 1, 1, f);
	printf("%d\n", c);
}
