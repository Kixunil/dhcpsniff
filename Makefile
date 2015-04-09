all: dhcpsniff

clean:
	rm -f dhcpsniff

dhcpsniff: dhcpsniff.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: all clean
