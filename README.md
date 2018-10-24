CI 303/INFO7011 - Desempenho de Sistemas de Banco de Dados
Registro de modificações do banco de dados
2018-2

Entrega: 09/10
Objetivo:

Implementar uma estrutura de geração de registros de log das modificações de um banco de dados. Pra simplificar o trabalho e concentrar em conceitos de SGBDs, os programas não estarão em ambiente concorrente. Em vez disso, seu programa irá receber transações via stdin, armazenar as modificações em um arquivo de log e responder os valores válidos de atributos via stdout. LEMBRETE: a ordem dos registros de log seguem os conceitos dos testes de serialidade pro conflito e visão do primeiro trabalho.
Entrada:

A entrada deve ser feita pela entrada padrão (stdin). O arquivo é formado por uma sequência de linhas, onde cada linha representa uma transação chegando (parecido com o primeiro trabalho). Cada linha tem 5 campos: o primeiro campo é a linha do tempo que será usada no timestamp (TS) dos regitros de log, o segundo é o identificador da transação, o terceiro é a operação (R=read, W=write, C=commit, A=abort), o quarto é o atributo que será acessado e o quinto é o novo valor do atributo (somente no caso da operação W). Estas linhas estão ordenadas pelo primeiro campo simulando a linha do tempo.
Saída:
Serão esperadas 2 saídas de acordo com o executável. Os registros de log devem ser copiados para um arquivo enquanto que a visualização dos atributos será feita pela saída padrão (stdout).
Saída para arquivo de log:

Os registros de log deverão ser informados no arquivo chamado archive.log. Os comandos registrados em nosso arquivo de log possuem 2 formatos apresentados à seguir. Em ambos os formatos, os campos registrados são separados por ";" (ponto e vírgula). Os comandos start, commit e abort possuem 3 campos em seu formato: o timestamp do comando (TS), o identificador da transação (Ti) e o comando. Por ex., o registro de log "10; T4; start" significa que a transação 4 começou no momento 10. O ultimo comando indica alteração de um atributo do banco de dados e deve sempre ficar entre comandos start e commit/abort de uma mesma transação. Por ex., o registro de log "11; T4; X; 10; 20" informa que o atributo X possuía valor 10 no inicio da transação e será atualizado para valor 20 caso o proximo comando seja commit. Contudo, se o próximo comando for abort, então o valor de X continuará como 10.
< TS; Ti; start >

< TS; Ti; commit >

< TS; Ti; abort >

< TS; Ti; Xj; V1; V2 >

Saída com valores de atributos:

A visualização dos atributos será feita pela saída padrão (stdout). Este arquivo de saída será composto por uma sequência de linhas. Uma linha para cada atributo informando apenas o valor válido para ele simulando a instância atual do banco de dados. Cada linha tem 2 campos separados por ";" (ponto e vírgula). O primeiro campo é o atributo. O segundo campo é o valor atual no banco de dados.

Exemplo de arquivos com uma entrada e uma saída válida:
Entrada 	Arquivo de log 	Valores dos atributos
1 1 R X -
2 2 R X -
3 2 W X 100
4 1 W X 200
5 1 C - -
6 2 C - -
7 3 R X
8 3 R Y
9 4 R X -
10 3 W Y 150
11 4 C - -
12 3 C - -
14 5 R Y -
15 6 R Y -
16 6 W Y 450
19 5 W Y -50
20 5 C - -
21 6 A - -
	
1;T1;start
4;T1;X;NULL;200
5;T1;commit
6;T2;start
7;T2;X;200;100
8;T2;commit
9;T3;start
10;T3;Y;NULL;150
12;T3;commit
14;T5;start
19;T5;Y;150;-50
20;T5;commit
21;T6;start
22;T6;Y;-50;450
23;T6;abort
	
X;100
Y;-50
Requisitos mínimos:

O trabalho deve ser implementado em linguagem C de forma que possa ser compilado e executado nas servidoras de computação do Departamento de Informática.

O nome do executável deve ser escalona.

Não deve ter nenhuma opção de linha comando.

O que deve ser entregue:

Além dos arquivos fonte, deve acompanhar um makefile e um relatório com no máximo 2 páginas contendo a documentação sintetizada do sistema implementado. Qualquer particularidade deve estar descrita neste texto, como: algoritmo de detecção de ciclo em grafo.

Para compilar será usado o comando make (sem nenhum parâmetro), portanto preparem o Makefile para fazer isso mesmo que o programa seja implementado em linguagens interpretadas (e.g., Java).

Para testar será executado um script como o abaixo (o codigo abaixo "< teste.in > teste.out" significa entrada/saída padrão).

$ ./escalona < teste.in > teste.out
$ diff teste.sol teste.out

Onde teste.in é o arquivo de entrada do teste e teste.sol é o esperado como saída.
Caso o teste seja positivo (não imprime nada) será analisado o código fonte e o relatório.
Os fontes também devem ser comentados e a qualidade do codigo vale nota (e.g., Doxygen, Javadoc).
Equipe:

O trabalho é individual.
Forma de entrega:

O trabalho deve ser empacotado em um arquivo login.tar.gz, onde "login" é uma string com o login do aluno nas servidoras do DInf. Ao descompactar este arquivo deverá ser criado um diretório de nome "login" que conterá todos os demais arquivos. O make e o script acima deverão funcionar dentro deste diretório (não em subdiretórios). Por exemplo, o aluno com login GRR2000 enviará o arquivo GRR2000.tar.gz que deverá descompactar seus arquivos e subdiretorios no diretorio "GRR2000/"

O arquivo "...tar.gz" deve ser enviado por e-mail ao endereço do professor com o assunto "INFO7011-trab2" (exatamente). IMPORTANTE: Minha caixa de email usa o assunto do email como filtro.