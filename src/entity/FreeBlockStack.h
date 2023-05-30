//
// Created by AlexHoring on 2023/5/28.
//

#ifndef FILESYSTEM_FREEBLOCKSTACK_H
#define FILESYSTEM_FREEBLOCKSTACK_H

#include <cstdint>
#include "../Constraints.h"

class FreeBlockStack {
public:
    FreeBlockStack();
    uint32_t getBlock();
    void revokeBlock(uint32_t block);
    uint32_t* getBlocks();
    uint32_t getMaxSize();
    bool empty();
    bool full();
    void setStackTop(int st);
private:
    const uint32_t maxSize;                                    //栈大小
    uint32_t stackTop;                                   //栈顶指针
    uint32_t blocks[BLOCK_SIZE/(8*sizeof(uint32_t))];   //栈本体，占一个块大小
};


#endif //FILESYSTEM_FREEBLOCKSTACK_H
