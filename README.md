# Real_Time_VOIP_Chat

## Pulse Audio

prec-simple.c, pcat-rec.c are the two example "C" files provided by pulse audio to record and play an pulse audio recorded signal respectively. We can compile them as follow.

``` sh

$ gcc prec-simple.c -lpulse -lpulse-simple -o prec-simple
$ gcc pcat-simple.c -lpulse -lpulse-simple -o pcat-simple

```