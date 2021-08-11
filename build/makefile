# This makefile was written by Kolotushkin Danil
# github: https://github.com/vrodedanya
#
# The makefile located one directory below root directory of your project. 
# If that is not you need comment string with 'addprefix'
#
# Edit variables below for you project

#---------------------------------------#

# Program name (ex: test, program)
NAME :=
# Compiler name (ex: gcc, g++, clang)
CC :=
# File extension (ex: .cpp, .c)
LANG :=
# Compiler flags (ex: -Wall, -O2, -g)
CFLAGS :=
# Linker flags (ex: -pthread)
LDFLAGS :=
# Libs (ex: -lm, -lncurses, -lSDL2)
LIBS :=
# Defines (ex: -D DEBUG, -Dcount=5)
DEF :=
# Directories with source code (ex: source)
DIR :=

#---------------------------------------#

# Colors
BLACK := \033[0;30m     
DARKGRAY := \033[1;30m
RED := \033[0;31m
LIGHTRED := \033[1;31m
GREEN := \033[0;32m
LIGHTGREEN := \033[1;32m
BROWN := \033[0;33m
YELLOW := \033[1;33m
BLUE := \033[0;34m
LIGHTBLUE := \033[1;34m
PURPLE := \033[0;35m
LIGHTPURPLE := \033[1;35m
CYAN := \033[0;36m
LIGHTCYAN := \033[1;36m
LIGHTGRAY := \033[0;37m
WHITE := \033[1;37m
NC= \033[0m

#Colorscheme

PROCESS_C := $(LIGHTGREEN)
FILE_C := $(LIGHTRED)
FLAGS_C := $(LIGHTBLUE)
SEPARATOR_C := $(DARKGRAY)

#---------------------------------------#

# For this function thanks LightStruk from this stackoverflow question
# https://stackoverflow.com/questions/4036191/sources-from-subdirectories-in-makefile
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

SRC := $(DIR)

# Comment this string if you don't use special directory for building project
SRC := $(addprefix ../, $(DIR))

# Storages all source files with path
FILES := $(call rwildcard,$(SRC),*$(LANG))

# Storages all object files
OBJ_FILES := $(notdir $(patsubst %$(LANG), %.o, $(FILES)))

# Added directories from $(SRC) to directories which contain source code
VPATH := $(SRC) $(dir $(FILES))

all: $(OBJ_FILES) 
	@$(CC) $^ $(LIBS) $(LDFLAGS) -o $(NAME)
	@echo -e "$(SEPARATOR_C)====================================$(NC)"
	@echo -e "$(PROCESS_C)Linking:$(NC) $(FILE_C)$(NAME)$(NC) with $(FLAGS_C)$(LDFLAGS) $(LIBS)$(NC)"

static_library: $(filter-out main.o, $(OBJ_FILES))
	ar cr lib$(NAME).a $^

# Creating object files
%.o: %$(LANG)
	@$(CC) -c $(CFLAGS) $(DEF) -MD $<
	@echo -e "$(PROCESS_C)Compiling:$(NC) $(FILE_C)$(notdir $<)$(NC) with $(FLAGS_C)$(CFLAGS) $(DEF)$(NC)"
clean:
	@rm -rf *.o *.d $(NAME)
	@echo -e "$(PROCESS_C)Cleaning...$(NC)"

include $(wildcard *.d)
