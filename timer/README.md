# Internal Kernel API (Time Management)

## Homework

1. User space. Implement program which shows absolute time in user space.
Pull request should contain the commit with source code and
generated output in text format.

2. Kernel space.
  a) Implement kernel module with API in sysfs,
     which show relation time in maximum possible resolution 
     passed since previous read of it.
  b) Implement kernel module with API in sysfs which shows absolute time of
     previous reading with maximum resolution like ‘400.123567’ seconds.
  c) (optional) Implement kernel module with API in sysfs which shows
     average processor load updated once in a second.
Pull request should contain the commit with source code and
text output from sysfs.
 
## Solution

The program in user space is not much (literally one line of code), but this
one is not that important, right?

The program in kernel space creates a class in sysfs and 3 attributes that
correspond to 3 sub tasks. The interval time is passed in nanoseconds.

#### User space output

`Absolute time: 1581088893 s`

#### Kernel space output

`[milaner@/sys/class/mytimer]$ ls`  
`interval  load  time`  
`[milaner@/sys/class/mytimer]$ cat interval`  
`9535071734`  
`[milaner@/sys/class/mytimer]$ cat load`  
`12%`  
`[milaner@/sys/class/mytimer]$ cat time`  
`1581088996.47667980`  

