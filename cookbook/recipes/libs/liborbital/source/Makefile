.PHONY: install

install:
	mkdir -pv "$(DESTDIR)/include"
	mkdir -pv "$(DESTDIR)/lib"
	cp -v "include/orbital.h" "$(DESTDIR)/include"
	cp -v "target/$(HOST)/release/liborbital.a" "$(DESTDIR)/lib"
