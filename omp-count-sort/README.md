5.3 OpenMP Counting Sort
========================

Questão a
---------
As variáveis `i`, `j` e `count` devem ser privadas, ao passo que as
variáveis `a`, `n` e `temp` devem ser compartilhadas.

Questão b
---------
Não haverá dependência de loop, uma vez que nenhuma variável
compartilhada é alterada na mesma posição por duas threads. Vejamos, as
variáveis `a` e `n` nunca são escritas, portanto não há conflitos nela.
Já a variável `temp` é modificiada pelas threads, mas como cada elemento
do vetor tem exatamente uma posição bem definida, nenhuma thread irá
escrever na mesma posição que outra.

Questão c
---------
A chamada para `memcpy` não pode ser paralelizada, entretanto podemos
alterar o código para realizar esta operação em um loop; neste caso
podemos paraleliza-lo.

    #pragma omp parallel for shared(a, n, temp) private(i)
    for (i = 0; i < n; i++)
    	a[i] = temp[i];

Questão d
---------
O programa está em `main.c`.

Questão e
---------
O algoritmo de counting-sort dado é quadrático no número de elementos a
serem ordenados. Testes mostraram que, mesmo sendo paralelizado, o
algoritmo perde para o quick-sort, que é `n log n`. A tabela abaixo
mostra os resultados gerados ao executar o benchmark.

    Elements    1-Thread   2-Threads   3-Threads   4-Threads Quick-Sort
        1000     9160108     4669623     3164560     2956490     230942
       10000   915967266   473209914   310594737   244862219    1882896
      100000 91623892739 45809274943 30736720102 24242175754   22359816

E abaixo mostro o speedup em relação ao Counting-Sort com uma thread:

    Elements 1-Thread 2-Threads 3-Threads 4-Threads Quick-Sort
        1000      1.0      1.96      2.89      3.10      39.66
       10000      1.0      1.94      2.95      3.74     486.47
      100000      1.0      2.00      2.98      3.78    4097.70
