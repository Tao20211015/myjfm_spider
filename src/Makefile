#Program, flags, etc.
CC			= g++
CFLAGS	= -Wall -c
LD			= g++
LDFLAGS	= -lpthread

OBJS		= myjfm_spider.o global.o utility.o smart.o
BIN			= myjfm_spider

UTILITY_UT_OBJS	= utility.o utility_ut.o
UTILITY_UT_BIN	= utility_ut

export OUTPUT_PATH=../output

.PHONY : everything clean realclean all

everything : $(BIN) $(UTILITY_UT_BIN) others

all : clean everything

clean : 
	rm -rf $(OBJS)
	rm -rf $(UTILITY_UT_OBJS)

realclean : 
	rm -rf $(OBJS) $(BIN)
	rm -rf $(OUTPUT_PATH)/$(BIN)
	rm -rf $(UTILITY_UT_OBJS) $(UTILITY_UT_BIN)

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

utility_ut.o : utility_ut.cc
	$(CC) $(CFLAGS) -o $@ $<

smart.o : smart.cc
	$(CC) $(CFLAGS) -o $@ $<

$(UTILITY_UT_BIN) : $(UTILITY_UT_OBJS)
	$(LD) $(LDFLAGS) -o $(UTILITY_UT_BIN) $(UTILITY_UT_OBJS)

