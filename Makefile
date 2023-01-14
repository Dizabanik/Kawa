all:
	gcc test/test.c
gitdev:
	git commit -m "kawa dev0.0.1(bugs, unoptimized)"
	git add test/test.c
	git push -u origin main
	git remote add origin https://github.com/Dizabanik/Kawa.git