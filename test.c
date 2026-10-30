#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void say(const char *s) { write(1, s, strlen(s)); }

int main() {
    char *a = malloc(16);
    strcpy(a, "hello, cmalloc\n");
    say(a);

    a = realloc(a, 200);
    say(a);

    int *v = calloc(5, sizeof(int));
    say(v[0] == 0 && v[4] == 0 ? "calloc has been reduced to zero\n" : "calloc failed\n");

    free(a);
    free(v);
    say("ended normally\n");
    
    return 0;
}
