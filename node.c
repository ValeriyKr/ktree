/* Made by ValeriyKr */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include "node.h"


static int node_cmp(const void *a, const void *b) {
    node_t *n1 = *((node_t**) a);
    node_t *n2 = *((node_t**) b);
    return NULL == n1 ? (
               NULL == n2 ?
                   0 : -1
               ) : (NULL == n2 ? 1 : strcmp(n1->name, n2->name));
}


/* global variables help to avoid frequent malloc() and heap fragmentation */
static char *fname;
static size_t fname_capacity;

node_t* make_tree(const char *path, size_t *total_files, size_t *total_dirs) {
    if (NULL == path) return NULL;

    size_t path_len;
    if (0 == (path_len = strlen(path))) return NULL;

    node_t *root = (node_t*) malloc(sizeof(node_t));
    if (NULL == root) return NULL;
    if (NULL == (root->name = (char*) malloc(path_len + 2))) return NULL;

    strcpy(root->name, path);
    if (root->name[path_len-1] != '/') {
        root->name[path_len++] = '/';
        root->name[path_len] = '\0';
    }
    root->subnodes_count = 0;
    root->flags = NODE_DIR;

    DIR *dir = opendir(root->name);
    if (NULL == dir) {
        root->flags |= NODE_ERR;
        root->subnodes = NULL;
        return root;
    }
    if (NULL == (root->subnodes = (node_t**) malloc(sizeof(node_t*)*256))) {
        delete_tree(root);
        return NULL;
    }

    struct dirent *dp;
    while (NULL != (dp = readdir(dir))) {
        if (dp->d_name[0] == '.') continue;
        size_t fname_len = strlen(dp->d_name) + path_len + 1;
        if (fname_capacity < fname_len) {
            char *buf = realloc(fname, fname_len*2);
            if (NULL == buf) {
                delete_tree(root);
                return NULL;
            }
            fname = buf;
            fname_capacity = fname_len*2;
        }
        strcpy(fname, root->name);
        strcat(fname, dp->d_name);
        struct stat st;
        if (0 != lstat(fname, &st)) {
            delete_tree(root);
            return NULL;
        }
    if (S_ISDIR(st.st_mode)) {
            // Directory
            if (NULL == (root->subnodes[root->subnodes_count++] =
                         make_tree(fname, total_files, total_dirs))) {
                delete_tree(root);
                return NULL;
            }
            (*total_dirs)++;
        } else {
            // Regular file
            if (NULL == (root->subnodes[root->subnodes_count] = 
                         (node_t*) malloc(sizeof(node_t))) ||
                NULL == (root->subnodes[root->subnodes_count]->name = 
                         (char*) malloc(strlen(dp->d_name)))) {
                delete_tree(root);
                return NULL;
            }
            strcpy(root->subnodes[root->subnodes_count]->name, dp->d_name);
            root->subnodes[root->subnodes_count]->subnodes = NULL;
            root->subnodes[root->subnodes_count]->subnodes_count = 0;
            root->subnodes[root->subnodes_count]->flags = NODE_REG;
            root->subnodes_count++;
            (*total_files)++;
        }
    }
    qsort(root->subnodes, root->subnodes_count, sizeof(node_t*), node_cmp);
    closedir(dir);
    return root;
}


void delete_tree(node_t *root) {
    free(fname);
    fname = NULL;
    if (NULL == root) return;
    for (size_t i = 0; i < root->subnodes_count; ++i)
        delete_tree(root->subnodes[i]);
    free(root->subnodes);
    free(root);
}
