# Ensures that any warnings become errors for both builds and tests.
CFLAGS += -Werror

# ==============================================================================
# Continuous Integration Recipes
# ==============================================================================
FILE_EXCLUDES = grep -v $(addprefix -e ,$(LINT_FILTER))
LINT_FILES    = $(shell \
find $(LIBRARY_DIR) $(SJSU_DEV2_BASE)/projects/hello_world \
     $(SJSU_DEV2_BASE)/projects/starter $(SJSU_DEV2_BASE)/projects/barebones \
     $(SJSU_DEV2_BASE)/demos/ -name "*.hpp" -o -name "*.cpp" | \
     $(FILE_EXCLUDES) 2> /dev/null)

lint:
	@python3 $(SJ2_TOOLS_DIR)/cpplint/cpplint.py $(LINT_FILES)


TIDY_FILES = $(addprefix $(SJ2_OBJECT_DIR)/, $(LINT_FILES:=.tidy))
tidy: $(TIDY_FILES)
	@printf '$(GREEN)Tidy Evaluation Complete. Everything clear!$(RESET)\n'


TIDY_COMMIT_SOURCES := $(shell git show --name-only HEAD | grep ".[hc]pp")
SHORT_TIDY_FILES    := $(addprefix $(SJ2_OBJECT_DIR)/, \
                                   $(TIDY_COMMIT_SOURCES\:=.tidy))
commit-tidy: $(SHORT_TIDY_FILES)
	@printf '$(GREEN)Commit Tidy Evaluation Complete. Everything clear!$(RESET)\n'


$(SJ2_OBJECT_DIR)/%.tidy: %
	@mkdir -p "$(dir $@)"
	@$(CLANG_TIDY) -extra-arg="-std=c++2a" "$<"  -- \
	  -D PLATFORM=host -D HOST_TEST=1 \
		$(MAC_TIDY_INCLUDES) $(INCLUDES) $(SYSTEM_INCLUDES) 2> $@
	@printf '$(GREEN)Evaluated file: $(RESET)$< \n'


spellcheck:
	@$(SJ2_TOOLS_DIR)/spell_checker.sh $(LINT_FILES)


all-projects:
	+@$(SJ2_TOOLS_DIR)/build_all_projects.sh


presubmit:
	+@$(SJ2_TOOLS_DIR)/presubmit.sh


quick-presubmit:
	+@$(SJ2_TOOLS_DIR)/presubmit.sh quick


FILES_WITH_TESTS=$(filter-out $(NO_TEST_NEEDED), $(LINT_FILES))
find-missing-tests:
	@$(SJ2_TOOLS_DIR)/find_sources_without_tests.sh $(FILES_WITH_TESTS)


# FIND_ALL_TEST_FILES = $(shell find $(LIBRARY_DIR) -name "*_test.cpp")
# UNUSED_TEST_FILES = $(filter-out $(TESTS), $(FIND_ALL_TEST_FILES))
find-unused-tests:
	@$(SJ2_TOOLS_DIR)/find_unused_test_files.sh $(UNUSED_TEST_FILES)

