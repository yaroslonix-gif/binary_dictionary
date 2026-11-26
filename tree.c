#include "tree.h"

/////////////////////////////////////////////////////////// TREE ///////////////////////////////////////////////////////////
void InitTree(struct bin_tree* tree) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);

    tree->root = NULL;
    tree->save_str.str = NULL;
    tree->save_str.len = 0;
}

void DestroyTree(struct bin_tree* tree) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);

    _DestroySubTree(tree->root);

    tree->root = NULL;
    free(tree->save_str.str);
}

void _DestroySubTree(struct node* node) {
    if (node == NULL) return;

    if (node->left != NULL)
        _DestroySubTree(node->left);
    
    if (node->right != NULL)
        _DestroySubTree(node->right);
    
    if (node->value_own == 1)
        free(node->value);
    
    free(node);
    return;
}

/////////////////////////////////////////////////////////// Print ///////////////////////////////////////////////////////////
void PrintTreePreorder(const struct bin_tree* tree, FILE* fileout) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);
    HARD_ASSERT(fileout != NULL, NULL_ERROR);

    _PrintSubTreePreorder(tree->root, fileout);
    fprintf(fileout, "\n");
}

void PrintTreeInorder(const struct bin_tree* tree, FILE* fileout) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);
    HARD_ASSERT(fileout != NULL, NULL_ERROR);

    _PrintSubTreeInorder(tree->root, fileout);
    fprintf(fileout, "\n");
}

void PrintTreePostorder(const struct bin_tree* tree, FILE* fileout) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);
    HARD_ASSERT(fileout != NULL, NULL_ERROR);

    _PrintSubTreePostorder(tree->root, fileout);
    fprintf(fileout, "\n");
}

void _PrintSubTreePreorder(const struct node* node, FILE* fileout) {
    HARD_ASSERT(fileout != NULL, NULL_ERROR);

    if (node == NULL) {
        fprintf(fileout, "()");
        return;
    }

    fprintf(fileout, "(");

    fprintf(fileout, PRNODE, node->value);
    _PrintSubTreePreorder(node->left, fileout);
    _PrintSubTreePreorder(node->right, fileout);

    fprintf(fileout, ")");
}

void _PrintSubTreeInorder(const struct node* node, FILE* fileout) {
    HARD_ASSERT(fileout != NULL, NULL_ERROR);

    if (node == NULL) {
        fprintf(fileout, "()");
        return;
    }

    fprintf(fileout, "(");

    _PrintSubTreeInorder(node->left, fileout);
    fprintf(fileout, PRNODE, node->value);
    _PrintSubTreeInorder(node->right, fileout);

    fprintf(fileout, ")");
}

void _PrintSubTreePostorder(const struct node* node, FILE* fileout) {
    HARD_ASSERT(fileout != NULL, NULL_ERROR);

    if (node == NULL) {
        fprintf(fileout, "()");
        return;
    }

    fprintf(fileout, "(");

    _PrintSubTreePostorder(node->left, fileout);
    _PrintSubTreePostorder(node->right, fileout);
    fprintf(fileout, PRNODE, node->value);

    fprintf(fileout, ")");
}

void SaveTree(const struct bin_tree* tree) {
    TreeDump(tree, "save", NO_VIEW);

    CopyFile(SAVE_FILENAME, SAVE_FILENAME_PREV);

    FILE* fileout = fopen(SAVE_FILENAME, "w");
    HARD_VERIFY(fileout != NULL, CANT_OPEN_FILE);

    PrintTreePreorder(tree, fileout);

    if (fclose(fileout) != 0) {
        LOGG("WARNING: fclose failed on %s\n", SAVE_FILENAME);
    }
}

int CopyFile(const char* from, const char* to) {
    HARD_ASSERT(from != NULL, NULL_ERROR);
    HARD_ASSERT(to != NULL, NULL_ERROR);

    FILE* in = NULL;
    FILE* out = NULL;
    char buf[4096] = {};
    size_t n = 0;
    int ok = 1;

    in = fopen(from, "rb");
    if (in == NULL) return 0;

    out = fopen(to, "wb");
    if (out == NULL) {
        fclose(in);
        return 0;
    }

    while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
        if (fwrite(buf, 1, n, out) != n) {
            ok = 0;
            break;
        }
    }
    if (ferror(in))
        ok = 0;

    if (fclose(out) != 0)
        ok = 0;
    
    if (fclose(in) != 0)
        ok = 0;

    return ok;
}

