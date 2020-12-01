#ifndef CACHE_SIM_H_
#define CACHE_SIM_H_

#include <vector>
#include <cmath>
#include <cassert>


/*  helper function for extracting decimal-integers 
    out of boulian-representation.                    */
unsigned int bitExtracted(unsigned int number, int k, int p){
    return  (((1 << k) - 1) & (number >> (p - 1))) ;
}




struct Block{
    
// blocks data-------------------
    bool valid;
    bool dirty;
    unsigned long int tag; 
    unsigned long int data;
    unsigned long int statusLRU; 
// ------------------------------------------------------------------------------------
    explicit Block(bool valid=false, bool dirty=false , unsigned long int tag=0,
                    unsigned long int data=0, unsigned long int statusLRU=0 ):
                    valid(valid), dirty(dirty), tag(tag), data(data){}
//--------------------------------------------------------------------------------------
};



struct Cache{

    std::vector<std::vector<Block>> sets;
    const unsigned int layerSize;  
    const unsigned int blockSize;
    const unsigned int cyclesNum;
    //for statistics:
    int missNum;
    int acssesNum;


    Cache(unsigned int associativity, unsigned int layerSize, unsigned int blockSize, unsigned int cyclesNum):
            sets(  pow(2,layerSize-blockSize-associativity), std::vector<Block>( pow(2,associativity),Block() )  ),
            layerSize(layerSize),blockSize(blockSize),cyclesNum(cyclesNum),missNum(0),acssesNum(0){
    }

    Cache operator=(const Cache&)=delete;
    Cache(const Cache&)=delete;
    ~Cache();


    std::vector<Block>& getSet(unsigned long int address){
        int waysNum = sets[0].size();
        int bitsNum = layerSize-blockSize-waysNum;
        int index = bitExtracted(address,bitsNum,blockSize+1);

        return sets[index];
    }

    unsigned long int getTag(unsigned int address){
        int waysNum = sets[0].size();
        int bitsNum = 32-(layerSize-waysNum+1);

        return bitExtracted(address,bitsNum,layerSize-waysNum+1);
    }


    
    Block& getBlock(unsigned long int address){
        assert( hasBlockOf(address) );
        
        std::vector<Block>& set = getSet(address);
        unsigned long int tag = getTag(address);

        std::vector<Block>::iterator block;
        for( block = set.begin() ; block!=set.end() ; block++){
            if(  (block->valid==true) && (block->tag==tag)  ){
                return *block;
            }
        }
    }




    bool hasBlockOf(unsigned long int address){
        std::vector<Block>& set = getSet(address);
        unsigned long int tag = getTag(address);

        std::vector<Block>::iterator block;
        for( block = set.begin() ; block!=set.end() ; block++){
            if(  (block->valid==true) && (block->tag==tag)  ){
                return true;
            }
        }
        return false;
    }


};


struct Memory{

    Cache L1;
    Cache L2;

    bool isWriteAllocate;
    unsigned int blockSize; // log2(blockSize)
    unsigned int cyclesNum;
    //for statistics:

    double totalTime;
    double acessNum;

    Memory(unsigned int associativity1, unsigned int Size1, unsigned int cyclesNum1,
          unsigned int associativity2, unsigned int Size2, unsigned int cyclesNum2,
          bool isWriteAllocate, unsigned int blockSize, unsigned int cyclesNum3):
          L1(associativity1, Size1, blockSize,cyclesNum1), L2(associativity2, Size2, blockSize,cyclesNum2),
          isWriteAllocate(isWriteAllocate),blockSize(blockSize),cyclesNum(cyclesNum3), totalTime(0),acessNum(0){
        

    }

    void L1_Hit(unsigned long address,char operation){
         Block& desired = L1.getBlock(address);

    }
    

};


#endif          //  CACHE_SIM_H_