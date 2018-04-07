TARGET = main
SRCS = $(wildcard *.cc)
OBJS = $(SRCS:.cc=.o)
DEPS = $(SRCS:.cc=.d)
CC = g++
CCFLAGS = $(shell llvm-config --cxxflags)
LDFLAGS = $(shell llvm-config --ldflags)
LLVM_LIBS = -lLLVM -lpthread

.PHONY: all clean

all:$(TARGET)

$(TARGET):$(DEPS) $(OBJS) $(SRCS)
	@$(CC) $(OBJS) $(LDFLAGS) $(LLVM_LIBS) -o $@
	@echo linking $(TARGET)
%.d:%.cc
	@$(CC) -MM $(CCFLAGS) $< > $@
	@echo generating $@
%.o:%.cc
	@$(CC) -c $(CCFLAGS) $< -o $@
	@echo compiling $<

-include $(DEPS)

clean:
	@rm -f $(TARGET)
	@rm -f $(DEPS)
	@rm -f $(OBJS)
