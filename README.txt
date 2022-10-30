--Grupo 58
-Luís Santos 56341
-Afonso Aleluia 56363
-Daniel Marques 56379

O accept() nem sempre funciona, apesar de aparentar estar correto. Foi colocado
um print logo no início do loop infinito do network_main_loop(). Se, depois de 
conectar o cliente, este print for executado mais uma vez, torna-se impossível
de conectar e é preciso abortar a execução de ambos os programas e voltar a tentar.

O comportamento dos programas é errático, às vezes lança um erro de "Bad address" ou 
"Bad file descriptor", outras vezes não.

As mensagens nunca parecem chegar ao invoke() do tree_skel, logo, mesmo se não houver
nenhum erro do lado do cliente, se fizer put, por exemplo, não é colocada nenhuma entrada
na árvore.

No makefile, tivemos problemas a compilar o o sdmessage.proto porque o #include no .c
incluía a diretória onde o .h e o .c foram compilados mas era preciso mover um deles para
outra pasta. Por isso, os ficheiros sdmessage.pb-c já vão compilados.