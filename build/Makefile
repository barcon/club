# Alternative GNU Make workspace makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifeq ($(config),debug)
  club_config = debug

else ifeq ($(config),release)
  club_config = release

else
  $(error "invalid configuration $(config)")
endif

PROJECTS := club

.PHONY: all clean help $(PROJECTS) 

all: $(PROJECTS)

club:
ifneq (,$(club_config))
	@echo "==== Building club ($(club_config)) ===="
	@${MAKE} --no-print-directory -C . -f club.make config=$(club_config)
endif

clean:
	@${MAKE} --no-print-directory -C . -f club.make clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  debug"
	@echo "  release"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   club"
	@echo ""
	@echo "For more information, see https://github.com/premake/premake-core/wiki"