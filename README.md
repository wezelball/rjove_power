rjove_power
===========

This is osmocom_spectrum_sense modified to produce output that can be turned
into a spectrograph for Radio Jove work, which analyzes solar flares and storm
activity from Jupiter.

The application sweeps a frequency range and reports dB output at each frequency
interval. It will write to a CSV file, which can be read by heatmap.py.

In the future, it should work something like Jim Sky's Radio Ssky Spectrorgaph,
producing waterfall type heatmaps that are stored in date-coded directories. 
Because this uses an Osmocom source, it suppoorts hardware other than just
RTL-SDR. This is currently being used with the HackRF.

Gnu Radio is required to use this application.

CURRENT STATUS 2014-10-12

A couple of hours of work has produced output that can be read by heatmap.py.  
The uppper frequency limit may be a little off, but the output looks good.

There is a problem where the C++ callback crashes the python program, and
thereby loses control of the process.  I have to kill the PID to get it
to stop.
