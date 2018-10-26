#include<stdio.h>
#include<string>
using namespace std;
#include "Tx.h"
#include "Esc.h"
#include "Graph.h"
#include "TxIdx.h"
#include "Log.h"
#include <vector>
#include <algorithm> // std::find
#include <cstring>
#include <list>

#define ARQUIVO_ENTRADA "teste.in" // nome do arquivo de entrada
#define ARQUIVO_SAIDA "teste.sol" // nome do arquivo de saída
#define ARQUIVO_SAIDA_LOG "archive.log" // nome do arquivo de saída
#define C "C" // commit
#define A "A" // abort
#define W "W" // write
#define R "R" // read
#define START "start"
#define NULO "NULL"
#define COMMIT "commit"
#define ABORT "abort"
#define TX "T" // transacao

vector<Tx> txList; // lista de transacoes
vector<Esc> escListList; // lista de escalonamentos 
vector<Log> logList; // lista de transacoes de log
vector<Log> saidaLogList; // lista de resultado de saida de log

/*
 * função para criação de arquivo de entrada com dados fornecidos
 */
void criarArquivoEntrada();
/*
 * carrega transacoes de arquivo de entrada
 */
void carregaArquivoEntrada();
/*
 * triagem de ids para lista de escalonamento
 */
void triagemIdsEsc();
/*
 * triagem grupo de transacoes de escalonamento
 */
void triagemEscalonamento();
/*
 * insere transacoes em grafo de acordo com regras de conflito de serializacao para verificacao de seriabilidade quanto ao conflito
 */
void testeSeriabilidadeConflito();
/*
 * ordena transacoes de escalonamento nao serializavel para escrita em log 
 * em escalonamento nao serilizavel, executa-se primeiro todas as transacoes 1, 
 * em seguida executa-se todas as transacoes 2, e assim por diante
 * se o escalonamento for serializavel, copia-se as transacoes para a lista sem fazer ordenacao
*/
void ordenaEscNaoSerial();
/*
 * cria um indexador em forma de lista identificando qual transacao em um escalonamento possui uma operacao Write
 */
void verificaTxWrite();
/*
* gera log de escalonamentos 
*/
void geraLog();
/*
 * gera timestamp para transacoes de log
 */
int geraTimestamp (int tc);
/*
 * salva transacoes de arquivo de log
 */
void salvaArquivoLog();
/*
 * salva transacoes de arquivo de log
 */
void geraSaidaLog();
/*
 * salva em arquivo valores de atributos executados em log
 */
void salvaArquivoAtributos();
void ordenaEsc();

int main()
{
//    criarArquivoEntrada();
    carregaArquivoEntrada();
    triagemIdsEsc();
    triagemEscalonamento();
    ordenaEsc();
    testeSeriabilidadeConflito();
    ordenaEscNaoSerial();
    geraLog();
    geraSaidaLog();
    salvaArquivoLog();
    salvaArquivoAtributos();
    return 0;
}

void ordenaEsc(){
    for (unsigned k = 0; k < escListList.size(); k++){
        vector<Tx> escList = escListList.at(k).GetEscList();
        int temp;
	for (unsigned j = 0; j < escList.size() - 1; ++j) {
            Tx min = escList.at(j);
            temp = j;
            for (unsigned i = j+1; i < escList.size(); ++i) {
                if (min.getTc() > escList.at(i).getTc()) {
                    min = escList.at(i);  
                    temp = i;
                    }
                }  
		swap(escList.at(j), escList.at(temp));
	}
        escListList.at(k).SetEscList(escList);
    }
}

/*
 * gera transacoes de arquivo de log
 */
