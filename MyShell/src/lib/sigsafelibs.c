#include "sigsafelibs.h"


void unix_error(char msg[]) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}


void* sigsafe_malloc(size_t size) {
    void* m = NULL;
    int t = MAX_TRIAL;
    while (t-- && (m = malloc(size)) == NULL);
    if (t == 0) {
        unix_error("sigsafe_malloc failed");
    }
    return m;
}

void* sigsafe_calloc(size_t num, size_t size) {
    void* m = NULL;
    int t = MAX_TRIAL;
    while (t-- && (m = calloc(num, size)) == NULL);
    if (t == 0) {
        unix_error("sigsafe_calloc failed");
    }
    return m;
}


void* sigsafe_realloc(void* ptr, size_t new_size) {
    void* m = NULL;
    int t = MAX_TRIAL;
    while (t-- && (m = realloc(ptr, new_size)) == NULL);
    if (t == 0) {
        unix_error("sigsafe_realloc failed");
    }
    return m;
}

