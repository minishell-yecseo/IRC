include ./Makefile_Variable

$(NAME) : $(ALL_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

all : $(OBJ_DIR) $(NAME)

clean :
	@$(RM) $(RMFLAGS) $(OBJ_DIR)

fclean :
	@$(RM) $(RMFLAGS) $(OBJ_DIR)
	@$(RM) $(RMFLAGS) $(NAME)

re :
	@$(MAKE) fclean
	@$(MAKE) all

.DEFAULT_GOAL = all