void geraSaidaLog(){

    vector<Log> lgAbrtL; // armazena transacoes com abort
    // percorre a lista de log para armazenar transacoes com abort
    for (unsigned i = 0; i < logList.size(); i++){
        if (logList.at(i).getOp() == ABORT){
            lgAbrtL.push_back(logList.at(i));
        }
            
    }
    // percorre a lista de log
    for (unsigned i = 0; i < logList.size(); i++){
        // verifica se lista de saida esta vazia.
        // se estiver vazia, cria um novo registra na lista de saida
        if (logList.at(i).getValIni() == NULO
                &&
            logList.at(i).getValRes() != ""){
            saidaLogList.push_back(logList.at(i));
        }else // se nao estiver vazia, substitui o valor de resultado na lista de saida
        if (logList.at(i).getValIni() != ""
                &&
            logList.at(i).getValRes() != ""){
            bool isAbort = false;
            for (unsigned j = 0; j < lgAbrtL.size(); j++){ 
                if (lgAbrtL.at(j).getTxId() == logList.at(i).getTxId()){
                    isAbort = true;
                }
            }
            if (!isAbort){
                for (unsigned j = 0; j < saidaLogList.size(); j++){ 

                    if (
                        saidaLogList.at(j).getOp() == logList.at(i).getOp()
                    ){
                        saidaLogList.erase(saidaLogList.begin() + j);
                        saidaLogList.push_back(logList.at(i));
                    }
                }
            }
        }
    }
}
/*
 * salva transacoes de arquivo de log
 */
void salvaArquivoLog(){
    FILE *fptr = fopen(ARQUIVO_SAIDA_LOG, "w");

    for (unsigned i = 0; i < logList.size(); i++){
        
        int ts = logList.at(i).getTs(); // tempo de stamp da transacao em log
        char txId[9];// identificador da transação
	char op[9];// operação 
	char valIni[9];// valor de inicio
	char valRes[9];// valor atualizado
        strcpy(txId, logList.at(i).getTxId().c_str());
        strcpy(op, logList.at(i).getOp().c_str());
        strcpy(valIni, logList.at(i).getValIni().c_str());
        strcpy(valRes, logList.at(i).getValRes().c_str());
        if (logList.at(i).getValRes() != "")
            fprintf(fptr, "%d;%s;%s;%s;%s\n", ts, txId, op, valIni, valRes);
        else
            fprintf(fptr, "%d;%s;%s\n", ts, txId, op);
        
    }
    fclose(fptr);
}

/*
 * gera timestamp para transacoes de log
 */
