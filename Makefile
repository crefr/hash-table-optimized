FILENAME = hashtable

FLAME_GRAPH_PATH = ~/FlameGraph/

OBJDIR  = Obj/
SRCDIR  = sources/
HEADDIR = headers/

CC = g++
BUILD  = RELEASE
# windows
CFLAGS_WINDOWS = -masm=intel -march=native -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline						\
		-Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default					\
		-Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy 				\
		-Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers 	\
		-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing 		\
		-Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE					\
		-I./$(HEADDIR)

# linux
CFLAGS_LINUX = -I./$(HEADDIR)  -masm=intel -march=native -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations 		\
		-Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported 			\
		-Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security 					\
		-Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual 			\
		-Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo 						\
		-Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods 					\
		-Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code 		\
		-Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing 		\
		-Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector 				\
		-fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE	\
		-Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

# release
CFLAGS_RELEASE = -g -masm=intel -march=native -DNDEBUG -I./$(HEADDIR) -O3

CFLAGS_PERF    = -g -masm=intel -march=native -DNDEBUG -I./$(HEADDIR) -O3 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-inline

ifeq ($(BUILD),WIN)
	CFLAGS = $(CFLAGS_WINDOWS)
else ifeq ($(BUILD),LINUX)
	CFLAGS = $(CFLAGS_LINUX)
else ifeq ($(BUILD),RELEASE)
	CFLAGS = $(CFLAGS_RELEASE)
else ifeq ($(BUILD),PERF)
	CFLAGS = $(CFLAGS_PERF)
endif

asm_sources = crc32hash.s
c_sources 	= main.cpp hashtable.cpp buckets.cpp word_finder.cpp

ALLDEPS = $(HEADDIR)strlen_strcmp_opt.h $(HEADDIR)buckets.h $(HEADDIR)hashtable.h $(HEADDIR)hash.h $(HEADDIR)word_finder.h

ASM_OBJS = $(addprefix $(OBJDIR), $(addsuffix .o, $(basename $(asm_sources))))
C_OBJS   = $(addprefix $(OBJDIR), $(addsuffix .o, $(basename $(c_sources))))

$(FILENAME): $(ASM_OBJS) $(C_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(ASM_OBJS): $(OBJDIR)%.o: $(SRCDIR)%.s
	mkdir -p $(OBJDIR)
	nasm -g -f elf64 -l $(addsuffix .lst, $(basename $<)) $< -o $@

$(C_OBJS): $(OBJDIR)%.o: $(SRCDIR)%.cpp $(ALLDEPS)
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

test_data: compile_test_data_gen test_data/shakespeare.txt test_data/war-and-peace.txt
	cd test_data && ./test_data_gen

compile_test_data_gen: test_data/test_data_gen.cpp
	$(CC) $(CFLAGS) test_data/test_data_gen.cpp -o test_data/test_data_gen

perf:
	sudo perf record -g --call-graph dwarf -F 999 ./$(FILENAME) -t
	sudo perf script | $(FLAME_GRAPH_PATH)stackcollapse-perf.pl | $(FLAME_GRAPH_PATH)flamegraph.pl > graph.svg

dump:
	objdump -d -Mintel $(FILENAME) > $(basename $(FILENAME)).disasm

clean:
	rm $(OBJDIR)*

run:
	./$(FILENAME)
