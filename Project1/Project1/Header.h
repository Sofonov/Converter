#pragma once
#include <cadex/Base_ProgressScope.hxx>
#include <cadex/Base_ProgressStatus.hxx>
#include <cadex/LicenseManager_Activate.h>
#include <cadex/MeshAlgo_MefistoFactory.hxx>
#include <cadex/MeshAlgo_NetgenFactory.hxx>
#include <cadex/ModelData_Body.hxx>
#include <cadex/ModelData_BodyList.hxx>
#include <cadex/ModelAlgo_BRepMesher.hxx>
#include <cadex/ModelAlgo_BRepMesherParameters.hxx>
#include <cadex/ModelData_BRepRepresentation.hxx>
#include <cadex/ModelData_IndexedTriangleSet.hxx>
#include <cadex/ModelData_Model.hxx>
#include <cadex/ModelData_Part.hxx>
#include <cadex/ModelData_Assembly.hxx>
#include <cadex/ModelData_Instance.hxx>
#include <cadex/ModelData_PolyRepresentation.hxx>
#include <cadex/ModelData_PolyShapeList.hxx>
#include <cadex/ModelData_SceneGraphElementUniqueVisitor.hxx>
#include <cadex/ModelData_Transformation.hxx>
#include <cadex/ModelData_PropertyTable.hxx>
#include <cadex/ModelData_Vector.hxx>
#include <cadex/STEP_Reader.hxx>
#include <cadex/IFC_Reader.hxx>
#include <cadex/STEP_Writer.hxx>
#include <iostream>
#include <fstream>
#include<set>
#include <string>
#include "../cadex_license.cxx"
using namespace std;
using namespace cadex;



static bool Import(const string theSource, ModelData_Model& theModel)
{
	if (theSource.substr(theSource.find_last_of('.')) == ".ifc")
	{
		IFC_Reader aReader; 
		return aReader.ReadFile(theSource.c_str()) && aReader.Transfer(theModel);
	}

	else
	{
		STEP_Reader aReader;
		return aReader.ReadFile(theSource.c_str()) && aReader.Transfer(theModel);
	}
}



static bool Export(const char* theDest, const ModelData_Model& theModel)
{
	STEP_Writer aWriter;
	if (!aWriter.Transfer(theModel) || !aWriter.WriteFile(theDest)) {
		cerr << "Failed to write the file " << theDest << endl;
		return false;
	}
	return true;
}



class TXTExport
{
	wofstream out;

public:
	
	TXTExport(const char* theDest, const ModelData_Model& aModel)
	{
		
		this->out.open(theDest);
		cout << "In TXTExport" << endl;
		foo(aModel);
		this->out.close();

	}

	template <typename T>
	void foo(T& aModel, int deep = 0)
	{

		ModelData_Model::ElementIterator anIterator(aModel);
		while (anIterator.HasNext())
		{

			ModelData_SceneGraphElement& aSGE = anIterator.Next();
			if (aSGE.TypeId() != ModelData_Part::GetTypeId())
			{
				for (int i = 0; i < deep; i++)
					this->out << "-";


				if (aSGE.TypeId() == ModelData_Assembly::GetTypeId())
					this->out << "(Assembly)" << aSGE.Name().ToWString().c_str();
				if (aSGE.TypeId() == ModelData_Instance::GetTypeId())
					this->out << "(Instance)" << aSGE.Name().ToWString().c_str();

				this->out << endl;

				foo(aSGE, deep + 1);
			}
			else
			{
				for (int i = 0; i < deep; i++)
					this->out << "-";
				this->out << "(Part)" << aSGE.Name().ToWString().c_str() << endl;

				return;
			}

		}

	}
};


class TXTReader
{
	ifstream in;
	
public:
	
	TXTReader(string FileName, ModelData_Model& aModel)
	{ 
		 cout << "In TXTReader" << endl;
		 in.open(FileName, ios::in);
		 string line;
		 getline(in, line);
		 aModel.AddRoot(reader(line));
		 
		 this->in.close();
	}
	
	ModelData_SceneGraphElement reader(string line, int depth = 0)
	{
		if (this->in.eof()) 	return	ModelData_SceneGraphElement();
				
			
			if (line.find("(Assembly)") != std::string::npos)
			{
				string line2 = line.substr(line.find_first_of(')') + 1);
				ModelData_Assembly anAssembly(Base_UTF16String(line2.c_str()));
				
				while (!this->in.eof())
				{
					getline(this->in, line);
					
					int n = (int)line.find_first_of('(');	 ++depth;

					if (depth - n > 1)  break; 
						 
					anAssembly.AddInstance(reader(line,depth));
					
					depth=n-1;
									
				}
					
					return anAssembly;
			}

			if (line.find("(Instance)") != std::string::npos)
			{
					getline(this->in, line);
					ModelData_Instance anInstance(reader(line,++depth));
					return anInstance;
			}

			if (line.find("(Part)") != std::string::npos)
			{
				string line2 = line.substr(line.find_first_of(')') + 1);
				ModelData_Part aPart(Base_UTF16String(line2.c_str()));
				return aPart;
			}
	}
};

void IsEqual(string FileName1, string FileName2)
{
	cout << "InIsEqual"  << endl;
	ifstream in1, in2;
	in1.open(FileName1, ios::in);
	string line1;
	
	in2.open(FileName2, ios::in);
	string line2;
	
	while (in1.eof()&&in2.eof())
	{
		getline(in1, line1);
		getline(in2, line2);
		if(line1!=line2)
		{
			cout << "Error" << endl;
			system("pause");
			return;
		} 
	}
	cout << "Success!" << endl;
	system("pause");
}
