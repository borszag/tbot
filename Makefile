objects = bot.o irc.o service.o

tbot : $(objects)
	cc -o bot $(objects)

bot.o : irc.h service.h
irc.o : irc.h
service.o : irc.h service.h

.PHONY : install clean

install : tbot
 
clean :
	-rm $(objects)