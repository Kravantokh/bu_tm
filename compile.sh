#gcc -Wall -std=c89 tests/fancy.c src/*.c dependencies/benutils_unicode/src/*.c -I dependencies/benutils_unicode/include -I include -o test.o

cmake -S . -B build -GNinja
