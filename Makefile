# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ntan <ntan@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/12/03 14:56:07 by ntan              #+#    #+#              #
#    Updated: 2022/12/21 18:57:15 by ntan             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS			=	srcs/main.cpp\
					srcs/config/Config.cpp\
					srcs/config/Keys.cpp\
					srcs/config/Location.cpp\
					srcs/config/Server.cpp\
					srcs/client/Request.cpp\
					srcs/response/Context.cpp\
					srcs/response/Response.cpp\
					srcs/response/Status.cpp\
					srcs/response/CgiHandler.cpp\
					srcs/webserver/Webserver.cpp\
					srcs/webserver/SocketInfo.cpp

OBJS			= 	$(SRCS:.cpp=.o)

DEPENDS 		= 	$(patsubst %.cpp,%.d,$(SRCS))			
CC				=	c++
RM				=	rm -f
CFLAGS			=	-Wall -Wextra -Werror -g -std=c++98
NAME			=	webserv

all:			$(NAME)

%.o:%.tpp
				$(CC) $(CFLAGS) -I. -c $< -o $@ -E

%.o:%.cpp
				$(CC) $(CFLAGS) -MMD -MP -I. -c $< -o $@ -E

$(NAME):		$(OBJS)
				$(CC) $(CFLAGS) -I. $(SRCS) -o $(NAME)

-include $(DEPENDS)

clean:
				$(RM) $(OBJS) $(DEPENDS)

fclean:
				$(RM) $(OBJS) $(DEPENDS)
				$(RM) $(NAME)

re:				fclean all
