// Copyright [2024] <Aaron Santana Valdelomar>

#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char* jobFile;
    size_t threadsCount;
} Arguments;


Arguments processArguments(int argc, char** argv);
