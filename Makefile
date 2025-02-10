
# dbintro: *.c
# 	gcc $< -I /usr/include/fuse3 -lfuse3 -o $@

# clean:
# 	rm dbintro

dbintro: main.o utils.o pg_class_parser.o
	gcc $^ -I /usr/include/fuse3 -lfuse3 -o $@

main.o: main.c pg_class_parser.h
	gcc -c main.c -I /usr/include/fuse3

utils.o: utils.c pg_class_parser.h
	gcc -c utils.c

pg_class_parser.o: pg_class_parser.c pg_class_parser.h
	gcc -c pg_class_parser.c

mount: dbintro
	mkdir -p fuse
	./dbintro ./fuse || true

umount:
	umount ./fuse || true
	if [ -d "fuse" ]; then rmdir fuse; fi

clean:
	rm -f dbintro *.o
