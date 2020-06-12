#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void perm(int D, int arr[]); //permutates an array
void genRandomHV(int D, int randomHV[]); //generates random hypervector
void circShift(int n, int d, int arr[][d]); //shifts the array circularly
void createItemMemoryHV(int D, int iMHV[][D]); //creates hypervectors for every single character in itemMemory
void lookupItemMemory(int D, int iMHV[][D], char itemMemory[], char key, int block[][D]); // sets the first row of block to the cooresponding randHV of key
void computeSumHV(int D, int N, int sumHV[D], int count, char buffer[]);
void buildLangHV(int N, int D);

int main() {
	//Declarations/initializations for computeSumHV
	int N = 4;
	int D = 10000;
	
	buildLangHV(N, D);

	return 0;
		
}

void perm(int D, int arr[]) {	
	//Initialize Array
	for(int i=0; i<D; i++) {
		arr[i] = i;
	}
	
	//Randomize the numbers of the array
	int k, l;
	for(int j=0; j<D; j++) {
		k = rand() % (D-j) + j;
		l = arr[k];
		arr[k] = arr[j];
		arr[j] = l; 
	}
}

void genRandomHV(int D, int randomHV[]) {
	//Later will alter to be scanf (user input availability)
	if (D % 2 != 0) {
		printf("Dimension is odd");
	}
	else {
		//Permute the Array
		perm(D, randomHV);
		
		for(int i=0; i<D; i++) {
			if (randomHV[i] <= D/2) {
				randomHV[i] = 1;
			}
			else {
				randomHV[i] = -1;
			}
		}
	}
}
void circShift(int n, int d, int arr[][d]) {
	int arr1[n][d];
	
  //Pushes every row down one
	for(int i=0; i<n; i++) {
		for(int j=0; j<d; j++) {
			if (i==0) {
				arr1[i][j] = arr[n-1][j];
			}
			else {
				arr1[i][j] = arr[i-1][j];
			}
		}
	}
  
  //Pushes every column to the right by 1
	for(int i=0; i<n; i++) {
		for(int j=0; j<d; j++) {
			if (j == (d-1)) {
				arr[i][0] = arr1[i][d-1];
				//printf("%i ", arr[i][0]);   //#this was during testing
			}
			else {
				arr[i][j+1] = arr1[i][j];
			}
		}
	}
}
void createItemMemoryHV(int D, int iMHV[][D]) {
	for(int i=0; i<27; i++) {
		int randomHV[D];
		genRandomHV(D, randomHV);
		for(int j=0; j<D; j++) {
			iMHV[i][j] = randomHV[j];
		}
	}
}

void lookupItemMemory(int D, int iMHV[][D], char itemMemory[], char key, int block[][D]) {
	for (int i=0; i<27; i++) {
		if (itemMemory[i] == key) {
			for(int j=0; j<D; j++) {
				block[0][j] = iMHV[i][j];
			}
			break;
		}
	}	
}
void computeSumHV(int D, int N, int sumHV[D], int count, char buffer[]) {
	int block[N][D];
	char key;
	char itemMemory[27] = "abcdefghijklmnopqrstuvwxyz "; //every character in the alphabet & ' '
	int iMHV[27][D];
	
	//Initializing block and sum to be arrays of 0
	for(int i=0; i<N; i++) {
		for(int j=0; j<D; j++) {
			block[i][j] = 0;
		}
	}
	for(int i=0; i<D; i++) {
		sumHV[i] = 0;
	}
	
	//assignItemMemory(count, buffer, itemMemory);
	createItemMemoryHV(D, iMHV);
	
	
	for(int i=0; i<=8; i++) {			
			
		key = buffer[i];
		circShift(N, D, block);
		lookupItemMemory(D, iMHV, itemMemory, key, block); 	
			
		if (i >= N) {
			//int *temp = block[0];
			int nGrams[D];
				
			//assigns nGrams to the first row of block
			for(int j=0; j<D; j++) {
				nGrams[j] = block[0][j];
			}		
				
			for(int j=1; j<N; j++) {
				for(int k=0; k<1; k++) {
					for(int l=0; l<D; l++) {
						nGrams[l] = nGrams[l] * block[j][l];
					}
				}
			}	
			for(int j=0; j<D; j++) {
				sumHV[j] = sumHV[j] + nGrams[j];
			}
		}
	}
}
void buildLangHV(int N, int D) {
	int sumHV[D];
	
	//Creating langLabels
	char langLabels[][4] = {"afr", "bul", "ces", "dan", "nld", "deu", "eng", "est", "fin", "fra", "ell", "hun", "ita", "lav", "lit", "pol", "por", "ron", "slk", "slv", "spa", "swe"};
	int length =  (sizeof langLabels)/(sizeof langLabels[0]); //size of langLabels
	char buffer[2000000]; //array for every c in .txt files
	int langAM[length][D]; //assistive memory storage for testing
	
	//iterating through every file computingHV
	for(int t=0; t<length; t++) {
		//Creating the file address 
		char fileAddress[29];
		
		sprintf(fileAddress, "%s%s%s", "./code/training_texts/", langLabels[t], ".txt");
		
		
		//Opening the file address
		FILE *fileID = fopen(fileAddress, "r"); 
		
		//Check to make sure the file can be opended
		if (fileID == NULL) {
			printf("Failed: File could not be opened.\n");
			break;
		}
		
		
		//Compiles every character in the text document into array, buffer
		int count=0;
		
		while(1) {
			buffer[count] = fgetc(fileID);
			if(feof(fileID)) {
				break;
			}
			count++;	
		}
		fclose(fileID);
		printf("Loaded training language file %s\n", fileAddress);

		 computeSumHV(D, N, sumHV, count, buffer);
		 
		 //Uncomment to test that the void function works
		 /*
		 for(int i=0; i<D; i++) {
			 langAM[t][i] = sumHV[i];
			 printf("%i ", langAM[t][i]);
		 }
		 printf("\n");
		 */
	}
}