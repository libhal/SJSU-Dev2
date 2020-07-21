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
                                   $(TIDY_COMMIT_SOURCES:=.tidy))
commit-tidy: $(SHORT_TIDY_FILES)
	@printf '$(GREEN)Commit Tidy Evaluation Complete. Everything clear!$(RESET)\n'


$(SJ2_OBJECT_DIR)/%.tidy: %
	@mkdir -p "$(dir $@)"
	@$(CLANG_TIDY) -extra-arg="-std=c++2a" "$<"  -- \
	  -D PLATFORM=host -D HOST_TEST=1 \
		$(INCLUDES) $(SYSTEM_INCLUDES) 2> $@
	@printf '$(GREEN)Evaluated file: $(RESET)$< \n'


spellcheck:
	@./scripts/spell_checker.sh $(LINT_FILES)


all-projects:
	+@./scripts/build_all_projects.sh


presubmit:
	+@./scripts/presubmit.sh


quick-presubmit:
	+@./scripts/presubmit.sh quick


find-missing-tests:
	@./scripts/find_sources_without_tests.sh $(LINT_FILES)


# FIND_ALL_TEST_FILES = $(shell find $(LIBRARY_DIR) -name "*_test.cpp")
# UNUSED_TEST_FILES = $(filter-out $(TESTS), $(FIND_ALL_TEST_FILES))
find-unused-tests:
	@./scripts/find_unused_test_files.sh $(UNUSED_TEST_FILES)


doxygen:
	+@./scripts/generate_doxygen_docs.sh

