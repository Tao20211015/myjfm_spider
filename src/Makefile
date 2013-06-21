#Program, flags, etc.
CC			= g++
CFLAGS	= -Wall -c -g
LD			= g++
LDFLAGS	= -lpthread

OBJS		= myjfm_spider.o utility.o hash.o global.o shared.o \
					thread.o threadpool.o threadtask.o
BIN			= myjfm_spider

UTILITY_UT_OBJS	= utility.o utility_ut.o
UTILITY_UT_BIN	= utility_ut

THREAD_TEST_OBJS	= thread_test.o thread.o threadpool.o threadtask.o shared.o
THREAD_TEST_BIN	= thread_test

export OUTPUT_PATH=../output

.PHONY : everything clean realclean all

everything : $(BIN) $(UTILITY_UT_BIN) $(THREAD_TEST_BIN) others

all : clean everything

clean : 
	rm -rf $(OBJS)
	rm -rf $(UTILITY_UT_OBJS)
	rm -rf $(THREAD_TEST_OBJS)

realclean : 
	rm -rf $(OBJS) $(BIN)
	rm -rf $(OUTPUT_PATH)/$(BIN)
	rm -rf $(UTILITY_UT_OBJS) $(UTILITY_UT_BIN)
	rm -rf $(THREAD_TEST_OBJS) $(THREAD_TEST_BIN)

$(BIN) : $(OBJS)
	$(LD) $(LDFLAGS) -o $(BIN) $(OBJS)
	cp $(BIN) $(OUTPUT_PATH)/

$(UTILITY_UT_BIN) : $(UTILITY_UT_OBJS)
	$(LD) $(LDFLAGS) -o $(UTILITY_UT_BIN) $(UTILITY_UT_OBJS)

$(THREAD_TEST_BIN) : $(THREAD_TEST_OBJS)
	$(LD) $(LDFLAGS) -o $(THREAD_TEST_BIN) $(THREAD_TEST_OBJS)

others : 
	#cp plot.sh $(OUTPUT_PATH)

myjfm_spider.o : myjfm_spider.cc
	$(CC) $(CFLAGS) -o $@ $<

global.o : global.cc
	$(CC) $(CFLAGS) -o $@ $<

hash.o : hash.cc
	$(CC) $(CFLAGS) -o $@ $<

utility.o : utility.cc
	$(CC) $(CFLAGS) -o $@ $<

utility_ut.o : utility_ut.cc
	$(CC) $(CFLAGS) -o $@ $<

shared.o : shared.cc
	$(CC) $(CFLAGS) -o $@ $<

thread.o : thread.cc
	$(CC) $(CFLAGS) -o $@ $<

threadpool.o : threadpool.cc
	$(CC) $(CFLAGS) -o $@ $<

threadtask.o : threadtask.cc
	$(CC) $(CFLAGS) -o $@ $<

thread_test.o : thread_test.cc
	$(CC) $(CFLAGS) -o $@ $<

