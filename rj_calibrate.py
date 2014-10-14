#!/usr/bin/env python
# rj_calibrate.py
# by Dave Cohen
# Load CSV files created by rjove_power
# and perform some math on them, like averaging,
# subtraction, etc

# imports
import csv
import numpy as np
import pylab as plt
import datetime
from matplotlib.dates import datestr2num


# globals
fileName = "data/testdata.csv"


def plotData(data, dbmin, dbmax):
    """
    Plots a 2D heatmap of the CSV array. dbmin is the lowest
    value (typically negative), and dbmax is the highest value
    (typically zero).
    """
    plot_title = "Heat Map of %s" %fileName
    plt.title( plot_title)
    plt.xlabel("Frequency")
    plt.ylabel("Time")
    im = plt.imshow(data, cmap='hot')
    plt.clim(dbmin,dbmax)
    plt.colorbar(im, orientation='horizontal')
    plt.show()


def getShape(array):
    """
    Returns number of rows and columns of an array
    as a tuple.
    """
    return array.shape

    
# determine the number of columns in file
with open(fileName) as f:
    ncols = len(f.readline().split(','))
f.close()

# Load the data array from the 6th column to the last column
data = np.loadtxt(fileName, delimiter=',', usecols=range(6,ncols))
# Load the values array from the 0th column to the 5th column
#timeValues = np.loadtxt(fileName, converters={0:lambda s:datestr2um(s)}, delimiter=',', usecols=range(0,1))

# Determine the size of the array 
numrows, numcols = getShape(data)

# Determine the max and min values of the array
db_max = np.amax(data)
db_min = np.amin(data)

print "Number of rows: %d" %numrows
print "Number of columns: %d" %numcols
print "Maximum db value: %f" %db_max
print "Minimum db value: %f" %db_min

plotData(data, -8, 0)
#plotData(db_min, db_max)