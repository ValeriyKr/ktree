/* Made by ValeriyKr */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#define NODE_DIR 0x1
#define NODE_REG 0x2

typedef struct node_t {
    char *name;
    struct node_t **subnodes;
    size_t subnodes_count;
    unsigned int flags;
} node_t;


int ncmp(const void *a, const void *b) {
    node_t *n1 = *((node_t**) a);
    node_t *n2 = *((node_t**) b);
    return strcmp(n1->name, n2->name);
    if ((n1->flags & NODE_DIR) && !(n2->flags & NODE_DIR)) return -1;
    if ((n2->flags & NODE_DIR) && !(n1->flags & NODE_DIR)) return 1;
    return n1>n2;
}


inline static int fill_node(node_t *node, const char *name,
                            node_t **subnodes, size_t subnodes_count,
                            unsigned int flags) {
    if (NULL == (node->name = (char*) malloc(strlen(name)))) return 1;
    strcpy(node->name, name);
    node->subnodes = subnodes;
    node->subnodes_count = subnodes_count;
    node->flags = flags;
    return 0;
}


node_t* make_tree(const char *path) {
    DIR *curr;
    size_t path_len;
    if (0 == (path_len = strlen(path))) return 0;
    if (NULL == (curr = opendir(path))) {
        perror(path);
        return NULL;
    }

    node_t *node = (node_t*) malloc(sizeof(node_t));
    if (NULL == node) return NULL;
    node->name = (char*) malloc(path_len + 2);
    strcpy(node->name, path);
    if (path[path_len-1] != '/') {
        strcat(node->name, "/");
        path_len++;
    }
    if (NULL == (node->subnodes = (node_t**) malloc(sizeof(node_t*)*2056))) {
        free(node);
        return NULL;
    }
    node->subnodes_count = 0;
    node->flags = NODE_DIR;

    struct dirent *dp;
    while (NULL != (dp = readdir(curr))) {
        if (!(dp->d_type & DT_DIR)) {
            if (NULL == (node->subnodes[node->subnodes_count] = (node_t*) malloc(sizeof(node_t))) ||
                fill_node(node->subnodes[node->subnodes_count++], dp->d_name,
                          NULL, 0, NODE_REG)) {
                free(node);
                return NULL;
            }
        } else {
            if (!strcmp(dp->d_name, "..") || !strcmp(dp->d_name, ".")) continue;
            char *subdir = (char*) malloc(path_len + strlen(dp->d_name) + 1);
            if (NULL == subdir) {
                free(node);
                return NULL;
            }
            strcpy(subdir, node->name);
            strcat(subdir, dp->d_name);
            node->subnodes[node->subnodes_count++] = make_tree(subdir);
            free(subdir);
        }
    }
    closedir(curr);
    qsort(node->subnodes, node->subnodes_count, sizeof(node_t*), ncmp);
    return node;
}


static const char * basename(const char *name) {
    const char *end = name+strlen(name)-1;
    while (end != name && *(--end) != '/');
    return end == name ? end : end+1;
}


void nputs(const node_t *node, size_t deep) {
    if (NULL == node) return;
    puts(basename(node->name));
    for (size_t i = 0; i < node->subnodes_count; ++i) {
        for (size_t j = 0; j < deep; ++j)
            printf("│   ");
        if (i+1 == node->subnodes_count)
            printf("└── ");
        else
            printf("├── ");
        nputs(node->subnodes[i], deep+1);
    }
}


int main(int argc, char **argv) {
    node_t *root;
    if (1 == argc) {
        root = make_tree(".");
    } else {
        for (int i = 1; i < argc; ++i)
            root = make_tree(argv[i]);
    }
    nputs(root, 0);

    return 0;
}
