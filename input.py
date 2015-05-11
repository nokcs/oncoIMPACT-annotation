from collections import Counter
import os

mypath = "./"
numTopDrivers = 20
numTopAnnotation = 20
numCol = numTopAnnotation * 2 + 3

os.chdir(mypath)
#print os.getcwd()

#read annotations
annotations = []
filename = mypath + "annotation_list.txt"
print filename
f = open(filename, 'r')
for line in f:
    annotations.append(line.strip())
print "# annotations ", len(annotations)

# datasets = {"BLCA", "COAD", "GBM", "LIHC", "LUAD", "OV", "PRAD"}
datasets = {"GBM"}

allTopAnnotatedDrivers = []

#for filename in os.listdir(mypath):
for dataset in datasets:
    
    print dataset

    #read annotated drivers
    filename = mypath + "output/0.05_25/" + dataset + "_DRIVER_ANNOTATION.dat"
#     print filename
    annotatedDrivers = []
    f = open(filename, 'r')
    for line in f:
        #print line,
        annotatedDriver = line.strip().split("\t")
        #print annotatedDriver[0]
        if len(annotatedDriver) >= numCol:
            annotatedDrivers.append(annotatedDriver[0:numCol-1])
        else:
            #fill in the blank
            numAnnotationToFill = numCol - len(annotatedDriver)
            for i in range(0, numAnnotationToFill):
                annotatedDriver.append('NA')
                annotatedDriver.append('0')
#             print annotatedDriver
            annotatedDrivers.append(annotatedDriver[0:numCol-1])
            
    print "# all annotated drivers ", len(annotatedDrivers), " (have annotation > top)"

    #read top drivers
    filename = mypath + "oncoIMPACT-results/" + dataset + "/ONCOIMPACT/LATEST/driver_list.txt"
#     print filename
    topDrivers = []
    count = 0
    f = open(filename, 'r')
    for line in f:
        if count > 0:    #skip the first line
            temp = line.split("\t")
            topDrivers.append(temp[0])
            #print temp[0]
        count += 1
        if count > numTopDrivers:
            break
        
    #get annotated top drivers
    annotatedTopDrivers = []    
    for topDriver in topDrivers:
        for annotatedDriver in annotatedDrivers:
            if topDriver == annotatedDriver[0]: #same gene name
                annotatedTopDrivers.append(annotatedDriver)
                #print topDriver, " is annotated"
                break
    print "# annotated top drivers ", len(annotatedTopDrivers)
            
    #create list dictionary for each driver
    i = 1
    for annotatedTopDriver in annotatedTopDrivers:
        #print annotatedTopDriver[2 : 2 * top]
        
        #[gene name: frequency]
        a = dict(annotatedTopDriver[i:i+2] for i in range(2, 2*numTopAnnotation, 2))
        
        #convert freq string to float
        for key, value in a.iteritems():
            a[key] = float(value)
            
        da = [dataset + "_" + str(i) + "_" + annotatedTopDriver[0], a]
        allTopAnnotatedDrivers.append(da)
        i = i + 1

#get only annotations that are annotated to the drivers
annotationInUsed = []
for driver in allTopAnnotatedDrivers:
    rowData = driver[1] #contains only enriched annotation
    
    for key in rowData:
        if(rowData[key] > 0):
            annotationInUsed.append(key)

uniqueAnnotationInUsed = list(set(annotationInUsed))
print len(uniqueAnnotationInUsed)

colName = uniqueAnnotationInUsed
outFilename = 'annotation_matrix_' + str(numTopDrivers) + 'drivers_' + str(numTopAnnotation) + 'annnotations_GBM.dat'
f = open(outFilename,'w')
f.write('\t'.join(uniqueAnnotationInUsed) + "\n")

#print matrix to file
for driver in allTopAnnotatedDrivers:
    rowName = driver[0]
    rowData = driver[1]
    tempDict =  {x: 1 for x in uniqueAnnotationInUsed} #contain all annotation
    #create row with all annotation
    A = Counter(rowData)
    B = Counter(tempDict)
    C = B + A   
    rowData = dict(C)
    for key in rowData:
        rowData[key] -= 1;
#     print rowData

    rowValues = rowData.values()
    f.write(rowName + "\t" + '\t'.join(str(v) for v in rowValues) + "\n")

