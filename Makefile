all:
	gcc kawa.c
opt:
	gcc kawa.c -Ofast
size:
	gcc kawa.c -Oz
gitdev:
	git add kawa.c
	git commit -m --allow-empty-message ''
	git push -u origin main --force