static void _DownloadTreeBasePart(struct bin_tree* tree, char* ptr_save, int* is_ok);

static inline void _DownloadProcessOpenBracket(struct bin_tree* tree, struct node** now, int* left_empty, char** save, char* end, int* is_ok);

static inline void _DownloadProcessCloseBracket(struct node** now, int* left_empty, char** save, int* is_ok);

static inline void _DownloadProcessValue(struct node** now, char** save, char* end, int* is_ok);

void DownloadTree(struct bin_tree* tree, const char* filename) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);
    HARD_ASSERT(filename != NULL, NULL_ERROR);

    int opened = 0;

    _DownloadTreeFromFile(tree, filename, &opened);
    if (opened && tree->root != NULL) return;

    _DownloadTreeFromFile(tree, SAVE_FILENAME_PREV, &opened);
    if (opened && tree->root != NULL) return;

    LOGG("Both saves unusable, using default tree\n");
    printf("Both saves unusable, using default tree\n");

    DefaultTree(tree);
}

void _DownloadTreeFromFile(struct bin_tree* tree, const char* filename, int* opened) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);
    HARD_ASSERT(filename != NULL, NULL_ERROR);
    HARD_ASSERT(opened != NULL, NULL_ERROR);

    *opened = 0;

    FILE* source = fopen(filename, "r");
    if (source == NULL) {
        LOGG("Can't open %s\n", filename);
        return;
    }
    *opened = 1;

    char* ptr_save = read_line(source);
    fclose(source);
    if (ptr_save == NULL) {
        LOGG("%s: empty\n", filename);
        return;
    }

    tree->save_str.str = ptr_save;
    tree->save_str.len = strlen(ptr_save);

    int is_ok = 1;
    _DownloadTreeBasePart(tree, ptr_save, &is_ok);

    if (is_ok == 0 || tree->root == NULL) {
        LOGG("%s: parse failed\n", filename);
        DestroyTree(tree);
        InitTree(tree);
        return;
    }
}

static void _DownloadTreeBasePart(struct bin_tree* tree, char* ptr_save, int* is_ok) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);
    HARD_ASSERT(ptr_save != NULL, NULL_ERROR);
    HARD_ASSERT(is_ok != NULL, NULL_ERROR);

    char* save = ptr_save;
    struct node* now = NULL;
    char* end = ptr_save + tree->save_str.len;
    int left_empty = 0;

    while (save < end && *is_ok) {
        if (save[0] == '(') {
            _DownloadProcessOpenBracket(tree, &now, &left_empty, &save, end, is_ok);
            continue;
        }

        if (save[0] == ')') {
            _DownloadProcessCloseBracket(&now, &left_empty, &save, is_ok);

            continue;
        }

        _DownloadProcessValue(&now, &save, end, is_ok);
    }
}

static inline void _DownloadProcessOpenBracket(struct bin_tree* tree, struct node** now, int* left_empty, char** save, char* end, int* is_ok) {
    HARD_ASSERT(is_ok != NULL, NULL_ERROR);
    if (!*is_ok) return;
    
    if ((*save) + 1 < end && (*save)[1] == ')') {
        if ((*now) != NULL) {
            if ((*now)->left == NULL && !(*left_empty)) {
                (*left_empty) = 1;
            }
        }
        (*save) += 2;
        return;
    }

    if (tree->root == NULL) {
        tree->root = (struct node*)calloc(1, sizeof(struct node));
        HARD_VERIFY(tree->root != NULL, NULL_ERROR);
        (*now) = tree->root;
        (*now)->parent = NULL;

    } else {
        if ((*now) == NULL) {
            LOGG("WRONG save format: node is NULL on open bracket\n");
            *is_ok = 0;
            return;
        }

        struct node* new_node = (struct node*)calloc(1, sizeof(struct node));
        HARD_VERIFY(new_node != NULL, NULL_ERROR);

        if ((*now)->left == NULL && !(*left_empty)) {
            (*now)->left = new_node;
        } else if ((*now)->right == NULL) {
            (*now)->right = new_node;
        } else {
            LOGG("WRONG save format: node already has two children\n");
            free(new_node);
            *is_ok = 0;
            return;
        }

        new_node->parent = (*now);
        (*left_empty) = 0;
        (*now) = new_node;
    }

    (*save) += 1;
}

