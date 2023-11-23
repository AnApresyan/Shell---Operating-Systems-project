NAME = p1
SRCS = $(wildcard *.c)
OBJS = ${SRCS:.c=.o}
CC = gcc
CFLAGS = -Wall

all: ${NAME}

${NAME}: ${OBJS}
	@${CC} ${OBJS} ${CFLAGS} -o ${NAME}

clean :
	@rm -rf *.o

fclean : clean
	@rm -rf ${NAME}

re : fclean all

.PHONY : all clean fclean re