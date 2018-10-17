#include<stdio.h>
#include<string>
using namespace std;
#include "Tx.h"
#include "Esc.h"
#include "Graph.h"
#include "TxIdx.h"
#include <vector>
#include <algorithm> // std::find

#define ARQUIVO_ENTRADA "teste.in" // nome do arquivo de entrada
#define ARQUIVO_SAIDA "teste.sol" // nome do arquivo de saída
#define COMMIT "C" 
#define ABORT "A" 
#define WRITE "W"
#define READ "R"
#define START "start"
#define NULO "NULL"
#define TX "T"

int tc; // tempo de chegada
int id; // identificador da transação
char op[5]; // operação (R=read, W=write, C=commit)
char at[5]; // atributo lido/escrito
char wr[5]; // em caso de operacao write
vector<Tx> txList; // lista de transacoes
vector<Esc> escListList; // lista de escalonamentos 
vector<Log> logList; 

/*
 * função para criação de arquivo de entrada com dados fornecidos
 */
void criarArquivoEntrada();
/*
 * carrega transacoes de arquivo de entrada
 */
void carregaArquivoEntrada();
/*
 * triagem grupo de transacoes de escalonamento
 */
void triagemEscalonamento();
/*
 * insere transacoes em grafo de acordo com regras de conflito de serializacao para verificacao de seriabilidade quanto ao conflito
 */
void testeSeriabilidadeConflito(unsigned idx, Esc *esc);
/*
 * ordena transacoes de escalonamento nao serializavel para escrita em log 
 * em escalonamento nao serilizavel, executa-se primeiro todas as transacoes 1, 
 * em seguida executa-se todas as transacoes 2, e assim por diante
 * se o escalonamento for serializavel, copia-se as transacoes para a lista sem fazer ordenacao
*/
void ordenaEscNaoSerial(unsigned idx, Esc *esc);
/*
 * cria um indexador em forma de lista identificando qual transacao em um escalonamento possui uma operacao Write
 */
void verificaTxWrite(unsigned idx, Esc *esc);
/*
* gera log de escalonamento 
*/
void geraLog(unsigned idx, Esc esc);

int main()
{
//    criarArquivoEntrada();
    carregaArquivoEntrada();
    triagemEscalonamento();
    for (unsigned i = 0; i < escListList.size(); i++){
        testeSeriabilidadeConflito(i, &escListList.at(i));
        verificaTxWrite(i, &escListList.at(i));
        ordenaEscNaoSerial(i, &escListList.at(i));

    }
    for (unsigned i = 0; i < escListList.size(); i++){
        geraLog(i, escListList.at(i));
    }
    return 0;
}

