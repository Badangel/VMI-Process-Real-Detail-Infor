#!/usr/bin/python
# -*- coding: utf-8 -*-
import SocketServer 
class Globalvar:
    exitstat = True
    globalser = None
    socketserver = True
    tainover = True

def setexit():
    print "exit chang False!!"
    Globalvar.exitstat=False

def setexitT():
    print "exit chang True!!"
    Globalvar.exitstat=True

def getexit():
    return Globalvar.exitstat

def setser(ser):
    print "set ser port: ",ser.port
    Globalvar.globalser=ser

def getser():
    return Globalvar.globalser

def setsocketstate():
    print "socket start!"
    Globalvar.socketserver = False

def getsocketstate():
    return Globalvar.socketserver

def clocksocketstate():
    print "socket close!"
    Globalvar.socketserver = True

def settrainover():
    Globalvar.tainover = False

def gettrainover():
    return Globalvar.tainover