int geraTimestamp (int tc){
    int ts = 1; 
    // se a lista nao estiver vazia, a transacao de timestamp recebe o valor de timestamp da transacao
    // ou se o valor de timestamp da transacao for menor que o timestamp da lista de log,
    // 
    if (!logList.empty()
            &&
        logList.at(logList.size()-1).getTs() >= tc
    )
        ts = logList.at(logList.size()-1).getTs() + 1;
    else // se a lista de log estiver vazia, o primeiro timestamp recebe valor 1
        ts = tc;
    
    return ts;
}
/*
* gera log de escalonamentos 
*/
void geraLog(){
    for (unsigned l = 0; l < escListList.size(); l++){
        Esc esc = escListList.at(l);
        for (unsigned i = 0; i < esc.GetEscListSort().size(); i++){
            string op;

            if (esc.GetEscListSort().at(i).getOp() == C){
                op = COMMIT;

                Log log(geraTimestamp(esc.GetEscListSort().at(i).getTc()), // tempo de chegada
                    TX+to_string(esc.GetEscListSort().at(i).getId()), // identificador da transação 
                    op // operação   
                );
                logList.push_back(log);
            }else
            if (esc.GetEscListSort().at(i).getOp() == A){
                op = ABORT;
                Log log(geraTimestamp(esc.GetEscListSort().at(i).getTc()), // tempo de chegada
                    TX+to_string(esc.GetEscListSort().at(i).getId()), // identificador da transação 
                    op // operação   
                );
                logList.push_back(log);
            }else
            if (esc.GetEscListSort().at(i).getOp() == R){
                // flag para indicar que na lista de log "logList" existe transacao start de algum atributo 
                bool hasTxIdStart = false;
                // entra em if somente se a lisa de log nao serializavel estiver nao estiver vazia
                if (!logList.empty()){
                    // percorre itens da lista de log nao serializavel por txId
                        // percorre itens da lista de log nao serializavel
                        for (unsigned k = 0; k < logList.size(); k++){
                            // entra em if se nao houver txId e "start" nao lista 
                            if (
                                logList.at(k).getTxId() == TX+to_string(esc.GetEscListSort().at(i).getId())
                                        &&    
                                logList.at(k).getOp() == "start"){
                                    hasTxIdStart = true;
                            }
                        }
                        if (!hasTxIdStart){
                            op = "start";
                            Log log(geraTimestamp(esc.GetEscListSort().at(i).getTc()), // tempo de chegada
                              TX+to_string(esc.GetEscListSort().at(i).getId()), // identificador da transação 
                              op // operação   
                            );
                            logList.push_back(log);
                        }

                }else{ // entra em else se a lisa de log nao serializavel estiver estiver vazia
                    op = "start";
                    Log log(geraTimestamp(esc.GetEscListSort().at(i).getTc()), // tempo de chegada
                        TX+to_string(esc.GetEscListSort().at(i).getId()), // identificador da transação 
                        op // operação   
                    );
                    logList.push_back(log);
                }
            }
            else
            if (esc.GetEscListSort().at(i).getOp() == W){

                // flag para indicar que na lista de log "logList" existe transacao com atributo 
                bool hasAtr = false;
                // percorre lista de id de transacoes do escalonamento para verificacao de existencia de operacoes Write
    //            for (int j = 0; j < esc.GetIdList().size(); j++){
                    // percorre lista lista de log para verificar se ja existe transacao de operacao Write para um id de transacao
                    for (int k = logList.size()-1; k >= 0; k--){
                        // verifica se logList ja tem transacao de operacao W e valor inicial nulo
                        if (
                                logList.at(k).getOp() == esc.GetEscListSort().at(i).getAt()
                                    &&
                                logList.at(k).getValIni() == NULO
    //                                &&
    //                            logList.at(k).getTxId() == to_string(esc.GetIdList().at(j))
                        ){
                            string valIni = logList.at(k).getValRes();
                            Log log(geraTimestamp(esc.GetEscListSort().at(i).getTc()), // tempo de chegada
                                TX+to_string(esc.GetEscListSort().at(i).getId()), // identificador da transação 
                                esc.GetEscListSort().at(i).getAt(), // operação   
                                valIni, // valIni
                                esc.GetEscListSort().at(i).getWr() // valRes
                            );
                            logList.push_back(log);
                            hasAtr = true;
                            break;
                        }else 
                        // verifica se logList ja tem transacao de operacao W e valor inicial nao nulo
                        if (
                            logList.at(k).getOp() == esc.GetEscListSort().at(i).getAt()
                                &&
                            logList.at(k).getValIni() != NULO
    //                            &&
    //                        logList.at(k).getTxId() == to_string(esc.GetIdList().at(j))
                        ){
                            string valIni = logList.at(k).getValRes();
                            Log log(geraTimestamp(esc.GetEscListSort().at(i).getTc()), // tempo de chegada
                                TX+to_string(esc.GetEscListSort().at(i).getId()), // identificador da transação 
                                esc.GetEscListSort().at(i).getAt(), // operação   
                                valIni, // valIni
                                esc.GetEscListSort().at(i).getWr() // valRes
                            );
                            logList.push_back(log);
                            hasAtr = true;
                            break;
                        }
                    }

    //            }
                if (!hasAtr){

//                        int k = logList.empty()? 1 : logList.size()+1;
//                        string o = TX+to_string(esc.GetEscListSort().at(i).getId());
//                        string j = esc.GetEscListSort().at(i).getAt();
//                        string d = esc.GetEscListSort().at(i).getWr();
                        Log log(geraTimestamp(esc.GetEscListSort().at(i).getTc()), // tempo de chegada
                                TX+to_string(esc.GetEscListSort().at(i).getId()), 
                                esc.GetEscListSort().at(i).getAt(), 
                                NULO, 
                                esc.GetEscListSort().at(i).getWr());
                        logList.push_back(log);
                }

            }        
        }
        escListList.at(l).SetLogList(logList);
    }
}

///*
// * cria um indexador em forma de lista identificando qual transacao em um escalonamento possui uma operacao Write
// */
//void verificaTxWrite(){
//    vector<int> idNoWList; // ids que nao possuem operacao Write
//    for (unsigned k = 0; k < escListList.size(); k++){
//        Esc* esc = &escListList.at(k);
//        for (unsigned i = 0; i <esc->GetIdList().size(); i++){
//            bool hasW = false;
//            for (unsigned j = 0; j < esc->GetEscList().size(); j++){
//                if (esc->GetEscList().at(j).getId() == esc->GetIdList().at(i)){
//                    if (esc->GetEscList().at(j).getOp() == W){
//                        hasW = true;
//                    }
//                }
//            }
//            if (!hasW){
//                idNoWList.push_back(esc->GetIdList().at(i));
//                escListList.at(k).SetIdNoWList(idNoWList);
//            }
//        }
//        idNoWList.clear();
//    }
//        
//}

