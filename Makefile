all:
	make pull
	make build
	cd resources; rcc resource.qrc -o resource.rcc
	cd cppclient; qmake -o Makefile freepoint.pro
	cd cppclient; make -f ./Makefile
	cd cppclient; rm *.o; rm ui_*; rm moc_*

setup:
	python2 setup.py py2exe

push:
	git push origin master

pull:
	git pull origin master

build:
	cd settings_server; go build settings_server.go
