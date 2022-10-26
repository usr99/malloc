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
SRC		= malloc.c utils.c debug.c
HEADERS = libft_malloc.h
DEPS = ${addprefix include/, ${HEADERS}}

OBJDIR	= ./objs/
OBJS	= ${addprefix ${OBJDIR}, ${SRC:.c=.o}}

LIBFT	= libft/libft.a

##################################################
#			OUTPUT VARIABLES DEFINITION			 #
##################################################

RED = \x1b[1;31m
GREEN = \x1b[1;32m
BLUE = \x1b[1;34m
PURPLE = \x1b[0;35m
RESET = \x1b[0;0m

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
	@ln -fs ${SO_LIB} $@
	@echo "${GREEN}$@${PURPLE} was built successfully${RESET}"

${SO_LIB}: ${OBJDIR} ${OBJS} ${LIBFT}
	@echo "${BUILD} $@"
	@${CC} ${LFLAGS} ${OBJS} ${LIBFT} -o $@

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
	rm -rf *.dSYM test
# do not forget to remove the above line

fclean: clean
	@echo "${CLEAN} ${TARGET}"
	@rm -rf ${TARGET}
	@rm -rf ${SO_LIB}
	@${MAKE} --no-print-directory fclean -C libft

re: fclean all

.PHONY:	all clean fclean re