/*
 * ordena transacoes de escalonamento nao serializavel para escrita em log 
 * em escalonamento nao serilizavel, executa-se primeiro todas as transacoes 1, 
 * em seguida executa-se todas as transacoes 2, e assim por diante
 * se o escalonamento for serializavel, copia-se as transacoes para a lista sem fazer ordenacao
*/
void ordenaEscNaoSerial(){
    
    for (unsigned l = 0; l < escListList.size(); l++){
        vector<Tx> escListSort; // lista de transacoes de escalonamento nao serial ordenadas por txId
        Esc* esc = &escListList.at(l);
        // se escalonamento nao for serializavel, ordena transacoes por txId e passa escalonamento para lista EscListSort
        if (!esc->IsSerial()){
            for (unsigned i = 0; i < esc->GetIdList().size(); i++){
                for (unsigned j = 0; j < esc->GetEscList().size(); j++){
                    if (esc->GetEscList().at(j).getId() == esc->GetIdList().at(i)){
//                        if (!esc->GetIdNoWList().empty()){
//                            for (unsigned k = 0; k < esc->GetIdNoWList().size(); k++){
//                                if (esc->GetEscList().at(j).getId() != esc->GetIdNoWList().at(k))
                                    escListSort.push_back(esc->GetEscList().at(j));
//                            }
//                        }else{
//                            escListSort.push_back(esc->GetEscList().at(j));
//                        }

                    }
                }
            }
        }else {     // se escalonamento for serializavel, nao ordena transacoes por txId e passa escalonamento sem ordenacao alguma para lista EscListSort
            for (unsigned i = 0; i < esc->GetEscList().size(); i++){ 
    //            for (int j = 0; j < esc->GetIdNoWList().size(); j++){
    //                if (esc->GetEscList().at(i).getId() != esc->GetIdNoWList().at(j))
                    escListSort.push_back(esc->GetEscList().at(i));
    //            }
            }
        }
        escListList.at(l).SetEscListSort(escListSort);
    }
}
/*
 * triagem grupo de transacoes de escalonamento
 */
void triagemEscalonamento(){
    for (unsigned i = 0; i < escListList.size(); i++){
        Esc esc = escListList.at(i);
        vector<Tx> escList;
        for (unsigned j = 0; j < esc.GetIdList().size(); j++){
//            for (int k = 0; k < esc.GetIdList().size(); k++){
                int id = esc.GetIdList().at(j);
                for (unsigned l = 0; l < txList.size();l++){
                    Tx tx = txList.at(l);
                    if (tx.getId() == id){
                        tx.setIdG(j);
                        escList.push_back(tx);
                    }
                }
//            }
            
        }
        escListList.at(i).SetEscList(escList);
    }
}

/*
 * triagem de ids para lista de escalonamento
 */
