import os
import sys
import re

def calcPartial(val_file, rst_file, points):
	
	global partial
	
	case = "default"
	
	val_cycles = 0
	val_icache_hits = 0
	val_prefetch_hits = 0
	
	rst_cycles = 0
	rst_icache_hits = 0
	rst_prefetch_hits = 0
	
	with open(val_file) as fp:
		for line in fp:
			m = re.search(r"#\s*./sim_ds\s*\d+\s*\d+\s*\d+\s*(\d+)\s*(\d+)\s*.+", line, flags=re.IGNORECASE)
			if m:
				#print line[:-1]
				cacheSize = int(m.group(1))
				prefetch = int(m.group(2))
				
				if(cacheSize==0 and prefetch==0):
					case = "default"
				else:
					if(prefetch==0):
						case = "only_icache"
					else:
						case = "prefetch"
				
			m = re.search(r"#\s*Cycles\s*=\s*(\d+).*", line, flags=re.IGNORECASE)
			if m:
				val_cycles = int(m.group(1))
				
			m = re.search(r"#\s*Instruction\s*Cache\s*Hits:\s*=\s*(\d+).*", line, flags=re.IGNORECASE)
			if m:
				val_icache_hits = int(m.group(1))
				
			m = re.search(r"#\s*Prefetch\s*Hits:\s*=\s*(\d+).*", line, flags=re.IGNORECASE)
			if m:
				val_prefetch_hits = int(m.group(1))
	
	with open(rst_file) as fp:
		for line in fp:
			m = re.search(r"#\s*Cycles\s*=\s*(\d+).*", line, flags=re.IGNORECASE)
			if m:
				rst_cycles = int(m.group(1))

			m = re.search(r"#\s*Instruction\s*Cache\s*Hits:\s*=\s*(\d+).*", line, flags=re.IGNORECASE)
			if m:
				rst_icache_hits = int(m.group(1))
				
			m = re.search(r"#\s*Prefetch\s*Hits:\s*=\s*(\d+).*", line, flags=re.IGNORECASE)
			if m:
				rst_prefetch_hits = int(m.group(1))
	if(case=="default"):
		if(val_cycles == rst_cycles):
			partial += 0.75* points
	else:
		if(case=="only_icache"):
			if(val_cycles == rst_cycles):
				partial += 0.40* points
			if(val_icache_hits == rst_icache_hits):
				partial += 0.35* points
		else:
			if(case=="prefetch"):
				if(val_cycles == rst_cycles):
					partial += 0.25* points
				if(val_icache_hits == rst_icache_hits):
					partial += 0.25* points
				if(val_prefetch_hits == rst_prefetch_hits):
					partial += 0.25* points


def runTest(line, val_file, val_dir, points):
	global score
	global total
	global partial
	total = total + points
	rst_file = os.path.join(rst_dir, val_file[:-3] + r"rst")
	line += (r"> " + rst_file)
	if(lang==1):
		line = r"java " + line[2:]

	line = r"timeout 120s " + line	# timeout after 2 minutes
	x = os.system(line)
	if(x>>8==124):	# didn't finish within the time limit
		print val_file + r": Timeout"
		print "Points: 0"
		print
		print >>fb, val_file + r": Timeout"
		print >>fb, "Points: 0"
		print >>fb
	else:
		diff_file = rst_file[:-3] + r"diff"
		os.system(r"diff -iw " + os.path.join(val_dir,val_file) + r" " + rst_file + r" > " + diff_file)
		if(os.path.getsize(diff_file) > 0):
			print val_file + r": Failed"
			print "Calculating partial credits."
			print >>fb, val_file + r": Failed"
			print >>fb, "Calculating partial credits."
			partial = 0
			calcPartial(os.path.join(val_dir,val_file), rst_file, points)
			print "Partial credit: " + str(partial) + " points"
			print >>fb, "Partial credit: " + str(partial) + " points"
			print
			print >>fb
			score = score + partial
		else:
			print val_file + r": Passed"
			print >>fb, val_file + r": Passed"
			score = score + points
			rst_del = "rm -f "+ rst_file + " " + diff_file
			os.system(rst_del)

def runValidation(val_dir):
	validations = os.listdir(val_dir)
	validations.sort()
	for v in validations:
		if(("icache" in v) or ("prefetch" in v)):
			points = 5
		else:
			points = 4
		with open(os.path.join(val_dir,v)) as fp:
			for line in fp:
				if "sim_ds" in line:	#extract the command from the second line of the validation file
					runTest(line[2:-1], v, val_dir, points)	
					break
			

def runScript():
	global lang
	#global points

	print
	print "Copying traces into the current directory. Note that the copied 'traces' folder will be deleted in the end."
	os.system(r"cp -r "+traces_dir+r" ./traces")
	print "Copying done."
	print

	print "Unzipping 'proj3.zip'. This will overwrite any files with the same name in this directory."
	os.system("unzip -o proj3")

	print
	os.system("make clean")
	os.system("make")
	print

	# lang = 0 (C/C++/Python), lang = 1 (Java)
	if(os.path.isfile("sim_ds") and os.access("sim_ds", os.X_OK)):
		print "Found executable 'sim_ds'."
		lang = 0
	else:
		print "Executable 'sim_ds' not found."
		print
		print "Trying Java."
		lang = 1

	print
	
	#points = 4
	print "Running all validations:"
	print
	print >>fb, "Running all validations:"
	print >>fb
	runValidation(val_dir)
	
	print
	print "Final score: " + str(score) + " out of " + str(total)
	print >>fb
	print >>fb, "Final score: " + str(score) + " out of " + str(total)
	print >>fb
	
	print
	print "Deleting the copied 'traces' directory."
	os.system("rm -rf traces")
	print "Deleting done."
	print
	print "The output and diff files are stored in the 'results' directory."
	print

	os.system("make clean")

	print

#main():

traces_dir = sys.argv[1]
val_dir = sys.argv[2]

root_dir = os.getcwd()
uid = os.path.basename(os.path.normpath(root_dir))

#Run for current directory

rst_dir = os.path.join(root_dir, "results")
if(not(os.access(rst_dir, os.F_OK))):
	os.mkdir(rst_dir)
fb = open(os.path.join(rst_dir, "feedback.txt"), "w")
score = 40 #Make sure (manually) there is substantial programming effort
lang = 0
total = 40
#points = 4
partial = 0

runScript()

fb.close()

