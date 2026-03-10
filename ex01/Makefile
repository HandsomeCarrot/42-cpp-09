#-----VARIABLES-----#

NAME := RPN

CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98
DP_FLAGS := -MP -MD
DB_FLAGS := -g -DDEBUG
SANITIZE_FLAGS := $(DB_FLAGS) -fsanitize=address,leak,undefined

INCLUDE := -Iinclude -Iinclude/helpers
COMP := $(CXX) $(CXXFLAGS) $(INCLUDE)

SRCS :=	main.cpp

B := build
OBJS := $(SRCS:%.cpp=$(B)/%.o)
DEPS := $(OBJS:%.o=%.d)

#-----COLORS-----#

GREEN = \033[1;32m
RED = \033[1;31m
BLUE = \033[1;34m
YELLOW = \033[1;33m
CYAN = \033[1;36m
RESET = \033[0m

#-----PRINTING TOOLS-----#

# Usage: $(call print_target, target_name)
define print_target
	@printf "🛠️  $(BLUE)Compiling target: $(CYAN)$(1)$(RESET)\n"
endef

# Usage: $(call print_compile, file_name)
define print_compile
	@printf "🔧 $(YELLOW)Compiling: $(RESET)$(1)\n"
endef

# Usage: $(call print_clean, message)
define print_clean
	@printf "🧹 $(RED)$(1)$(RESET)\n"
endef

# Usage: $(call print_success, message)
define print_success
	@printf "✨ $(GREEN)$(1)$(RESET)\n"
endef

# Usage: $(call print_debug, message)
define print_debug
	@printf "🐞 $(CYAN)$(1)$(RESET)\n"
endef

#-----RULES-----#

all: $(NAME)

$(NAME): $(OBJS)
	$(call print_target, $(NAME))
	@$(COMP) $(OBJS) -o $(NAME)
	$(call print_success, $(NAME) ready!)

$(B)/%.o: %.cpp
	@mkdir -p $(@D)
	$(call print_compile, $<)
	@$(COMP) $(DP_FLAGS) -c $< -o $@

clean:
	@rm -rf $(B)
	$(call print_clean, Removed build directory)

fclean: clean
	@rm -f $(NAME)
	$(call print_clean, Removed executable)

re: fclean all

debug: fclean
	$(call print_debug, Compiling with debug flags...)
	@make --no-print-directory all CXXFLAGS="$(CXXFLAGS) $(DB_FLAGS)"

sanitize: fclean
	$(call print_debug, Compiling with fsanitize flags...)
	@make --no-print-directory all CXXFLAGS="$(CXXFLAGS) $(SANITIZE_FLAGS)"

run: re
	@printf "🚀 $(BLUE)Running $(NAME)...$(RESET)\n"
	@printf "$(CYAN)--------------------------------$(RESET)\n"
	@./$(NAME)

.PHONY: all clean fclean re run debug sanitize

-include $(DEPS)