static inline void _DownloadProcessCloseBracket(struct node** now, int* left_empty, char** save, int* is_ok) {
    HARD_ASSERT(is_ok != NULL, NULL_ERROR);
    if (!*is_ok) return;
    
    if (*now == NULL) {
        LOGG("WRONG save format: unmatched ')'\n");
        *is_ok = 0;
        return;
    }
    *left_empty = ((*now)->parent != NULL && (*now)->parent->left == *now);
    *now = (*now)->parent;
    *save += 1;
}

static inline void _DownloadProcessValue(struct node** now, char** save, char* end, int* is_ok) {
    HARD_ASSERT(is_ok != NULL, NULL_ERROR);
    if (!*is_ok) return;

    char* close = memchr(*save, '>', (size_t)(end - *save));
    if (close == NULL) {
        LOGG("WRONG save format: missing '>'\n");
        *is_ok = 0;
        return;
    }

    if (*now == NULL || (*now)->value != NULL) {
        LOGG("WRONG save format: unexpected value\n");
        *is_ok = 0;
        return;
    }

    (*now)->value = *save + 1;
    (*now)->value_own = 0;
    *close = '\0';

    *save = close + 1;
}

void DefaultTree(struct bin_tree* tree) {
    HARD_VERIFY(tree != NULL, NULL_ERROR);

    tree->root = (struct node*)calloc(1, sizeof(struct node));
    HARD_VERIFY(tree->root != NULL, NULL_ERROR);

    char* value = (char*)calloc(100, sizeof(char));
    snprintf(value, 100, "neizvestnyy obekt");
    tree->root->value = value;
    tree->root->value_own = 1;
}

/////////////////////////////////////////////////////////// Dump ///////////////////////////////////////////////////////////
void TreeDump(const struct bin_tree* tree, const char* message, const int view_flag) {
    HARD_ASSERT(message != NULL, NULL_ERROR);

    LOGG(message);
    time_t now_time = 0;
    struct tm* time_info = NULL;

    time(&now_time);
    time_info = localtime(&now_time);
    clock_t tik_now = clock();

    char file_name[BASE_STR_LEN] = "";
    strftime(file_name, BASE_STR_LEN, "dump%d_%m_%Y_%H_%M_%S_", time_info);
    snprintf(file_name + strlen(file_name), BASE_STR_LEN - strlen(file_name), "%llu", tik_now);
    int have_err = TreeGraphicDump(tree, file_name);

    if (view_flag == YES_VIEW) {
        char sys_cmd[2 * BASE_STR_LEN] = "";
        char path[BASE_STR_LEN] = {};
        _getcwd(path, BASE_STR_LEN);
        snprintf(sys_cmd, 2 * BASE_STR_LEN, "start \"\" \"%s\\%s\\%s.png\"", path, DIR_NAME, file_name);
        system(sys_cmd);
    }

    if (have_err == HAVE_ERROR) {
        abort();
    }
}

int TreeGraphicDump(const struct bin_tree* tree, const char* file_name) {
    HARD_ASSERT(file_name != NULL, NULL_ERROR);

    if (tree == NULL) {
        LOGG("NULL_TREE: TREE is NULL\n");
        return HAVE_ERROR;
    }

    int have_err = 0;

    LOGG("<table border=\"1\">\n");
    LOGG("<tr><td>root</td><td>%p</td></tr>\n", tree->root);
    LOGG("</table>\n");

    if (tree->root == NULL) return have_err;
    
    char file_dot[BASE_STR_LEN] = "";
    sprintf(file_dot, "%s\\%s.dot", DIR_NAME, file_name);

    FILE* dump_out = fopen(file_dot, "w");
    HARD_VERIFY(dump_out != NULL, CANT_OPEN_FILE);

    fprintf(dump_out, "digraph {\n");
    fprintf(dump_out, "  rankdir = TB;\n  node[shape = \"Mrecord\", style = filled, fontsize = 12];\n");
    fprintf(dump_out, "  graph[nodesep = 1];\n"); //splines = ortho, 
    fprintf(dump_out, "  edge[radius = 5];\n");
    
    _TreeBasePart(tree, dump_out, &have_err);

    fprintf(dump_out, "}\n");
    fclose(dump_out);

    char sys_cmd[200] = "";
    sprintf(sys_cmd, GRAPHVIZ_DOT_PATH " -Tpng %s\\%s.dot -o %s\\%s.png", DIR_NAME, file_name, DIR_NAME, file_name);
    system(sys_cmd);

    LOGG("<img src=\"%s\\%s.png\">\n", DIR_NAME, file_name);
    
    return have_err;
}

