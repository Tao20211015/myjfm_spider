.PHONY: all

all:
	cd adns && \
		./configure --disable-dynamic --prefix=`pwd` --exec-prefix=`pwd` && \
		make && \
		make install && \
		cd ..
