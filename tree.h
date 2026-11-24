#pragma once

#include "base.h"

typedef char* node_elem_t;
#define PRNODE "<%s>"
#define PRNODE_FOR_GR "\\<%s\\>"
#define SCNODE "<%s>"
#define HAVE_ERROR 1
#define NO_VIEW 0
#define YES_VIEW 1
#define BASE_STR_LEN 200

#define SAVE_FILENAME "treesave.txt"
#define GRAPHVIZ_DOT_PATH "\"C:\\Program Files\\Graphviz\\bin\\dot.exe\""

extern const char DIR_NAME[];

/////////////////////////////////////////////////////////// STRUCT ///////////////////////////////////////////////////////////
struct node {
    node_elem_t value;
    int value_own;
    
    struct node* left;
    struct node* right;
    struct node* parent;
};

struct bin_tree {
    struct node* root;
    struct String save_str;
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


//================================= DUMP =================================//
void TreeDump(const struct bin_tree* tree, const char* message, const int view_flag);

int TreeGraphicDump(const struct bin_tree* tree, const char* file_name);

void SetDumpNode(FILE* dump_out, const struct node* node, const char* color, const char* fillcolor);

void _TreeBasePart(const struct bin_tree* tree, FILE* dump_out, int* have_err);

void _DumpSubTree(const struct node* node, FILE* dump_out);


void DefaultTree(struct bin_tree* tree);

uint64_t GetNodeDepth(const struct node* node);

int CreateDir(const char* dir_name);
