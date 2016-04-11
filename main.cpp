/* author: Ran Wang`
 * Created on 2016/3/29 */
#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <errno.h>
#include <math.h>
#include <cstdlib>
#include <inttypes.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctime>

#include "ethash.h"
#include "internal.h"

#define cacheFileName "cacheFile"
#define blocknumberFileName "blocknumberFile"

using namespace std;

FILE * openFile(const char* fileName){
    FILE * fileHandler = NULL; // read full_dataset file
    fileHandler = fopen(fileName,"r+");
    if(fileHandler == NULL) {
        printf("%s open failed!\n", fileName);
        return NULL;
    }
    else
        printf("%s open succeeded!\n", fileName);
    return fileHandler;
}

uint64_t sizeOfFile(FILE * fileHandler){
    int bytes=0, c;
    while ((c = fgetc(fileHandler)) != EOF) {
       bytes++;
    }
    printf("File size: %d bytes\n",bytes);
    return uint64_t(bytes);
}

int main() {    
    //+++ begin: using the cache file to generate full
    ethash_light_t light = (ethash_light_t)malloc(sizeof(char)* 1000);
    FILE* readCacheFileHandler = openFile(cacheFileName);
    uint64_t cacheSize = sizeOfFile(readCacheFileHandler);
    light->cache_size = cacheSize;
    char* readCacheTempBuffer = (char*)malloc(sizeof(char)* cacheSize);
    for(uint64_t i =0; i< cacheSize; i++ )
        fscanf(readCacheFileHandler, "%c", &readCacheTempBuffer[i]);	
    light->cache =  (void*)readCacheTempBuffer;
    fclose(readCacheFileHandler);
    //+++ end: using the cache file to generate full

    //+++ begin: file to store blocknumber
    FILE* readBlocknumberFileHandler = openFile(blocknumberFileName);
    uint64_t writeBlocknumber = 0;
    fscanf(readBlocknumberFileHandler, "%lu", &writeBlocknumber);
    light->block_number = writeBlocknumber;
    fclose(readBlocknumberFileHandler);
    //+++ end: file to store blocknumber

    // get header
    ethash_h256_t header; 
    printf("The header is ");
    for(int i=0;i<32;i++){
        header.b[i]=uint8_t(i*2);
        printf("%u ", unsigned(header.b[i]));
    }
    printf("\n");
    
    // get nonce
    uint64_t nonce = 12345678;
    printf("The nonce is %ld.\n",nonce);
    printf("\n");

    //*********************************** main loop *************************************
    printf("Starting ethash hash computing using cache...\n");
    printf("\n");
    ethash_return_value_t ret = ethash_light_compute(light, header, nonce);
  
   
    //********************************* print results *************************************
    printf("Computation done. The results are\n");
    printf("    Ethash result: ");
    for(int i=0;i<32;i++) printf("%u, ",ret.result.b[i]);
    printf("\n");
    printf("    Ethash mixHash: ");
    for(int i=0;i<32;i++) printf("%u, ",ret.mix_hash.b[i]);
    printf("\n");
    printf("    Ethash computation successful? %s\n",ret.success ? "Yes":"No");
    free(light);
    free(readCacheTempBuffer);

    return 0;
}

/*
#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <errno.h>
#include <math.h>
#include <cstdlib>
#include <inttypes.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctime>

#include "ethash.h"
#include "internal.h"

uint8_t magic_number[]  = {0xfe, 0xca, 0xdd, 0xba, 0xad, 0xde, 0xe1, 0xfe};
#define FILE_NAME "full-R23-0000000000000000"

using namespace std;

FILE * openFile(const char* fileName){
    FILE * full_dataset_file = NULL; // read full_dataset file
    full_dataset_file = fopen(fileName,"r+");
    if(full_dataset_file == NULL) {
        printf("open failed!\n");
        return NULL;
    }
    else
        printf("open succeeded!\n");
    return full_dataset_file;
}

uint64_t sizeOfFile(FILE * full_dataset_file){
    int bytes=0, c;
    while ((c = fgetc(full_dataset_file)) != EOF) {
       bytes++;
    }
    printf("File size: %d bytes\n",bytes);
    return uint64_t(bytes);
}

int main() {    
    
    //+++++++++++++++ Initialize input parameters 
    ethash_full_t dataset =  ethash_full_t(malloc(sizeof(char)*100000000));
    //srand(time(0));
    
    FILE * full_dataset_file = openFile(FILE_NAME); // open file
    if(!full_dataset_file){ 
        printf("Program exits. \n");
        return 0;
    }

    dataset->file_size = (uint64_t)(sizeOfFile(full_dataset_file)-8); //  return the size of the file in bytes
    dataset->file = full_dataset_file;  // set the dataset file handler
 
    FILE *fp = openFile(FILE_NAME); 
    uint8_t* dataWord = (uint8_t*)malloc(sizeof(char)*dataset->file_size);
    uint8_t* magicWord = (uint8_t*)malloc(sizeof(char)*8);
    cout<< endl << "Comparing 8-byte magic number at the beginning: " << endl;
    for(int i =0; i< dataset->file_size;i++ ){
        if(i<8){
            fscanf(fp, "%c", magicWord+i);
            printf("    Check magic number: %d\n", magicWord[i]);
            assert(magicWord[i] ==  magic_number[i]);
        }
        else{
            fscanf(fp, "%c", dataWord+i-8);
        }		
    }
    printf("\n");
    dataset->data = (node*)dataWord;
    fclose(fp);

    // get header
    ethash_h256_t header; 
    printf("The header is ");
    for(int i=0;i<32;i++){
        header.b[i]=uint8_t(i*2);
        printf("%u ", unsigned(header.b[i]));
    }
    printf("\n");
    
    // get nonce
    uint64_t nonce = 12345678;
    printf("The nonce is %ld.\n",nonce);
    printf("\n");

    //*********************************** main loop *************************************
    printf("Starting ethash hash computing...\n");
    printf("\n");
    ethash_light_t const light= NULL; // This parameter does not matter in this case
    ethash_return_value_t ret = ethash_full_compute(dataset, header, nonce);
  
    //********************************* close data file ***********************************
    fclose(full_dataset_file);
    full_dataset_file=NULL;
    
    //********************************* print results *************************************
    printf("Computation done. The results are\n");
    printf("    Ethash result: ");
    for(int i=0;i<32;i++) printf("%u, ",ret.result.b[i]);
    printf("\n");
    printf("    Ethash mixHash: ");
    for(int i=0;i<32;i++) printf("%u, ",ret.mix_hash.b[i]);
    printf("\n");
    printf("    Ethash computation successful? %s\n",ret.success ? "Yes":"No");
    free(dataWord);
    free(magicWord);

    return 0;
}
*/