void triagemIdsEsc(){
    string s ; // recebe resultado se NS ou SS
    unsigned i,j;  // counter
    vector<int> idList; // lista com identificadores unicos de transacoes de um grupo de escalonamento
    
    // percorre itens de transacoes da lista
    for(i = 0; i < txList.size(); i++) {
//        Graph* g = new Graph(txList.size());
        // fixa o for em transacao i para teste de conflito de seriazabilidade
        Tx txi = txList.at(i);
        int k = 0; // contador para iterador de lista de ids
        for(j = i; j < txList.size(); j++){
            // fixa o for em transacao j para teste de conflito de seriazabilidade
            Tx txj = txList.at(j);

            // entra no if se tempo de chegada de transacoes
            // estao em ordem crescente,
            // o id das transacoes sao diferentes
            // e os atributos sao os mesmos
            if (txi.getTc() < txj.getTc()
                        &&
                txi.getId() != txj.getId()
                        &&
                txi.getAt() == txj.getAt()
            ){
                if (
                    // aresta Ti -> Tj para cada w(x) em Tj depois de r(x) em Ti
                    (txi.getOp() == R && txj.getOp() == W)
                    ||
                    // aresta Ti -> Tj para cada r(x) em Tj depois de w(x) em Ti
                    (txi.getOp() == W && txj.getOp() == R)
                    ||
                    // aresta Ti -> Tj para cada w(x) em Tj depois de w(x) em Ti
                    (txi.getOp() == W && txj.getOp() == W)
                ){
                    // fonte para algoritmo 'find': http://www.cplusplus.com/reference/algorithm/find/
                    // usado para contar quantidade de vertices em escalonamento
                    vector<int>::iterator it;
                    it = find (idList.begin(), idList.end(), txi.getId());
                    if (it == idList.end()){
                        idList.push_back(txi.getId());
                    }
                    // adiciona aresta em grafo
//                    g->addEdge(txi.getId(), txj.getId());
                    k++;
                }
                if (
                    // aresta Ti -> Tj para cada r(x) em Tj depois de r(x) em Ti
                    // essa regra nao se aplica a regra de teste de seriabilidade,
                    // se aplica somente para esta selecao de triagem de transacoes de escalonamento
                    (txi.getOp() == R && txj.getOp() == R)
                ){
                    // fonte para algoritmo 'find': http://www.cplusplus.com/reference/algorithm/find/
                    // usado para contar quantidade de vertices em escalonamento
                    vector<int>::iterator it;
                    it = find (idList.begin(), idList.end(), txi.getId());
                    if (it == idList.end()){
                        idList.push_back(txi.getId());
                    }
                    k++;
                }
            }
            
            // se houver transacao com commit,
            // finaliza-se a triagem de agrupamento de transacoes de escalonamento
            if (
                (txi.getOp() == C)
                        ||
                (txi.getOp() == A)
            ){
//                i = j; // salva checkpoint para novo escalonamento
                for (unsigned i = 0; i < idList.size(); i++){
                    if (idList.at(0) == txi.getId()){
                        Esc esc(idList);             
//                        // aponta se existe ciclo no escalonamento
//                        if(g->isCyclic()){
//                                esc.SetSerial(false);
//                        }else{
//                                esc.SetSerial(true);
//                        }
//                        delete g;
                        escListList.push_back(esc);
                        idList.clear();
                        
                        
                    }
                }
            }
        }
        
    }
}

/*
 * insere transacoes em grafo de acordo com regras de conflito de serializacao para verificacao de seriabilidade quanto ao conflito
 */
void testeSeriabilidadeConflito(){
    string s ; // recebe resultado se NS ou SS
    unsigned i,j;  // counter
    
    for (unsigned k = 0; k < escListList.size(); k++){
        Esc* esc = &escListList.at(k);
        Graph* g = new Graph(esc->GetEscList().size());
        // percorre itens de transacoes da lista
        for(i = 0; i < esc->GetEscList().size(); i++) {
            // fixa o for em transacao i para teste de conflito de seriazabilidade
            Tx txi = esc->GetEscList().at(i);
            for(j = i; j < esc->GetEscList().size(); j++){
                // fixa o for em transacao j para teste de conflito de seriazabilidade
                Tx txj = esc->GetEscList().at(j);

                // entra no if se tempo de chegada de transacoes
                // estao em ordem crescente,
                // o id das transacoes sao diferentes
                // e os atributos sao os mesmos
                if (txi.getTc() < txj.getTc()
                            &&
                    txi.getId() != txj.getId()
                            &&
                    txi.getAt() == txj.getAt()
            ){
                    if (
                            // aresta Ti -> Tj para cada w(x) em Tj depois de r(x) em Ti
                            (txi.getOp() == R && txj.getOp() == W)
                            ||
                            // aresta Ti -> Tj para cada r(x) em Tj depois de w(x) em Ti
                            (txi.getOp() == W && txj.getOp() == R)
                            ||
                            // aresta Ti -> Tj para cada w(x) em Tj depois de w(x) em Ti
                            (txi.getOp() == W && txj.getOp() == W)
                    ){
                            // adiciona aresta em grafo
                            g->addEdge(txi.getIdG(), txj.getIdG());
                    }
                }
            }
        }

        // aponta se existe ciclo no escalonamento
        if(g->isCyclic()){
                escListList.at(k).SetSerial(false);
        }else{
                escListList.at(k).SetSerial(true);
        }
        delete g;
    }
    
}
/*
 * salva em arquivo valores de atributos executados em log
 */