int geraTimestamp ()
/*
* gera log de escalonamento nao seriallizavel
*/
void geraLog(unsigned idx, Esc esc){
    for (int i = 0; i < esc.GetEscListSort().size(); i++){
        string op;

        if (esc.GetEscListSort().at(i).getOp() == COMMIT){
            op = "commit";
            int ts = 1; 
            if (!logList.empty()
                    &&
                logList.at(logList.size()-1).getTs() >= esc.GetEscListSort().at(i).getTc()
            )
                ts = logList.size()+1;
            else
                ts = esc.GetEscListSort().at(i).getTc();
            Log log(ts, // tempo de chegada
                TX+to_string(esc.GetEscListSort().at(i).getId()), // identificador da transação 
                op // operação   
            );
            logList.push_back(log);
        }else
        if (esc.GetEscListSort().at(i).getOp() == ABORT){
            op = "abort";
            int ts = 1; 
            if (!logList.empty()
                    &&
                logList.at(logList.size()-1).getTs() >= esc.GetEscListSort().at(i).getTc()
            )
                ts = logList.size()+1;
            else
                ts = esc.GetEscListSort().at(i).getTc();
            Log log(ts, // tempo de chegada
                TX+to_string(esc.GetEscListSort().at(i).getId()), // identificador da transação 
                op // operação   
            );
            logList.push_back(log);
        }else
        if (esc.GetEscListSort().at(i).getOp() == READ){
            // flag para indicar que na lista de log "logList" existe transacao start de algum atributo 
            bool hasTxIdStart = false;
            // entra em if somente se a lisa de log nao serializavel estiver nao estiver vazia
            if (!logList.empty()){
                // percorre itens da lista de log nao serializavel por txId
                    // percorre itens da lista de log nao serializavel
                    for (int k = 0; k < logList.size(); k++){
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
                        int ts = 1; 
                        if (!logList.empty()
                                &&
                            logList.at(logList.size()-1).getTs() >= esc.GetEscListSort().at(i).getTc()
                        )
                            ts = logList.size()+1;
                        else
                            ts = esc.GetEscListSort().at(i).getTc();
                        Log log(ts, // tempo de chegada
                          TX+to_string(esc.GetEscListSort().at(i).getId()), // identificador da transação 
                          op // operação   
                        );
                        logList.push_back(log);
                    }
            
            }else{ // entra em else se a lisa de log nao serializavel estiver estiver vazia
                op = "start";
                int ts = 1; 
                if (!logList.empty()
                        &&
                    logList.at(logList.size()-1).getTs() >= esc.GetEscListSort().at(i).getTc()
                )
                    ts = logList.size()+1;
                else
                    ts = esc.GetEscListSort().at(i).getTc();
                Log log(ts, // tempo de chegada
                    TX+to_string(esc.GetEscListSort().at(i).getId()), // identificador da transação 
                    op // operação   
                );
                logList.push_back(log);
            }
        }
        else
        if (esc.GetEscListSort().at(i).getOp() == WRITE){

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
                        int ts = 1; 
                        if (!logList.empty()
                                &&
                            logList.at(logList.size()-1).getTs() >= esc.GetEscListSort().at(i).getTc()
                        )
                            ts = logList.size()+1;
                        else
                            ts = esc.GetEscListSort().at(i).getTc();
                        Log log(ts, // tempo de chegada
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
                        int ts = 1; 
                        if (!logList.empty()
                                &&
                            logList.at(logList.size()-1).getTs() >= esc.GetEscListSort().at(i).getTc()
                        )
                            ts = logList.size()+1;
                        else
                            ts = esc.GetEscListSort().at(i).getTc();
                        Log log(ts, // tempo de chegada
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

                    int k = logList.empty()? 1 : logList.size()+1;
                    string o = TX+to_string(esc.GetEscListSort().at(i).getId());
                    string j = esc.GetEscListSort().at(i).getAt();
                    string d = esc.GetEscListSort().at(i).getWr();
                    int ts = 1; 
                    if (!logList.empty()
                            &&
                        logList.at(logList.size()-1).getTs() >= esc.GetEscListSort().at(i).getTc()
                    )
                        ts = logList.size()+1;
                    else
                        ts = esc.GetEscListSort().at(i).getTc();
                    Log log(ts, // tempo de chegada
                            TX+to_string(esc.GetEscListSort().at(i).getId()), 
                            esc.GetEscListSort().at(i).getAt(), 
                            NULO, 
                            esc.GetEscListSort().at(i).getWr());
                    logList.push_back(log);
            }
            
        }        
    }
    escListList.at(idx).SetLogList(logList);
}

/*
 * cria um indexador em forma de lista identificando qual transacao em um escalonamento possui uma operacao Write
 */
void verificaTxWrite(unsigned idx, Esc *esc){
    vector<int> idNoWList; // ids que nao possuem operacao Write
    for (int i = 0; i <esc->GetIdList().size(); i++){
        bool hasW = false;
        for (int j = 0; j < esc->GetEscList().size(); j++){
            if (esc->GetEscList().at(j).getId() == esc->GetIdList().at(i)){
                if (esc->GetEscList().at(j).getOp() == WRITE){
                    hasW = true;
                }
            }
        }
        if (!hasW){
            idNoWList.push_back(esc->GetIdList().at(i));
            escListList.at(idx).SetIdNoWList(idNoWList);
        }
    }
    idNoWList.clear();
}

/*
 * ordena transacoes de escalonamento nao serializavel para escrita em log 
 * em escalonamento nao serilizavel, executa-se primeiro todas as transacoes 1, 
 * em seguida executa-se todas as transacoes 2, e assim por diante
 * se o escalonamento for serializavel, copia-se as transacoes para a lista sem fazer ordenacao
*/
void ordenaEscNaoSerial(unsigned idx, Esc *esc){
    vector<Tx> escListSort; // lista de transacoes de escalonamento nao serial ordenadas por txId
    
    // se escalonamento nao for serializavel, ordena transacoes por txId e passa escalonamento para lista EscListSort
    if (!esc->IsSerial()){
        for (int i = 0; i < esc->GetIdList().size(); i++){
            for (int j = 0; j < esc->GetEscList().size(); j++){
                if (esc->GetEscList().at(j).getId() == esc->GetIdList().at(i)){
                    if (!esc->GetIdNoWList().empty()){
                        for (int k = 0; k < esc->GetIdNoWList().size(); k++){
                            if (esc->GetEscList().at(j).getId() != esc->GetIdNoWList().at(k))
                                escListSort.push_back(esc->GetEscList().at(j));
                        }
                    }else{
                        escListSort.push_back(esc->GetEscList().at(j));
                    }
                    
                }
            }
        }
    }else {     // se escalonamento for serializavel, nao ordena transacoes por txId e passa escalonamento sem ordenacao alguma para lista EscListSort
        for (int i = 0; i < esc->GetEscList().size(); i++){ 
            for (int j = 0; j < esc->GetIdNoWList().size(); j++){
                if (esc->GetEscList().at(i).getId() != esc->GetIdNoWList().at(j))
                escListSort.push_back(esc->GetEscList().at(i));
            }
        }
    }
    escListList.at(idx).SetEscListSort(escListSort);
}

/*
* 
*/
//void writeLogIsSerial(Esc esc){
//    FILE *fptr = fopen(ARQUIVO_SAIDA, "w");
//	unsigned i;
//	for (i = 0; i < esc.GetEscList().size(); i++){
//            fprintf(fptr, "%d %s %s %s %s\n",
//                esc.GetEscList().at(i).getId(),
//                esc.GetEscList().at(i).getTxs().c_str(),
//                esc.GetEscList().at(i).getSr().c_str(),
//                esc.GetEscList().at(i).getVs().c_str());
//	}
//	fclose(fptr);
//}
/*
 * insere transacoes em grafo de acordo com regras de conflito de serializacao para verificacao de seriabilidade quanto ao conflito
 */
void testeSeriabilidadeConflito(unsigned idx, Esc *esc){
    string s ; // recebe resultado se NS ou SS
    unsigned i,j;  // counter
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
                        (txi.getOp() == "R" && txj.getOp() == "W")
                        ||
                        // aresta Ti -> Tj para cada r(x) em Tj depois de w(x) em Ti
                        (txi.getOp() == "W" && txj.getOp() == "R")
                        ||
                        // aresta Ti -> Tj para cada w(x) em Tj depois de w(x) em Ti
                        (txi.getOp() == "W" && txj.getOp() == "W")
                ){
                        // adiciona aresta em grafo
                        g->addEdge(txi.getId(), txj.getId());
                }
            }
        }
    }

    // aponta se existe ciclo no escalonamento
    if(g->isCyclic()){
            escListList.at(idx).SetSerial(false);
    }else{
            escListList.at(idx).SetSerial(true);
    }
    delete g;
}

