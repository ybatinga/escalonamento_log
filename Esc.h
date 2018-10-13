/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Esc.h
 * Author: ubuntu
 *
 * Created on October 6, 2018, 4:26 PM
 */


#ifndef ESC_H
#define ESC_H
#include <vector>
#include "Tx.h"
#include "Log.h"
using namespace std;
class Esc {
public:
    Esc();
    Esc(vector<Tx> escList, vector<int> idList) ;
    virtual ~Esc();
    void SetEscList(vector<Tx> escList);
    vector<Tx> GetEscList();
    void SetSerial(bool Serial);
    bool IsSerial();
    void SetIdList(vector<int> idList);
    vector<int> GetIdList();
    void SetLogList(vector<Log> logNaoSerialList);
    vector<Log> GetLogList();
    void SetEscListSort(vector<Tx> escListSortNS);
    vector<Tx> GetEscListSort() const;
private:
    bool isSerial; // serializavel se nao houver ciclo
    vector<Tx> escList; // transacoes de um escalonamento
    vector<int> idList; // ids unicos de transacoes de escalonamento
    // lista para escrita em log de transacoes de escalonamento nao serializavel: 
    // primeiro somente transacoes 1 executam; em seguida somente transacoes 2 executam; em seguida somente transacoes 3 executam; etc.
    vector<Log> logList; 
    vector<Tx> escListSort; // lista de transacoes de escalonamento nao serial ordenadas por txId

};

#endif /* ESC_H */

