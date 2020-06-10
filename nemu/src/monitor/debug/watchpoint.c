#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
static int wp_index;
static WP *temp;
void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].old_value = 0;
    wp_pool[i].new_value = 0;
    wp_pool[i].hitnum = 0;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
  wp_index = 0;
}

/* TODO: Implement the functionality of watchpoint */
int new_wp(char *args) {//return an idle watchpoint from the linked list
  if(free_==NULL) {
    assert(0);
  }
  WP *wptemp=free_;
  free_=free_->next;
  wptemp->NO=wp_index;
  wp_index++;//index record +1
  wptemp->next=NULL;
  strcpy(wptemp->expr,args);
  wptemp->hitnum=0;
  bool success;
  wptemp->old_value=expr(wptemp->expr,&success);//compute the value of expr as old_value
  if(success==false) {
     printf("Expr error in new_wp\n");
     return 0;
  }
  temp=head;
  if(temp==NULL) {
    head=wptemp;
  }
  else {
    while(temp->next!=NULL) {
	    temp=temp->next;
    }
    temp->next=wptemp;
  }//to the last
  printf("Success: set watchpoint %d,old_value=%d\n",wptemp->NO,wptemp->old_value);
  return 1;
}

int free_wp(int num){//free a watchpoint
    temp=head;
    if(temp==NULL) {
      printf("Error:There is no watchpoint in free_wp\n");
      return 0;
    }
    if(temp->NO==num) {//if head->NO==num
      head=head->next;
    }
    else {//if head->NO!=num
      while(temp->next!=NULL) {
        if(temp->next->NO==num) {
	  WP *wqtemp=temp->next;
	  temp->next=temp->next->next;//updata head
	  temp=wqtemp;
	  break;
	}
	temp=temp->next;
      }
    }

    if(temp!=NULL) {//insert temp in free_
      temp->next=free_;
      free_=temp;
      return 1;
    }
    return 0;
}

int check_wp() {//check triggering
  bool success;
  if(head==NULL) {
    return 1;
  }
  temp=head;
  while(temp!=NULL) {
    temp->new_value=expr(temp->expr,&success);
    if(success==false) {//if success is false,assert 0
      printf("Expr error in chek_wp\n");
      assert(0);
    }
    if(temp->new_value!=temp->old_value) {//if the vaule of expr changes
      temp->hitnum+=1;
      printf("Hardware watchpoint %d:%s\n",temp->NO,temp->expr);
      printf("Old value:%d\nNew value:%d\n",temp->old_value,temp->new_value);
      temp->old_value=temp->new_value;
      return 0;
    }
    temp=temp->next;
  }
  return 1;
}
  
void print_wp() {//print all watchpoints
  if(head==NULL) {
    printf("There is no watchpoint in print_wp\n");
    return;
  }
  printf("Watchpont:\n");
  printf("%-10s %-12s %-16s %-12s\n","NO","old_value","expr","hitnum");
  temp=head;
  while(temp!=NULL) {
    printf("%-10d %-12d %-16s %-12d\n",temp->NO,temp->old_value,temp->expr,temp->hitnum);
    temp=temp->next;
  }
}









