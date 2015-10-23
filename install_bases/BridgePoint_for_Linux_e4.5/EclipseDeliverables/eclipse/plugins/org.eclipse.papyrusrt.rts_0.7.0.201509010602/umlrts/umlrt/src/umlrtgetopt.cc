// umlrtgetopt.cc

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * *******************************************************************************/

#include <stddef.h>
#include <string.h>
#include "umlrtgetopt.hh"

const int no_argument = 0;
const int required_argument = 1;
const int optional_argument = 2;

char* optarg;
int optind = 1;

/* this implementation of getopt only supports required argument and
 * no argument options. Optional argument options are not supported.
 *
 * returns the short option character without the '-' and sets optarg
 * to the option value if it is required */
int getopt(int argc, char * const argv[], const char *optstring)
{
    int val = -1;

    if (optind > argc || argv[optind] == NULL)
    {
        return val;
    }

    optarg = argv[optind];

    int len = strlen(argv[optind]);
    /* check for '-' folowed by a character other than '-' */
    if (len > 1 && optarg[0] == '-' && optarg[1] != '-')
    {
        /* it is a short option, find the option in optstring */
        const char * optchar = strchr(optstring, optarg[1]);
        if (optchar)
        {
            if (optchar[1] == ':')
            {
                /* requires an argument */
                if (len > 2) // option argument included in the same string as the option
                {
                    optarg = optarg + 2; /* remove short option with the '-' */
                    val = *optchar;
                }
                else /* option argument included in the next argv */
                {
                    if (++optind <= argc)
                    {
                        /* get the option argument */
                        optarg = argv[optind];
                        val = *optchar;
                    }
                    else
                    {
                        optarg = 0;
                        val = '?';
                    }
                }
            }
            else /* no option argument required */
            {
                optarg = 0;
                val = *optchar;
            }
        }
    }

    optind++;
    return val;
}

/* this implementation of getopt only supports required argument and
 * no argument options. Optional argument options are not supported.
 * If longindex is not NULL, it points to a variable which is set to
 * the index of the long option relative to longopts.
 *
 * returns the option character val */
int getopt_long(int argc, char* const argv[], const char* optstring,
        const struct option* longopts, int longopts_size, int* longindex)
{
    int val = -1;

    if (optind > argc || argv[optind] == NULL)
    {
        return val;
    }

    optarg = argv[optind];

    int len = strlen(optarg);
    if (len > 1 && optarg[0] == '-' && optarg[1] != '-')
    {
        /* it is a short option so call getopt */
        return getopt(argc, argv, optstring);
    }

    /* it is a long option specified with --option or --option=value */
    if (len > 2)
    {
        optarg = optarg + 2; /* remove the -- */
        int optarg_size = strcspn(optarg, "=");
        // get the option struct
        bool optionfound = false;
        int numlongopts = longopts_size / sizeof(struct option);
        for (int i = 0; i < numlongopts; i++)
        {
            if (strncmp(longopts[i].name, optarg, optarg_size) == 0)
            {
                optionfound = true;
                if (longopts[i].has_arg == required_argument)
                {
                    val = longopts[i].val;
                    /* get the option value after '=' */
                    optarg = optarg + optarg_size + 1;
                    if (longindex)
                    {
                        *longindex = i;
                    }
                }
                else if (longopts[i].has_arg == no_argument)
                {
                    val = longopts[i].val;
                    if (longindex)
                    {
                        *longindex = i;
                    }
                    optarg = 0;
                }
                else
                {
                    optarg = 0;
                    val = '?';
                }
                break;
            }
        }
        if (!optionfound)
        {
            optarg = 0;
            val = '?';
        }
    }

    optind++;
    return val;
}
