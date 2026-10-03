PROG = $(patsubst %.cpp,%,$(wildcard *Test.cpp))

include $(shell echo $${PREFIX-/usr})/share/smartmet/devel/makefile.inc

CFLAGS = -DUNIX -O0 -g $(FLAGS)

INCLUDES += \
	-I../smarttools \
	-isystem /usr/include/smartmet

LIBS += -L.. -Wl,--rpath,$(shell pwd)/.. -lsmartmet-smarttools \
	$(PREFIX_LDFLAGS) \
	-lsmartmet-newbase \
	-lsmartmet-macgyver \
	-lboost_iostreams \
	-lboost_thread \
	-lfmt

all: $(PROG)
clean:
	rm -f $(PROG) *~

test: $(PROG)
	@echo Running tests:
	@ok=true; for prog in $(PROG); do ./$$prog || ok=false; done; $$ok

$(PROG) : % : %.cpp ../libsmartmet-smarttools.so
	$(CXX) $(CFLAGS) -o $@ $@.cpp $(INCLUDES) $(LIBS)

.PHONY: test clean all
