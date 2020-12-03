#ifndef CACHE_SIM_H_
#define CACHE_SIM_H_

#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <string>

static const char READ = 'r';
static const char WRITE = 'W';


/*  helper function for extracting decimal-integers 
    out of boulian-representation.                    */
unsigned long int bitExtracted(unsigned long int number, int k, int p){
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
    std::vector<int> lruPolicy;
    const unsigned long int layerSize;  
    const unsigned long int blockSize;
    const unsigned long int cyclesNum;
    //for statistics:
    int missNum;
    int acssesNum;


    Cache(unsigned long int associativity, unsigned long int layerSize, unsigned long int blockSize, unsigned long int cyclesNum):
            sets(  pow(2,layerSize-blockSize-associativity), std::vector<Block>( pow(2,associativity),Block() )  ),
            lruPolicy( sets.size() , 0 ), layerSize(layerSize),blockSize(blockSize),
            cyclesNum(cyclesNum),missNum(0),acssesNum(0){
    }

    Cache operator=(const Cache&)=delete;
    Cache(const Cache&)=delete;

    std::vector<Block>& getSet(unsigned long int address){
        int waysNum = sets[0].size();
        int bitsNum = layerSize-blockSize-waysNum;
        int index = bitExtracted(address,bitsNum,blockSize+1);

        return sets[index];
    }

    unsigned long int getTag(unsigned long int address){
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
                break;
            }
        }
        return *block;
    }


    // assums THERE IS a free way
    std::vector<Block>::iterator freeWayFor(unsigned long int address){
        int index= bitExtracted(address,layerSize-blockSize-sets[0].size(),blockSize+1);
        std::vector<Block>::iterator i;
        for( i=sets[index].begin() ; i!=sets[index].end(); i++){
            if ( i->valid==false ){
                break;
            }
        }
        assert( i!=sets[index].end() );
        return i;
    }

    // assums THERE IS a free way
    std::vector<Block>::iterator leastRecentlyUsed(unsigned long int address){
        int index= bitExtracted(address,layerSize-blockSize-sets[0].size(),blockSize+1);
        std::vector<Block>::iterator it = sets[index].begin();
        assert( it->valid==false );
        return it;
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


    Block& updateLRU(int address){

        int i = bitExtracted(address,layerSize-blockSize-sets[0].size(),blockSize+1);
        getBlock(address).statusLRU = ++lruPolicy[i];

        class compare{
            bool operator()(int x,int y)const{
                return (x < y) ;
            }
        };

        std::sort(sets[i].begin(), sets[i].end(), compare());
        return getBlock(address);
    }


};


struct Memory{

    Cache L1;
    Cache L2;

    bool writeAllocatePolicy;
    unsigned long int blockSize; // log2(blockSize)
    unsigned long int cyclesNum;
    //for statistics:

    double totalTime;
    double acessNum;

    Memory(unsigned long int associativity1, unsigned long int Size1, unsigned long int cyclesNum1,
          unsigned long int associativity2, unsigned long int Size2, unsigned long int cyclesNum2,
          bool writeAllocatePolicy, unsigned long int blockSize, unsigned long int cyclesNum3):
          L1(associativity1, Size1, blockSize,cyclesNum1), L2(associativity2, Size2, blockSize,cyclesNum2),
          writeAllocatePolicy(writeAllocatePolicy),blockSize(blockSize),cyclesNum(cyclesNum3), totalTime(0),acessNum(0){
    }


    //assumes that Level_i got miss and that Level_i+1 got hit
    std::vector<Block>::iterator putInFreeWay(unsigned long int address){
        std::vector<Block>::iterator free;
        unsigned long int tag;
        
        if( (L1.hasBlockOf(address)==false) && (L2.hasBlockOf(address)) ){
            free = L1.freeWayFor(address);
            tag = L1.getTag(address);
            L2.updateLRU(address); // read-request sent to L2
        }
        else{ assert( (L1.hasBlockOf(address)==false) && (L2.hasBlockOf(address)==false) );
            tag = L1.getTag(address);
            free = L2.freeWayFor(address);
            // no-need for LRU-policy managing. read-request sent to Mem
        }
        assert( free->valid==false );
        free->valid = true;
        free->tag = tag;
        free->data = address;
        assert ( free->dirty==false );

        return free;
    }

    std::vector<Block>::iterator evictAndPut(unsigned long int address){
        std::vector<Block>::iterator evicted;
        unsigned long int tag;

        if( (L1.hasBlockOf(address)==false) && (L2.hasBlockOf(address)) ){
            tag = L1.getTag(address);
            evicted = L1.leastRecentlyUsed(address);
        }
        else{ assert( (L1.hasBlockOf(address)==false) && (L2.hasBlockOf(address)==false) );
            tag = L2.getTag(address);
            evicted = L2.leastRecentlyUsed(address);
        }


    }

    void L1_Hit(unsigned long address,char operation){
       Block& target = L1.updateLRU(address);
       target.dirty = (operation=='w') ? true :  target.dirty;
    }


    void L2_Hit(unsigned long address,char hit){
        std::vector<Block> set = L2.getSet(address);
        char hit = operation;
        int usedWays = 0;
        for( int i=0 ; i<set.size() ; ++i ) usedWayws = (set[i].valid) ? (usedWayws+1) : (usedWayws);
        

        if( WRITE==hit ){
            if( writeAllocatePolicy ){
                if( usedWayws!=set.size() ){
                    putInFreeWay(address)->dirty=true;
                    return;
                }
                //else - evictANDput

            }
            //else: noWriteAllocatePolicy
            L2.updateLRU(address).dirty = true;

        }else{ assert( READ==hit );

        }
    }



};

    


#endif          //  CACHE_SIM_H_