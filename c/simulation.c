#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h> 


void perm(int D, int arr[]); //permutates an array
void genRandomHV(int D, int randomHV[]); //generates random hypervector
void createItemMemoryHV(int D, int iMHV[][D], int size); //creates hypervectors for every single character in itemMemory
void buildLangHV(int N, int D, int langAM[][D], int iMHV[][D], char itemMemory[]); //loads and computes the training 
void computeSumHV(int N, int D, int *sumHV, int count, char *buffer, int iMHV[][D], char itemMemory[]); //performs the encoding on a text files
void circShift(int n, int d, int *arr);
void lookupItemMemory(int D, int iMHV[][D], char itemMemory[], char key, int *block);
double test(int N, int D, int langAM[][D], int iMHV[][D], char itemMemory[]);
void binarizeHV(int *v, int szofv);
double norm(int *a, int n);
double dotProduct(int *a, int *b, int n);
double cosAngle(int *a, int *b, int n);
void writeLines(int length, char langLabels[][4], int lineMax);

int main() {
	int N = 4;
	int D = 10000;

	srand(time(0));
	
	char langLabels[][4] = {"afr", "bul", "ces", "dan", "nld", "deu", "eng", "est", "fin", "fra", "ell", "hun", "ita", "lav", "lit", "pol", "por", "ron", "slk", "slv", "spa", "swe"};
	int length = (sizeof langLabels)/(sizeof langLabels[0]);
	char itemMemory[27] = "abcdefghijklmnopqrstuvwxyz ";
	int langAM[length][D];
	double accuracy;
	int iMHV[27][D];


	DIR *simDir;
    simDir = opendir("training_textsSIM");

  	if(simDir) {
	    printf("Found dir training_textsSIM\n");
    }
    else {
	mkdir("training_textsSIM", 0777);
	}

	for(int lineMax=1000; lineMax<10001; lineMax = lineMax+1000) {
		writeLines(length, langLabels, lineMax);
		createItemMemoryHV(D, iMHV, 27);
		buildLangHV(N, D, langAM, iMHV, itemMemory);
		//printf("Finished training of %d language files\n", length);
		accuracy = test(N, D, langAM, iMHV, itemMemory);
		printf("Accuracy @ %d lines = %f%%\n", lineMax, accuracy);
	}	
	/*
	
	createItemMemoryHV(D, iMHV, 27);
	buildLangHV(N, D, langAM, iMHV, itemMemory);
	accuracy = test(N, D, langAM, iMHV, itemMemory);
	printf("Accuracy = %f%%\n", accuracy);
	*/
	   
	return 0;
}

void writeLines(int length, char langLabels[][4], int lineMax) {
	
	FILE *fp;
    FILE *langf;
    char fileAddress[150];
    char fileAddresslang[150];
	
	 for(int i=0; i<length; i++) {

		int count = 0;
  		int lines = 0;

		char *buff;
		buff = (char*)malloc((2000000) * sizeof(char));
		if(buff == NULL) {
			printf("not enough memory\n");
			exit(1);
		}
		
		sprintf(fileAddress, "%s%s%s", "/home/benlucas/Desktop/sim/training_textsSIM/", langLabels[i], ".txt");
		fp = fopen(fileAddress, "w+");
		if(fp == NULL) {
			printf("Error opening: %s\n", fileAddress);
		}
		sprintf(fileAddresslang, "%s%s%s", "/home/benlucas/Desktop/internship/training_texts/", langLabels[i], ".txt");
		langf = fopen(fileAddresslang, "r");
		if(langf == NULL) {
			printf("Error opening: %s\n", fileAddresslang);
			exit(1);
		}
		
		while(lines < lineMax) {
			buff[count] = fgetc(langf);
			if(buff[count] == '\n') {
				lines++;
			}
			count++;	
		}
		buff[count] = '\0';
		fprintf(fp, "%s", buff);
		
		fclose(fp);
		fclose(langf);
		free(buff);
	}
	
	//printf("Finished writing the first %d lines to files\n", lineMax);
}

