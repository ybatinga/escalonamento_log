/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Log.cpp
 * Author: ubuntu
 * 
 * Created on October 7, 2018, 1:25 AM
 */

#include "Log.h"

Log::Log(){
}
Log::Log(int ts, string txId, string op){
    this->ts = ts;
    this->txId = txId;
    this->op = op;
}
Log::Log(int ts, string txId, string op, string valIni, string valRes){
    this->ts = ts;
    this->txId = txId;
    this->op = op;
    this->valIni = valIni;
    this->valRes = valRes;
}

void Log::setTs(int ts) {
    this->ts = ts;
}

int Log::getTs() const {
    return ts;
}

void Log::setTxId(string txId) {
    this->txId = txId;
}

string Log::getTxId() const {
    return txId;
}

string Log::getOp() {
	return op;
}

void Log::setOp(string op) {
	this->op = op;
}

void Log::setValRes(string valRes) {
    this->valRes = valRes;
}

string Log::getValRes() const {
    return valRes;
}

void Log::setValIni(string valIni) {
    this->valIni = valIni;
}

string Log::getValIni() const {
    return valIni;
}



