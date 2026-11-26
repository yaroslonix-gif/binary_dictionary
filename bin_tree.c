#include "bin_tree.h"
#include "speaker.h"

int main() {
    START_LOGG;

    LOGG("<pre>\n");
    LOGG("<head><style>table {border-collapse: collapse;background-color: #e6f2ff;font-family: Arial, sans-serif;} th, td {border: 1px solid #b3d7ff;padding: 12px; text-align: center;}th {background-color: #cbdff7; font-weight: bold;}</style></head>\n");
    if (CreateDir(DIR_NAME) == -1) {
        return 0;
    }

    struct bin_tree tree = {};
    InitTree(&tree);
    DownloadTree(&tree, SAVE_FILENAME);
    TreeDump(&tree, "dump", NO_VIEW);
    //PrintTreePreorder(&tree, stdout);

    RunBinDirectory(&tree);

    //PrintTreePreorder(&tree, stdout);
    SaveTree(&tree);
    TreeDump(&tree, "dump", NO_VIEW);

    STOP_LOGG;
    DestroyTree(&tree);
    return 0;
}

/////////////////////////////////////////////////////////// BinDictionary ///////////////////////////////////////////////////////////
void RunBinDirectory(struct bin_tree* tree) {
    HARD_ASSERT(tree != NULL, NULL_ERROR);

    while (1) {
        SPEAK_AND_PRINT_BASE("Vyberite komandu: [p]prosmotr, [i]igrat, [d]dobavit, [o]opredelit, [s]sravnit ili [v]vyhod\n");

        switch (WaitCommand()) {
        case VIEW_MODE:
            TreeDump(tree, "view", YES_VIEW);
            break;

        case FIND_MODE:
            FindElementInTree(tree);
            break;

        case ADD_MODE:
            AddObjInTree(tree);
            break;

        case DEFINITION_MODE:
            PrintObjectDefinition(tree);
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

        if (strlen(str) == 1 && (str[0] == VIEW_MODE       || str[0] == FIND_MODE    || str[0] == ADD_MODE ||
                                 str[0] == DEFINITION_MODE || str[0] == COMPARE_MODE || str[0] == EXIT_MODE))
            return str[0];
        
        SPEAK_AND_PRINT_BASE("Nevernyy vvod, poprobuyte snova\n");
    }
}

int WaitYesOrNo() {
    char str[10] = {};

    for (;;) {
        if (scanf("%9s", str) != 1) return 0;

        SkipForEOS(stdin);

        if (strlen(str) == 1 && (str[0] == 'y' || str[0] == 'n'))
            return (str[0] == 'y');
        
        SPEAK_AND_PRINT_BASE("Nevernyy vvod, poprobuyte snova\n");
    }
}

void FindElementInTree(struct bin_tree* tree) {
    TreeDump(tree, "FindElementInTree", NO_VIEW);

    _FindElement(tree->root);
}

void _FindElement(struct node* node) {
    if (node == NULL) return;

    SPEAK_AND_PRINT_BASE("Eto %s? ", node->value);
    printf("(Otvet: [y]da ili [n]net)\n");
    SpeakFlush();
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
            SPEAK_AND_PRINT_BASE("Ya ugadal\n");
            return;
        } else {
            _AddUnknownObj(node);

            return;
        }
    }
    
    _AddUnknownObjInleftOrRight(node, ans);
}

void _AddUnknownObjInleftOrRight(struct node* node, int ans) {
    if (node == NULL) return;

    if (ans == 1 && node->right == NULL) {
        SPEAK_AND_PRINT_BASE("Ya ne znayu chto eto\n");
        SPEAK_AND_PRINT_BASE("Eto: ");
        SpeakFlush();

        char* str = read_line(stdin);
        node->right = (struct node*)calloc(1, sizeof(struct node));
        HARD_VERIFY(node->right != NULL, NULL_ERROR);
        node->right->value = str;
        node->right->value_own = 1;
        node->right->parent = node;

        return;
    }

    if (ans == 0 && node->left == NULL) {
        SPEAK_AND_PRINT_BASE("Ya ne znayu chto eto\n");
        SPEAK_AND_PRINT_BASE("Eto: ");
        SpeakFlush();

        char* str = read_line(stdin);
        node->left = (struct node*)calloc(1, sizeof(struct node));
        HARD_VERIFY(node->left != NULL, NULL_ERROR);
        node->left->value = str;
        node->left->value_own = 1;
        node->left->parent = node;

        return;
    }

    _FindElement(ans ? node->right : node->left);
}