double test(int N, int D, int langAM[][D], int iMHV[][D], char itemMemory[]) {
	DIR *dir;
	struct dirent *sd;
	FILE *fileID;

	int *testSumHV;
	testSumHV = (int*)malloc(D * sizeof(int));
	
	int *tmp;
	tmp = (int*)malloc(D * sizeof(int));
	
	char *buffer;
	buffer = (char*)malloc(300000 * sizeof(char));
	if(buffer == NULL) {
		printf("Not enough memory for buffer\n");
	}
	
	char fileLabel[][3] = {"af", "bg", "cs", "da", "nl", "de", "en", "et", "fi", "fr", "el", "hu", "it", "lv", "lt", "pl", "pt", "ro", "sk", "sl", "es", "sv"};
	int length = (sizeof fileLabel)/(sizeof fileLabel[0]);
	
	double angle = 0.0;
	char predicLang[3];
	double correct=0.0;
	double total = 0.0;
	double accuracy = 0.0;
	
	dir = opendir("/home/benlucas/Desktop/sim/testing_texts");
	
    if (dir == NULL) {
        printf("Failed: Directory could not be openned.\n");
        exit(1);
    }
	
	while((sd=readdir(dir)) != NULL) {
		char fileAddress[300];
		double maxAngle = -1.0;
		
        if (!strcmp (sd->d_name, "."))
            continue;
        if (!strcmp (sd->d_name, ".."))    
            continue;
		
		
		sprintf(fileAddress, "%s%s", "/home/benlucas/Desktop/sim/testing_texts/", sd->d_name);
		fileID = fopen(fileAddress, "r"); 
		
        if (fileID == NULL) {
            printf("Failed: %s could not be opened.\n", fileAddress);
            break;
        }

        int count=0;
		while(1) {
			buffer[count] = fgetc(fileID);
			if(feof(fileID)) {
				break;
			}
			count++;
		}
		buffer[count] = '\0';	//ending string, closing file

		fclose(fileID);
		//printf("Loaded: %s\n", fileAddress);
		
		computeSumHV (N, D, testSumHV, count, buffer, iMHV, itemMemory);
		binarizeHV(testSumHV, D);
		
		for(int l=0; l<length; l++) { //loop to go through the languages	
			for(int i=0; i<D; i++) {
				tmp[i] = langAM[l][i];
			}	

			angle = cosAngle(testSumHV, tmp, D);
			//printf("%f ", angle);
			
			if (angle > maxAngle) {
				maxAngle = angle;
				snprintf(predicLang, 3, "%c%c", fileLabel[l][0], fileLabel[l][1]);
			}
		}
		
		//printf("%s ", predicLang);
		
		if ((predicLang[0] == (sd->d_name)[0]) && (predicLang[1] == (sd->d_name)[1])) {
			correct++;
		}
		total++;
		
	}
	closedir(dir);
	free(buffer);
	free(tmp);
	free(testSumHV);
	
    accuracy = correct/total;
    accuracy = accuracy * 100.0;
	return accuracy;
}
double norm(int *a, int n) {
	double sum=0.0;
	double norm=0.0;
	for (int i=0; i<n; i++) {
		sum += ((double)a[i] * (double)a[i]);
	}
	norm = sqrt(sum);

	return norm;
}
double dotProduct(int *a, int *b, int n) {
	double product = 0.0;
	for (int i=0; i<n; i++) {
		product += ((double)a[i] * (double)b[i]);
	}	
	return product;
}
double cosAngle(int *a, int *b, int n) {
	return dotProduct(a, b, n)/(norm(a, n)*norm(b, n));	
}

void binarizeHV(int *v, int szofv) {
	int threshold = 0;
	for( int i=0; i<szofv; i++) {
		if( v[i] > threshold ) 
			v[i] = 1;
		else 
			v[i] = -1;
	}
}

