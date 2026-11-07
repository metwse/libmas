MODE ?= release
CURRENT_PLAT ?= thumb2
CURRENT_OS ?= unix


DIST_DIR = dist
TARGET_DIR = $(DIST_DIR)/$(MODE)
OBJ_DIR = $(TARGET_DIR)/obj

SRC_DIR = src
TEST_DIR = tests
PLAT_DIR = plat
OSAL_DIR = osal
VENDOR_DIR = vendor


CPPFLAGS := $(CPPFLAGS) -MMD -MD

CFLAGS_COMMON = -std=c11 -Wall -Wextra -pedantic -fPIC -DLIBFUN_ASSERTIONS
CFLAGS_release = -O3 -flto
CFLAGS_debug = -O0 -g3
CFLAGS_test = -O0 -g3 --coverage
CFLAGS_PLAT_thumb2 = -mthumb

CFLAGS2 = $(CFLAGS_$(MODE))
ifeq ($(CFLAGS2),)
$(error "Unknown mode $(MODE)")
endif

ifeq ($(wildcard $(OSAL_DIR)/$(CURRENT_OS).c),)
$(error "Unknown OS $(CURRENT_OS)")
endif

CFLAGS := $(CFLAGS) $(CFLAGS_COMMON) $(CFLAGS2) $(CFLAGS_PLAT_$(CURRENT_PLAT)) \
	  -DCURRENT_PLAT_$(CURRENT_PLAT) -DCURRENT_OS_$(CURRENT_OS)


SRCS = $(wildcard $(SRC_DIR)/*.c)
PLAT_SRCS = $(wildcard $(PLAT_DIR)/*.c)
TEST_SRCS = $(filter-out $(TEST_DIR)/only_%.c,$(wildcard $(TEST_DIR)/*.c)) \
	      $(wildcard $(TEST_DIR)/only_plat-$(CURRENT_PLAT)*.c) \
	      $(wildcard $(TEST_DIR)/only_os-$(CURRENT_OS)*.c)
OSAL_SRCS = $(wildcard $(OSAL_DIR)/$(CURRENT_OS)*.c)

OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/%.test.o,$(TEST_SRCS))

PLAT_OBJS = $(foreach p, \
		      $(notdir $(wildcard $(PLAT_DIR)/*)),\
		      $(patsubst $(PLAT_DIR)/$p/%.c,\
			        $(OBJ_DIR)/$p/%.o,\
			        $(wildcard $(PLAT_DIR)/$p/*.c)))
OSAL_OBJS = $(patsubst $(OSAL_DIR)/%.c,$(OBJ_DIR)/%.$(CURRENT_OS).o,$(OSAL_SRCS))


TARGET = $(TARGET_DIR)/libmas.a
TEST_TARGETS = $(patsubst $(TEST_DIR)/%.c,$(TARGET_DIR)/%.test,$(TEST_SRCS))

default: all

# External libraries
LIBFUN_DIR := $(VENDOR_DIR)/libfun
LIBFUN_MODE := release

$(LIBFUN_DIR)/libfun.mk: | $(LIBFUN_DIR)/
	cd $(LIBFUN_DIR)/ && \
		git init -q && \
		git remote add origin https://github.com/metwse/libfun.git && \
		git fetch --depth 1 origin 4c6941f60e9b596a5615152eab48e0e0c958b5ec && \
		git checkout -q FETCH_HEAD

include $(LIBFUN_DIR)/libfun.mk


all: $(TARGET) $(TEST_TARGETS)


# Library & test targets.
$(TARGET): $(OBJS) $(PLAT_OBJS) $(OSAL_OBJS) $(LIBFUN) | $(TARGET_DIR)/
	$(AR) rcs $@ $^

$(TARGET_DIR)/%.test: $(OBJ_DIR)/%.test.o $(OBJS) $(PLAT_OBJS) $(OSAL_OBJS) \
		$(LIBFUN) | $(TARGET_DIR)/
	$(CC) $(CFLAGS) -o $@ $^ $(CPPFLAGS)


# Object targets.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)/
	$(CC) $(CFLAGS) -o $@ $< -c $(CPPFLAGS)

.SECONDEXPANSION:
$(OBJ_DIR)/%.o: $(PLAT_DIR)/%.c | $$(dir $(OBJ_DIR)/%)
	$(CC) $(CFLAGS) -o $@ $< -c $(CPPFLAGS)

$(OBJ_DIR)/%.$(CURRENT_OS).o: $(OSAL_DIR)/%.c | $(OBJ_DIR)/
	$(CC) $(CFLAGS) -o $@ $< -c $(CPPFLAGS)

$(OBJ_DIR)/%.test.o: $(TEST_DIR)/%.c | $(OBJ_DIR)/
	$(CC) $(CFLAGS) -o $@ $< -c $(CPPFLAGS)


# Automatic directory targets.
.SECONDARY:
$(DIST_DIR)%/:
	mkdir -p $@
.SECONDARY:
$(VENDOR_DIR)%/:
	mkdir -p $@


clean:
	$(RM) -r $(DIST_DIR)
clean-vendor:
	$(RM) -r $(VENDOR_DIR)

.PHONY: clean clean-vendor default all

-include $(wildcard $(OBJ_DIR)/*.d)
