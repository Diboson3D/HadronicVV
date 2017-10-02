#!/usr/bin/env python
import os, glob, sys
from commands import getoutput
import re

import thread
import subprocess

def waitForBatchJobs(runningJobs, listOfJobs, userName):
  print "waiting for %d job(s) in the queue" %(len(runningJobs))
  while not len(runningJobs)==0:
    time.sleep(30.)
    queryString="qstat -u %s | grep %s | awk {\'print $10\'}" %(userName, userName)
    lock=thread.allocate_lock()
    lock.acquire()
    compileProcess=subprocess.Popen(queryString, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    compileProcess.wait()
    lock.release()
    jobList = compileProcess.stdout.read()
    jobList = jobList.split("\n")
    for j in runningJobs:
      jobId=j[0]
      if (jobId not in jobList):
        for l in listOfJobs:
          if j[1]==(l[2]+l[3]):
            l[4]="."
        runningJobs.remove(j)
        print "waiting for %d job(s) in the queue" %(len(runningJobs))
		

def createJobs(i,f, outfolder, outname):
	fname = "submitVV.xml"
	
	print "Using config " , fname
	template=open("config/%s"%fname, 'r').read()
	template=template.replace('OUTPUT', ('<Cycle Name="VVanalysis" OutputDirectory="%s/" PostFix="" TargetLumi="1.0">')%outfolder)
	template=template.replace('INPUTHEADER', ('<InputData Lumi="0.0" NEventsMax="-1" NEventsSkip="0" Type="%s" Version="%s">')%(outname,i) )
	template=template.replace('INFILE', ('<In FileName="dcap://t3se01.psi.ch:22125/%s" Lumi="1.0" />')%f)
	if   f.find("JetHT")!=-1: 
		template=template.replace('ISSIGNAL', '<Item Name="IsSignal" Value="false"/>')
		template=template.replace('ISDATA'  , '<Item Name="IsData"   Value="true"/>')
	elif f.find("QCD")!=-1 or f.find("WJets")!=-1: 
		template=template.replace('ISSIGNAL', '<Item Name="IsSignal" Value="false"/>')
		template=template.replace('ISDATA'  , '<Item Name="IsData"   Value="false"/>')
	else:
		template=template.replace('ISSIGNAL', '<Item Name="IsSignal" Value="true"/>')
		template=template.replace('ISDATA'  , '<Item Name="IsData"   Value="false"/>')
	
	try: os.stat("xmls") 
	except: os.mkdir("xmls")
	xml = "xmls/"+outname+""+str(i)+".xml"
	outFile=open(xml, 'w')
	outFile.write(template)
	outFile.close()
	
	cmd = 'sframe_main '+xml +'\n'
	print cmd
	jobs.write(cmd)
	return outfolder+'/VVanalysis.'+outname+'.'+str(i)+'.root'

def submitJobs(jobList, outfolder):
    print 'Reading joblist'
    jobListName = jobList
    print jobList
#    subCmd = 'qsub -t 1-%s -o logs nafbatch_runner_GEN.sh %s' %(nchunks,jobListName)
    subCmd = 'qsub -o %s/logs/ %s %s' %(outfolder,"psibatch_runner.sh",jobListName)
    print 'Going to submit jobs with', subCmd
    os.system(subCmd)

    return 1


if __name__ == "__main__":
	
    print "Pass argument. For signal : QstarToQW/QstarToQZ  , for background: QCD_Pt_"
    pattern = ""
    if   sys.argv[1].find("signal")!=-1: pattern= "QstarToQ"
    elif  sys.argv[1].find("QCDpt")!=-1: pattern= "QCD_Pt_"
    elif sys.argv[1].find("QCDht")!=-1: pattern= "QCD_HT"
    elif sys.argv[1].find("QCDherwig")!=-1: pattern= "QCD_Pt-15to7000"
    elif sys.argv[1].find("TTpythia")!=-1: pattern= "TT_TuneCUETP8M2T4_13TeV-powheg-pythia8"
    elif sys.argv[1].find("data")!=-1 : pattern= "JetHT"
    elif sys.argv[1].find("VJets")!=-1 : pattern= "WJetsToQQ*"
    else:
    	print "Please pass either: QCDpt/ht/herwig og TTpythia"
    	sys.exit()
	

    outfolder = "Output"

    try: os.stat(outfolder) 
    except: os.mkdir(outfolder)
	
    try: os.stat(outfolder+'/logs') 
    except: os.mkdir(outfolder+'/logs')
   
#    pattern = "/pnfs/psi.ch/cms/trivcat/store/t3groups/uniz-higgs/Summer16/Ntuple_80_20170203/QstarToQW_M-2500_TuneCUETP8M2T4_13TeV-pythia8/QstarToQW_M-2500_TuneCUETP8M2T4_13TeV-pythia820170203_signal/170203_131617/0000/flatTuple_1.root"
    if   pattern.find("TT_TuneCUETP8M2T4_13TeV-powheg-pythia8")!=-1: filelist = glob.glob('/pnfs/psi.ch/cms/trivcat/store/t3groups/uniz-higgs/Summer16/Ntuple_80_20170206/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/*/*/*.root')
    elif pattern.find("JetHT")!=-1                                 : filelist = glob.glob('/pnfs/psi.ch/cms/trivcat/store/user/clange/Ntuple_8021_20170602/JetHT/JetHT_*/*/*/flatTupl*.root')
    else: 															 filelist = glob.glob('/pnfs/psi.ch/cms/trivcat/store/t3groups/uniz-higgs/Summer16/Ntuple_80_20170203/'+pattern+'*/*/*/*/*.root')
    jobList = 'joblist.txt'
    jobs = open(jobList, 'w')
    outs = []
    for i,f in enumerate(filelist):
		if f.find("JetHT_Run2016F-23Sep2016-v1JetHT_G_20170602")!=-1: continue
		outname = f.split("/")[10]
		print outname
		fout = createJobs(i,f, outfolder, outname)
		outs.append(fout)
    print outs
	
    jobs.close()
    submit = raw_input("Do you also want to submit the jobs to the batch system? [y/n] ")
    if submit == 'y' or submit=='Y':
		
	    lock=thread.allocate_lock()
	    lock.acquire()
	    print 'Reading joblist'
	    jobListName = jobList
	    subCmd = 'qsub -t 1-%s -o %s/logs/ psibatch_runner.sh %s' %(i,outfolder,jobListName)
	    print 'Going to submit jobs with', subCmd
	    	
	    subProcess=subprocess.Popen( subCmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
	    subProcessStatus=subProcess.poll()
	    if subProcessStatus==1:
	      print "job '%s' failed" %(subCmd, subProcessStatus)
	      lock.release()
	      sys.exit()
	    else:
	      lock.release()
	      print "job finished"
	      userName=os.environ['USER']
	      # mergeCmd='hadd -f %s.root %s.root && rm -rf %s.root'  %(mergeFileBaseName,  fileToMerge,  fileToMerge)
  #         print "mergeCmd is %s " %mergeCmd
		  
       
    else:
        print "Not submitting jobs"


    
