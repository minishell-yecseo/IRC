ifeq ($(filter $(MAKECMDGOALS), test), test)
include ./Makefile_Test
else
include ./Makefile_Variable
endif

$(NAME) : $(ALL_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

all : $(OBJ_DIR) $(NAME)

test : $(OBJ_DIR) $(NAME)

clean :
	@$(RM) $(RMFLAGS) $(OBJ_DIR)

fclean :
	@$(RM) $(RMFLAGS) $(OBJ_DIR)
	@$(RM) $(RMFLAGS) ircserv ircserv_test

re :
	@$(MAKE) fclean
	@$(MAKE) all

.DEFAULT_GOAL = all