void SetDumpNode(FILE* dump_out, const struct node* node, const char* color, const char* fillcolor) {
    HARD_ASSERT(dump_out != NULL, NULL_ERROR);
    HARD_ASSERT(node != NULL, NULL_ERROR);
    HARD_ASSERT(color != NULL, NULL_ERROR);
    HARD_ASSERT(fillcolor != NULL, NULL_ERROR);

    //fprintf(dump_out, "  NODE%p[color = \"%s\", fillcolor = \"%s\", label=\"{ node = 0x%p | left = 0x%p | right = 0x%p | parent = 0x%p | val = " PRNODE_FOR_GR " }\"];\n", 
    //        node, color, fillcolor, node, node->left, node->right, node->parent, node->value);
    fprintf(dump_out, "  NODE%p[color = \"%s\", fillcolor = \"%s\", label=\"{ " PRNODE_FOR_GR " }\"];\n", 
            node, color, fillcolor, node->value);
}

void _TreeBasePart(const struct bin_tree* tree, FILE* dump_out, int* have_err) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);
    HARD_ASSERT(dump_out != NULL, NULL_ERROR);
    HARD_ASSERT(have_err != NULL, NULL_ERROR);

    _DumpSubTree(tree->root, dump_out);

    fprintf(dump_out, "  ROOT[color = \"#c08000\", fillcolor = \"#FFC000\", label=\"ROOT\"];\n");
    fprintf(dump_out, "  edge[color = \"#c08000\", weight = 5, dir = forward];\n");
    fprintf(dump_out, "  ROOT->NODE%p;\n", tree->root);
}

void _DumpSubTree(const struct node* node, FILE* dump_out) {
    HARD_ASSERT(dump_out != NULL, NULL_ERROR);
    if (node == NULL) return;

    SetDumpNode(dump_out, node, "#008000", "#C0FFC0");
    if (node->left != NULL) {
        fprintf(dump_out, "NODE%p->NODE%p[color = \"#0000b0\", weight = 5, dir = forward, label=\"no\", fontcolor = \"#0000b0\"];\n", node, node->left);
        _DumpSubTree(node->left, dump_out);
    }

    if (node->right != NULL) {
        fprintf(dump_out, "NODE%p->NODE%p[color = \"#ff8000\", weight = 5, dir = forward, label=\"yes\", fontcolor = \"#ff8000\"];\n", node, node->right);
        _DumpSubTree(node->right, dump_out);
    }

    // if (node->parent != NULL) {
    //     fprintf(dump_out, "NODE%p->NODE%p[style=dashed, color = \"#000000\", weight = 0, dir = forward];\n", node, node->parent);
    // }
}

uint64_t GetNodeDepth(const struct node* node) {
    HARD_ASSERT(node != NULL, NULL_ERROR);
    uint64_t depth = 0;

    while (node->parent != NULL) {
        depth += 1;
        node = node->parent;
    }

    return depth;
}


int CreateDir(const char* dir_name) {
    HARD_ASSERT(dir_name != NULL, NULL_ERROR);

    if (_mkdir(dir_name) == 0)
        return 0;
    
    if (errno == EEXIST)
        return 1;

    LOGG("ERROR of make dir\n");
    return -1;
}

const char DIR_NAME[] = {'d', 'u', 'm', 'p', __TIME__[0], __TIME__[1], __TIME__[3], __TIME__[4], __TIME__[6], __TIME__[7], '\0'};