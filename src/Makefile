#Program, flags, etc.
CC			= g++
CFLAGS	= -Wall -c
LD			= g++
LDFLAGS	= 

OBJS		= myjfm_spider.o global.o utility.o
BIN		= myjfm_spider

export OUTPUT_PATH=../output

.PHONY : everything clean realclean all

everything : $(BIN) others

all : clean everything

clean : 
	rm -rf $(OBJS)

realclean : 
	rm -rf $(OBJS) $(BIN)
	rm -rf $(OUTPUT_PATH)/$(BIN)

$(BIN) : $(OBJS)
	$(LD) $(LDFLAGS) -o $(BIN) $(OBJS)
	cp $(BIN) $(OUTPUT_PATH)/

others : 
	#cp plot.sh $(OUTPUT_PATH)

myjfm_spider.o : myjfm_spider.cc
	$(CC) $(CFLAGS) -o $@ $<

global.o : global.cc
	$(CC) $(CFLAGS) -o $@ $<

utility.o : utility.cc
	$(CC) $(CFLAGS) -o $@ $<
