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

Time-stamped files are now creted

Fixed problem where C++ callback crashes python program.

Uses UTC time-stamped files of length determined by user.

CURRENT STATUS 2014-10-12
I found out the problem with the HackRF hang - my cable. I've been testing
with a really short USB cable with very good results so far. I'm using a 
dummy load as an antenna, with very good results.

I also tested with a 6-foot cable with no problems. The cable that was giving
me isssues was a 20ft "amplified" USB cable.

TODO
Write external application to average "dark frames", and subtract them from
actual data.  So take several CSV files recorded usign a dummy load and
average them, and subtract (or divide) out of good data to remove DC
offset and other artifacts, just like one might do with an optical 
telescope.


Test frequency ranges that exceed the bandwidth of the SDR, as a lowwer
priority.

ISSUES

