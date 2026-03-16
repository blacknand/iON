/**
    Because iON is a standalone allocator, it does not need a typical frontend
    like a full compiler would require. Instead, iON only takes in a specific IR
    so there is no need for a scanner and parser to tokenise the input and then
    determine if the source program is valid. iON (currently) assumes only valid IR.
*/

#pragma once

#include "IR.h"

