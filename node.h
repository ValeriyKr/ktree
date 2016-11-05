#ifndef _NODE_H_
#define _NODE_H_

#define NODE_DIR 0x1
#define NODE_REG 0x2
#define NODE_ERR 0x4

typedef struct node_t {
    char *name;
    struct node_t **subnodes;
    size_t subnodes_count;
    unsigned int flags;
} node_t;

node_t* make_tree(const char *path, size_t *total_files, size_t *total_dirs);
void delete_tree(node_t *root);

#endif /* _NODE_H_ */
