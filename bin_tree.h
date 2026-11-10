#pragma once

#include "base.h"

typedef char* node_elem_t;
#define PRNODE "<%s>"
#define PRNODE_FOR_GR "\\<%s\\>"
#define SCNODE "<%s>"
#define TREESAVEFILE "treesave.txt"
#define HAVE_ERROR 1
#define NO_VIEW 0
#define YES_VIEW 1

#define VIEW_MODE 'v'
#define FIND_MODE 'f'
#define ADD_MODE  'a'
#define COMPARE_MODE  'c'
#define EXIT_MODE 'e'

const char DIR_NAME[] = {'d', 'u', 'm', 'p', __TIME__[0], __TIME__[1], __TIME__[3], __TIME__[4], __TIME__[6], __TIME__[7], '\0'};

struct node {
    node_elem_t value;
    struct node* left;
    struct node* right;
    struct node* parent;
};

struct bin_tree {
    struct node* root;
};

/////////////////////////////////////////////////////////// ПРОТОТИПЫ ФУНКЦИЙ ///////////////////////////////////////////////////////////
void InitTree(struct bin_tree* tree);

void DestroyTree(struct bin_tree* tree);

void _DestroySubTree(struct node* node);

//================================= PRINT =================================//

void PrintTreePreorder(const struct bin_tree* tree, FILE* fileout);

void PrintTreeInorder(const struct bin_tree* tree, FILE* fileout);

void PrintTreePostorder(const struct bin_tree* tree, FILE* fileout);

void _PrintSubTreePreorder(const struct node* node, FILE* fileout);

void _PrintSubTreeInorder(const struct node* node, FILE* fileout);

void _PrintSubTreePostorder(const struct node* node, FILE* fileout);

void SaveTree(const struct bin_tree* tree);

void DownloadTree(struct bin_tree* tree, const char* filename);



void TreeDump(const struct bin_tree* tree, const char* message, const int view_flag);

int TreeGraphicDump(const struct bin_tree* tree, const char* file_name);

void SetDumpNode(FILE* dump_out, const struct node* node, const char* color, const char* fillcolor);

void _TreeBasePart(const struct bin_tree* tree, FILE* dump_out, int* have_err);

void _DumpSubTree(const struct node* node, FILE* dump_out);

int WaitYesOrNo();

void FindElementInTree(struct bin_tree* tree);

void _FindElement(struct node* node);

int WaitCommand();

void RunBinDirectory(struct bin_tree* tree);

void AddObjInTree(struct bin_tree* tree);

struct node* _FindListByName(struct node* node, const node_elem_t obj_name);

uint64_t GetNodeDepth(const struct node* node);

void PrintNodeCharacteristicReverse(const struct node* node);

void CompareObjects(const struct bin_tree* tree);

void _CompareObjects(const struct node* p_obj_1, const struct node* p_obj_2);


void PrintNodeCharacteristic(const struct node* node);

void _PrintNodeCharacteristic(const struct node* node);