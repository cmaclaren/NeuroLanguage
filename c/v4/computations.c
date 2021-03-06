#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <wchar.h>
 
void perm(int D, int *arr) {	
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
void genRandomHV(int D, int *randomHV) {
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
void lookupItemMemory(int D, int *block, wchar_t *buffer, wchar_t *itemMemory, wchar_t key, int *iMHV, int imSize) {
    for(int i=0; i<imSize; i++) {
        if (key == itemMemory[i]) {
            for(int j=0; j<D; j++) {
                *(block + j) = *(iMHV + i*D + j);
            }
			break;
        }
    }
}
void computeSumHV(int N, int D, int *sumHV, wchar_t *buffer, int count, wchar_t *itemMemory, int *iMHV, int imSize) {
    int *block = (int*)malloc(N * D * sizeof(int));
     for(int i=0; i<D; i++) {
        sumHV[i] = 0;
    }

    for(int i=0; i<N; i++) {
        for(int j=0; j<D; j++) {
            *(block + i*D + j) = 0;
        }
    }

    for(int i=0; i<count; i++) {
        wchar_t key = buffer[i];
        circShift(N, D, block);
        lookupItemMemory(D, block, buffer, itemMemory, key, iMHV, imSize);

        if (i >= N) {
            int *nGrams;
            nGrams = (int*)malloc(D * sizeof(int));

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