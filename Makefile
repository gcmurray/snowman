OBJDIR = bin
VPATH = src
OBJ = main.o slist.o update.o palette.o
LIB = -L/usr/lib/x86_64-linux-gnu -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm
INC = -I/usr/include/
FLAG = -std=c11 -g -Wall

game: $(addprefix $(OBJDIR)/, $(OBJ))
	gcc -o game $^ $(INC) $(LIB) $(FLAG)

$(OBJDIR)/main.o: main.c main.h slist.h
	gcc -c $(INC) $(LIB) $(FLAG) $< -o $@

$(OBJDIR)/slist.o: slist.c slist.h structs.h main.h
	gcc -c $(FLAG) $< -o $@
	
$(OBJDIR)/update.o: update.c macro.h structs.h update.h
	gcc -c $(FLAG) $< -o $@
	
$(OBJDIR)/palette.o: palette.c palette.h
	gcc -c $(FLAG) $< -o $@
    
.PHONY : clean
clean:
	-rm game $(addprefix $(OBJDIR)/, $(OBJ))

