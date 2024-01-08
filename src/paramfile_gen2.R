#!/usr/bin/env Rscript

args = commandArgs(trailingOnly=TRUE)

if(length(args) == 0) cat("need arguments!")

#args=c("ez,1,2", "az,A,B,C", "nem,")

arg = sapply(strsplit(args, ","), function(x) {
	out = list()
	#print(str(x[1]))
	if(length(x) > 1){
		out[x[1]] = list(x[2:length(x)])
		return(out)
		#out <- as.character(x[2:length(x)])
	}
	else {
		return(NA)
	}
	} )

arg <- arg[sapply(arg, function(x) !is.na(x[1]) )] # remove listelements with NA

#l <- list(1:2, LETTERS[3:6], character(), letters[7:8])
#pres <- c("--number", "--LETTER", "--nothing", "--letter")

po <- function (lista, level, pre){
	if(length(pre) != length(lista)) cat("WRONG: ", length(pre), " ", length(lista))
	out <- list()
	if( length(lista[[level]]) > 0 ){
		for(elem in lista[[level]]){
			if (length(lista) > level) {
				out <- append(out, paste(pre[level], elem, po(lista, level+1, pre)))
			}
			else out <- append(out,paste(pre[level], elem))
		}
		return(out)
	}
	else return(po(lista, level+1, pre))
}

#po(arg, 1, names(arg))

##filtering out neighbourhoods and unifying them
argf <- arg[!names(arg) %in% c("--par_Nmet", "--par_Nrep")]

ol <- po(argf, 1, names(argf))

ol <- outer(ol, 
      sapply(arg["--par_Nmet"], function(n) paste("--par_Nmet", n, "--par_Nrep", n)), 
      paste) |>
  as.list()

# printing
if(length(arg) > 0){
	cat(paste(ol, collapse="\n"))
} else {
	cat ("\n")
}

