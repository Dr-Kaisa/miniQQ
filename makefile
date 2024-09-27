ser=server/
con=common/
cli=client/

client: ${cli}main.c ${cli}utils.c ${cli}menu.c ${cli}myClient.c ${cli}start.c ${con}cJSON.c ${con}myNetwork.c
	gcc ${cli}main.c ${cli}utils.c ${cli}menu.c ${cli}myClient.c ${cli}start.c ${con}cJSON.c ${con}myNetwork.c  -o ${cli}main

server: ${ser}myService.c ${ser}main.c ${ser}myDB.c ${con}cJSON.c ${con}myNetwork.c ${con}myThread.c
	gcc ${ser}myService.c ${ser}main.c ${ser}myDB.c ${con}cJSON.c ${con}myNetwork.c ${con}myThread.c -o ${ser}main -lmysqlclient
	


.PHONY : clean	
clean:
		rm m2 main