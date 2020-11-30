# Ensures that any warnings become errors for both builds and tests.
CFLAGS += -Werror

# ==============================================================================
# Continuous Integration Recipes
# ==============================================================================

FILE_EXCLUDES      := grep -v $(addprefix -e ,$(LINT_FILTER))
GREP_CPP_SOURCES   := grep ".[hc]pp$$" | $(FILE_EXCLUDES)
GIT_ALL_REPO_FILES := git ls-tree --full-tree -r --name-only HEAD
GIT_DELETED_FILES  := git show --diff-filter=D  --name-only HEAD...master
GIT_MODIFIED_FILES := git show --diff-filter=AM --name-only HEAD...master

TRACKED_SOURCES   := $(shell $(GIT_ALL_REPO_FILES) | $(GREP_CPP_SOURCES))
DELETED_SOURCES   := $(shell $(GIT_DELETED_FILES)  | $(GREP_CPP_SOURCES))
MODIFIED_SOURCES  := $(shell $(GIT_MODIFIED_FILES) | $(GREP_CPP_SOURCES))
UPDATED_SOURCES   := $(filter-out $(DELETED_SOURCES), $(MODIFIED_SOURCES))

LINT_FILES         = $(addprefix $(SJSU_DEV2_BASE)/, $(TRACKED_SOURCES))
TIDY_FILES         = $(addprefix $(SJ2_OBJECT_DIR)/, $(TRACKED_SOURCES:=.tidy))
UPDATED_TIDY_FILES = $(addprefix $(SJ2_OBJECT_DIR)/, $(UPDATED_SOURCES:=.tidy))

MAC_TIDY_INCLUDES = \
		-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk \
		-isysroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/


# ==============================================================================
# Targets
# ==============================================================================


lint:
	@python3 $(SJ2_TOOLS_DIR)/cpplint/cpplint.py $(LINT_FILES)


commit-tidy: $(UPDATED_TIDY_FILES)
	@printf '$(GREEN)Commit Tidy Evaluation Complete. Everything clear!$(RESET)\n'


tidy: $(TIDY_FILES)
	@printf '$(GREEN)Tidy Evaluation Complete. Everything clear!$(RESET)\n'


$(SJ2_OBJECT_DIR)/%.tidy: $(SJSU_DEV2_BASE)/%
	@mkdir -p "$(dir $@)"
	@$(CLANG_TIDY) -extra-arg="-std=c++2a" "$<"  -- \
	  -D PLATFORM=host -D HOST_TEST=1 \
		$(MAC_TIDY_INCLUDES) $(INCLUDES) $(SYSTEM_INCLUDES) 2> $@.tmp
	@# This is here to convert the temp file into the correct .tidy if the above
	@# command was successful
	@mv $@.tmp $@
	@printf '$(GREEN)Evaluated file: $(RESET)$< \n'


spellcheck:
	@./scripts/spell_checker.sh $(LINT_FILES)


all-projects:
	+@./scripts/build_all_projects.sh


presubmit:
	+@./scripts/presubmit.sh


find-missing-tests:
	@./scripts/find_sources_without_tests.sh $(LINT_FILES)


# FIND_ALL_TEST_FILES = $(shell find $(LIBRARY_DIR) -name "*_test.cpp")
# UNUSED_TEST_FILES = $(filter-out $(TESTS), $(FIND_ALL_TEST_FILES))
find-unused-tests:
	@./scripts/find_unused_test_files.sh $(UNUSED_TEST_FILES)


doxygen:
	+@./scripts/generate_doxygen_docs.sh


format-code:
	@$(SJCLANG)/bin/git-clang-format --binary="$(SJCLANG)/bin/clang-format" \
	  --force
