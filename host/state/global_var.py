#!/usr/bin/python
# -*- coding: utf-8 -*-
import SocketServer 
class Globalvar:
    exitstat = True
    globalser = None
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