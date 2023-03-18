NAME		= webserv

CC			= c++

INC			= -I includes/ -I includes/Parsing/ -I includes/utils

CPPFLAGS	= -std=c++98 -g -Wall -Wextra -Werror

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
			Cgi.cpp \
			Client.cpp \
			DefaultPage.cpp

SRCS_BONUS =	main_bonus.cpp \
			parsing/Configuration_bonus.cpp \
			parsing/Server_bonus.cpp \
			parsing/Directives_bonus.cpp \
			parsing/Location_bonus.cpp \
			parsing/utils_bonus.cpp \
			SocketServer_bonus.cpp \
			Request_bonus.cpp \
			Response_bonus.cpp \
			Header_bonus.cpp \
			Cgi_bonus.cpp \
			Client_bonus.cpp \
			DefaultPage_bonus.cpp

SRCDIR		= srcs

SRCDIR_BONUS	= bonus

OBJDIR		= obj

OBJDIR_BONUS		= $(OBJDIR)/bonus

OBJS		= $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

OBJS_BONUS	= $(addprefix $(OBJDIR_BONUS)/, $(SRCS_BONUS:.cpp=.o))

DEPS		= $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(INC) $(OBJS) -o $(NAME)

bonus: | $(OBJS_BONUS)
	$(CC) $(CPPFLAGS) $(INC) $(OBJS_BONUS) -o $(NAME)_bonus

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(INC) -MMD -c $< -o $@ -I include

$(OBJDIR_BONUS)/%.o: $(SRCDIR_BONUS)/%.cpp
	@mkdir -p $(OBJDIR_BONUS)
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(INC) -MMD -c $< -o $@ -I include

clean:
	rm -f $(OBJS)
	rm -f $(OBJS_BONUS)
	rm -f $(DEPS)
	@rm -rf $(OBJDIR)
	@rm -rf $(OBJDIR_BONUS)

fclean: clean
	rm -f $(NAME)
	rm -rf $(NAME)_bonus

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
