all:
	scons
clean:
	find -name "*~"|xargs rm -f
	find -name "*.o"|xargs rm -f
	find -name "*.a"|xargs rm -f
	find -name "*.lo"|xargs rm -f
	scons -c

dist: clean
	mkdir lincity-gotm
	cp * lincity-gotm -a || echo ""
	tar cfz lincity-gotm.tar.gz lincity-gotm
	rm lincity-gotm -rf
