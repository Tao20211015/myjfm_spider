#Program, flags, etc.
CC			= g++
CFLAGS	= -Wall -c -g
LD			= g++
LDFLAGS	= -lpthread

OBJS		= myjfm_spider.o utility.o global.o shared.o thread.o threadpool.o \
					threadtask.o site.o url.o md5.o
BIN			= myjfm_spider

UTILITY_UT_OBJS	= utility.o utility_ut.o
UTILITY_UT_BIN	= utility_ut

THREAD_TEST_OBJS	= thread_test.o thread.o threadpool.o threadtask.o shared.o
THREAD_TEST_BIN	= thread_test

HASH_TEST_OBJS	= hash_test.o shared.o
HASH_TEST_BIN	= hash_test

MD5_TEST_OBJS	= md5_test.o md5.o
MD5_TEST_BIN	= md5_test

URL_TEST_OBJS	= url_test.o url.o site.o md5.o
URL_TEST_BIN	= url_test

export OUTPUT_PATH=../output

.PHONY : everything clean realclean all

everything : $(BIN) $(UTILITY_UT_BIN) $(THREAD_TEST_BIN) \
	$(HASH_TEST_BIN) $(MD5_TEST_BIN) $(URL_TEST_BIN) others

all : clean everything

clean : 
	rm -rf $(OBJS)
	rm -rf $(UTILITY_UT_OBJS)
	rm -rf $(THREAD_TEST_OBJS)
	rm -rf $(HASH_TEST_OBJS)
	rm -rf $(MD5_TEST_OBJS)
	rm -rf $(URL_TEST_OBJS)

realclean : 
	rm -rf $(OBJS) $(BIN)
	rm -rf $(OUTPUT_PATH)/$(BIN)
	rm -rf $(UTILITY_UT_OBJS) $(UTILITY_UT_BIN)
	rm -rf $(THREAD_TEST_OBJS) $(THREAD_TEST_BIN)
	rm -rf $(HASH_TEST_OBJS) $(HASH_TEST_BIN)
	rm -rf $(MD5_TEST_OBJS) $(MD5_TEST_BIN)
	rm -rf $(URL_TEST_OBJS) $(URL_TEST_BIN)

$(BIN) : $(OBJS)
	$(LD) $(LDFLAGS) -o $(BIN) $(OBJS)
	cp $(BIN) $(OUTPUT_PATH)/

$(UTILITY_UT_BIN) : $(UTILITY_UT_OBJS)
	$(LD) $(LDFLAGS) -o $(UTILITY_UT_BIN) $(UTILITY_UT_OBJS)

$(THREAD_TEST_BIN) : $(THREAD_TEST_OBJS)
	$(LD) $(LDFLAGS) -o $(THREAD_TEST_BIN) $(THREAD_TEST_OBJS)

$(HASH_TEST_BIN) : $(HASH_TEST_OBJS)
	$(LD) $(LDFLAGS) -o $(HASH_TEST_BIN) $(HASH_TEST_OBJS)

$(MD5_TEST_BIN) : $(MD5_TEST_OBJS)
	$(LD) $(LDFLAGS) -o $(MD5_TEST_BIN) $(MD5_TEST_OBJS)

$(URL_TEST_BIN) : $(URL_TEST_OBJS)
	$(LD) $(LDFLAGS) -o $(URL_TEST_BIN) $(URL_TEST_OBJS)

others : 
	#cp plot.sh $(OUTPUT_PATH)

myjfm_spider.o : myjfm_spider.cc
	$(CC) $(CFLAGS) -o $@ $<

global.o : global.cc
	$(CC) $(CFLAGS) -o $@ $<

hash_test.o : hash_test.cc
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

site.o : site.cc
	$(CC) $(CFLAGS) -o $@ $<

url.o : url.cc
	$(CC) $(CFLAGS) -o $@ $<

url_test.o : url_test.cc
	$(CC) $(CFLAGS) -o $@ $<

md5.o : md5.cc
	$(CC) $(CFLAGS) -o $@ $<

md5_test.o : md5_test.cc
	$(CC) $(CFLAGS) -o $@ $<

