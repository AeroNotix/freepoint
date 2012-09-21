setup:
	python setup.py py2exe

push:
	git push origin master
pull:
	git pull origin master
bigpush:
	git add .
	git commit -m "Big push"
	git push origin master
