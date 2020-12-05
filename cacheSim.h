#ifndef CACHE_SIM_H_
#define CACHE_SIM_H_

#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <string>

static const char READ = 'r';
static const char WRITE = 'W';
static const bool DIRTY = true;
static const bool NOT_DIRTY = false;
static const bool NO_WRITE_ALLOCATE = false;
static const bool WRITE_ALLOCATE = true;
static const int LEAST_RECENTLY_USED = 0;


/*  helper function for extracting decimal-integers 
    out of boulian-representation.                    */
unsigned long int bitExtracted(unsigned long int number, int k, int p){
    return  (((1 << k) - 1) & (number >> (p - 1))) ;
}




struct Block{
    
// blocks data-------------------
    bool valid;
    bool dirtyBit;
    unsigned long int tag; 
    unsigned long int data;
    unsigned long int statusLRU; 
// ------------------------------------------------------------------------------------
    explicit Block(bool valid=false, bool dirtyBit=NOT_DIRTY , unsigned long int tag=0,
                    unsigned long int data=0, unsigned long int statusLRU=0 ):
                    valid(valid), dirtyBit(dirtyBit), tag(tag), data(data){}
//--------------------------------------------------------------------------------------
};




struct Cache{

    std::vector<std::vector<Block>> sets;
    std::vector<int> lruPolicy;
    const unsigned long int layerSize;  
    const unsigned long int blockSize;
    const unsigned long int cyclesNum;
    const unsigned long int totalWaysNum;
    //for statistics:
    int missNum;
    int acssesNum;


