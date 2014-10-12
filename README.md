rjove_power
===========

This is osmocom_spectrum_sense modified to produce output that can be turned into a spectrograph for Radio Jove work, which analyzes solar flares and storm activity from Jupiter.

The application sweeps a frequency range and reports dB output at each frequency interval. It will write to a CSV file, which can be read by heatmap.py.

In the future, it should work something like Jim Sky's Radio Ssky Spectrorgaph, producing waterfall type heatmaps that are stored in date-coded directories.

Gnu Radio is required to use this application.
