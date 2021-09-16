#ifndef CACHE_SIM_H_
#define CACHE_SIM_H_

#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <string>

static const char READ = 'r';
static const char WRITE = 'w';
static const bool DIRTY = true;
static const bool NOT_DIRTY = false;
static const bool NO_WRITE_ALLOCATE = false;
static const bool WRITE_ALLOCATE = true;
static const int LEAST_RECENTLY_USED = 0;


/*      helper function for extracting decimal-integers 
                out of boulian-representation.                         */
//======================================================================
unsigned long int bitExtracted(unsigned long int number, int k, int p){
    return  (((1 << k) - 1) & (number >> (p - 1))) ;
}



/*     Auxiliary struct for representing a block from the main-Memory,
        that should be (or already) stored inside the cache-Memory          */
//====================================================================================
struct Block{
// block's data-------------------
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





/*                      Auxiliary struct for representing a layer of the cache-Memory,
                                        i.e L1-cache or L2-cache                                                 */
//===============================================================================================================
struct Cache{
// Cache's data---------------------------
    std::vector< std::vector<Block> > sets;
    std::vector<int> lruPolicy;
    const unsigned long int layerSize;  
    const unsigned long int blockSize;
    const unsigned long int cyclesNum;
    const unsigned long int totalWaysNum;
// for statistics:------------------------
    int missNum;
    int acssesNum;
// -------------------------------------------------------------------------------------------------------------------
    Cache(unsigned long associativity, unsigned long layerSize, unsigned long blockSize, unsigned long cyclesNum) :
            sets(  pow(2,layerSize-blockSize-associativity), std::vector<Block>( pow(2,associativity),Block() )  ),
            lruPolicy( sets.size() , 0 ), layerSize(layerSize),blockSize(blockSize),
            cyclesNum(cyclesNum),totalWaysNum( sets[0].size() ),missNum(0),acssesNum(0){
    }

    // gets the set in which the address is mapped to
    std::vector<Block>& getSet(unsigned long int address){
        int waysBits = log2(totalWaysNum);
        int setBits = layerSize-blockSize-waysBits;
        int index = bitExtracted(address,setBits,blockSize+1);

        return sets[index];
    }


    // gets the tag of the address in this level-cache's point of view
    unsigned long int getTag(unsigned long int address){
        int waysBits = log2(totalWaysNum);
        int bitsNum = 32-(layerSize-waysBits+1);

        return bitExtracted(address,bitsNum,layerSize-waysBits+1);
    }


