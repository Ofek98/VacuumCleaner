# Define the subdirectories
SUBDIRS = simulator algorithm_A algorithm_B

# Default target
all: $(SUBDIRS)

# Rule to build each subdirectory
$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: all $(SUBDIRS) clean

# Clean target to clean all subdirectories
clean:
	for dir in $(SUBDIRS); do \
	    $(MAKE) -C $$dir clean; \
	done
