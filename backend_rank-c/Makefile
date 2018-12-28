index:
	gcc -Wall -pedantic -g -o indexdb index-db.c index-tools.c hashtools.c docindex.c util.c

query:
	gcc -Wall -pedantic -g -o querydb query-db.c query-tools.c hashtools.c docindex.c util.c -lm

queryjs:
	gcc -Wall -pedantic -g -DJSON_OUT -o queryjson query-db.c query-tools.c hashtools.c docindex.c jsontools.c util.c -lm

queryweb:
	gcc -Wall -pedantic -g -DJSON_OUT -o ../queryjson query-db.c query-tools.c hashtools.c docindex.c jsontools.c util.c -lm

view:
	gcc -Wall -pedantic -g -o tableview tableview.c hashtools.c docindex.c
	
updateweb:
	cp -R data/ ../
	cp -R index-db/ ../
	cp stoplist_id ../
