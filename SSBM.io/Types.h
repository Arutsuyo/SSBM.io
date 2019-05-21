#pragma once

#define _NUM_BUTTONS 5

enum VsType
{
    Self = 0,
    CPU = 1,
    Human = 2
};

enum Button
{
    A = 0,
    B = 1,
    X = 2,
    Z = 3,
    L = 4,
    None
};

// The order must match enum Button
char ButtonNames[] = {
    'A',
    'B',
    'X',
    'Z',
    'L'
};
