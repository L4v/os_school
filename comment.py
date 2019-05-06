import sys
with open(raw_input("Putanja fajla za citanje: ")  if sys.version_info[0] < 3 else
				input("Putanja fajla za citanje: ") , "rt") as ins:
	lines = []
	for line in ins:
		lines.append(line)
text = []
started = False
for i in lines:
	if "/*" in i and not started:
		text.append(i)
		started = True
		continue
	if started:
		text.append(i)
		if "*/" in i:
			started = False
			break

f = open(raw_input("Putanja fajla za pisanje: ")  if sys.version_info[0] < 3 else
				input("Putanja fajla za pisanje: ") , "w")
for i in text:
	f.write(i)
f.close()