void _AddUnknownObj(struct node* node) {
    if (node == NULL) return;

    SPEAK_AND_PRINT_BASE("Ya ne znayu chto eto\n");

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
    wrong_node->value_own = node->value_own;

    SPEAK_AND_PRINT_BASE("Eto: ");
    SpeakFlush();
    char* str_obj_name = read_line(stdin);
    correct_node->value = str_obj_name;
    correct_node->value_own = 1;

    SPEAK_AND_PRINT_BASE("V chem raznitsa mezhdu %s i %s:\n", str_obj_name, wrong_node->value);
    SPEAK_AND_PRINT_BASE("%s: ", str_obj_name);
    SpeakFlush();

    char* str_obj_ch = read_line(stdin);
    ch_node->value = str_obj_ch;
    ch_node->value_own = 1;
}

void PrintObjectDefinition(const struct bin_tree* tree) {
    SPEAK_AND_PRINT_BASE("Kakoy obekt vy hotite opredelit? (Vvedite ego imya)\n");
    SPEAK_AND_PRINT_BASE("Oprediliaemiy obekt: ");
    SpeakFlush();
    char* obj_name = read_line(stdin);
    struct node* obj = _FindListByName(tree->root, obj_name);

    if (obj == NULL) {
        SPEAK_AND_PRINT_BASE("V baze net obekta s imenem %s\n", obj_name);
        free(obj_name);
        return;
    }
    free(obj_name);

    PrintNodeCharacteristic(obj);
}

static void _AddObjInTreeOrganizeNodes(struct node* old_obj, char* new_obj_name, char* str_obj_ch);

void AddObjInTree(struct bin_tree* tree) {
    TreeDump(tree, "Before AddObjInTree", NO_VIEW);

    SPEAK_AND_PRINT_BASE("Kakoy obekt vy hotite dobavit? (Vvedite ego imya)\n");
    SPEAK_AND_PRINT_BASE("Novyy obekt: ");
    SpeakFlush();

    char* new_obj_name = read_line(stdin);
    if (new_obj_name == NULL) return;

    SPEAK_AND_PRINT_BASE("S kakim obektom vy hotite sravnit %s? (Vvedite ego imya)\n", new_obj_name);
    SPEAK_AND_PRINT_BASE("Obekt: ");
    SpeakFlush();

    char* old_obj_name = read_line(stdin);
    if (old_obj_name == NULL) {
        free(new_obj_name);
        return;
    }

    struct node* old_obj = _FindListByName(tree->root, old_obj_name);
    if (old_obj == NULL) {
        SPEAK_AND_PRINT_BASE("V baze net obekta s imenem %s\n", old_obj_name);
        free(new_obj_name);
        free(old_obj_name);
        return;
    }

    SPEAK_AND_PRINT_BASE("V chem raznitsa mezhdu %s i %s:\n", new_obj_name, old_obj_name);
    SPEAK_AND_PRINT_BASE("%s: ", new_obj_name);
    char* str_obj_ch = read_line(stdin);
    
    _AddObjInTreeOrganizeNodes(old_obj, new_obj_name, str_obj_ch);

    free(old_obj_name);
    TreeDump(tree, "After AddObjInTree", NO_VIEW);
}