void createItemMemoryHV(int D, int iMHV[][D], int size) {
	for(int i=0; i<size; i++) {
		int randomHV[D];
		genRandomHV(D, randomHV);
		for(int j=0; j<D; j++) {
			iMHV[i][j] = randomHV[j];
		}
	}
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

void buildLangHV(int N, int D, int langAM[][D], int iMHV[][D], char itemMemory[]) {
	FILE *fileID;
	int *sumHV;
	sumHV = (int*)malloc(D * sizeof(int));
	
	
	//Creating langLabels
	char langLabels[][4] = {"afr", "bul", "ces", "dan", "nld", "deu", "eng", "est", "fin", "fra", "ell", "hun", "ita", "lav", "lit", "pol", "por", "ron", "slk", "slv", "spa", "swe"};
	int length =  (sizeof langLabels)/(sizeof langLabels[0]); //size of langLabels
	
	char *buffer;
	buffer = (char*)malloc(2000000 * sizeof(char));
	
	if(buffer == NULL) {
		printf("Not enough memory!\n");
		exit(1);
	}
	
	//iterating through every file computingHV
	for(int t=0; t<length; t++) {
		//Creating the file address 
		char fileAddress[110];
		
		snprintf(fileAddress, 110, "%s%s%s", "/home/benlucas/Desktop/sim/training_textsSIM/", langLabels[t], ".txt");
		//printf("%s\n", fileAddress);
		
		//Opening the file address
		fileID = fopen(fileAddress, "r"); 
		
		//Check to make sure the file can be opened
		if (fileID == NULL) {
			printf("Failed: File could not be opened.\n");
			break;
		}
		
		
		//Compiles every character in the text document into array, buffer
		
		int count=0;
		while(1) {
			buffer[count] = fgetc(fileID);
			if(feof(fileID)) {
				buffer[count] = '\0';
				break;
			}
			count++;	
		}
		fclose(fileID);
		//printf("Loaded training language file %s\n", fileAddress);
		
		computeSumHV(N, D, sumHV, count, buffer, iMHV, itemMemory);
		 
		for(int i=0; i<D; i++) {
			langAM[t][i] = sumHV[i];
			//printf("%i ", langAM[t][i]);
		}
		//printf("\n");
	}
	free(sumHV);
	free(buffer);
}

void computeSumHV(int N, int D, int *sumHV, int count, char *buffer, int iMHV[][D], char itemMemory[]) { 
	int *block = (int *)malloc(N * D * sizeof(int)); 
	
	char key;
	
	for(int i=0; i<N; i++) {
		for(int j=0; j<D; j++) {
			*(block + i*D + j) = 0;
		}
	}
	
	for(int i=0; i<D; i++) {
		*(sumHV +i) = 0;
	}
	
	for(int i=0; i<count; i++) {
		key = buffer[i];
		circShift(N, D, block);
		lookupItemMemory(D, iMHV, itemMemory, key, block); 
		
		if (i >= N) {
			int *nGrams;
			nGrams = (int*)malloc(D * sizeof(int));
			
			//assigns nGrams to the first row of block
			for(int j=0; j<D; j++) {
				nGrams[j] = *(block + j);
			}	
			
			for(int j=1; j<N; j++) {
				for(int l=0; l<D; l++) {
					nGrams[l] = nGrams[l] * (*(block + j*D + l));
				}
			}
			
			for(int j=0; j<D; j++) {
				sumHV[j] = sumHV[j] + nGrams[j];
			}
			free(nGrams);
		}

	}
	free(block);
}
void circShift(int n, int d, int *arr) {	
    int *arr1 = (int *)malloc(n * d * sizeof(int)); 

	//format: *(arr + i*col + j));

	for(int i=0; i<1; i++) {
		for(int j=0; j<d; j++) {
			*(arr1 + i*d + j) = *(arr + (n-1)*d + j);
		}
	}
	
	for(int i=1; i<n; i++) {
		for(int j=0; j<d; j++) {
			*(arr1 + i*d + j) = *(arr + (i-1)*d + j);
		}
	}
	

	for(int i=0; i<n; i++) {
		for(int j=0; j<d-1; j++){
			*(arr + i*d + (j+1)) = *(arr1 + i*d + j);
		}
		
	}
	
	for(int i=0; i<n; i++) {
		for(int j=d-1; j<d; j++) {
			*(arr + i*d) = *(arr1 + i*d + (d-1));
		}
	}

	free(arr1);
}
void lookupItemMemory(int D, int iMHV[][D], char itemMemory[], char key, int *block) {
	for (int i=0; i<27; i++) {
		if (itemMemory[i] == key) {
				for(int j=0; j<D; j++) {
					*(block + j) = iMHV[i][j];
			}
			break;
		}
	}

}