/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Log.h
 *
 * Created on October 7, 2018, 1:25 AM
 */

#ifndef LOG_H
#define LOG_H
#include<string> // for string class
using namespace std;

class Log {
public:
    Log();
    Log(int ts, string txId, string op);
    Log(int ts, string txId, string op, string at, string wr);

    void setTs(int ts);
    int getTs() const;
    void setTxId(string txId);
    string getTxId() const;
    string getOp();
    void setOp(string op);
    void setValRes(string valRes);
    string getValRes() const;
    void setValIni(string valIni);
    string getValIni() const;
    


private:
	int ts; // tempo de stamp da transacao em log
	string txId; // identificador da transação
	string op; // operação 
	string valIni; // valor de inicio
	string valRes; // valor atualizado
};

#endif /* LOG_H */

