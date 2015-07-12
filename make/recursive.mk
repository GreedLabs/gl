
MAKES   = $(TARGETS:.proj=.mk)
CLEANS  = $(TARGETS:.proj=.cl)

all  : $(MAKES)
clean: $(CLEANS)

%.mk: %
	$(MAKE) -C $^

%.cl: %
	$(MAKE) -C $^ clean