    // gets the memory-block in which address is part of it
    std::vector<Block>::iterator getBlock(unsigned long int address){
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




    //finds the free-way in the relevant set for this cache-layer. (assums THERE IS a free way)
    std::vector<Block>::iterator freeWayFor(unsigned long int address){
        int index= bitExtracted(address,layerSize-blockSize-log2(totalWaysNum),blockSize+1);
        std::vector<Block>::iterator i;
        for( i=sets[index].begin() ; i!=sets[index].end(); i++){
            if ( i->valid==false ){
                break;
            }
        }
        assert( i!=sets[index].end() );
        return i;
    }


    //finds the least-recently-used way in the relevant set for this cache-level. (assums THERE IS a free way)
    std::vector<Block>::iterator leastRecentlyUsed(unsigned long int address){
        int index= bitExtracted(address,layerSize-blockSize-log2(totalWaysNum),blockSize+1);
        std::vector<Block>::iterator it = sets[index].begin();
        assert( it->valid==true );
        return it;
    }


    // checks if this cache-level is currently holding the address's block
    bool containsBlockOf(unsigned long int address){
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


    // method for managing the policy of Blocks-evacuation from the cache
    std::vector<Block>::iterator updateLRU(int address){
        int i = bitExtracted(address,layerSize-blockSize-log2(totalWaysNum),blockSize+1);

        class compareLRU{
        // Functor for comparing Blocks
        // based on LRU-policy-status
        public:
            inline bool operator()(const Block& b1,const Block& b2)const{
                return (b1.statusLRU < b2.statusLRU) ;
            }
        };

        getBlock(address)->statusLRU = ++lruPolicy[i];
        std::sort( sets[i].begin(), sets[i].end(), compareLRU() );
        return getBlock(address);
    }
};






/*                     THE data-type for representing the whole memory structure.
                                this struct is the type that main calls                                    */
//=========================================================================================================
struct Memory{
// Memory's cache levels:
    Cache L1;
    Cache L2;
// Memory's mete_data-------------------------------
    bool writePolicy;
    unsigned long int blockSize; // log2(blockSize)
    unsigned long int cyclesNum;
// for statistics-----------------------------------
    double totalTime;
    double acessNum;
//-------------------------------------------------------------------------------------------------------
    Memory(unsigned long int associativity1, unsigned long int Size1, unsigned long int cyclesNum1,
          unsigned long int associativity2, unsigned long int Size2, unsigned long int cyclesNum2,
          bool writePolicy, unsigned long int blockSize, unsigned long int cyclesNum3):
          L1(associativity1, Size1, blockSize,cyclesNum1), L2(associativity2, Size2, blockSize,cyclesNum2),
          writePolicy(writePolicy),blockSize(blockSize),cyclesNum(cyclesNum3), totalTime(0),acessNum(0){
    }



    // vacates a block with address - due to Li (where i=1 OR i=2 ) got Miss for capacity or compulsary
    void evacuateFrom(Cache& Li,unsigned long int address){
        assert( Li.getSet(address)[LEAST_RECENTLY_USED].valid );
        unsigned long int evictedAddress = Li.getSet(address)[LEAST_RECENTLY_USED].data;
        bool& bit = Li.getSet(address)[LEAST_RECENTLY_USED].dirtyBit;

        if( bit==DIRTY  &&  &Li==&L1 ){
            L2.updateLRU( evictedAddress )->dirtyBit=DIRTY; // write L2
        }
        bit = NOT_DIRTY;            
    }



    // writes a block to a non-full-set in cache (assumes that Level_1 got miss)
    std::vector<Block>::iterator putInFreeWay(unsigned long int address){
        Cache* targetCache;
        std::vector<Block>::iterator free;
        unsigned long int tag;
        
        if( (L1.containsBlockOf(address)==false) && (L2.containsBlockOf(address)) ){
            targetCache = &L1;
            free = L1.freeWayFor(address);
            tag = L1.getTag(address);
            L2.updateLRU(address); // read-request sent to L2
        }
        else{ assert( (L1.containsBlockOf(address)==false) && (L2.containsBlockOf(address)==false) );
            targetCache = &L2;
            free = L2.freeWayFor(address);
            tag = L2.getTag(address);
            // no-need for LRU-policy managing. read-request sent to Mem
        }
        assert( free->valid==false );
        free->valid = true;
        free->tag = tag;
        free->data = address;
        assert ( free->dirtyBit==NOT_DIRTY );

        targetCache->updateLRU(address); // <---- read target cache ( L1 or L2 )
        return targetCache->getBlock(address);
    }



    // writes a block to a full-set in the cache (assumes that Level_1 got miss)
    std::vector<Block>::iterator evacuateAndPut(unsigned long int address){
        Cache* targetCache;
        std::vector<Block>::iterator evicted;
        unsigned long int tag;

        // miss in L1 and hit in L2
        if( (L1.containsBlockOf(address)==false) && (L2.containsBlockOf(address)) ){
            targetCache = &L1;
            evicted = L1.leastRecentlyUsed(address);
            L2.updateLRU(address); //  <----- read L2
            evacuateFrom(L1,address);
            tag = L1.getTag(address);
        }
        // miss in L2 thus acsses Mem
        else{ assert( (L1.containsBlockOf(address)==false) && (L2.containsBlockOf(address)==false) );
            targetCache = &L2;
            evicted = L2.leastRecentlyUsed(address);
            evacuateFrom(L2,address);
            tag = L2.getTag(address);
        }

        assert( evicted->valid==true );
        evicted->tag = tag;
        evicted->data = address;
        assert ( evicted->dirtyBit == NOT_DIRTY );

        targetCache->updateLRU(address); // <----  R E A D  the target cache ( L1 or L2 )
        return targetCache->getBlock(address);
    }



    // acssessing the data in L1 cache
    void L1_Hit(unsigned long address,char operation){
       std::vector<Block>::iterator modified = L1.updateLRU(address);
       modified->dirtyBit = (operation==WRITE) ? DIRTY :  modified->dirtyBit;
    }



    // acssessing the data in L2 cache
    void L2_Hit(unsigned long address,char operation){
        std::vector<Block>& set = L1.getSet(address);
        char missInL1 = operation;
        unsigned int occupiedWaysNum = 0;
        for( unsigned int i=0 ; i<set.size() ; ++i ) occupiedWaysNum = (set[i].valid) ? (occupiedWaysNum+1) : (occupiedWaysNum);
        
        if( WRITE==missInL1 ){
                if(/*******************/ writePolicy==WRITE_ALLOCATE /*************/){//----->> read_Hit in L2
                        if( occupiedWaysNum < L1.totalWaysNum ){
                            putInFreeWay(address) -> dirtyBit=DIRTY;
                            return;
                        }
                        else{ assert( occupiedWaysNum == L1.totalWaysNum );
                            evacuateAndPut(address) -> dirtyBit=DIRTY;
                            return;
                        }

                }else{ /*********/assert( writePolicy==NO_WRITE_ALLOCATE );/*******///----->> write_Hit in L2
                        L2.updateLRU(address) -> dirtyBit=DIRTY;
                        return;
                }

        }else{ assert( READ==missInL1 );
                if( occupiedWaysNum < L1.totalWaysNum ){
                    putInFreeWay(address);
                    return;
                }
                //else - no availible way
                    evacuateAndPut(address);
                    return;
        }
    }
                
            

    // keeping coherent by asserting the evacuation of data from L1 when evicting from l2
    void L2_snoops_L1(unsigned long int address){
        unsigned long int evictedAddress_L2 = L2.getSet(address)[LEAST_RECENTLY_USED].data;
        if( L1.containsBlockOf(evictedAddress_L2)==false ) return;

        (*L1.getBlock(evictedAddress_L2)) = Block(); 
        // if the data is dirty in L1     ===>>   "we assign" dirtyBit=DIRTY in cache-Level2
        // when evicting its L2 block                          (but its meaningless)
        }
    


    // acssessing the data in the main Memory, and fetching it into L2 cache
    void handle_L2_Miss(unsigned long int address){
        std::vector<Block>& set = L2.getSet(address);
        unsigned int occupiedWaysNum = 0;
        for( unsigned int i=0 ; i<set.size() ; ++i ) occupiedWaysNum = (set[i].valid) ? (occupiedWaysNum+1) : (occupiedWaysNum);
        
        if( occupiedWaysNum < L2.totalWaysNum ){
            putInFreeWay(address);
            return;
        }
        else{ assert( occupiedWaysNum == L2.totalWaysNum );
            L2_snoops_L1(address);
            evacuateAndPut(address);
            return;
        }
    }
    


    // acssessing the data in L2 cache, and fetching it into L1 cache after bringing it from main-Memory
    void handle_L1_Miss(unsigned long int address, char operation){
        std::vector<Block>& set = L1.getSet(address);
        unsigned int occupiedWaysNum = 0;
        for( unsigned int i=0 ; i<set.size() ; ++i ) occupiedWaysNum = (set[i].valid) ? (occupiedWaysNum+1) : (occupiedWaysNum);
        
        if( occupiedWaysNum < L1.totalWaysNum ){
            putInFreeWay(address);
        }
        else{ assert( occupiedWaysNum == L1.totalWaysNum );
            evacuateAndPut(address);
        }
        
        std::vector<Block>::iterator modified = L1.getBlock(address);
        modified->dirtyBit = (operation==WRITE) ? DIRTY :  modified->dirtyBit;
        return;
    }
        



    void L1_and_L2_Miss(unsigned long int address,char operation){
        if ( (writePolicy==NO_WRITE_ALLOCATE) && (operation == WRITE) ){
            /*      we "only" need to write the data into
                      its address  in the main memory                  */
                                return;
        }
        assert( (writePolicy==WRITE_ALLOCATE) || (operation == READ) );
        handle_L2_Miss(address);
        handle_L1_Miss(address,operation);
    }
};

#endif          //  CACHE_SIM_H_
