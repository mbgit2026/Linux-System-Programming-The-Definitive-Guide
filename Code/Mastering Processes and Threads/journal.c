#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-journal.h>

int main(void) {
    sd_journal *j;
    int r;

    /* Open the journal (local logs) */
    r = sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY);
    if (r < 0) {
        fprintf(stderr, "Failed to open journal: %d\n", r);
        return 1;
    }

    /* Start from the beginning */
    sd_journal_seek_head(j);

    /* Iterate through entries */
    while (sd_journal_next(j) > 0) {
        const void *data;
        size_t length;

        /* Read the MESSAGE field */
        r = sd_journal_get_data(j, "MESSAGE", &data, &length);
        if (r == 0) {
            /* data is in the form "FIELD=value" */
            printf("%.*s\n", (int)length, (const char *)data);
        }
    }

    /* Close journal */
    sd_journal_close(j);

    return 0;
}