NAME		= webserv

CC			= c++

CPPFLAGS	= -std=c++98 -Wall -Wextra -Werror

SRCS	=	srcs/main.cpp \
			srcs/parsing/Configuration.cpp \
			srcs/parsing/Server.cpp \
			srcs/parsing/Directives.cpp \
			srcs/parsing/Location.cpp \
			srcs/parsing/utils.cpp \
			srcs/SocketServer.cpp \
			srcs/Request.cpp \
			srcs/Response.cpp \
			srcs/Header.cpp

OBJS		= $(SRCS:.cpp=.o)

DEPS		= $(SRCS:.cpp=.d)

%.o: %.cpp
	$(CC) $(CPPFLAGS) -MMD -c $< -o $@ -I include

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re