#!/bin/bash
gnuplot -e "set terminal png; set output \"aaaa.png\"; \
            set datafile missing \"X\" ; \
            plot \"-\" matrix with line ;
            "
