#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;
int main() {
	//initializes file reader and opens file
	ifstream inFile;
	inFile.open("data.txt");

	//initializes text line string and area array
	string textLine;
	float area[206];

	//reads file line by line, converts line from string to float, and stores into area array
	for (int i=0; i<=205; i++) {
		getline(inFile, textLine);
		area[i]=stof(textLine.substr(0,7));
	}
	
	//closes file reader, initializes file writer, and opens file 
	inFile.close();
	ofstream out("output.csv");
	//streambuf *coutbuf = cout.rdbuf();
	//cout.rdbuf(out.rdbuf());

	freopen("output.csv","w",stdout);
	cout<<"# Elements,d(x=0mm),d(x=100mm),sigma(x=100mm),sigma(x=205mm)" << endl;



	for(int z = 2; z <= 205; z++) {

		int elements = z;
		int nodes = elements + 1;

		//INITIALIZE VECTORS AND MATRICES
		float a[elements];		//area
		float f[elements][2];	//force
		float s[elements];		//stress
		float k[nodes][nodes];	//stiffness
		float d[nodes];			//displacement
		float eal[elements];	//EA/L
		

		
		//int step = (205 / elements);    //sets element size

		int index = 0;
		int step = 205 / elements;
		for (int i = 0; i < 205; i += step) {
		float sum = 0;
			if (i <= 205 && i >= step*elements) {
				step = 205 - step*elements;
				for (int j = i; j <= 205; j++) {
					sum += area[j];
				}
				a[index] = sum / step;
			}
			if (i <= 205 && i < (step*elements)) {								
				for (int j = i; j < (i + step); j++) {	
					sum += area[j];												//gets sum of areas between steps
				}
				a[index] = sum / step;							//gets average area between steps
			}
			index += 1;														//increments index
		}

		
		//CALCULATE EA/L FOR EACH ELEMENT
		for (int i = 0; i < elements; i++) {
			eal[i] = (18600*a[i]/(205/elements));
		}
		

		
		//CALCULATES STIFFNESS MATRIX
		k[0][0] = eal[0];
		k[nodes-1][nodes-1] = eal[elements-1];
		for (int i = 0; i < nodes; i++) {
			for (int j = 0; j < nodes; j++) {
				//SETS ZERO VALUES OF MATRIX
				if (i != j && j != i-1 && j != i+1) {
					k[i][j] = 0;
				}
				//SETS SUM VALUES OF MATRIX
				if (i == j && i != 0 && i != nodes-1) {
					k[i][j] = (eal[i]+eal[i-1]);
				}
				//SETS NEGATIVE VALUES OF MATRIX
				if (i == j && i != nodes-1) {
					k[i][j+1] = -eal[i];
					k[i+1][j] = -eal[i];
				}
			}
		}
		

		
		//CALCULATES DISPLACEMENT VECTORS
		d[nodes-1] = 0;
		d[nodes-2] = -1380/k[nodes-1][nodes-2];
		for(int i = nodes-2; i>=1; i--) {
			d[i-1] = (k[i][i]*d[i]+k[i][i+1]*d[i+1])/-k[i][i-1];
		}
		

		
		//CALCULATE REACTION FORCES AND STRESS
		for (int i = 0; i <= elements; i++) {							
			for (int j = 0 ; j <= 1; j++) {								
				if (j == 0) {								
					f[i][j] = eal[i]*(d[i]-d[i+1]);					//calculate positive reaction force
					s[i] = f[i][j] / (a[i]) ;				//calculate stress
				}
				if (j == 1) {
					f[i][j] = eal[i]*(d[i+1]-d[i]);					//calculate negative reaction force
				}
			}
		}

		
		if (z == 0) {
			s[0] = 1380/(438.506);
		}
		//PRINT OPTIONS
		bool print = 0;
		if (print) {
			//print options, can output matrices/vectors to see math behind code
			bool print_s = 1;    												//set to 1 to print stress array
			bool print_k = 1;  													//set to 1 to print stiffness matrix
			bool print_a = 1;    												//set to 1 to print area array
			bool print_d = 1;    												//set to 1 to print nodal displacements

			//prints the displacement of each node
			if (print_d) {														
				for (int i = 0; i <= elements; i++) {
					printf("d%i: %f mm\n",i+1,d[i]);
				}
			}
			//prints the area of each element
			if (print_a) {
				printf("...............................................\n");
				printf("|    Element #    |    Average Area (mm^2)    |\n");
				for (int i = 0; i < elements; i++) {
					printf("|        %i        |          %3.2f           |\n",i+1,a[i]);
				}
				printf("'''''''''''''''''''''''''''''''''''''''''''''''\n");
			}
			//prints the stiffness matrix
			if (print_k) {
				for (int i = 0; i < nodes; i++) {
					for (int j = 0; j < nodes; j++) {
						printf("%6.0f  ",k[i][j]);
					}
					printf("\n");
				}
			}
			//prints the stress of each elements						
			if (print_s) {
				float stress_average = 0;
				printf("...............................................\n");
				printf("|    Element #        |       Stress (MPa)    |\n");
				for (int i = 0; i < elements; i++) {
					stress_average += s[i];
					printf("|        %i            |         %3.3f         |\n",i+1,s[i]);
				}
				printf("'''''''''''''''''''''''''''''''''''''''''''''''\n");
				stress_average = stress_average / elements;
				printf("Average Stress: %1.3f MPa\n",stress_average);
			}
		}

		//prints answers

		cout << z << "," << d[0] << "," << d[(elements/2)] << "," << s[(elements/2)-1] << "," << s[elements-1] << endl;
	}
	return 0;
}
