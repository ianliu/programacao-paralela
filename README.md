MO802 - Programação Paralela
============================
Repositório de códigos para a matéria MO802 - Programação Paralela ministrada
pelo Prof. Guido Araújo e Prof. Nelson Amaral, na Unicamp em 2013, 1º semestre.

Pré-requisitos
--------------
Antes de compilar o código, é necessário realizar a instalação de alguns
pacotes. No Ubuntu 12.04, basta executar a seguinte linha de comando:

    sudo apt-get install openmpi-bin libopenmpi-dev cmake

Compilando
----------
Basta executar a linha de comando abaixo:

    cmake . && make

Executando
----------
Após compilado, os programas serão gerados nas subpastas. Todos eles deveriam
ter uma opção `-h` que exibe um help mostrando o funcionamento de cada um. No
caso do programa *histogram*, temos dois parâmetros `-n` e `-b`:

    $ histogram/histogram -h
    Usage: histogram/histogram [-h] | [-n DATA SIZE] [-b BUCKET SIZE]

Quando você decidir como quer executa-lo, basta utilizar o programa `mpirun`.
Ao executarmos o programa *histogram*, ele irá esperar por `n` entradas
inteiras. Veja alguns exemplos de execução:

    $ mpirun -np 10 histogram/histogram -n 10 -b 5
    1 2 3 4 5 6 7 8 9 10
    [Histogram result]
    1 2 2 2 3
    $ seq 1 10 | mpirun -np 10 histogram/histogram -n 10 -b 5
    [Histogram result]
    1 2 2 2 3
