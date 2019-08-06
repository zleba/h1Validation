All files referenced in this text can be obtained from the H1 softwware
repository using this command:
  cvs co h1rivet
  cd h1rivet

How to run rivet on H1 machines
===============================
Rivet is not installed on the H1 machines, but you can use the
installation on cvmfs. To set this up, open a new terminal and type:
  source rivet_setup.sh
Now you can run rivet, for example like
 rivet -h
 rivet < hepmctest.txt -a H1_2000_S4129130 -o myRivetTest.yoda --ignore-beams
 rivet-mkhtml myRivetTest.yoda
Look at the file rivet_plots/index.html with a web-browser

In this example, rivet is run with the "--ignore-beams" command,
because the proton beam energy of the test events (920) and of the
analysis (820) do not match.

How to convert H1 MC files to the HepMC format
==============================================
Rivet reads events in the HepMC format.
There are two H1 specific programs to convert MC files to the HepMC format.
  h1oohepmc : to read hat+mods files (for simulated MC)
  dst2hepmc : to read FPACk files (for generator files or DST files)

At present, these programs are available only with release 4.0.25. To
set them up, login to naf-hone11.desy.de and type:
 h1oo -n 4.0.25
Unfortunately you can not run this together with the rivet executable
in the same terminal, because "h1oo" is not compativle with the
"rivet_setup.sh" explained above.


The programs can write out ASCII files in the HepMC format as follows:
  h1oohepmc -f readModsHat.steer -o hepmcfile.txt
or
  dst2hepmc < ReadFPACK.fpacksteer -o hepmcfile.txt

Be careful, the resulting "hepmcfile.txt" is very large, unless you
terminate the program with control-C.
Below a better method is explained for running with rivet. It uses a
unix pipe. 


How to directly pipe the converted H1 files into rivet
======================================================
This method runs "h1oohepmc" or "dst2hepmc" in parallel to Rivet. The
converted enets are not stored. Only the "yoda" file created by rivet
is kept.

(a) open a terminal and set up rivet, then create a unix pipe, then
  start rivet with input from the pipe
> source rivet_setup.sh
> rm -f /tmp/rivet.pipe
> mkfifo /tmp/rivet.pipe
> rivet < /tmp/rivet.pipe -a H1_2000_S4129130 --ignore-beams -o myRivetTest.yoda

(b) open another terminal, set up h1oo, convert H1 DST files directly
to the pipe
> h1oo -n 4.0.25
> dst2hepmc < ReadFPACK.fpacksteer -o /tmp/rivet.pipe

Events will start to flow. You can terminate the program in terminal
(b), it will also finish the rivet program (and close the files).


How to write a new  H1 Rivet analysis
=====================================
Have a look at the tutorial:
 https://indico.desy.de/indico/event/21792/
   ("Rivet Intro" talk by Andy Buckley)

In a nutshell: start with
  rivet-mkanalysis ANALYSIS_NAME
Be careful, this will overwrite the files
  ANALYSIS_NAME.info 
  ANALYSIS_NAME.plot 
  ANALYSIS_NAME.cc
  ANALYSIS_NAME.yoda
(your previous work may get lost!!!).
Edit the ANALYSIS_NAME.cc file, then run 
  rivet-buildplugin ANALYSIS_NAME
  rivet -a ANALYSIS_NAME" < rivet.pipe
Improve ANALYSIS_NAME.cc until you are happy with the results.

For H1 data analyses, replace "ANALYSIS_NAME" by
 H1_YYYY_IXXXXXXX
where:
 YYYY is the year
and:
 XXXXXXX is the Inspire-Id.

Example: start with the H1 list of papers
 http://www-h1.desy.de/publications/H1publication.short_list.html
select a paper (e.g. DESY-14-035).
Click on "_Spires", on inspire click on "Detailed Record".
The web browser opens: http://inspirehep.net/record/1288065?ln=en
So the Inspire-id is: 1288065
To import the analysi from HEPDATA, run the command:
  rivet-mkanalysis H1_2014_I1288065
This creates the files
 H1_2014_I1288065.yoda
 H1_2014_I1288065.cc
 H1_2014_I1288065.plot
 H1_2014_I1288065.info
The task is to implement the analysis in
   H1_2014_I1288065.cc
and to improve the plots (axis labels etc) in
   H1_2014_I1288065.plot
and to describe the analysis in
   H1_2014_I1288065.info

There is an example analysis available: H1_2016_I1496981
