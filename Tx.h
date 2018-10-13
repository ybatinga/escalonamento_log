/*
 * Tx.h
 *
 * Tx (transacao) representa uma linha do arquivo de entrada
 *
 * Author: ELTON EIJI SASAKI
 *
 */

#ifndef TX_H_
#define TX_H_
#include<string> // for string class
using namespace std;
#define NAO_SERIAL "NS"

class Tx {
public:
    Tx();
    Tx(int tc, int id, string op, string at, string wr);

    int getTc();
    void setTc(int tc);
    int getId();
    void setId(int id);
    string getOp();
    void setOp(string op);
    string getAt();
    void setAt(string at);
    string getWr();
    void setWr(string wr);
    
    bool operator==(const Tx & obj2) const
    {
            if(this->op.compare(obj2.op) == 0)
                    return true;
            else
                    return false;
    }

private:
	int tc; // tempo de chegada
	int id; // identificador da transação
	string op; // operação (R=read, W=write, C=commit)
	string at; // atributo lido/escrito
	string wr; // em caso de operacao write
};

#endif /* TX_H_ */
