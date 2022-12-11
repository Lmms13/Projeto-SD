--Grupo 58
-Luís Santos 56341
-Afonso Aleluia 56363
-Daniel Marques 56379

Possíveis erros no delete, particularmente numa estrutura do tipo:
a
 \
  b
   \
    c
i.e. sempre para a direita, em que, ao apagar 'a', 'b' passa a ser
a root mas 'c' desaparece.

Em algumas circunstâncias, os valores das estruturas 'data' ficavam
"corrompidos", provavelmente por má gestão de memória. Em que,
se o valor fosse 'abc' e o datasize fosse, consequentemente, 3,
o valor apresentado no cliente mostraria 'abc' com símbolos 
aleatórios à frente, por exemplo 'abc&% ,$)'. Assim, apenas são 
mostrados ao cliente os primeiros 'datasize' caracteres, neste 
caso 3, logo, 'abc'.