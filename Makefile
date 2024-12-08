
dbintro: *.c
	gcc $< -I /usr/local/include/fuse3 -lfuse3 -o $@