void salvaArquivoAtributos(){
//    FILE *fptr = fopen(ARQUIVO_SAIDA, "w");

    for (unsigned i = 0; i < saidaLogList.size(); i++){      
        char atr[9]; // atributo
	char val[9]; // valor de atributo
        strcpy(atr, saidaLogList.at(i).getOp().c_str());
        strcpy(val, saidaLogList.at(i).getValRes().c_str());
        // imprime arquivo atribuido pela variavel "FILE *fptr = fopen(ARQUIVO_SAIDA, "w");"
//        fprintf(fptr, "%s;%s\n", atr, val);
        // imprime um arquivo com nome designado via comando de terminal: "$ ./escalona < teste.in > teste.out" 
        fprintf(stdout, "%s;%s\n", atr, val);

    }
    // imprime arquivo atribuido pela variavel "FILE *fptr = fopen(ARQUIVO_SAIDA, "w");"
//    fclose(fptr);
}

/*
 * carrega transacoes de arquivo de entrada
 */
void carregaArquivoEntrada(){
//	FILE *fptr = fopen(ARQUIVO_ENTRADA, "r");
        char buf[20];
        char tc[5]; // tempo de chegada
        char id[5]; // identificador da transação
        char op[5]; // operação (R=read, W=write, C=commit)
        char at[5]; // atributo lido/escrito
        char wr[5]; // em caso de operacao write
        
//	fonte: https://support.microsoft.com/en-hk/help/60336/the-fscanf-function-does-not-read-consecutive-lines-as-expected
        // stdin eh usado para ler arquivo executado pela linha de comando via terminal
//	while (fscanf(stdin, "%d %d %[^ ] %[^ ] %[^\n]\n", &tc, &id, op, at, wr) != EOF) // carrega arquivo pela linha de comando no terminal
        // fptr usado para ler arquivo de variavel: "FILE *fptr = fopen(ARQUIVO_ENTRADA, "r");"
//	while (fscanf(fptr, "%d %d %[^ ] %[^ ] %[^\n]\n", &tc, &id, op, at, wr) != EOF)
        
//	while (fgets(buf,sizeof(buf), fptr) != NULL)
        // carrega cada linha do arquivo
        while (fgets(buf,sizeof(buf), stdin) != NULL)
	{       
            buf[strlen(buf) - 1] = '\0'; // eat the newline fgets() stores
            
            // inicia variaveis com vazio-nulo
            tc[0] = '\0';
            id[0] = '\0';
            op[0] = '\0';
            at[0] = '\0';
            wr[0] = '\0';
            
            // le cada linha
            sscanf(buf, "%s %s %s %s %s", tc, id, op, at, wr);
            string a;
            a += tc;
            string b;
            b += id;
            
            // carrega cada linha de arquivo de entrada em objeto Tx
            Tx tx (atoi(a.c_str()), atoi(b.c_str()), op, at, wr);
//		// insere objeto Tx em lista
            txList.push_back(tx);
//		printf("%d %d %s %s\n", tc, id, op, at);
	}
//        fclose(fptr);
}

/*
 * função para criação de arquivo de entrada com dados fornecidos
 */
void criarArquivoEntrada(){
    FILE *fptr = fopen(ARQUIVO_ENTRADA, "w");

    fprintf(fptr, "%d %d %s %s %s\n", 1, 1, "R", "X", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 2, 2, "R", "X", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 3, 2, "W", "X", "100");
    fprintf(fptr, "%d %d %s %s %s\n", 4, 1, "W", "X", "200");
    fprintf(fptr, "%d %d %s %s %s\n", 6, 2, "C", "-", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 7, 3, "R", "X", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 8, 3, "R", "Y", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 9, 4, "R", "X", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 5, 1, "C", "-", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 10, 3, "W", "Y", "150");
    fprintf(fptr, "%d %d %s %s %s\n", 11, 4, "C", "-", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 14, 5, "R", "Y", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 15, 6, "R", "Y", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 16, 6, "W", "Y", "450");
    fprintf(fptr, "%d %d %s %s %s\n", 12, 3, "C", "-", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 19, 5, "W", "Y", "-50");
    fprintf(fptr, "%d %d %s %s %s\n", 20, 5, "C", "-", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 21, 6, "A", "-", "-");
    fclose(fptr);

}
