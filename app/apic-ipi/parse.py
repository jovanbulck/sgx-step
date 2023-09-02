#!/usr/bin/python3

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.font_manager as font_manager

# SKL laptop: IPI self nops
LATENCY_MIN = 170
LATENCY_MAX = 200

## SKL laptop: IPI self tsc
#LATENCY_MIN = 530
#LATENCY_MAX = 580
#
## ICX: IPI self tsc
#LATENCY_MIN = 520
#LATENCY_MAX = 550

# SKL laptop: IPI cross no-HT nops
LATENCY_MIN = 270
LATENCY_MAX = 400

# SKL laptop: IPI cross no-HT tsc
#LATENCY_MIN = 770
#LATENCY_MAX = 820

# SKL laptop: IPI cross HT nops
#LATENCY_MIN = 230
#LATENCY_MAX = 500

# SKL laptop: IPI cross HT tsc
LATENCY_MIN = 750
LATENCY_MAX = 850


pd.set_option('display.precision', 0)
np.set_printoptions(precision=0)

def hist(latencies, labels=[], title='', filename='hist', bins=500, xlabel='Latency (cycles)'):
    plt.figure(figsize=(10,5))

    for i in range(0,len(labels)):
        d = latencies[i]
        labels[i] += f' (μ={int(np.mean(d))}, σ={int(np.std(d))})' #, M={int(np.median(d))})'

    plt.hist(latencies,
             label=labels,
             bins=bins,
             histtype='stepfilled',
             alpha=0.5,
             range=[LATENCY_MIN, LATENCY_MAX])

    #for x in latencies:
    #    plt.axvline(np.median(x), color='k', linestyle='dashed', linewidth=1)

    font = font_manager.FontProperties(family='monospace', size=10) #weight='bold', style='normal')
    if len(labels) > 0:
        plt.legend(prop=font)

    plt.xlabel(xlabel)
    plt.ylabel('Frequency')
    plt.title(title)
    
    print(f".. writing histogram '{title}' to '{filename}.pdf'")
    plt.savefig(filename + '.pdf', bbox_inches='tight')
    plt.show() 

def reject_outliers(data, m=2):
    stdev = np.std(data)
    mean = np.mean(data)
    mask_min = mean - stdev * m
    mask_max = mean + stdev * m

    outliers = [d for d in data if d < mask_min or d > mask_max ]
    print(f'Warning: removing {len(outliers)} outliers:')
    print(outliers)

    return [d for d in data if d >= mask_min and d <= mask_max ]

def load_data(file, col):
    print(f".. loading data from '{file}'")
    #data = np.loadtxt(file, dtype=int, skiprows=1)
    d = pd.read_csv(file)
    data = d[col]#.astype(int)
    #data = reject_outliers(data)
    print('---------------------------------------------------------------------------')
    s = pd.Series(data)
    print(s.describe())
    print(f'med      {int(np.median(data))}')
    print('---------------------------------------------------------------------------')
    return data

#s0 = load_data('ipi-self.csv', col='inc_count')
#hist([s0], ['ipi-self'], 'APIC inter-processor interrupt CPU self NOP slide distribution')

#s0 = load_data('ipi-self.csv', col='tsc_diff')
#hist([s0], ['ipi-self'], 'APIC inter-processor interrupt CPU self TSC distribution')

#s0 = load_data('ipi-cross.csv', col='inc_count')
#hist([s0], ['ipi-cross-no-ht'], 'APIC inter-processor interrupt cross-CPU no-HT NOP slide distribution')

#s0 = load_data('ipi-cross.csv', col='tsc_diff')
#hist([s0], ['ipi-cross-no-ht'], 'APIC inter-processor interrupt cross-CPU no-HT TSC distribution')

#s0 = load_data('ipi-cross.csv', col='tsc_diff')
#hist([s0], ['ipi-cross-ht'], 'APIC inter-processor interrupt cross-CPU HyperThreading TSC distribution')

#s0 = load_data('ipi-cross.csv', col='inc_count')
#hist([s0], ['ipi-cross-ht'], 'APIC inter-processor interrupt cross-CPU HyperThreading NOP slide distribution')

LATENCY_MIN = 450
LATENCY_MAX = 750
s0 = load_data('logs/icx/ipi-ht.csv', col='inc_count')
s1 = load_data('logs/icx/ipi-no-ht.csv', col='inc_count')
hist([s0,s1], ['w/  HyperThreading', 'w/o HyperThreading'], bins=300, xlabel='NOP instructions executed before IRQ')

## Appendix figure in paper
LATENCY_MIN = 900
LATENCY_MAX = 2000
s0 = load_data('logs/icx/ipi-ht.csv', col='tsc_diff')
s1 = load_data('logs/icx/ipi-no-ht.csv', col='tsc_diff')
hist([s0,s1], ['w/  HyperThreading', 'w/o HyperThreading'])


