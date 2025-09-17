CC = gcc
CFLAGS = -Wall -Werror -Wextra -pthread
RM = rm -f

SRCS =	eat.c \
	forks.c \
	parse_and_init.c \
	sleep_and_think.c \
	utils.c \
	dead.c \
	main.c

OBJS = $(SRCS:.c=.o)
NAME = philo

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re