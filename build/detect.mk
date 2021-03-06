# Make library that auto-detects the build environment.

UNAME_S := $(shell uname -s)

HOST_IS_LINUX := $(call string_equals,$(UNAME_S),Linux)
HOST_IS_DARWIN := $(call string_equals,$(UNAME_S),Darwin)
HOST_IS_CYGWIN := $(call string_equals,$(UNAME_S),Cygwin)
HOST_IS_MINGW := $(call string_contains,$(UNAME_S),MINGW)
HOST_IS_WIN32 := $(call bool_or,$(HOST_IS_CYGWIN),$(HOST_IS_MINGW))
HOST_IS_UNIX := $(call bool_not,$(HOST_IS_WIN32))
