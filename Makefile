NAME		= webserv

CC			= c++




INC			= -I includes/ 


CPPFLAGS	= -std=c++98 -Wall -Wextra -Werror

SRCS	=	main.cpp \
			parsing/Configuration.cpp \
			parsing/Server.cpp \
			parsing/Directives.cpp \
			parsing/Location.cpp \
			parsing/utils.cpp \
			SocketServer.cpp \
			Request.cpp \
			Response.cpp \
			Header.cpp \
			CGI/CGI.cpp

SRCDIR		= srcs

OBJDIR		= obj

OBJS		= $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

DEPS		= $(OBJS:.o=.d)

obj/%.o: srcs/%.cpp
	@mkdir -p $(OBJDIR)
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(INC) -MMD -c $< -o $@ -I include

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(INC) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	@rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re