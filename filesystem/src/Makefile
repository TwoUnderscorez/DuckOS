LIBS := libduck string
SUBDIRS := bootscreen cat crashtest dmatest getstest ls man memoryusage pipetest ps terminal xxd

all: $(SUBDIRS)
$(SUBDIRS): $(LIBS)
	$(MAKE) -C $@

$(LIBS):
	$(MAKE) -C $@

clean:
	for dir in $(SUBDIRS); do \
        $(MAKE) -C $$dir clean; \
    done

.PHONY: all $(SUBDIRS) $(LIBS)