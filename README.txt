--Grupo 58
-Luís Santos 56341
-Afonso Aleluia 56363
-Daniel Marques 56379

O nosso tree.c apenas passa a 2 testes. Pensamos que o problema
é principalmente o tree_put, que acaba por afetar os outros testes
porque é preciso colocar entradas na árvore, mas não conseguimos 
resolver o problema daí o código não funcionar como esperado.

Para além disso, no test_tree.c, na função testDelExistente(), 
no data_destroy(data) que está fora do loop, (linha 184 no código 
fornecido), é levantado um erro de double free. Ao passar a secção
'data = tree_get(tree,"a/key/b-1023"' que está dentro da atribução 
de valor ao 'result' para fora dessa atribuição, ou simplesmente 
comentando a linha 184, os testes são executados sem levantar esse 
erro. Também não conseguimos entender o que causava esse erro, logo
é provavel que a execução do test_tree seja abortada por um double free.