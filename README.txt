--Grupo 58
-Luís Santos 56341
-Afonso Aleluia 56363
-Daniel Marques 56379

Na linha 107 do network_server.c, "if(desc_set[i].revents & POLLIN)",
a condição nunca é cumprida, nunca entrando no if, e o grupo não 
conseguiu perceber porquê, visto que o código aparenta estar correto. 
Como esta instrução está perto do ponto de partida da interação entre 
o servidor e o(s) cliente(s), tornou-se impossível testar a interação 
completa, especialmente a parte do tree_skel.c.