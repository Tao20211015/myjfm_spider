.PHONY: all realclean

all:
	cd adns && \
		./configure --disable-dynamic --prefix=`pwd` --exec-prefix=`pwd` && \
		make && \
		make install && \
		cd ..

realclean:
	rm -rf adns/bin/
	rm -rf adns/lib/
	rm -rf adns/include/
	rm -rf adns/Makefile
	rm -rf adns/config.cache
	rm -rf adns/config.log
	rm -rf adns/config.status
	rm -rf adns/settings.make
	rm -rf adns/src/*.o
	rm -rf adns/client/*.o
	rm -rf adns/regress/*.o
	rm -rf adns/src/Makefile
	rm -rf adns/src/libadns.a
	rm -rf adns/client/Makefile
	rm -rf adns/client/adnsheloex
	rm -rf adns/client/adnshost
	rm -rf adns/client/adnslogres
	rm -rf adns/client/adnsresfilter
	rm -rf adns/client/adnstest
	rm -rf adns/client/fanftest
	rm -rf adns/regress/Makefile
	rm -rf adns/regress/adnshost_playback
	rm -rf adns/regress/adnshost_record
	rm -rf adns/regress/adnslogres_playback
	rm -rf adns/regress/adnslogres_record
	rm -rf adns/regress/adnsresfilter_playback
	rm -rf adns/regress/adnsresfilter_record
	rm -rf adns/regress/adnstest_playback
	rm -rf adns/regress/adnstest_record
	rm -rf adns/dynamic/Makefile
