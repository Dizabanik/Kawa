all:
	gcc test/test.c
opt:
	gcc test/test.c -Ofast
size:
	gcc test/test.c -Oz
gitdev:
	git add test/test.c
	git commit -m --allow-empty-message ''
	git push -u origin main --force