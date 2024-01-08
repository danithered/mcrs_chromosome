setwd("/home/danielred/data/programs/mcrs_chromosome/OUT/")

dirs = list.dirs(recursive = F)
dirs = grep("partest", dirs, value = T)

res = data.frame(dir=dirs, A=NA, Nrep=NA, Nmet=NA, D=NA, time=NA, alive=NA)
for(l in 1:length(dirs)){
  lines = readLines(paste0(res[l, "dir"], "/SAVE/parameters.txt"))
  table = read.table( text = sub(" ", "\t", lines) , header=F, sep="\t")
  p <- as.list(as.character(table$V2))
  names(p) <- table$V1
  
  res[l, "A"] = as.numeric(p$par_noEA)
  res[l, "Nrep"] = as.numeric(p$par_Nrep)
  res[l, "Nmet"] = as.numeric(p$par_Nmet)
  res[l, "D"] = as.numeric(p$par_diffusion_rate)
  
  endresults = strsplit(system(paste0("tail -n 1 ", res[l, "dir"], "/output.csv"), intern = T), ";")[[1]]
  res[l, "time"] = as.numeric(endresults[1])
  res[l, "alive"] = as.numeric(endresults[2]) > 0
  
}
