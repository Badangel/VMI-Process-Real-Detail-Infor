#!/usr/bin/python
# -*- coding: utf-8 -*-
import SocketServer 
class Globalvar:
    exitstat = True
    globalser = None
    socketserver = True
def setexit():
    print "exit chang False!!"
    Globalvar.exitstat=False
def getexit():
    return Globalvar.exitstat
def setser(ser):
    print "set ser port: ",ser.port
    Globalvar.globalser=ser
def getser():
    return Globalvar.globalser
def setsocketstate():
    print "socket start!"
    socketserver = False
def getsocketstate():
    return Globalvar.socketserver
def clocksocketstate():
    print "socket clock!"
    socketserver = True