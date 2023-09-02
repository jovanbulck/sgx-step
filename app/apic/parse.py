#!/usr/bin/python3

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.font_manager as font_manager
import statistics

# ensure no type 3 fonts
import matplotlib
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42

# TSC deadline + IRQ gate; deadline diff
LATENCY_MIN = 1000
LATENCY_MAX = 1500

# TSC deadline + IRQ gate; tsc diff
#LATENCY_MIN = 11000
#LATENCY_MAX = 11500

# TSC deadline w/o IRQ gate; deadline diff
#LATENCY_MIN = 900
#LATENCY_MAX = 1300

# TSC oneshot; tsc diff
LATENCY_MIN = 14600
LATENCY_MAX = 15200

# ICX TSC oneshot; tsc diff
LATENCY_MIN = 10800
LATENCY_MAX = 11200

pd.set_option('display.precision', 0)
np.set_printoptions(precision=0)

def hist(latencies, labels=[], title='', filename='hist', xlabel='Latency (cycles)', legend_loc='best'):
    plt.figure(figsize=(10,5))
    #colors = []

    for i in range(0,len(labels)):
        d = latencies[i]
        labels[i] += f' (μ={int(np.mean(d))}, σ={int(np.std(d))})' #, M={int(np.median(d))})'
        #colors.append(f'C{i}')

    plt.hist(latencies,
             label=labels,
             bins=500,
             #histtype='step',
             histtype='stepfilled',
             alpha=0.5,
             range=[LATENCY_MIN, LATENCY_MAX])

    #for i in range(0,len(latencies)):
    #    x = latencies[i]
    #    plt.axvline(np.mean(x), color=colors[i], linestyle='dashed', linewidth=1)

    font = font_manager.FontProperties(family='monospace', size=10) #weight='bold', style='normal')
    if len(labels) > 0:
        plt.legend(prop=font,loc=legend_loc)

    plt.xlabel(xlabel)
    plt.ylabel('Frequency')
    plt.title(title)
    
    print(f".. writing histogram '{title}' to '{filename}.pdf'")
    plt.savefig(filename + '.pdf', bbox_inches='tight')
    plt.show() 

def reject_outliers(data, m=3):
    stdev = np.std(data)
    mean = np.mean(data)
    mask_min = mean - stdev * m
    mask_max = mean + stdev * m

    outliers = [d for d in data if d < mask_min or d > mask_max ]
    print(f'Warning: removing {len(outliers)} outliers:')
    print(outliers)

    return [d for d in data if d >= mask_min and d <= mask_max ]

def reject_syscall_inc_outliers(data):
    for i in range(0, len(data)):
        if data[i] > 1000000:
            print(f'Warning: removing outlier: {data[i]}')
            data[i] = 0
    return data

def load_data(file, col):
    print(f".. loading data from '{file}'")
    #data = np.loadtxt(file, dtype=int, skiprows=1)
    d = pd.read_csv(file)
    data = d[col]
    #data = reject_outliers(data)

    #HACK: if the IRQ arrived before the slide the counter in RAX is invalid (zero)
    if file == 'logs/icx/deadline_results_syscall.txt' and col == 'inc_count':
        data = reject_syscall_inc_outliers(data)

    print('---------------------------------------------------------------------------')
    s = pd.Series(data)
    print(s.describe())
    print(f'med      {int(np.median(data))}')
    print('---------------------------------------------------------------------------')
    return data


#LATENCY_MIN = 10700
#LATENCY_MAX = 11500
#s0 = load_data('oneshot_results.txt', col='#tsc_diff')
#hist([s0], ['oneshot'])
#exit()

##s0 = load_data('deadline_results.txt', col='tsc_deadline_drift')

#s0 = load_data('logs/icx/oneshot_results.txt', col='inc_count')
#s1 = load_data('logs/icx/deadline_results_syscall.txt', col='inc_count')
#s2 = load_data('logs/icx/deadline_results_irq_gate.txt', col='inc_count')
#LATENCY_MIN = 2500
#LATENCY_MAX = 10000
#hist([s2,s1,s0], ['TSC IRQ gate', 'TSC syscall', 'oneshot'], xlabel='ADD instructions executed before IRQ', legend_loc='upper left')

## Appendix figure in paper
LATENCY_MIN = 10680
LATENCY_MAX = 11120
s0 = load_data('logs/icx/oneshot_results.txt', col='#tsc_diff')
s1 = load_data('logs/icx/deadline_results_syscall.txt', col='#tsc_diff')
s2 = load_data('logs/icx/deadline_results_irq_gate.txt', col='#tsc_diff')
hist([s2,s1,s0], ['TSC IRQ gate', 'TSC syscall', 'oneshot'])

#s0 = load_data('oneshot_results.txt', col='#tsc_diff')
#hist([s0], ['oneshot'], 'APIC timer oneshot TSC distribution')

#LATENCY_MIN = 650
#LATENCY_MAX = 680
##s0 = load_data('pt_results.txt', col='pte-not-a')
#s0 = load_data('pt_results.txt', col='pte-a')
#hist([s0], ['pte-not-a'], 'Assisted page-table walk latency distribution')

