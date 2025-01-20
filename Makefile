NAME 			= ircserv
CC				= c++

CLASSES_SRCS	= Channel.cpp Client.cpp Command.cpp Server.cpp
COMMANDS_SRCS	= CAP.cpp INVITE.cpp JOIN.cpp KICK.cpp MODE.cpp NICK.cpp NOTICE.cpp PART.cpp PASS.cpp PING.cpp PRIVMSG.cpp QUIT.cpp TOPIC.cpp USER.cpp
UTILS_SRCS		= print_utils.cpp RPN.cpp string_utils.cpp
MAIN_SRCS		= ft_irc.cpp

SOURCES			= $(addprefix srcs/classes/, $(CLASSES_SRCS)) \
				  $(addprefix srcs/commands/, $(COMMANDS_SRCS)) \
				  $(addprefix srcs/utils/, $(UTILS_SRCS)) \
				  $(addprefix srcs/, $(MAIN_SRCS))
OBJECTS			= $(SOURCES:.cpp=.o)
OBJECTS_FOLDER  = $(addprefix objs/, $(OBJECTS))

CMD_HEADERS		= CAP.hpp INVITE.hpp JOIN.hpp KICK.hpp MODE.hpp NICK.hpp NOTICE.hpp PART.hpp PASS.hpp PING.hpp PRIVMSG.hpp QUIT.hpp TOPIC.hpp USER.hpp
HEADERS_FILES	= Channel.hpp Client.hpp Command.hpp ft_irc.hpp replies.hpp RPN.hpp Server.hpp

HEADERS			= $(addprefix includes/commands/, $(CMD_HEADERS)) \
				  $(addprefix includes/, $(HEADERS_FILES))

FLAGS 			= -Wall -Wextra -Werror -std=c++98
OPTIONS 		= -I includes -I includes/commands

ifeq ($(DEBUG), true)
	FLAGS += -g3
endif

#################################################################################

RED 	 = '\033[0;31m'
GREEN 	 = '\033[0;32m'
YELLOW 	 = '\033[0;33m'
BLUE 	 = '\033[0;34m'
PURPLE 	 = '\033[0;35m'
NC 		 = '\033[0m'

#################################################################################

objs/%.o: %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	@$(CC) $(FLAGS) -c $(OPTIONS) $< -o $@
	@echo $(GREEN)✅ $< compiled! $(NC)

$(NAME): $(OBJECTS_FOLDER) $(HEADERS)
	@$(CC) $(FLAGS) $(OPTIONS) -o $@ $(OBJECTS_FOLDER)
	@echo $(BLUE)✅ $@ built! $(NC)

all: $(NAME)

clean:
	@rm -rf objs
	@echo $(YELLOW)🗑️ objs directory deleted! $(NC)

fclean: clean
	@rm -f $(NAME)
	@echo $(RED)🗑️ $(NAME) deleted! $(NC)

re: fclean all

.PHONY: all clean fclean re
