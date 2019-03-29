#include "Header.h"

using namespace std;
using namespace cadex;


int main(int argc, char *argv[])
{
	if (argc < 2 || argc > 3) {
		cerr << "Usage: " << argv[0] << " <input_file> [<mesher>], where:" << endl;
		cerr << "    <input_file> is a name of the STEP file to be read" << endl;
		cerr << "    <mesher> is 'm' for Mefisto, 'n' for Netgen computational" << endl;
		cerr << "        mesher. Default is n" << endl;
		return 1;
	}
	
	//activate the license (aKey must be defined in cadex_license.cxx)
	if (!CADExLicense_Activate(aKey)) 
	{
		cerr << "Failed to activate CAD Exchanger license." << endl;
		return 1;
	}

	ModelData_Model aModel;
	const char* aSource = argv[1];

	
	if (!Import(aSource, aModel)) 
	{
		cerr << "Failed to read the file " << aSource << endl;
		system("pause");
		return 1;
	}
	
	
	
	TXTExport("gra.txt", aModel);
	
	ModelData_Model Model;
	
	TXTReader("gra.txt", Model);

	TXTExport("gra2.txt", Model);

	IsEqual("gra.txt", "gra2.txt");
	
	//if (!Export("graph.stp", Model)) {return 1;}
	
	return 0;
}