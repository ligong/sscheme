# Copyright (c) 2012 Li Gong. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

#-----------------------------------------------
# Uncomment exactly one of the lines labelled (A), (B), and (C) below
# to switch between compilation modes.

# OPT ?= -O2 -DNDEBUG       # (A) Production use (optimized mode)
OPT ?= -g2              # (B) Debug mode, w/ full line-level debugging symbols
# OPT ?= -O2 -g2 -DNDEBUG # (C) Profiling mode: opt, but w/debugging symbols
#-----------------------------------------------

GTEST_PATH = /home/lg/site/gtest

TEST_LIB = $(GTEST_PATH)/libgtest.a -lpthread

CXXFLAGS += -Wall -I. -I$(GTEST_PATH)/include $(OPT)
LIBS +=

SOURCES = $(shell find src -name '*test*.cc' -prune -o '*.cc' -print | sort | tr "\n" " ")
TEST_SOURCES = $(shell find src -name '*test*.cc' | sort | tr "\n" " ")
TEST_SOURCES += src/atom.cc src/utils.cc

OBJECTS = $(SOURCES:.cc=.o)
TEST_OBJECTS = $(TEST_SOURCES:.cc=.o)


test: $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $(TEST_OBJECTS) -o test $(TEST_LIB)

.cc.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

run_test: test
	./test

clear:
	find . -name "*.o" -o -name test |xargs rm