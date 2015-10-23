// umrtgetopt.hh

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * *******************************************************************************/

#ifndef UMLRTGETOPT_HH
#define UMLRTGETOPT_HH

#if defined(__cplusplus)
extern "C"
{
#endif

extern const int no_argument;
extern const int required_argument;
extern const int optional_argument;

extern char* optarg;

/* the index of the next element to be processed in argv, the system initializes
 * this value to 1. The caller can reset it to 1 to restart scanning of the same
 * argv, or when scanning a new argument vector. */
extern int optind;

struct option
{
    const char* name;
    int has_arg;
    int* flag;
    int val;
};

int getopt_long(int argc, char* const argv[], const char* optstring, const struct option* longopts,
        int longopts_size, int* longindex);

#if defined(__cplusplus)
}
#endif

#endif // UMLRTGETOPT_HH
