/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TxIdx.cpp
 * Author: ubuntu
 * 
 * Created on October 12, 2018, 3:08 PM
 */

#include "TxIdx.h"

TxIdx::TxIdx() {
}

TxIdx::TxIdx(const TxIdx& orig) {
}

TxIdx::TxIdx(int id, bool noW){
    this->id = id;
    this->noW = noW;
}

TxIdx::~TxIdx() {
}

void TxIdx::SetNoW(bool noW) {
    this->noW = noW;
}

bool TxIdx::IsNoW() const {
    return noW;
}

void TxIdx::SetId(int id) {
    this->id = id;
}

int TxIdx::GetId() const {
    return id;
}

