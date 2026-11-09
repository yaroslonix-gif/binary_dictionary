#include "bin_tree.h"

int main() {
    START_LOGG;
    LOGG("<pre>\n");
    LOGG("<head><style>table {border-collapse: collapse;background-color: #e6f2ff;font-family: Arial, sans-serif;} th, td {border: 1px solid #b3d7ff;padding: 12px; text-align: center;}th {background-color: #cbdff7; font-weight: bold;}</style></head>\n");
    HARD_VERIFY(_mkdir(DIR_NAME) != -1, CANT_MAKE_DIR);

    struct bin_tree tree = {};
    InitTree(&tree);
    DownloadTree(&tree, "treesave.txt");
    TreeDump(&tree, "dump", NO_VIEW);
    PrintTreeInorder(&tree, stdout);

    RunBinDirectory(&tree);

    PrintTreeInorder(&tree, stdout);
    SaveTree(&tree);
    TreeDump(&tree, "dump", NO_VIEW);
    STOP_LOGG;
    return 0;
}

/////////////////////////////////////////////////////////// BinDirectory ///////////////////////////////////////////////////////////
void RunBinDirectory(struct bin_tree* tree) {
    while(1) {
        printf("Chose comand: [v]view or [f]find or [a]add or [e]exit\n");
        switch (WaitComand()){
        case VIEW_MOD:
            TreeDump(tree, "view", YES_VIEW);
            break;

        case FIND_MOD:
            FindElementInTree(tree);
            break;

        case ADD_MOD:
            AddObjInTree(tree);
            break;

        case EXIT_MOD:
            return;

        default:
            break;
        }
    }
}

int WaitComand() {
    char str[10] = {};

    for (;;) {
        if (scanf("%9s", str) != 1) return 0;

        SkipForEOS(stdin);

        if (strlen(str) == 1 && (str[0] == VIEW_MOD || str[0] == FIND_MOD || str[0] == ADD_MOD || str[0] == EXIT_MOD))
            return str[0];
        
        printf("Wrong input, try again\n");
    }
}

int WaitYesOrNo() {
    char str[10] = {};

    for (;;) {
        if (scanf("%9s", str) != 1) return 0;

        SkipForEOS(stdin);

        if (strlen(str) == 1 && (str[0] == 'y' || str[0] == 'n'))
            return (str[0] == 'y');
        
        printf("Wrong input, try again\n");
    }
}

void FindElementInTree(struct bin_tree* tree) {
    if (tree == NULL) return;

    _FindElement(tree->root);
}

void _FindElement(struct node* node) {
    if (node == NULL) return;

    printf("It is %s? (Answer: [y]yes or [n]no)\n", node->value);
    int ans = WaitYesOrNo();

    if (node->left != NULL && node->right != NULL) {
        if (ans) {
            _FindElement(node->right);
            return;
        } else {
            _FindElement(node->left);
            return;
        }
    }

    if (node->left == NULL && node->right == NULL) {
        if (ans) {
            printf("I guess\n");
            return;
        } else {
            printf("I don't know what is it\n");
            struct node* ch_node = node;
            struct node* wrong_node = (struct node*)calloc(1, sizeof(struct node));
            HARD_VERIFY(wrong_node != NULL, NULL_ERROR);
            struct node* correct_node = (struct node*)calloc(1, sizeof(struct node));
            HARD_VERIFY(correct_node != NULL, NULL_ERROR);

            ch_node->left = wrong_node;
            ch_node->right = correct_node;
            wrong_node->parent = ch_node;
            correct_node->parent = ch_node;
            wrong_node->value = node->value;

            printf("It is: ");
            char* str_obj_name = read_line(stdin);
            correct_node->value = str_obj_name;

            printf("What is the difference between %s and %s:\n", str_obj_name, wrong_node->value);
            printf("%s is: ", str_obj_name);
            char* str_obj_ch = read_line(stdin);
            ch_node->value = str_obj_ch;

            return;
        }
    }
    
    if (ans == 1 && node->right == NULL) {
        printf("I don't know what is it\n");
        printf("It is: ");

        char* str = read_line(stdin);
        node->right = (struct node*)calloc(1, sizeof(struct node));
        HARD_VERIFY(node->right != NULL, NULL_ERROR);
        node->right->value = str;
        node->right->parent = node;

        return;
    }

    if (ans == 0 && node->left == NULL) {
        printf("I don't know what is it\n");
        printf("It is: ");

        char* str = read_line(stdin);
        node->left = (struct node*)calloc(1, sizeof(struct node));
        HARD_VERIFY(node->left != NULL, NULL_ERROR);
        node->left->value = str;
        node->left->parent = node;

        return;
    }
}

