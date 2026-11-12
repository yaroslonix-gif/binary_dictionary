#include "bin_tree.h"

int main() {
    START_LOGG;
    LOGG("<pre>\n");
    LOGG("<head><style>table {border-collapse: collapse;background-color: #e6f2ff;font-family: Arial, sans-serif;} th, td {border: 1px solid #b3d7ff;padding: 12px; text-align: center;}th {background-color: #cbdff7; font-weight: bold;}</style></head>\n");
    if (CreateDir(DIR_NAME) == -1) {
        abort();
    }

    struct bin_tree tree = {};
    InitTree(&tree);
    DownloadTree(&tree, SAVE_FILENAME);
    TreeDump(&tree, "dump", NO_VIEW);
    PrintTreePreorder(&tree, stdout);

    RunBinDirectory(&tree);

    PrintTreePreorder(&tree, stdout);
    SaveTree(&tree);
    TreeDump(&tree, "dump", NO_VIEW);

    STOP_LOGG;
    DestroyTree(&tree);
    return 0;
}

/////////////////////////////////////////////////////////// BinDirectory ///////////////////////////////////////////////////////////
void RunBinDirectory(struct bin_tree* tree) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);

    while(1) {
        printf("Chose command: [v]view or [f]find or [a]add or [c]compare or [e]exit\n");

        switch (WaitCommand()){
        case VIEW_MODE:
            TreeDump(tree, "view", YES_VIEW);
            break;

        case FIND_MODE:
            FindElementInTree(tree);
            break;

        case ADD_MODE:
            AddObjInTree(tree);
            break;
        
        case COMPARE_MODE:
            CompareObjects(tree);
            break;

        case EXIT_MODE:
            return;

        default:
            break;
        }
    }
}

int WaitCommand() {
    char str[10] = {};

    for (;;) {
        if (scanf("%9s", str) != 1) return 0;

        SkipForEOS(stdin);

        if (strlen(str) == 1 && (str[0] == VIEW_MODE || str[0] == FIND_MODE || str[0] == ADD_MODE || str[0] == COMPARE_MODE || str[0] == EXIT_MODE))
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
    TreeDump(tree, "FindElementInTree", NO_VIEW);

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

    LOGG("wrong tree\n");
    abort();
}

void AddObjInTree(struct bin_tree* tree) {
    TreeDump(tree, "Before AddObjInTree", NO_VIEW);

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

    free(old_obj->value);
    ch_node->left = wrong_node;
    ch_node->right = correct_node;
    wrong_node->parent = ch_node;
    correct_node->parent = ch_node;
    wrong_node->value = old_obj_name;
    correct_node->value = new_obj_name;
    ch_node->value = str_obj_ch;

    TreeDump(tree, "After AddObjInTree", NO_VIEW);
}

struct node* _FindListByName(struct node* node, const node_elem_t obj_name) {
    if (node == NULL) return NULL;

    if (node->left == NULL && node->right == NULL) {
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

uint64_t GetNodeDepth(const struct node* node) {
    HARD_ASSERT(node != NULL, NULL_ERROR);
    uint64_t depth = 0;

    while (node->parent != NULL) {
        depth += 1;
        node = node->parent;
    }

    return depth;
}

void PrintNodeCharacteristicReverse(const struct node* node) {
    HARD_ASSERT(node != NULL, NULL_ERROR);
    printf("%s is:", node->value);

    while (node->parent != NULL) {
        if (node->parent->left == node)
            printf(" not");
        
        node = node->parent;
        printf(" %s,", node->value);
    }

    printf("\b \n");
}

void PrintNodeCharacteristic(const struct node* node) {
    HARD_ASSERT(node != NULL, NULL_ERROR);

    printf("%s is:", node->value);
    _PrintNodeCharacteristic(node);
    printf("\b \n");
}

void _PrintNodeCharacteristic(const struct node* node) {
    HARD_ASSERT(node != NULL, NULL_ERROR);
    if(node->parent == NULL) return;
    
    _PrintNodeCharacteristic(node->parent);

    if (node->parent->left == node)
        printf(" not");
    
    printf(" %s,", node->parent->value);
    return;
}

void CompareObjects(const struct bin_tree* tree) {
    TreeDump(tree, "CompareObjects", NO_VIEW);

    printf("What objects do you want to compare? (Enter first name of object)\n");
    printf("The first object is: ");
    char* obj_1_name = read_line(stdin);

    printf("What second do you want to compare? (Enter second name of object)\n");
    printf("The second object is: ");
    char* obj_2_name = read_line(stdin);

    struct node* obj_1 = _FindListByName(tree->root, obj_1_name);
    struct node* obj_2 = _FindListByName(tree->root, obj_2_name);

    if (obj_1 == NULL) {
        printf("There is no object named %s in database\n", obj_1_name);
        return;
    }
    if (obj_2 == NULL) {
        printf("There is no object named %s in database\n", obj_2_name);
        return;
    }

    _CompareObjects(obj_1, obj_2);
}

void _CompareObjects(const struct node* p_obj_1, const struct node* p_obj_2) {
    HARD_ASSERT(p_obj_1 != NULL, NULL_ERROR);
    HARD_ASSERT(p_obj_2 != NULL, NULL_ERROR);

    const struct node* obj_1 = p_obj_1;
    const struct node* obj_2 = p_obj_2;
    PrintNodeCharacteristic(obj_1);
    PrintNodeCharacteristic(obj_2);

    uint64_t depth_obj_1 = GetNodeDepth(obj_1);
    uint64_t depth_obj_2 = GetNodeDepth(obj_2);
    uint64_t min_depth = MIN(depth_obj_1, depth_obj_2);

    if (depth_obj_1 < depth_obj_2) {
        for (uint64_t i = depth_obj_1; i < depth_obj_2; i++)
            obj_2 = obj_2->parent;
        
    } else if (depth_obj_1 > depth_obj_2) {
        for (uint64_t i = depth_obj_2; i < depth_obj_1; i++)
            obj_1 = obj_1->parent;
    }

    uint64_t not_sim_depth = 0;
    while (obj_1 != obj_2) {
        obj_1 = obj_1->parent;
        obj_2 = obj_2->parent;
        not_sim_depth++;
    }

    if (obj_1 != NULL) {
        if (min_depth - not_sim_depth != 0) {
            printf("The %s and %s are:", p_obj_1->value, p_obj_2->value);
            _PrintNodeCharacteristic(obj_1);
            printf("\b \n");
        }

        double same = 0;
        if (min_depth != 0)
            same = 100.0 * (double)(min_depth - not_sim_depth) / (double)min_depth;
        printf("The %s is %s on %lg percents\n", p_obj_1->value, p_obj_2->value, same);
    }
}

/////////////////////////////////////////////////////////// TREE ///////////////////////////////////////////////////////////
void InitTree(struct bin_tree* tree) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);

    tree->root = NULL;
}

void DestroyTree(struct bin_tree* tree) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);

    _DestroySubTree(tree->root);

    tree->root = NULL;
}

