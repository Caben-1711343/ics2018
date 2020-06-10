#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
  uint64_t N=0;
  if(args==NULL) { N=1; }//The args not provided
  else {
       sscanf(args,"%llu",&N);
       if(N==0) {
	       printf("Args error in cmd_si\n"); 
	       return 0;
       }//Error if transformed args minus
  } 
  cpu_exec(N);
  return 0;
}

static int cmd_info(char *args) {
  if(args==NULL) {
	  printf("Args error in cmd_info\n");
	  return 0;
  }//The args not provided
  if(*args=='r') {
    int i;
    printf("eip, 0x%x\n",cpu.eip);//eip register
    for(i=0;i<8;i++) { printf("%s, 0x%x\n",regsl[i],reg_l(i)); }//32-bits registers
    for(i=0;i<8;i++) { printf("%s, 0x%x\n",regsw[i],reg_w(i)); }//16-bits registers
    for(i=0;i<8;i++) { printf("%s, 0x%x\n",regsb[i],reg_b(i)); }//8-bits registers
    return 0;
  }
  else if(*args=='w') {
    print_wp();
    return 0;
  }	  
  else {
     printf("Args error in cmd_info\n");
     return 0;
  }
}

static int cmd_x(char *args) {
  int len=0;
  char *arg1=strtok(args, " ");//extract first parameter
  //printf("%s",arg1);
  int n=sscanf(arg1,"%d",&len);
  if(n<=0) {
	  printf("Arg1 error in cmd_x\n");
	  return 0;
  }
  arg1=strtok(NULL," ");//extract second parameter
  //printf("%s",arg1);
  bool flag;
  vaddr_t addr=expr(arg1,&flag);
  if(flag==false) {
    printf("Expr error in cmd_x\n");
   }
  else {
    printf("Memory:\n");
    int i=0;
    for(i=0;i<len;i++) {
	   printf("0x%x: 0x%08x\n",addr+4*i, vaddr_read(addr+4*i,4));//print 4 consecutive bytes
	  }
  }
  printf("\n");
  return 0;
}

static int cmd_p(char *args) {//the value of expr
  bool flag;
  // printf("0x08%x\n",expr(args,&flag));
  int val=expr(args,&flag);//the value of expr
  if(flag==false) {
    printf("Expr error in cmd_p\n");
  }
  else {
    printf("The value of expr is:%d\n",val);
  }
  return 0;
}

static int cmd_w(char *args) {//create the watchpoint 
  int n= new_wp(args);
  if(n==0) {
    printf("Creating watchpoint error in cmd_w\n");
  }
  return 0;
}

static int cmd_d(char *args) {//delete the watchpoint
  int N=0;
  int n1=sscanf(args,"%d",&N);
  if(n1<=0) {
    printf("Args error in cmd_d\n");
    return 0;
  }
  int n2=free_wp(N);
  if(n2==0) {
    printf("Error:no watchpoint %d\n",N);
  }
  else {
    printf("Success:the watchpoint %d has been deleted\n",N);
  }
  return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Args:[N];Execute N instructions step by step", cmd_si },
  { "info", "Args:r/w;Print information about registers or watchpoint", cmd_info },
  { "x", "Args:[N] [EXPR];Scan the memory", cmd_x },
  { "p", "Args:expr", cmd_p },
  { "w", "Args:expr;Set the watchpoint", cmd_w},
  { "d", "Args:[N];Delete the N watchpoint", cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
