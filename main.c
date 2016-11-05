/* Made by ValeriyKr */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include "node.h"


static const char * basename(const char *name) {
    const char *end = name+strlen(name)-1;
    while (end != name && *(--end) != '/');
    return end == name ? end : end+1;
}


void nputs(const node_t *node, size_t deep) {
    if (NULL == node) return;
    if (node->flags & NODE_ERR) {
        printf("%s [error while opening]\n", basename(node->name));
    } else {
        puts(basename(node->name));
    }
    for (size_t i = 0; i < node->subnodes_count; ++i) {
        for (size_t j = 0; j < deep; ++j)
            printf("│   ");
        if (i+1 == node->subnodes_count)
            printf("└── ");
        else
            printf("├── ");
        fflush(stdout);
        nputs(node->subnodes[i], deep+1);
    }
}


int main(int argc, char **argv) {
    node_t *root;
    size_t total_dirs = 0, total_files = 0;
    if (1 == argc) {
        root = make_tree(".", &total_files, &total_dirs);
    } else {
        for (int i = 1; i < argc; ++i)
            root = make_tree(argv[i], &total_files, &total_dirs);
    }
    if (NULL == root) {
        perror("Error while making tree");
        return -1;
    }
    nputs(root, 0);
    delete_tree(root);
    printf("\n%lu directories, %lu files\n", total_dirs, total_files);

    return 0;
}
