all:
	make pull
	make build

setup:
	python2 setup.py py2exe

push:
	git push origin master
pull:
	git pull origin master
bigpush:
	git add .
	git commit -m "Big push"
	git push origin master
build:
	cd settings_server; go build settings_server.go