void AddObjInTree(struct bin_tree* tree) {
    printf("What object do you want to add? (Enter it's name)\n");
    printf("The new object is: ");
    char* new_obj_name = read_line(stdin);
    printf("With witch object do you want to compare %s? (Enter it's name)\n", new_obj_name);
    printf("The object is: ");
    char* old_obj_name = read_line(stdin);
    struct node* old_obj = _FindListByName(tree->root, old_obj_name);
    if (old_obj == NULL) {
        printf("There is no object named %s in database\n", old_obj_name);
        return;
    }

    printf("What is the difference between %s and %s:\n", new_obj_name, old_obj_name);
    printf("%s is: ", new_obj_name);
    char* str_obj_ch = read_line(stdin);
    
    struct node* ch_node = old_obj;
    struct node* wrong_node = (struct node*)calloc(1, sizeof(struct node));
    HARD_VERIFY(wrong_node != NULL, NULL_ERROR);
    struct node* correct_node = (struct node*)calloc(1, sizeof(struct node));
    HARD_VERIFY(correct_node != NULL, NULL_ERROR);

    ch_node->left = wrong_node;
    ch_node->right = correct_node;
    wrong_node->parent = ch_node;
    correct_node->parent = ch_node;
    wrong_node->value = old_obj_name;
    correct_node->value = new_obj_name;
    ch_node->value = str_obj_ch;
}

struct node* _FindListByName(struct node* node, const node_elem_t obj_name) {
    if (node == NULL) return NULL;

    if (node->left == NULL && node->left == NULL) {
        if (strcmp(node->value, obj_name) == 0) {
            return node;
        }
        return NULL;
    }
    struct node* obj = _FindListByName(node->left, obj_name);
    if (obj != NULL) return obj;

    obj = _FindListByName(node->right, obj_name);
    return obj;
}

/////////////////////////////////////////////////////////// TREE ///////////////////////////////////////////////////////////
void InitTree(struct bin_tree* tree) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);

    tree->size = 0;
    tree->root = NULL;
}

void AddElement(struct bin_tree* tree, node_elem_t val) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);

    if (tree->size == 0) {
        tree->root = (struct node*)calloc(1, sizeof(struct node));
        HARD_VERIFY(tree->root != NULL, NULL_ERROR);

        tree->root->value = val;
        tree->root->left = NULL;
        tree->root->right = NULL;
        tree->root->parent = NULL;
        tree->size++;

        return;
    }

    struct node* now = tree->root;
    while (1) {
        if (val >= now->value) {
            if (now->right == NULL) {
                now->right = (struct node*)calloc(1, sizeof(struct node));
                HARD_VERIFY(now->right != NULL, NULL_ERROR);

                now->right->parent = now;
                now = now->right;
                break;
            }
            now = now->right;
        } else {
            if (now->left == NULL) {
                now->left = (struct node*)calloc(1, sizeof(struct node));
                HARD_VERIFY(now->left != NULL, NULL_ERROR);

                now->left->parent = now;
                now = now->left;
                break;
            }
            now = now->left;
        }
    }

    now->value = val;
    now->left = NULL;
    now->right = NULL;
    tree->size++;
    return;
}

void DestroyTree(struct bin_tree* tree) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);

    _DestroySubTree(tree->root);
    tree->size = 0;
    tree->root = NULL;
}

void _DestroySubTree(struct node* node) {
    if (node == NULL) return;

    if (node->left != NULL)
        _DestroySubTree(node->left);
    
    if (node->right != NULL)
        _DestroySubTree(node->right);
    
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
    HARD_ASSERT(tree != NULL, NULL_ERROR);

    FILE* fileout = fopen(TREESAVEFILE, "w");
    HARD_VERIFY(fileout != NULL, CANT_OPEN_FILE);

    PrintTreePreorder(tree, fileout);

    fclose(fileout);
}

void DownloadTree(struct bin_tree* tree, const char* filename) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);
    HARD_ASSERT(filename != NULL, NULL_ERROR);

    FILE* source = fopen(filename, "r");
    HARD_VERIFY(source != NULL, CANT_OPEN_FILE);
    
    char* ptr_save = read_line(source);
    char* save = ptr_save;
    struct node* now = NULL;
    int count = 0;
    char* end = ptr_save + strlen(ptr_save);
    int left_empty = 0;

    while (save < end) {
        if (save[0] == '(') {
            if (save + 1 < end && save[1] == ')') {
                if (now != NULL) {
                    if (now->left == NULL && !left_empty) {
                        left_empty = 1;
                    }
                }
                save += 2;
                continue;
            }
            if (tree->size == 0) {
                tree->root = (struct node*)calloc(1, sizeof(struct node));
                HARD_VERIFY(tree->root != NULL, NULL_ERROR);
                now = tree->root;
                now->parent = NULL;

            } else {
                HARD_VERIFY(now != NULL, NULL_ERROR);
                struct node* new_node = (struct node*)calloc(1, sizeof(struct node));
                HARD_VERIFY(new_node != NULL, NULL_ERROR);

                if (now->left == NULL && !left_empty) {
                    now->left = new_node;
                } else if (now->right == NULL) {
                    now->right = new_node;
                } else {
                    abort();
                }
                new_node->parent = now;
                left_empty = 0;
                now = new_node;
            }
            tree->size++;
            save += 1;
            continue;
        }

        if (save[0] == ')') {
            HARD_VERIFY(now != NULL, NULL_ERROR);
            if (now->parent != NULL && now->parent->left == now) {
                left_empty = 1;
            } else {
                left_empty = 0;
            }
            now = now->parent;
            save += 1;
            continue;
        }

        char* close = memchr(save, '>', (size_t)(end - save));
        HARD_VERIFY(close != NULL, NULL_ERROR);

        size_t vlen = (size_t)(close - save - 1);
        HARD_VERIFY(now != NULL, NULL_ERROR);
        HARD_VERIFY(now->value == NULL, NULL_ERROR);

        now->value = (char*)calloc(vlen + 1, 1);
        HARD_VERIFY(now->value != NULL, NULL_ERROR);
        memcpy(now->value, save + 1, vlen);

        save = close + 1;
    }

    free(ptr_save);
    fclose(source);
}

