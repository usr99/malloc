##################################################
#			GLOBAL VARIABLES DEFINITION			 #
##################################################

ifeq (${HOSTTYPE},)
	HOSTTYPE = ${shell uname -m}_${shell uname -s}
endif
SO_LIB	= libft_malloc_${HOSTTYPE}.so
TARGET	= libft_malloc.so

CFLAGS	= -Wall -Wextra -fpic -g #-Werror
LFLAGS	= -shared
CC		= gcc

INC 	= -I ./include -I ./libft

SRCDIR	= ./src/
SRC		= malloc.c
HEADERS = libft_malloc.h
DEPS = ${addprefix include/, ${HEADERS}}

OBJDIR	= ./objs/
OBJS	= ${addprefix ${OBJDIR}, ${SRC:.c=.o}}

LIBFT	= libft/libft.a

##################################################
#			OUTPUT VARIABLES DEFINITION			 #
##################################################

RED = \e[1;31m
GREEN = \e[1;32m
BLUE = \e[1;34m
PURPLE = \e[0;35m
RESET = \e[0;0m

COMPILE = ${GREEN}Compiling${RESET}
BUILD = ${BLUE}Building${RESET}
CLEAN = ${RED}Cleaning${RESET}

##################################################
#				COMPILATION RULES				 #
##################################################

${OBJDIR}%.o: ${SRCDIR}%.c ${DEPS}
	@echo "${COMPILE} $<"
	@${CC} ${CFLAGS} -c $< ${INC} -o $@

${TARGET}: ${SO_LIB} ${LIBFT}
	ln -fs ${SO_LIB} $@

${SO_LIB}: ${OBJDIR} ${OBJS} ${LIBFT}
	@echo "${BUILD} $@"
	@${CC} ${LFLAGS} ${OBJS} ${LIBFT} -o $@
	@echo "${GREEN}$@${PURPLE}was built successfully${RESET}"

${OBJDIR}:
	@mkdir -p ${OBJDIR}

${LIBFT}:
	@echo "${BUILD} libft"
	@${MAKE} --no-print-directory -C libft

##################################################
#  				   USUAL RULES					 #
##################################################

all: ${TARGET}

debug: ${TARGET}
	gcc -g main.c -o test -L. -lft_malloc

clean:
	@echo "${CLEAN} objects"
	@rm -rf ${OBJDIR}
	@echo "${CLEAN} libft"
	@${MAKE} --no-print-directory clean -C libft

fclean: clean
	@echo "${CLEAN} ${TARGET}"
	@rm -rf ${TARGET}
	@rm -rf ${SO_LIB}
	@${MAKE} --no-print-directory fclean -C libft

re: fclean all

.PHONY:	all clean fclean re