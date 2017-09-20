#ifndef CNET_SUDOKU_H
#define CNET_SUDOKU_H

#include <cnet/base/Types.h>
#include <cnet/base/StringPiece.h>

cnet::string solveSudoku(const cnet::StringPiece& puzzle);
const int kCells = 81;
extern const char kNoSolution[];

#endif //CNET_SUDOKU_H