/////////////////////////////////////////////////////////// Dump ///////////////////////////////////////////////////////////
void TreeDump(struct bin_tree* tree, const char* message, int view_flag) {
    HARD_ASSERT(message != NULL, NULL_ERROR);

    LOGG(message);
    time_t now_time = 0;
    struct tm* time_info = NULL;

    time(&now_time);
    time_info = localtime(&now_time);
    clock_t tik_now = clock();

    char file_name[200] = "";
    strftime(file_name, sizeof(file_name), "dump%d_%m_%Y_%H_%M_%S_", time_info);
    snprintf(file_name + strlen(file_name), 200, "%d", tik_now);
    int have_err = TreeGraphicDump(tree, file_name);

    if (view_flag == YES_VIEW) {
        char sys_cmd[400] = "";
        char path[200] = {};
        _getcwd(path, 200);
        snprintf(sys_cmd, 400, "start \"\" \"%s\\%s\\%s.png\"", path, DIR_NAME, file_name);
        system(sys_cmd);
    }

    if (have_err == HAVE_ERROR) {
        abort();
    }
}

int TreeGraphicDump(struct bin_tree* tree, const char* file_name) {
    HARD_ASSERT(file_name != NULL, NULL_ERROR);

    if (tree == NULL) {
        LOGG("NULL_TREE: TREE is NULL\n");
        return HAVE_ERROR;
    }

    if (tree->root == NULL && tree->size != 0) {
        LOGG("NULL_ROOT: ROOT is NULL\n");
        return HAVE_ERROR;
    }

    int have_err = 0;

    LOGG("<table border=\"1\">\n");
    LOGG("<tr><td>size</td><td>%llu</td></tr>\n", tree->size);
    LOGG("<tr><td>root</td><td>%p</td></tr>\n", tree->root);
    LOGG("</table>\n");

    if (tree->root == NULL) return have_err;
    
    char file_dot[200] = "";
    sprintf(file_dot, "%s\\%s.dot", DIR_NAME, file_name);

    FILE* dump_out = fopen(file_dot, "w");
    HARD_VERIFY(dump_out != NULL, CANT_OPEN_FILE);

    fprintf(dump_out, "digraph {\n");
    fprintf(dump_out, "  rankdir = TB;\n  node[shape = \"Mrecord\", style = filled, fontsize = 12];\n");
    fprintf(dump_out, "  graph[splines = ortho, nodesep = 1];\n");
    fprintf(dump_out, "  edge[radius = 5];\n");
    
    _TreeBasePart(tree, dump_out, &have_err);

    fprintf(dump_out, "}\n");

    fclose(dump_out);

    char sys_cmd[200] = "";
    sprintf(sys_cmd, "\"C:\\Program Files\\Graphviz\\bin\\dot.exe\" -Tpng %s\\%s.dot -o %s\\%s.png", DIR_NAME, file_name, DIR_NAME, file_name);
    system(sys_cmd);

    LOGG("<img src=\"%s\\%s.png\">\n", DIR_NAME, file_name);
    
    return have_err;
}

void SetDumpNode(FILE* dump_out, const struct node* node, const char* color, const char* fillcolor) {
    HARD_ASSERT(dump_out != NULL, NULL_ERROR);
    HARD_ASSERT(node != NULL, NULL_ERROR);
    HARD_ASSERT(color != NULL, NULL_ERROR);
    HARD_ASSERT(fillcolor != NULL, NULL_ERROR);

    fprintf(dump_out, "  NODE%p[color = \"%s\", fillcolor = \"%s\", label=\"{ node = 0x%p | left = 0x%p | right = 0x%p | parent = 0x%p | val = " PRNODE_FOR_GR " }\"];\n", 
            node, color, fillcolor, node, node->left, node->right, node->parent, node->value);
}

void _TreeBasePart(struct bin_tree* tree, FILE* dump_out, int* have_err) {
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
        fprintf(dump_out, "NODE%p->NODE%p[color = \"#0000b0\", weight = 5, dir = forward];\n", node, node->left);
        _DumpSubTree(node->left, dump_out);
    }
    if (node->right != NULL) {
        fprintf(dump_out, "NODE%p->NODE%p[color = \"#ff8000\", weight = 5, dir = forward];\n", node, node->right);
        _DumpSubTree(node->right, dump_out);
    }
    if (node->parent != NULL) {
        fprintf(dump_out, "NODE%p->NODE%p[style=dashed, color = \"#000000\", weight = 0, dir = forward];\n", node, node->parent);
    }
}