static void _AddObjInTreeOrganizeNodes(struct node* old_obj, char* new_obj_name, char* str_obj_ch) {
    HARD_ASSERT(old_obj != NULL, NULL_ERROR);
    HARD_ASSERT(new_obj_name != NULL, NULL_ERROR);
    HARD_ASSERT(str_obj_ch != NULL, NULL_ERROR);
    HARD_ASSERT(old_obj->left == NULL && old_obj->right == NULL, "not a leaf");

    struct node* ch_node = old_obj;
    struct node* wrong_node = (struct node*)calloc(1, sizeof(struct node));
    HARD_VERIFY(wrong_node != NULL, NULL_ERROR);
    struct node* correct_node = (struct node*)calloc(1, sizeof(struct node));
    HARD_VERIFY(correct_node != NULL, NULL_ERROR);

    ch_node->left = wrong_node;
    ch_node->right = correct_node;
    wrong_node->parent = ch_node;
    correct_node->parent = ch_node;
    wrong_node->value = old_obj->value;
    wrong_node->value_own = old_obj->value_own;
    correct_node->value = new_obj_name;
    correct_node->value_own = 1;
    ch_node->value = str_obj_ch;
    ch_node->value_own = 1;
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

void PrintNodeCharacteristic(const struct node* node) {
    HARD_ASSERT(node != NULL, NULL_ERROR);

    SPEAK_AND_PRINT_BASE("%s eto:", node->value);
    _PrintNodeCharacteristic(node);
    printf("\b \n");
    SpeakFlush();
}

void _PrintNodeCharacteristic(const struct node* node) {
    HARD_ASSERT(node != NULL, NULL_ERROR);
    if(node->parent == NULL) return;
    
    _PrintNodeCharacteristic(node->parent);

    if (node->parent->left == node)
        SPEAK_AND_PRINT_BASE(" ne");
    
    SPEAK_AND_PRINT_BASE(" %s,", node->parent->value);
    return;
}

void CompareObjects(const struct bin_tree* tree) {
    TreeDump(tree, "CompareObjects", NO_VIEW);

    SPEAK_AND_PRINT_BASE("Kakie obekty vy hotite sravnit? (Vvedite imya pervogo obekta)\n");
    SPEAK_AND_PRINT_BASE("Pervyy obekt: ");
    SpeakFlush();
    char* obj_1_name = read_line(stdin);

    SPEAK_AND_PRINT_BASE("Kakoy vtoroy obekt vy hotite sravnit? (Vvedite imya vtorogo obekta)\n");
    SPEAK_AND_PRINT_BASE("Vtoroy obekt: ");
    SpeakFlush();
    char* obj_2_name = read_line(stdin);

    struct node* obj_1 = _FindListByName(tree->root, obj_1_name);
    struct node* obj_2 = _FindListByName(tree->root, obj_2_name);

    if (obj_1 == NULL) {
        SPEAK_AND_PRINT_BASE("V baze net obekta s imenem %s\n", obj_1_name);
        free(obj_1_name);
        free(obj_2_name);
        return;
    }

    if (obj_2 == NULL) {
        SPEAK_AND_PRINT_BASE("V baze net obekta s imenem %s\n", obj_2_name);
        free(obj_1_name);
        free(obj_2_name);
        return;
    }

    free(obj_1_name);
    free(obj_2_name);

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
    while (obj_1 != obj_2 && obj_1 != NULL && obj_2 != NULL) {
        obj_1 = obj_1->parent;
        obj_2 = obj_2->parent;
        not_sim_depth++;
    }

    if (obj_1 != NULL) {
        if ((int64_t)min_depth - (int64_t)not_sim_depth != 0) {
            SPEAK_AND_PRINT_BASE("Chto obshchego u %s i %s:", p_obj_1->value, p_obj_2->value);
            _PrintNodeCharacteristic(obj_1);
            printf("\b \n");
        }

        double same = 100.0;
        if (min_depth > 0)
            same = 100.0 * (double)((int64_t)min_depth - (int64_t)not_sim_depth) / (double)min_depth;
        
        SPEAK_AND_PRINT_BASE("%s i %s pohozhi na %g procentov\n", p_obj_1->value, p_obj_2->value, same);
    }
}