/*
 * triagem grupo de transacoes de escalonamento
 */
void triagemEscalonamento(){
        vector<int> idList; // lista com identificadores unicos de transacoes de um grupo de escalonamento
        vector<Tx> escList; // lista de lista de grupo de transacoes de escalonamento
	unsigned i,j;  // counter

	Tx txAux;

	// percorre itens de transacoes da lista
	for(j = 0; j < txList.size(); j++) {
		int k = 0; // contador para iterador de lista de ids
		// percorre itens de transacoes da lista
		for(i = j; i < txList.size(); i++) {

			// usa-se txAux para comparar item anterior com atual na iteracao
			if (i != 0) {
				txAux = txList.at(i-1);
			}

			// fixa o for em transacao i para teste de conflito de seriazabilidade
			Tx txi = txList.at(i);

			escList.push_back(txi);

			// fonte para algoritmo 'find': http://www.cplusplus.com/reference/algorithm/find/
			// usado para contar quantidade de vertices em escalonamento
			vector<int>::iterator it;
			it = find (idList.begin(), idList.end(), txi.getId());
			if (it == idList.end()){
				idList.push_back(escList.at(k).getId());
			}
			k++;
			// se duas transacoes seguidas tiverem commit,
			// finaliza-se a triagem de agrupamento de transacoes de esconamento
			if (
				(txAux.getId() != 0)
					&&
				(txAux.getOp() == "C")
					&&
				(txi.getOp() == "C")
					||
				(txi.getOp() == "A")
			){
				j = i; // salva checkpoint para novo escalonamento
				break;
			}
		}
                Esc esc(escList, idList);                
		escListList.push_back(esc);
		escList.clear();
                idList.clear();
	}
}

/*
 * carrega transacoes de arquivo de entrada
 */
void carregaArquivoEntrada(){
	FILE *fptr = fopen(ARQUIVO_ENTRADA, "r");

//	fonte: https://support.microsoft.com/en-hk/help/60336/the-fscanf-function-does-not-read-consecutive-lines-as-expected
//	while (fscanf(stdin, "%d %d %[^ ] %[^ ] %[^\n]\n", &tc, &id, op, at, wr) != EOF) // carrega arquivo pela linha de comando no terminal
	while (fscanf(fptr, "%d %d %[^ ] %[^ ] %[^\n]\n", &tc, &id, op, at, wr) != EOF)
	{
		// carrega cada linha de arquivo de entrada em objeto Tx
		Tx tx (tc, id, op, at, wr);
		// insere objeto Tx em lista
		txList.push_back(tx);
//		printf("%d %d %s %s\n", tc, id, op, at);
	}
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
    fprintf(fptr, "%d %d %s %s %s\n", 5, 1, "C", "-", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 6, 2, "C", "-", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 7, 3, "R", "X", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 8, 3, "R", "Y", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 9, 4, "R", "X", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 10, 3, "W", "Y", "150");
    fprintf(fptr, "%d %d %s %s %s\n", 11, 4, "C", "-", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 12, 3, "C", "-", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 14, 5, "R", "Y", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 15, 6, "R", "Y", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 16, 6, "W", "Y", "450");
    fprintf(fptr, "%d %d %s %s %s\n", 19, 5, "W", "Y", "-50");
    fprintf(fptr, "%d %d %s %s %s\n", 20, 5, "C", "-", "-");
    fprintf(fptr, "%d %d %s %s %s\n", 21, 6, "A", "-", "-");
    fclose(fptr);

}
