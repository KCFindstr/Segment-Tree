CC = g++
FLAGS = -Wall -Wextra -std=c++11 -g -Wconversion
TARGETS = segtree_test

all: $(TARGETS)

segtree_test: segtree_test.cpp segtree.h
	$(CC) $(FLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGETS)
