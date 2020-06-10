#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_NUMBER, TK_HEX, TK_REG, 
  TK_EQ, TK_NEQ, TK_AND, TK_OR, TK_NEGATIVE, TK_POINTER,
  //TK_NEGATIVE represents negative,TK_POINTER represents pointer

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},  // spaces
  {"0x[1-9A-Fa-f][0-9A-Fa-f]*",TK_HEX},//16hex
  {"0|[1-9][0-9]*", TK_NUMBER},//number
  {"\\$(eax|ebx|ecx|edx|esp|ebp|esi|edi|eip|ax|bx|cx|dx|sp|bp|si|di|al|cl|dl|bl|ah|ch|dh|bh)",TK_REG},//registers
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},       // not equal
  {"&&", TK_AND},       // and
  {"\\|\\|", TK_OR},     // or
  {"!", '!'},            // not
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // sub
  {"\\*", '*'},         // mult
  {"\\/", '/'},         // div
  {"\\(", '('},         // lbrack
  {"\\)", ')'},         // rbrack
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

      // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
          // i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if(substr_len>32) {	
		assert(0);
	}
	if(rules[i].token_type==TK_NOTYPE){
		break;
	}
	else{
	    tokens[nr_token].type=rules[i].token_type;
	    switch (rules[i].token_type) {//if the  token_type is TK_NUMBER
	      case TK_NUMBER:
		   strncpy(tokens[nr_token].str,substr_start,substr_len);
		   *(tokens[nr_token].str+substr_len)='\0';
		   break;
	      case TK_HEX://pay attention to the substr_len
		   strncpy(tokens[nr_token].str,substr_start+2,substr_len-2);
		   *(tokens[nr_token].str+substr_len-2)='\0';
		   break;
	      case TK_REG:
		   strncpy(tokens[nr_token].str,substr_start+1,substr_len-1);
		   *(tokens[nr_token].str+substr_len-1)='\0';
		   break;
        }
        nr_token+=1;//nr_token++
        break;
	}
      }
    }
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  return true;
}

bool check_parentheses(int p,int q) {//judg whether brackets match
  if(p>q) {
    printf("Arror:p>=q in check_parentheses\n");
    return false;
  }
  if(tokens[p].type!='('||tokens[q].type!=')') {
    return false;
  }
  int count=0;
  for(int i=p+1;i<q;i++) {//from p+1 to q-1
     if(tokens[i].type=='(') {
       count+=1;
     }
     if(tokens[i].type==')') {
       if(count>0) {
	 count-=1;
       }
       else { return false; }
     }
  }
  if(count==0) {//matching
    return true;
  }
  else {//not matching
    return false;
  }
}

int findDominantOp(int p,int q) {//find dominant operator
  if(p>q) {
    printf("Arror:p>=q in findDominantop\n");
    assert(0);
  }
  // printf("%d %d\n",p,q);
  int level=0;
  int pos[6];
  for(int k=0;k<6;k++) {
    pos[k]=-1;
  }//six categories according to priority
  for(int i=p;i<=q;i++) {
    if(tokens[i].type=='(') {
      level+=1;
    }
    if(tokens[i].type==')') {
      if(level>0) {
        level-=1;
      }
      else {
	assert(0);
      }
    }
    if(level==0) {//find the operator
       if(tokens[i].type==TK_NEGATIVE||tokens[i].type==TK_POINTER||tokens[i].type=='!')
	       pos[0]=i;
       if(tokens[i].type=='/'||tokens[i].type=='*')
	       pos[1]=i;
       if(tokens[i].type=='-'||tokens[i].type=='+')
	       pos[2]=i;
       if(tokens[i].type==TK_EQ||tokens[i].type==TK_NEQ)
	       pos[3]=i;
       if(tokens[i].type==TK_AND)
	       pos[4]=i;
       if(tokens[i].type==TK_OR)
	       pos[5]=i;
    }
  }
  int j;
  for(j=5;j>=0;j--) {//take lower priority
    if(pos[j]!=-1) {
      return pos[j];
    }
  }
   printf("There is no dominant operator to find\n");
   return -1;
}

int eval(int p,int q) {//compute the value of expr
  if(p>q) {
    printf("Error:p>q in eval\n");
    assert(0);
  }
  if(p==q) {//single token
    int num;
    switch(tokens[p].type) {
      case TK_NUMBER:
	   sscanf(tokens[p].str,"%d",&num);
	   return num;//expr=num
      case TK_HEX:
	   sscanf(tokens[p].str,"%x",&num);
	   return num;
      case TK_REG://registers
	   for(int i=0;i<8;i++) {
	     if(strcmp(tokens[p].str,regsl[i])==0) { return reg_l(i); }
	     if(strcmp(tokens[p].str,regsw[i])==0) { return reg_w(i); }
	     if(strcmp(tokens[p].str,regsb[i])==0) { return reg_b(i); }
	   }
	   if(strcmp(tokens[p].str,"eip")==0) { return cpu.eip; }
	   else {
	     printf("Error in TK_REG in eval\n");
	     assert(0);
    }
  }
  }
  //if p<q
  if( check_parentheses(p,q)==true ) {
    return eval(p+1,q-1);
  }//the expr is surrounded bu a matched pair of parentheses
  else {
    int op=findDominantOp(p,q);
    //printf("%d\n",op);
    vaddr_t addr;
    int result;
    switch(tokens[op].type) {//unary operators
      case TK_NEGATIVE: return -eval(p+1,q);
      case TK_POINTER:
        addr=eval(p+1,q);
	result=vaddr_read(addr,4);
	printf("addr=%u(0x%x), value=%d(0x%08x)\n",addr,addr,result,result);
	return result;
      case '!':
	result=eval(p+1,q);
	if(result!=0) { return 0; }
	else { return 1; }
    }
    int val1=eval(p,op-1);
    int val2=eval(op+1,q);
    switch(tokens[op].type) {//binary operators
      case '+': return val1+val2;
      case '-': return val1-val2;
      case '*': return val1*val2;
      case '/': return val1/val2;
      case TK_EQ: return val1==val2;
      case TK_NEQ: return val1!=val2;
      case TK_AND: return val1&&val2;
      case TK_OR: return val1||val2;
      default: assert(0);
    }
  }//the expr is surrounded by a matched pair of parentheses
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  if(tokens[0].type=='-') { tokens[0].type=TK_NEGATIVE; }
  if(tokens[0].type=='*') { tokens[0].type=TK_POINTER; }
  for(int i=1;i<nr_token;i++) {
    if(tokens[i].type=='-') {
	    if(tokens[i-1].type!=TK_NUMBER && tokens[i-1].type!=')') {
		    tokens[i].type=TK_NEGATIVE;
	    }
   }//jude TK_NEGATIVE
    if(tokens[i].type=='*') { 
	    if(tokens[i-1].type!=TK_NUMBER && tokens[i-1].type!=')') { 
		     tokens[i].type=TK_POINTER; 
	    }
    }
  }//judg TK_POINTER
  *success = true;
  return eval(0,nr_token-1);
}
