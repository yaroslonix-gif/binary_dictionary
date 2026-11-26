#pragma once

#include "base.h"
#include "tree.h"

#define VIEW_MODE 'p'
#define FIND_MODE 'i'
#define ADD_MODE  'd'
#define DEFINITION_MODE   'o'
#define COMPARE_MODE  's'
#define EXIT_MODE 'v'

/////////////////////////////////////////////////////////// ПРОТОТИПЫ ФУНКЦИЙ ///////////////////////////////////////////////////////////

//================================= BinDictionary =================================//
int WaitYesOrNo();

int WaitCommand();

void FindElementInTree(struct bin_tree* tree);

void _FindElement(struct node* node);

void _AddUnknownObjInleftOrRight(struct node* node, int ans);

void _AddUnknownObj(struct node* node);

void RunBinDirectory(struct bin_tree* tree);

void AddObjInTree(struct bin_tree* tree);

struct node* _FindListByName(struct node* node, const node_elem_t obj_name);

void CompareObjects(const struct bin_tree* tree);

void _CompareObjects(const struct node* p_obj_1, const struct node* p_obj_2);


void PrintNodeCharacteristic(const struct node* node);

void _PrintNodeCharacteristic(const struct node* node);

void PrintObjectDefinition(const struct bin_tree* tree);
