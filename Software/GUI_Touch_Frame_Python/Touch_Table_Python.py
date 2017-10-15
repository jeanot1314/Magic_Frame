#!/usr/bin/python


import sys, time, threading, random, queue
from PyQt4 import QtGui, uic, QtCore as qt
import serial

#SERIALPORT = 'COM6'
SERIALPORT = "/dev/ttyACM0"

dataX = 10
dataY = 10

class GuiPart(QtGui.QMainWindow):

    def __init__(self, queue, endcommand, *args):
        QtGui.QMainWindow.__init__(self, *args)
        self.setWindowTitle('Arduino Serial Demo')
        self.queue = queue
        # We show the result of the thread in the gui, instead of the console
        window = QtGui.QMainWindow()
        window.setGeometry(0,0,2000,1000)
        self.pic = QtGui.QLabel(window)
        self.pic.setGeometry(70,0,2000,1000)
        self.pic2 = QtGui.QLabel(window)
        self.pixmap = QtGui.QPixmap("fond-ecran-univers.jpg")
        self.pixmap = self.pixmap.scaledToHeight(1500)
        
        self.editor = QtGui.QTextEdit(self)
        self.editor.setGeometry(0,0,70,1000)

        self.pic.setPixmap(self.pixmap)
        
        self.setCentralWidget(window)
        self.endcommand = endcommand


    """
    def __init__(self, queue, endcommand, *args):
        QtGui.QMainWindow.__init__(self, *args)
        self.setWindowTitle('Arduino Serial Demo')
        self.queue = queue
        # We show the result of the thread in the gui, instead of the console
        self.editor = QtGui.QTextEdit(self)
        self.setCentralWidget(self.editor)
        self.endcommand = endcommand    
    """

    def closeEvent(self, ev):
        self.endcommand()

    def processIncoming(self):
        """
        Handle all the messages currently in the queue (if any).
        """
        while self.queue.qsize():
            try:
                
                msg = self.queue.get(0)
                # Check contents of message and do what it says
                # As a test, we simply print it
                data = str(msg, 'UTF8')
                print(data)
                header = data[:2]
                dataX = data[2:5]
                dataY = data[7:10]
                
                dataX = int(dataX)
                dataY = int(dataY)
                
                dataX = dataX - 100
                dataY = dataY - 120

                
                
                dataX = (10*dataX)
                dataY = (4*dataY)

                dataX = dataX - 100
                

                print(dataY)

                self.editor.insertPlainText("  X = ")
                self.editor.insertPlainText(str(dataX))
                self.editor.insertPlainText("  Y = ")
                self.editor.insertPlainText(str(dataY))
                               
                
                #self.pic.cercle = QtGui.QPainter(self)
                #self.pic.cercle.begin(self)
                
                self.cursor = QtGui.QPixmap("cursor.jpg")
                self.cursor = self.cursor.scaledToHeight(30)
                self.pic2.setGeometry(dataX-15,dataY-15, dataX+15,dataY+15)
                self.pic2.setPixmap(self.cursor)
                
                """
                qp = QtGui.QPainter(self)
                qp.setBrush(QtGui.QColor(000,255,255))
                qp.drawRect(100, 100, 300, 300)
                self.pic.addWidget(qp)
                """
                
            except Queue.Empty:
                pass

class ThreadedClient:
    """
    Launch the main part of the GUI and the worker thread. periodicCall and
    endApplication could reside in the GUI part, but putting them here
    means that you have all the thread controls in a single place.
    """
    def __init__(self):
        # Create the queue
        self.queue = queue.Queue()

        # Set up the GUI part
        self.gui=GuiPart(self.queue, self.endApplication)
        self.gui.showFullScreen()

        # A timer to periodically call periodicCall :-)
        self.timer = qt.QTimer()
        qt.QObject.connect(self.timer,
                           qt.SIGNAL("timeout()"),
                           self.periodicCall)
        # Start the timer -- this replaces the initial call to periodicCall
        self.timer.start(100)

        # Set up the thread to do asynchronous I/O
        # More can be made if necessary
        self.running = 1
        self.thread1 = threading.Thread(target=self.workerThread1)
        self.thread1.start()

    def periodicCall(self):
        """
        Check every 100 ms if there is something new in the queue.
        """
        self.gui.processIncoming()
        if not self.running:
            root.quit()

    def endApplication(self):
        self.running = 0


    def workerThread1(self):
        """
        This is where we handle the asynchronous I/O. 
        Put your stuff here.
        """
        while self.running:
            #This is where we poll the Serial port. 
            #time.sleep(rand.random() * 0.3)
            #msg = rand.random()
            #self.queue.put(msg)
            ser = serial.Serial(SERIALPORT, 9600)
            msg = ser.readline();

            data = str(msg, 'UTF8')
            header = data[:2]
            print(header)
            #header = int(header)

            
            if (header == 'XX'):
                self.queue.put(msg)
               
            else: pass  
            ser.close()

#rand = random.Random()
root = QtGui.QApplication(sys.argv)
client = ThreadedClient()
sys.exit(root.exec_())
