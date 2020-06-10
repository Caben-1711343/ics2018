#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  int old_value;//old value
  int new_value;//new value
  char expr[32];//expr
  int hitnum;//trigger times

} WP;

int new_wp(char *args);
int free_wp(int num);
int check_wp();
void print_wp(); 
#endif
