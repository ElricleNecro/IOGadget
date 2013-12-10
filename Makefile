LIB      = IOGadget
VERSION  = 0.2

SRCEXT   = c
SRCDIR   = src
OBJDIR   = build
BINDIR   = build
LIBDIR   = build

PREFIX   = $$HOME/.local

PCFILE   = iogadget.pc
SRCS    := src/gadget_write.c src/gadget_read.c src/other.c src/error.c
HDRS    := include/IOGadget/gadget_read.h include/IOGadget/gadget_write.h include/IOGadget/gadget.h include/IOGadget/types.h include/IOGadget/other.h include/IOGadget/error.h
#$(shell find $(SRCDIR) -name '*.$(SRCEXT)')
SRCDIRS := $(shell find . -name '*.$(SRCEXT)' -exec dirname {} \; | uniq)
OBJS    := $(patsubst %.$(SRCEXT),$(OBJDIR)/%.o,$(SRCS))

DEBUG    = -g3
INCLUDES = -I./include
CFLAGS   = -W -Wall -Wshadow -Wcast-qual -Wcast-align -Wsign-compare -Wstrict-prototypes -Wredundant-decls -Wnested-externs -Wunreachable-code -Wwrite-strings -pedantic -fstrict-aliasing $(DEBUG) $(INCLUDES) -c

ifeq ($(SRCEXT), cpp)
CC       = $(CXX)
else
CFLAGS  += -std=gnu99
endif

CC=clang

.PHONY: all clean distclean

all: $(LIBDIR)/lib$(LIB).so

test:
	@echo $(SRCS)
	@echo $(SRCDIRS)
	@echo $(OBJS)

install: $(LIBDIR)/lib$(LIB).so install-hdrs
	install -m 755 $< $(PREFIX)/lib
	sed -e "s:HOME:$(PREFIX):g" -e "s:VERSION:${VERSION}:g" iogadget.pc > $(PREFIX)/lib/pkgconfig/iogadget.pc

install-hdrs: $(HDRS)
	mkdir -p $(PREFIX)/include/IOGadget
	install $^ $(PREFIX)/include/IOGadget

$(LIBDIR)/lib$(LIB).so: buildrepo $(OBJS)
	@mkdir -p `dirname $@`
	@echo "Linking $@..."
	@$(CC) -shared $(OBJS) -o $@

$(OBJDIR)/%.o: %.$(SRCEXT)
	@echo "Generating dependencies for $<..."
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@echo "Compiling $<..."
	@$(CC) -fPIC $(CFLAGS) $< -o $@

clean:
	$(RM) -r $(OBJDIR)

distclean: clean
	$(RM) -r $(BINDIR)

buildrepo:
	@$(call make-repo)

uninstall:
	$(RM) $(PREFIX)/lib/lib$(LIB).so

define make-repo
   for dir in $(SRCDIRS); \
   do \
	mkdir -p $(OBJDIR)/$$dir; \
   done
endef

# usage: $(call make-depend,source-file,object-file,depend-file)
define make-depend
  $(CC) -MM       \
        -MF $3    \
        -MP       \
        -MT $2    \
        $(CFLAGS) \
        $1
endef