    Cache(unsigned long int associativity, unsigned long int layerSize, unsigned long int blockSize, unsigned long int cyclesNum):
            sets(  pow(2,layerSize-blockSize-associativity), std::vector<Block>( pow(2,associativity),Block() )  ),
            lruPolicy( sets.size() , 0 ), layerSize(layerSize),blockSize(blockSize),
            cyclesNum(cyclesNum),totalWaysNum( sets[0].size() ),missNum(0),acssesNum(0){
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


    
    std::vector<Block>::iterator getBlock(unsigned long int address){
        assert( hasBlockOf(address) );
        
        std::vector<Block>& set = getSet(address);
        unsigned long int tag = getTag(address);

        std::vector<Block>::iterator block;
        for( block = set.begin() ; block!=set.end() ; block++){
            if(  (block->valid==true) && (block->tag==tag)  ){
                break;
            }
        }
        return block;
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


    std::vector<Block>::iterator updateLRU(int address){

        int i = bitExtracted(address,layerSize-blockSize-sets[0].size(),blockSize+1);
        getBlock(address)->statusLRU = ++lruPolicy[i];

        class compare{
        public:
            bool operator()(const Block& x,const Block& y)const{
                return (x.statusLRU < y.statusLRU) ;
            }
        };

        std::sort(sets[i].begin(), sets[i].end(), compare());
        return getBlock(address);
    }

};


struct Memory{

    Cache L1;
    Cache L2;

    bool writePolicy;
    unsigned long int blockSize; // log2(blockSize)
    unsigned long int cyclesNum;
    //for statistics:

    double totalTime;
    double acessNum;

    Memory(unsigned long int associativity1, unsigned long int Size1, unsigned long int cyclesNum1,
          unsigned long int associativity2, unsigned long int Size2, unsigned long int cyclesNum2,
          bool writePolicy, unsigned long int blockSize, unsigned long int cyclesNum3):
          L1(associativity1, Size1, blockSize,cyclesNum1), L2(associativity2, Size2, blockSize,cyclesNum2),
          writePolicy(writePolicy),blockSize(blockSize),cyclesNum(cyclesNum3), totalTime(0),acessNum(0){
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
            free = L2.freeWayFor(address);
            tag = L1.getTag(address);
            // no-need for LRU-policy managing. read-request sent to Mem
        }
        assert( free->valid==false );
        free->valid = true;
        free->tag = tag;
        free->data = address;
        assert ( free->dirtyBit==NOT_DIRTY );

        return free;
    }

    void evictFrom(Cache& Li,unsigned long int address){//xxx
        unsigned long int evictedAddress = Li.getSet(address)[LEAST_RECENTLY_USED].data;
        bool& bit = Li.getSet(address)[LEAST_RECENTLY_USED].dirtyBit;

        if( bit==DIRTY  &&  &Li==&L1 )  L2.updateLRU( evictedAddress )->dirtyBit=DIRTY; // write L2
        bit = NOT_DIRTY;            
    }
            

    std::vector<Block>::iterator evictAndPut(unsigned long int address){
        std::vector<Block>::iterator evicted;
        unsigned long int tag;

        // miss in L1 and hit in L2
        if( (L1.hasBlockOf(address)==false) && (L2.hasBlockOf(address)) ){
            tag = L1.getTag(address);
            evicted = L1.leastRecentlyUsed(address);
            L2.updateLRU(address); //  <----- read L2
            evictFrom(L1,address);
            L1.updateLRU(address); // <-----write\read L1
        }
        // miss in L2 thus acsses Mem
        else{ assert( (L1.hasBlockOf(address)==false) && (L2.hasBlockOf(address)==false) );
            tag = L2.getTag(address);
            evicted = L2.leastRecentlyUsed(address);
            evictFrom(L2,address);
            L2.updateLRU(address); // <-----write\read L1
        }

        assert( evicted->valid==true );
        evicted->tag = tag;
        evicted->data = address;
        assert ( evicted->dirtyBit == NOT_DIRTY );

        return evicted;
    }



    void L1_Hit(unsigned long address,char operation){
       std::vector<Block>::iterator target = L1.updateLRU(address);
       target->dirtyBit = (operation=='w') ? DIRTY :  target->dirtyBit;
    }


    void L2_Hit(unsigned long address,char operation){
        std::vector<Block> set = L1.getSet(address);
        char hit = operation;
        unsigned int usedWays = 0;
        for( unsigned int i=0 ; i<set.size() ; ++i ) usedWays = (set[i].valid) ? (usedWays+1) : (usedWays);
        
        if( WRITE==hit ){

            if( writePolicy==WRITE_ALLOCATE ){//----->> read_Hit in L2
                if( usedWays < L1.totalWaysNum ){
                    putInFreeWay(address) -> dirtyBit=DIRTY;
                    return;
                }
                else{ assert( usedWays == L1.totalWaysNum );
                    evictAndPut(address) -> dirtyBit=DIRTY;
                    return;
                }

            }else{ assert( writePolicy==NO_WRITE_ALLOCATE );//----->> write_Hit in L2
                L2.updateLRU(address) -> dirtyBit=DIRTY;
                return;
            }
            

        }else{ assert( READ==hit );
            if( usedWays!=set.size() ){
                putInFreeWay(address);
                return;
            }
            //else - no availible way
            evictAndPut(address);
            return;
        }
    }



    void L2_snoops_L1(unsigned long int address){//xxx
        unsigned long int evictedAddress_L2 = L2.getSet(address)[LEAST_RECENTLY_USED].data;
        if( L1.hasBlockOf(evictedAddress_L2)==false ){
            return;
        }

        *( L1.getBlock(evictedAddress_L2) ) = Block();
        // if the data is dirty in L1 ==>> we turn the dirtyBit also in L2
    }


    void handle_L2_Miss(unsigned long int address){
        std::vector<Block> set = L2.getSet(address);
        unsigned int usedWays = 0;
        for( unsigned int i=0 ; i<set.size() ; ++i ) usedWays = (set[i].valid) ? (usedWays+1) : (usedWays);
        
        if( usedWays < L2.totalWaysNum ){
           //todo
            return;
        }
        else{ assert( usedWays == L2.totalWaysNum );
            //todo
            return;
        }
    }
    


    void handle_L1_Miss(unsigned long int address, char operation){
        std::vector<Block> set = L1.getSet(address);
        unsigned int usedWays = 0;
        for( unsigned int i=0 ; i<set.size() ; ++i ) usedWays = (set[i].valid) ? (usedWays+1) : (usedWays);
        

        if( usedWays < L1.totalWaysNum ){
           //todo
            return;
        }
        else{ assert( usedWays == L1.totalWaysNum );
            //todo
            return;
        }

    }

    void L1_and_L2_Miss(unsigned long int address,char operation){
        if ( (writePolicy==NO_WRITE_ALLOCATE) && (operation == 'w') ) {
            /* we only need to write the data to the
             block that is allready i the main memory */
            return;
        }
        
        assert( (writePolicy==WRITE_ALLOCATE) || (operation == 'r') );
        handle_L2_Miss(address);
        handle_L1_Miss(address,operation);
    }



};

    


#endif          //  CACHE_SIM_H_