void _DestroySubTree(struct node* node) {
    if (node == NULL) return;

    if (node->left != NULL)
        _DestroySubTree(node->left);
    
    if (node->right != NULL)
        _DestroySubTree(node->right);
    
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

    FILE* fileout = fopen(TREESAVEFILE, "w");
    HARD_VERIFY(fileout != NULL, CANT_OPEN_FILE);

    PrintTreePreorder(tree, fileout);

    fclose(fileout);
}

void DownloadTree(struct bin_tree* tree, const char* filename) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);
    HARD_ASSERT(filename != NULL, NULL_ERROR);

    FILE* source = fopen(filename, "r");
    if (source == NULL) {
        LOGG("CAN'T OPEN FILE\n");
        printf("cant't download save\n");

        tree->root = (struct node*)calloc(1, sizeof(struct node));
        HARD_VERIFY(tree->root != NULL, NULL_ERROR);
        char* value = (char*)calloc(100, sizeof(char));
        snprintf(value, 100, "unknown object");
        tree->root->value = value;
        return;
    }
    
    char* ptr_save = read_line(source);
    char* save = ptr_save;
    struct node* now = NULL;
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

            if (tree->root == NULL) {
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
                    LOGG("WRONG save format\n");
                    abort();
                }

                new_node->parent = now;
                left_empty = 0;
                now = new_node;
            }

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

    if (tree->root == NULL) {
        tree->root = (struct node*)calloc(1, sizeof(struct node));
        HARD_VERIFY(tree->root != NULL, NULL_ERROR);
        char* value = (char*)calloc(100, sizeof(char));
        snprintf(value, 100, "unknown object");
        tree->root->value = value;
    }

    free(ptr_save);
    fclose(source);
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
    
    char file_dot[200] = "";
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


int CreateDir(const char* dir_name) {
    HARD_ASSERT(dir_name != NULL, NULL_ERROR);

    if (_mkdir(dir_name) == 0)
        return 0;
    
    if (errno == EEXIST)
        return 1;

    LOGG("ERROR of make dir\n");
    return -1;
}