import numpy as np
import os
import glob
import fnmatch
import time

import csv
#will read a training file and yield the buffer, looping thru langLabels
def readLabels(langLabels): 
	i = 0
	while i < langLabels.size:
		address = '../training_texts/' + langLabels[i] + '.txt'
		buff = []
		for row in open(address, 'r'):
			buff.append(row)
		yield np.array(buff, dtype='str'), i
		i+=1

#given numpy arr, scan each letter one by one and yield it
def getKeys(arr): 
	i = 0
	for line in arr:
		for char in line:
			yield char
			
			
#this gen function will scan through the testing_files and return buffer and file name
def readTestFiles(path): 
	pattern = '*.txt'
	for textFile in os.listdir(path):
		if fnmatch.fnmatch(textFile, pattern):
			buff=[]
			for row in open(os.path.join(path, textFile)):
				buff.append(row)
			yield np.array(buff, dtype='str'), textFile
	



#this function will generate a completely random array of 1's and -1's of
# size D
def genRandomHV (D):
	if D % 2:
		print('dimension is odd!')
	else:
		randomIndex = np.random.permutation(D)
		randomHV = np.zeros((1, D), dtype='int')
		half = D/2
		randomHV[0, np.where(randomIndex < half)] = 1
		randomHV[0, np.where(randomIndex >= half)] = -1
	return randomHV

#given a dictionary, a Key, and D, if key exists in dictionary return value, if not
# create a new hypervector and enter it into the dictionary as a new key and return the new
# dictionary and new hypervector
def lookupItemMemory(itemM, key, D):
	if key in itemM:
		randomHV = itemM[key]
	else:
		itemM[key] = genRandomHV(D)
		randomHV = itemM[key]
	
	return itemM, randomHV

#core function. takes hypervectors for letters N at a time and circularly shifts them down
#and to the right. the N block is multiplied and added to the sumHV to create the languageHV
def computeSumHV(text, itemMemory, N, D):
	block = np.zeros((N, D), dtype='int')
	sumHV = np.zeros((1, D), dtype='int')
	i = 0
	for key in getKeys(text):
		block = np.roll(block, 1, [0,1])
		
		itemMemory, block[0,:] = lookupItemMemory(itemMemory, key, D)
		
		if i >= N:
			nGrams = block[0, :]
			for j in range(1, N, 1):
				nGrams = np.multiply(nGrams, block[j, :])
			sumHV = np.add(sumHV, nGrams)
		
		i+=1
	return itemMemory, sumHV

#read in training files and computeSumHV on them
def buildLanguageHV(iM, langHV, langLabels, N, D):
	for data, i in readLabels(langLabels):
		iM, langHV = computeSumHV(data, iM, N, D)
		langAM[langLabels[i]] = langHV
		print('file read')
		
	print(iM)
	print(langAM)
	return iM, langAM


if __name__ == '__main__':
	print('training program')
	N= int(input('Enter value for N: '))
	D= int(input('Enter value for D: '))
	iM = {}
	langAM = {}
	langLabels = np.array(['afr', 'bul', 'ces', 'dan', 'nld', 'deu', 'eng', 'est', 'fin', 'fra', 'ell', 'hun', 'ita', 'lav', 'lit', 'pol', 'por', 'ron', 'slk', 'slv', 'spa', 'swe'])
	
	start_time = time.time()
	
	iM, langAM = buildLanguageHV(iM, langAM, langLabels, N, D)
	
	#check if folder exists, create or clear it
	path = 'cachedTraining/' + str(D) + '_' + str(N)
	if not os.path.isdir(path):
		os.mkdir(path)
	
	#write itemMemory
	for key, value in iM.items():
		filename = 'im_' + key + '.csv'
		with open(path + '/' + filename, 'w') as thefile:
			np.savetxt(thefile, value, delimiter=',')

	for key, value in langAM.items():
		filename = 'la_' + key + '.csv'
		with open(path + '/' + filename, 'w') as thefile:
			np.savetxt(thefile, value, delimiter=',')
	
	
	end_time = time.time()
	
	print('the program took ' + str(end_time-start_time) + ' seconds to run')
