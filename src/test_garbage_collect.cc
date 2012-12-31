#include "gtest/gtest.h"
#include <string.h>
#include <fstream>
#include <string>
#include <stdlib.h>

#include "memory.h"
#include "symbol.h"
#include "eval.h"
#include "reader.h"
#include "token.h"
#include "printer.h"
#include "test_utils.h"

using namespace sscheme;

const int kMemSize = 5*1024;

// return the occupied size in memory
int OccupiedSize()
{
  return kMemSize - Memory::FreeSize();
}

class EvalTestGarbageCollect  : public ::testing::Test {
 protected:
  virtual void SetUp() {
    sscheme::Initialize(5*1024);
  }
  // virtual void TearDown() {}
};

TEST_F(EvalTestGarbageCollect, test_garbage_collect) {

  g_machine.exp = INT(1);
  g_machine.env = LIST(INT(1),LIST(INT(2),LIST(INT(3))));

  // please do not collect useful pair reachable in register
  int old_size = OccupiedSize();
  EXPECT_EQ(5,old_size);
  Memory::GarbageCollect();
  EXPECT_EQ(old_size, OccupiedSize());
  // once more
  Memory::GarbageCollect();
  EXPECT_EQ(old_size, OccupiedSize());

  // collect garbage when not reachable from register
  g_machine.env = REST(g_machine.env);
  Memory::GarbageCollect();
  // collect 1 pair back
  EXPECT_EQ(1,old_size - OccupiedSize());
  // data in register is safe
  EXPECT_INTEQ(1,g_machine.exp);
  g_machine.arg1 = LIST(LIST(INT(2),LIST(INT(3))));
  EXPECT_LISTEQ(g_machine.arg1,
                g_machine.env);

  // please do not collect useful data reachable in stack  
  old_size = OccupiedSize();
  g_machine.Push(g_machine.env);
  g_machine.env = FIRST(g_machine.env);
  Memory::GarbageCollect();
  EXPECT_EQ(old_size, OccupiedSize());
  g_machine.val = LIST(INT(2),LIST(INT(3)));
  EXPECT_LISTEQ(g_machine.val,g_machine.env);

  // collect garbage when not reachable from stack
  old_size = OccupiedSize();
  g_machine.Pop();
  Memory::GarbageCollect();
  EXPECT_EQ(1,old_size - OccupiedSize());
  EXPECT_LISTEQ(g_machine.val,g_machine.env);

  // collect garbage of circular list
  old_size = OccupiedSize();
  Data x = LIST(INT(1),INT(2));
  SETCDR(REST(x),x);
  EXPECT_EQ(2,OccupiedSize() - old_size);
  Memory::GarbageCollect();
  EXPECT_EQ(old_size,OccupiedSize());
  
  // collect everything
  g_machine.exp = g_machine.env = Data::null;
  g_machine.arg1 = g_machine.val = Data::null;
  Memory::GarbageCollect();
  EXPECT_EQ(0,OccupiedSize());
  
}
