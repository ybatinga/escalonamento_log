/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TxIdx.h
 * Author: ubuntu
 *
 * Created on October 12, 2018, 3:08 PM
 */

#ifndef TXIDX_H
#define TXIDX_H

class TxIdx {
public:
    TxIdx();
    TxIdx(const TxIdx& orig);
    TxIdx(int id, bool noW);

    virtual ~TxIdx();
    void SetNoW(bool noW);
    bool IsNoW() const;
    void SetId(int id);
    int GetId() const;
private:
    int id; // id da transacao
    bool noW; // flag para rejeitar transacao se na transacao nao houver write

};

#endif /* TXIDX_H */

