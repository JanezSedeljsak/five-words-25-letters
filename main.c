#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

const char *order_chars = "qxjzvfwbkgpmhdcytlnuroisea";
int order[26];

typedef struct {
    uint32_t mask;
    char bytes[5];
} Word;

void solve(int *unique_words_count, int *unique_sets_count) {
    int fd = open("words_alpha.txt", O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    char *data = (char *)mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    Word *masks = (Word *)malloc(10000 * sizeof(Word));
    int masks_count = 0;
    uint8_t *seen = (uint8_t *)calloc(1 << (26 - 3), 1);

    char *ptr = data;
    char *end = data + st.st_size;
    while (ptr < end) {
        char *line_end = ptr;
        while (line_end < end && *line_end != '\n' && *line_end != '\r') line_end++;
        int len = (int)(line_end - ptr);
        if (len == 5) {
            uint32_t m = 0;
            int distinct = 0;
            for (int i = 0; i < 5; i++) {
                uint32_t bit = 1 << (ptr[i] - 'a');
                if (!(m & bit)) distinct++;
                m |= bit;
            }
            if (distinct == 5) {
                if (!(seen[m >> 3] & (1 << (m & 7)))) {
                    seen[m >> 3] |= (1 << (m & 7));
                    masks[masks_count].mask = m;
                    memcpy(masks[masks_count].bytes, ptr, 5);
                    masks_count++;
                }
            }
        }
        ptr = line_end;
        while (ptr < end && (*ptr == '\n' || *ptr == '\r')) ptr++;
    }

    uint32_t **ltm = (uint32_t **)malloc(26 * sizeof(uint32_t *));
    int *ltm_counts = (int *)calloc(26, sizeof(int));
    int *ltm_caps = (int *)malloc(26 * sizeof(int));
    for (int i = 0; i < 26; i++) {
        ltm_caps[i] = 1024;
        ltm[i] = (uint32_t *)malloc(ltm_caps[i] * sizeof(uint32_t));
    }

    for (int i = 0; i < masks_count; i++) {
        uint32_t m = masks[i].mask;
        for (int j = 0; j < 26; j++) {
            if ((m >> j) & 1) {
                if (ltm_counts[j] == ltm_caps[j]) {
                    ltm_caps[j] *= 2;
                    ltm[j] = (uint32_t *)realloc(ltm[j], ltm_caps[j] * sizeof(uint32_t));
                }
                ltm[j][ltm_counts[j]++] = m;
            }
        }
    }

    char (*mask_to_bytes)[5] = (char (*)[5])calloc(1 << 26, 5);
    for (int i = 0; i < masks_count; i++) {
        memcpy(mask_to_bytes[masks[i].mask], masks[i].bytes, 5);
    }

    FILE *out = fopen("c_out.txt", "w");
    int sets_count = 0;

    for (int m_l = 0; m_l < 26; m_l++) {
        uint32_t target = ((1U << 26) - 1) ^ (1U << m_l);
        struct { uint32_t cur; int b_idx; } stack[6];
        stack[0].cur = 0;
        stack[0].b_idx = 0;
        int d = 0;

        while (d < 5) {
            uint32_t cur = stack[d].cur;
            int b_idx = stack[d].b_idx;
            int r_idx = 0;
            while (r_idx < 26) {
                int l = order[r_idx];
                if (((target >> l) & 1) && !((cur >> l) & 1)) break;
                r_idx++;
            }

            if (r_idx == 26) {
                if (d == 0) break;
                d--;
                stack[d].b_idx++;
                continue;
            }

            int letter = order[r_idx];
            uint32_t *bucket = ltm[letter];
            int b_count = ltm_counts[letter];
            int found = 0;
            for (int i = b_idx; i < b_count; i++) {
                uint32_t m = bucket[i];
                if (!(m & ~target) && !(cur & m)) {
                    stack[d].b_idx = i;
                    d++;
                    stack[d].cur = cur | m;
                    stack[d].b_idx = 0;
                    if (d == 5) {
                        for (int x = 1; x <= 5; x++) {
                            uint32_t m_sub = stack[x].cur ^ stack[x-1].cur;
                            fwrite(mask_to_bytes[m_sub], 1, 5, out);
                            fputc(' ', out);
                        }
                        fputc('\n', out);
                        sets_count++;
                        d--;
                        stack[d].b_idx++;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (d == 0) break;
                d--;
                stack[d].b_idx++;
            }
        }
    }

    fclose(out);
    free(mask_to_bytes);
    *unique_words_count = masks_count;
    *unique_sets_count = sets_count;

    for (int i = 0; i < 26; i++) free(ltm[i]);
    free(ltm); free(ltm_counts); free(ltm_caps);
    free(masks); free(seen);
    munmap(data, st.st_size);
    close(fd);
}

int main() {
    for (int i = 0; i < 26; i++) order[i] = order_chars[i] - 'a';

    int unique_words = 0, unique_sets = 0;
    struct timespec start, end;
    long long total_ns = 0;

    for (int i = 0; i < 5; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        solve(&unique_words, &unique_sets);
        clock_gettime(CLOCK_MONOTONIC, &end);
        total_ns += (long long)(end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
    }

    printf("Total time:   %.6fms\n", (total_ns / 5.0) / 1000000.0);
    printf("Unique words: %d\n", unique_words);
    printf("Unique sets:  %d\n", unique_sets);

    return 0;
}
