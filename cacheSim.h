#ifndef CACHE_SIM_H_
#define CACHE_SIM_H_

#include <vector>
#include <cmath>


/*  helper function for extracting decimal-integers 
    out of boulian-representation.                    */
unsigned int bitExtracted(unsigned int number, int k, int p){
    return  (((1 << k) - 1) & (number >> (p - 1))) ;
}




struct Block{
    
// blocks data-------------------
    bool validBit;
    bool dirtyBit;
    unsigned long int tag; 
    unsigned long int data;
    unsigned long int statusLRU; 
// ------------------------------------------------------------------------------------
    explicit Block(bool validBit=false, bool dirtyBit=false , unsigned long int tag=0,
                    unsigned long int data=0, unsigned long int statusLRU=0 ):
                    validBit(validBit), dirtyBit(dirtyBit), tag(tag), data(data){}
//--------------------------------------------------------------------------------------
};



class Cache{

private:
    std::vector<std::vector<Block>> sets;
    const unsigned int layerSize;  
    const unsigned int blockSize;
    const unsigned int cyclesNum;

public:
Cache(unsigned int associativity, unsigned int layerSize, unsigned int blockSize, unsigned int cyclesNum):
            sets( pow(2,layerSize-blockSize-associativity), std::vector<Block>( pow(2,associativity),Block() )  ),
            layerSize(layerSize),blockSize(blockSize),cyclesNum(cyclesNum){
    }
    Cache operator=(const Cache&)=delete;
    Cache(const Cache&)=delete;
    ~Cache();


    std::vector<Block>& getSet(unsigned int address);
    Block& getBlock(unsigned int address);
    

};




#endif          //  CACHE_SIM_H_