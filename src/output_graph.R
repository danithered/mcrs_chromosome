#!/bin/R

par.orig <- par(no.readonly = T) # save default parameters

library(RColorBrewer)
library(ggplot2)

setwd("/home/danielred/data/programs/mcrs_chromosome/OUT/test3/")

t <- read.table("SAVE/parameters.txt", header=F, sep=" ")
p <- as.list(as.character(t$V2))
names(p) <- t$V1

strsplit(readLines("SAVE/parameters.txt"), " ")


table <- read.table("output.csv", header=T, sep=";")
str(table)

# oszlopok:
# 	time
#		replicators: number of replicators on the grid
#		no/mean_R/mean_length/mean_mfe: number/average properties of replicators having a given enzymatic activity. Note, that in case a replicator have more activities, its values are considered in more than one category.
# 	no_Ax: number of replicators having x activities


# number of promiscous replicators

linecolors <- brewer.pal(4, "Set1")
plot(table$time, table$no_A0
     , type="l"
     , ylim=c(0, max(table$replicators))
     , main="Number of promiscous replicators"
     , col=linecolors[1])
for(i in 1:3) lines(table$time, table[,paste0("no_A", i)], col=linecolors[i+1])
legend("topleft", fill=linecolors, legend=paste("A",0:3))

# by activity by property

{par(mfrow=c(4,5), oma=c(5,2,4,1), mar=c(0,4.1,0,0))
  plotprop=T
  axt="n"
  size = as.numeric(p$par_ncol) * as.numeric(p$par_nrow)
  for(ea in c(paste0("enz",0:2),"par" )) {
    plottype=T
    if(ea=="par") axt="s"
    for(prop in c("no", "mean_R", "mean_length", "mean_mfe", "mean_a")){
      what <- paste(prop, ea, sep="_")
      
      if(prop == "no") {
        table[,what] <- table[,what] / size
        yl = c(0,1)
      }
      else{
        yl=NULL
      }
      
      if(what == "mean_a_par") plot.new()
      else plot(table$time, table[,what], type="l", ylab="", xlab="", xaxt=axt, las=1, ylim=yl, col="red")
      
      if(plottype){
        plottype=F
        mtext(ea, 2,4)
      }
      
      if(plotprop) mtext(prop, 3,2)
    }
    if(plotprop) plotprop=F
  }
  mtext("time", 1, 3, outer=T)
}


par(par.orig) #restore default parameters



# unique data
state0 <- read.table("SAVE/0.tsv", sep="\t", header=F )
colnames(state0) <- c("seq", "str", "mfe", "Pfold", "Pdeg", "no_sites", "R", "M", "type", "a0", "a1", "a2")
state0 <- cbind(state0, type_f = as.factor(state0$type))
str(state)

hist(state[state$a0 > 0,"mfe"])

ggplot(data=state[state$seq != "N",],aes(mfe, fill=type_f)) +
  geom_histogram(binwidth=0.5) +
  xlim(0,as.numeric(p$par_Emin))

#maps
library(lattice)
image(1:300, 1:300,matrix(state$M, ncol=300) )
image(1:300, 1:300,matrix(state$R, ncol=300) )




















# Diagnostics

max(table$time)
table[table$time == max(table$time),]



#Pfold
ggplot(data=state[state$seq != "N",],aes(Pfold, fill=type_f)) +
  geom_histogram() 
ggplot(data=state0[state0$seq != "N",],aes(Pfold, fill=type_f)) +
  geom_histogram() 
curve(1- 1/(1+ exp( -0.3*x )), xlab="mfe", xlim=c(-25,0), ylab="Pfold")

#mfe
ggplot(data=state[state$seq != "N",],aes(mfe, fill=type_f)) +
  geom_histogram(binwidth=0.5) 
xlim(0,as.numeric(p$par_Emin))
plot(table$time, table$mean_mfe_par
     , type="l"
     , ylim=c(-25,0)
     , main="Number of promiscous replicators"
     , col=linecolors[1])
for(i in 0:2) lines(table$time, table[,paste0("mean_mfe_enz", i)], col=linecolors[i+1])
legend("topleft", fill=linecolors, legend=paste("A",0:3))

#Pdeg
ggplot(data=state[state$seq != "N",],aes(Pdeg, fill=type_f)) +
  geom_histogram() 
mfe=-0.00001
0.9-0.8*mfe/-25
curve(0.9-0.8*x/-25, xlim=c(-25,0))

#a
curve(x*1* 1/(1^1.1), xlab="Pfold", ylab="a", xlim=c(0,1))
curve(  ( 1- 1/(1+ exp( -0.3*x )) )  *1* 1/(1^1.1), xlab="mfe", ylab="a", xlim=c(-25,0))

#R
ggplot(data=state[state$seq != "N",],aes(R, fill=type_f)) +
  geom_histogram() 
ggplot(data=state0[state0$seq != "N",],aes(R, fill=type_f)) +
  geom_histogram() 

curve(    10/ (0.75 + 0.005 * 30) * ( 2 - x)   , xlab="Pfold", ylab="R", )
curve(    10/ (0.75 + 0.005 * 30) * ( 2 - ( 1- 1/(1+ exp( -0.3*x )) ) )   , xlab="mfe", ylab="R", xlim=c(-25,0))


# a*a*a*R

curve( (( 1- 1/(1+ exp( -0.3*x )) )  *1* 1/(1^1.1))^3  *  10/ (0.75 + 0.005 * 30) * ( 2 - ( 1- 1/(1+ exp( -0.3*x )) ) )   , xlab="mfe", ylab="R*a*a*a", xlim=c(-25,0))

#a*R
curve( (( 1- 1/(1+ exp( -0.3*x )) )  *1* 1/(1^1.1))  *  10/ (0.75 + 0.005 * 30) * ( 2 - ( 1- 1/(1+ exp( -0.3*x )) ) )   , xlab="mfe", ylab="R*a", xlim=c(-25,0))
