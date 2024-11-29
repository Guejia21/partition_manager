all: main.o mbr.o gpt.o
	gcc -o listpart main.o mbr.o gpt.o -lm

%.o: %.c
	gcc -g -c -o $@ $<

doc:
	doxygen

clean:
	rm -rf *.o listpart docs


install: all
	sudo cp listpart /usr/local/bin

uninstall:
	sudo rm -f /usr/local/bin/listpart