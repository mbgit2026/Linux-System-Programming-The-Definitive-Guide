#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <acl/libacl.h>
#include <sys/acl.h>
#include <errno.h>
#include <unistd.h>

void print_acl(acl_t acl) {
    char *text = acl_to_text(acl, NULL);
    if (text) {
        printf("ACL: \n%s\n", text);
        acl_free(text);
    } else {
        perror("acl_to_text");
    }
}

void get_acl(const char *filename) {
    acl_t acl = acl_get_file(filename, ACL_TYPE_ACCESS);
    if (acl == NULL) {
        perror("acl_get_file");
        return;
    }

    print_acl(acl);
    acl_free(acl);
}

void modify_acl(const char *filename, const char *param) {
    acl_t acl = acl_get_file(filename, ACL_TYPE_ACCESS);
    if (acl == NULL) {
        perror("acl_get_file");
        return;
    }

    acl_entry_t entry;
    if (acl_create_entry(&acl, &entry) == -1) {
        perror("acl_create_entry");
        acl_free(acl);
        return;
    }

    acl_tag_t tag;
    char entity[16];
    int id;
    char perms[4];

    if (sscanf(param, "%15[^:]:%d:%3s", entity, &id, perms) != 3) {
        fprintf(stderr, "Invalid parameter format. Expected format: user:uid:rwx\n");
        acl_free(acl);
        return;
    }

    acl_permset_t permset;
    acl_get_permset(entry, &permset);
    acl_clear_perms(permset);

    for (int i = 0; perms[i]; i++) {
        switch (perms[i]) {
            case 'r': acl_add_perm(permset, ACL_READ); break;
            case 'w': acl_add_perm(permset, ACL_WRITE); break;
            case 'x': acl_add_perm(permset, ACL_EXECUTE); break;
        }
    }

    if (strcmp(entity, "user") == 0) {
        acl_set_tag_type(entry, ACL_USER);
        acl_set_qualifier(entry, &id);
    } else if (strcmp(entity, "group") == 0) {
        acl_set_tag_type(entry, ACL_GROUP);
        acl_set_qualifier(entry, &id);
    } else {
        fprintf(stderr, "Invalid entity type: %s\n", entity);
        acl_free(acl);
        return;
    }

    if (acl_set_permset(entry, permset) == -1) {
        perror("acl_set_permset");
    }

    if (acl_valid(acl) != 0) {
        perror("acl_valid");
    }

    if (acl_set_file(filename, ACL_TYPE_ACCESS, acl) == -1) {
        perror("acl_set_file");
    }

    acl_free(acl);
    printf("ACL modified successfully.\n");
}

void delete_acl(const char *filename, const char *param) {
    acl_t acl = acl_get_file(filename, ACL_TYPE_ACCESS);
    if (acl == NULL) {
        perror("acl_get_file");
        return;
    }

    acl_entry_t entry;
    int entry_id = ACL_FIRST_ENTRY;
    while (acl_get_entry(acl, entry_id, &entry) == 1) {
        entry_id = ACL_NEXT_ENTRY;

        acl_tag_t tag_type;
        void *qual = acl_get_qualifier(entry);
        acl_get_tag_type(entry, &tag_type);

        int id;
        char entity[16];
        char perms[4];
        sscanf(param, "%15[^:]:%d:%3s", entity, &id, perms);

        if ((strcmp(entity, "user") == 0 && tag_type == ACL_USER && *((uid_t *)qual) == id) ||
            (strcmp(entity, "group") == 0 && tag_type == ACL_GROUP && *((gid_t *)qual) == id)) {

            if (acl_delete_entry(acl, entry) == -1) {
                perror("acl_delete_entry");
            } else {
                printf("ACL entry deleted.\n");
            }
            acl_free(qual);
            break;
        }

        acl_free(qual);
    }

    if (acl_set_file(filename, ACL_TYPE_ACCESS, acl) == -1) {
        perror("acl_set_file");
    }

    acl_free(acl);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "\nUsage: %s <get|modify|delete> <filename> <param>\n", argv[0]);
        fprintf(stderr, "Param format: user:<uid>:<permissions> | group:<gid>:<permissions>\n");
        exit(EXIT_FAILURE);
    }

    const char *operation = argv[1];
    const char *filename = argv[2];
    const char *param = argv[3];

    if (strcmp(operation, "get") == 0) {
        get_acl(filename);
    } else if (strcmp(operation, "modify") == 0) {
        modify_acl(filename, param);
    } else if (strcmp(operation, "delete") == 0) {
        delete_acl(filename, param);
    } else {
        fprintf(stderr, "Invalid operation: %s\n", operation);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
