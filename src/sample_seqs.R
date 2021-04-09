table = read.table("file:///home/danielred/data/programs/mcrs_chromosome/IN/1_InputMatrix.dat", sep=" ", header=F)
colnames(table) <- c("seq", "length", "met","A1", "A2", "A3")
str(table)
any(duplicated(table[table$V1 != "0",1]))
table(table$V1)
table[1,]
summary(table$V6)



state0 <- read.table("file:///home/danielred/data/programs/mcrs_chromosome/OUT/test3/SAVE/0.tsv", sep="\t", header=F )
colnames(state0) <- c("seq", "str", "mfe", "Pfold", "Pdeg", "no_sites", "R", "M", "type", "a0", "a1", "a2")
state0 <- cbind(state0, type_f = as.factor(state0$type))
str(state0)

table(state0$type_f)
what = c("1", "2", "4")
s2 <- state0[state0$type_f %in% what, ]
table(s2$type_f)

o <- lapply(what, function(x) sample(s2[s2$type_f == x, "seq"], 500))
str(o)
oo <- unlist(o)
str(oo)
head(oo)

setwd("/home/danielred/data/programs/mcrs_chromosome/IN/")
writeLines(oo, "sample500.txt")
