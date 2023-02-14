I			:=	header/
O			:=	obj/

NAME		:=	webserv
SRC			:=	$(wildcard src/*.cpp)

OBJ			:=	$(SRC:%.cpp=$O%.o)

CXX			:=	c++

TMP_FILE 	:= .tmpHostFile

CXXFLAGS	+=	-I$I
CXXFLAGS	+=	-Wall -Wextra -Werror -std=c++98

LDFLAGS		+=	-I$I

RM			:=	rm -f
RMDIR		:=	rm -rf

_RED		=	\033[31m
_GREEN		=	\033[32m
_YELLOW		=	\033[33m
_CYAN		=	\033[96m
_NC			=	\033[0m

$O%.o: $S%.cpp
	@mkdir -p $(@D)
	@echo "    $(_YELLOW) compiling... $*.cpp$(_NC)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

all: init $(NAME)
	

init:
		@ if test -f $(NAME);\
		then echo "$(_CYAN)[program already created]$(_NC)";\
		else echo "$(_YELLOW)[$(NAME)]\n  [Initialize program]$(_NC)";\
		fi

$(NAME): $(OBJ)
	@touch $(TMP_FILE)
	@sudo cp /etc/hosts $(TMP_FILE)
	sudo chmod 666 /etc/hosts
	@ echo "  $(_YELLOW)[Creating program]$(_NC)"
	@$(CXX) $(LDFLAGS) $(OBJ) -o $@
	@ echo "$(_GREEN)[program created $(NAME) & ready]$(_NC)"

clean:
	@$(RMDIR) $(wildcard $(NAME).dSYM)
	@$(RMDIR) $O
	@ echo "$(_RED)[cleaning up objects]$(_NC)"

fclean: clean
	@ if test -f $(TMP_FILE); then sudo cp $(TMP_FILE) /etc/hosts; fi
	@$(RM) $(TMP_FILE)
	@$(RM) $(NAME)
	@ echo "$(_RED)[cleaning up tmp host file]$(_NC)"
	@ echo "$(_RED)[cleaning up binary: $(NAME)]$(_NC)"

re: fclean
	@$(MAKE)

.PHONY: all clean fclean re

-include $(OBJ:.o=.d)
.SILENT:  