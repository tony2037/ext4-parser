BINS = lsfs

SRCS = filesystem.c
OBJS = $(SRCS:%.c=%.o)

.PHONY: all clean
all: $(BINS)

lsfs: lsfs.c $(OBJS)
		$(CC) $^ -o $@ $(CFLAGS) $(LD_FLAGS)

%.o: %.c
		$(CC) -c $< -o $@ $(CFLAGS) $(LD_FLAGS)

clean:
		/bin/rm -f *.o *~ $(BINS)
