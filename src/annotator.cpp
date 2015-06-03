/*
 * annotator.cpp
 *
 *  Created on: Apr 27, 2015
 *      Author: nok
 */

#include <iostream>
#include <string>
#include <limits>
#include <list>
#include <map>
#include <algorithm>
#include <ctime>
#include <typeinfo>
#include "header/input_output.h"
#include "header/calculator.h"

using namespace std;

/*
 * oncoIMPACT-annotation input_final_module output_prefix cutoff top_n_annotations
 */

int main( int argc, char *argv[] ) {


	if ( argc != 6 ){ // argc should be 4 for correct execution
	    cout<<"Usage: "<< argv[0] <<" <annotation_db_dir> <input_final_module> <output_prefix> <cutoff> <top_n_annotations>\n";
	    cout<<"Example: "<< argv[0] <<" MSigDB/ FINAL_MODULE.dat output/GBM_ 0.05 50\n\n";
	    cout<<"Description:\n";
	    cout<<"\t<annotation_db_dir> = path to MSigDB directory\n";
	    cout<<"\t<input_final_module> = the final module results of OncoIMPACT\n";
	    cout<<"\t<output_prefix> = prefix of the output files. The prefix can be both directory name and prefix of filename\n";
	    cout<<"\t<cutoff> = cutoff for hypergeometric test\n";
	    cout<<"\t<top_n_annotations> = number of top annotations for each driver\n";
	}else {

		string mSigDbPath = argv[1];
		string moduleFileName = argv[2];
		string outputPrefix = argv[3];

		double cutoff = atof(argv[4]);
		int top = atoi(argv[5]);

		/*
		 * Start timer
		 */

		clock_t begin_time = clock();

		//read gene list
		string geneListFileName = "driver_net_background_gene_list.dat";
		vector<string> geneList;
		readGeneList(&geneListFileName, &geneList);
		int totalGenes = geneList.size();


		//read gene sets from MSigDB
		vector<string> filenames;
		filenames.push_back(mSigDbPath + "h.all.v5.0.symbols.gmt");		//hallmark
	//	filenames.push_back(mSigDbPath + "c2.all.v5.0.symbols.gmt");		//pathway databases
		filenames.push_back(mSigDbPath + "c2.cp.kegg.v5.0.symbols.gmt");	//KEGG
		filenames.push_back(mSigDbPath + "c5.all.v5.0.symbols.gmt");		//GO
	//	filenames.push_back(mSigDbPath + "c6.all.v5.0.symbols.gmt");

		vector<string> geneSetNames;
		vector<string> geneSetUrls;
		vector< vector<string> > geneSetMembers;

		cout << "reading gene sets for annotation ...\n";
		readGeneSets(&filenames, &geneSetNames, &geneSetUrls, &geneSetMembers, &geneList);

		cout << "DONE (" << (float(clock() - begin_time) / CLOCKS_PER_SEC) << " sec)\n";
		begin_time = clock();	//update the clock
		int numGeneSets = geneSetNames.size();

		cout << "\tnumber of gene sets = "  << numGeneSets << endl;

		//printing annotation list for plotting
//		vector<string> outStr;
//		string filename = "annotation_list.txt";
//		for (int i = 0; i < numGeneSets; ++i) {
//			outStr.push_back(geneSetNames[i]);
//		}
//		writeStrVector(filename.c_str(), &outStr);

		//read oncoIMPACT final modules and combine driver, phenotype, and explained genes for each module
		cout << "reading oncoIMPACT modules for annotation ...\n";
		vector<string> moduleNames;
		vector< vector<string> > moduleMembers;
		vector< vector<string> > moduleDrivers;
		set<string> driversList;
		set<string> samplesList;

		readModules(&moduleFileName, &moduleNames, &moduleMembers, &moduleDrivers, &driversList, &samplesList);

		cout << "DONE (" << (float(clock() - begin_time) / CLOCKS_PER_SEC) << " sec)\n";
		begin_time = clock();	//update the clock
		int numModules = moduleNames.size();
		int numSamples = samplesList.size();

		cout << "\tnumber of modules = "  << numModules << " and number of samples = " << numSamples <<  endl;

		//get size of gene sets and overlap
		list<GeneSetPair> geneSetPairs;

		cout << "annotating ... ";
		int progress = 1;
		int interval = numModules / 100;

		for (int i = 0; i < numModules; ++i) {

			//print progression
			if (i % interval == 0) {
				const string progStatus = intToStr(progress) + "%";
				cout << progStatus << flush;
				progress++;
				cout << string(progStatus.length(), '\b');
			}

			vector<string> currentModuleMembers = moduleMembers[i];
			vector<string> currentModuleDrivers = moduleDrivers[i];

	//		cout << "annotating " << moduleNames[i] << endl;

			for (int j = 0; j < numGeneSets; ++j) {
				vector<string> currentGeneSetMembers = geneSetMembers[j];
				int numOverlap = countOverlap(&currentModuleMembers, &currentGeneSetMembers);
				if(numOverlap > 0){

					GeneSetPair pair;
					pair.geneListName = moduleNames[i];
					pair.sizeOfGeneList = currentModuleMembers.size();
					pair.geneSetName = geneSetNames[j];
					pair.sizeOfGeneSet = currentGeneSetMembers.size();
					pair.numOverlap = numOverlap;
					calculateHypergeometric(&pair, totalGenes, numGeneSets);

					if(pair.pValue < cutoff){
						geneSetPairs.push_back(pair);
					}
				}
			}
		}

		cout << endl; //for print progression
		cout << "DONE (" << (float(clock() - begin_time) / CLOCKS_PER_SEC) << " sec)\n";
		begin_time = clock();	//update the clock

		cout << "sorting p-values ...\n";

		//output annotation
		geneSetPairs.sort(sortByPValues);

		cout << "saving annotation and p-values to FINAL_MODULE_ANNOTATION.dat...\n";
		vector<string> outputStr;
		string outputFileName = outputPrefix + "FINAL_MODULE_ANNOTATION.dat";
		for(list<GeneSetPair>::iterator it = geneSetPairs.begin(); it != geneSetPairs.end(); it++){
			string str = it->geneListName + "\t" + intToStr(it->sizeOfGeneList) + "\t" +
					it->geneSetName + "\t" + intToStr(it->sizeOfGeneSet) + "\t" + intToStr(it->numOverlap) + "\t" + doubleToStr(it->pValue, 25);
			outputStr.push_back(str);
		}

		writeStrVector(outputFileName.c_str(), &outputStr);

		vector<string> outputFeqStr;
		outputFileName = outputPrefix + "DRIVER_ANNOTATION.dat";
		cout << "calculating gene set frequency for each driver genes (" << driversList.size() << ") ...\n";

		//output driver list with frequency of annotated functions
		for(set<string>::iterator it = driversList.begin(); it != driversList.end(); it++){

			string str = "";

			//for each driver, count number of samples
			vector<int> sampleCounts(numGeneSets);
			for (int i = 0; i < numGeneSets; ++i) {
				sampleCounts[i] = 0;
			}

			string currentDriver = *it;
			vector<string> modulesOfADriver;

	//		cout << "current driver gene is " << currentDriver << endl;
			str += currentDriver + "\t";

			//find which modules containing the current driver, then find which gene sets are annotated for the modules
			for (int i = 0; i < numModules; ++i) {

				//get driver of the first module
				vector<string> driversOfAModule = moduleDrivers[i];

				//if current driver gene is in module i
				if(find(driversOfAModule.begin(), driversOfAModule.end(), currentDriver) != driversOfAModule.end()){
					string moduleName = moduleNames[i];

	//				cout << "found driver in " << moduleName << endl;
					modulesOfADriver.push_back(moduleName);

				}
			}

			//for each module that contain the current driver gene
			int numModulesOfADriver = modulesOfADriver.size();
			str += intToStr(numModulesOfADriver) + "\t";

	//		cout << "\tis in " << numModulesOfADriver << " modules" << endl;

			for (int i = 0; i < numModulesOfADriver; ++i) {

				string moduleName = modulesOfADriver[i];

				//find which gene sets significantly overlap with the
				for(list<GeneSetPair>::iterator itp = geneSetPairs.begin(); itp != geneSetPairs.end(); itp++){

					//if the current gene set is annotated for module i
					if(itp->geneListName.compare(moduleName) == 0){

		//				cout << moduleName << " is annotated with " << itp->geneSetName << endl;

						int indexOfGeneSet = findIndex(&geneSetNames, itp->geneSetName);
						if(indexOfGeneSet == -1){
							cout << "not found " << itp->geneSetName << endl;
						}else{
							sampleCounts[indexOfGeneSet]++;
						}
					}
				}
			}

			list<GeneSetCount> sampleCountsList;		//contains name and count
			for (int i = 0; i < numGeneSets; ++i) {
				if(sampleCounts[i] > 0){
					GeneSetCount gsc;
					gsc.name = geneSetNames[i];
					gsc.count = sampleCounts[i];
					sampleCountsList.push_back(gsc);
	//				cout <<  currentDriver << "\t" << gsc.name << "\t" << gsc.count << endl;
				}
			}

			sampleCountsList.sort(sortByCount);

			int i = 0;
			for (list<GeneSetCount>::iterator itc = sampleCountsList.begin(); itc != sampleCountsList.end() and i < top; itc++, i++) {
	//			cout <<  currentDriver << "\t" <<
	//					itc->name << "\t" << 1.0 * itc->count / numSamples;
	//			cout << endl;
				str += itc->name + "\t";
				str += doubleToStr(1.0 * itc->count / numModulesOfADriver, 3) + "\t";
			}

			outputFeqStr.push_back(str);

		}

		writeStrVector(outputFileName.c_str(), &outputFeqStr);
		cout << "DONE (" << (float(clock() - begin_time) / CLOCKS_PER_SEC) << " sec)\n";

	} //end parameter check if-else
}


