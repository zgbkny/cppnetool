EXECUTABLE :=  proxyx 									# 可执行文件名
LIBDIR:=              									# 静态库目录
LIBS := pthread       									# 静态库文件名
INCLUDES:= . cppnetool/base cppnetool/net    			# 头文件目录
SRCDIR:= cppnetool/base cppnetool/net proxy   # 除了当前目录外，其他的源代码文件目录
TEST := proxyx

RM-F := rm -f

CC:=g++
CFLAGS := -g -Wall -O3
CPPFLAGS := $(CFLAGS)
CPPFLAGS += $(addprefix -I,$(INCLUDES))
CPPFLAGS += -MMD

# # You shouldn't need to change anything below this point.
#
SRCS := $(wildcard *.cc) $(wildcard $(addsuffix /*.cc, $(SRCDIR))) #list all *.cc in the current dir and SRCDIR
OBJS := $(patsubst %.cc,%.o,$(SRCS)) # alt cc to o in SRCS, patsubst is a function
DEPS := $(patsubst %.o,%.d,$(OBJS)) #alt o to d in OBJS
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS)) #remove $(wildcard $(DEPS)) from $(DEPS) then return
MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.cc,$(MISSING_DEPS))) #alt missing *.d to *.cc


.PHONY : all deps objs clean veryclean rebuild info

all: $(EXECUTABLE)

deps: $(DEPS)

objs: $(OBJS)

clean:
	@$(RM-F) $(OBJS)
	@$(RM-F) $(DEPS)

veryclean: clean
	@$(RM-F) $(EXECUTABLE)

rebuild: veryclean all
ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS) :
	@$(RM-F) $(patsubst %.d,%.o,$@)
endif
-include $(DEPS)
$(EXECUTABLE): $(OBJS)
	$(CC) -o $(EXECUTABLE) $(OBJS) $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(LIBS)) -std=c++11

$(OBJS):%.o:%.cc
	$(CC) $(CPPFLAGS) -o $@ -c $< -std=c++11
 
info:
	@echo $(SRCS)
	@echo $(OBJS)
	@echo $(DEPS)
	@echo $(MISSING_DEPS)
	@echo $(MISSING_DEPS_SOURCES)