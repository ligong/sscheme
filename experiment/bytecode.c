#include <stdio.h>

// switch threading

typedef enum {ASSIGN,ADD,SUB,MUL,DIV,HLT} Inst;
typedef enum {R0,R1,R2,R3,NREG} Reg;

int reg[NREG];

void switch_threading_execute(int *pc)
{
  for(;;) {
    switch(*pc) {
      case ASSIGN:
        reg[*(pc+1)] = *(pc+2);
        pc += 3;
        break;
      case ADD:
        reg[*(pc+1)] = reg[*(pc+2)] + reg[*(pc+3)];
        pc += 4;
        break;
      case SUB:
        reg[*(pc+1)] = reg[*(pc+2)] - reg[*(pc+3)];
        pc += 4;
        break;
      case MUL:
        reg[*(pc+1)] = reg[*(pc+2)] * reg[*(pc+3)];
        pc += 4;
        break;
      case DIV:
        reg[*(pc+1)] = reg[*(pc+2)] / reg[*(pc+3)];
        pc += 4;
        break;
      case HLT:
        return;
      default:
        printf("Error byte code\n");
        return;
    }
  }
}

// computed label
void direct_threading(int *pc)
{
  // following version is shared lib's friend, see
  // http://gcc.gnu.org/onlinedocs/gcc-3.2/gcc/Labels-as-Values.html
/* #define I(x) &&x##_L - &&ASSIGN_L */
/*   static const int offset[] = {I(ASSIGN), I(ADD), I(SUB), */
/*                                I(MUL),I(DIV),I(HLT)}; */
/* #define NEXT goto *(&&ASSIGN_L+offset[*pc]) */

#define NEXT goto *inst[*pc];
  static const void* inst[] = {&&ASSIGN_L, &&ADD_L, &&SUB_L,
                               &&MUL_L,&&DIV_L,&&HLT_L};
ASSIGN_L:
  reg[*(pc+1)] = *(pc+2);
  pc += 3;
  NEXT;
ADD_L:
  reg[*(pc+1)] = reg[*(pc+2)] + reg[*(pc+3)];
  pc += 4;
  NEXT;
SUB_L:
  reg[*(pc+1)] = reg[*(pc+2)] - reg[*(pc+3)];
  pc += 4;
  NEXT;
MUL_L:
  reg[*(pc+1)] = reg[*(pc+2)] * reg[*(pc+3)];
  pc += 4;
  NEXT;  
DIV_L:
  reg[*(pc+1)] = reg[*(pc+2)] / reg[*(pc+3)];
  pc += 4;
  NEXT;
HLT_L:
  return;
}

int main()
{
  int program[] = {
    ASSIGN, R0, 10,
    ASSIGN, R1, 5,
    ADD,    R2, R0,R1,
    HLT
  };
    
  switch_threading_execute(program);
  printf("R2 = %d\n", reg[R2]);
  direct_threading(program);
  printf("R2 = %d\n", reg[R2]);
  return 0;
}
  
